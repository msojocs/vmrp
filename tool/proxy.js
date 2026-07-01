const fs = require('node:fs');
const zlib = require('node:zlib');
const path = require('node:path');

const responseBody = 'helloworld';
const proxy2FixturePath = process.env.PROXY2_PAGE2_FIXTURE || '';
const proxy2MrpPath = process.env.PROXY2_PAGE2_MRP || path.resolve('build/mythroad/wbrw.mrp');
const proxy2Mode = process.env.PROXY2_MODE || 'packet';
const proxy2Page2Encoding = process.env.PROXY2_PAGE2_ENCODING || 'raw';
// proxy2二进制包响应使用octet-stream，与请求的Content-Type对称
const proxy2Page2ContentType = process.env.PROXY2_PAGE2_CONTENT_TYPE || 'application/octet-stream';
// WBRW's proxy2 downloader treats socket EOF as the response completion signal.
const proxy2Connection = process.env.PROXY2_CONNECTION || 'close';
const proxy2PacketVersion = Number(process.env.PROXY2_PACKET_VERSION || 1);
const proxy2PacketCode = Number(process.env.PROXY2_PACKET_CODE || 0);
const proxy2PacketStatus = Number(process.env.PROXY2_PACKET_STATUS || 200);
// tag33值3表示首页内容类型，响应必须包含此字段才会被WBRW接受
const proxy2Page2Tag33 = process.env.PROXY2_PAGE2_TAG33 ?? '3';
const proxy2Page2Tag21 = process.env.PROXY2_PAGE2_TAG21 ?? '';
const updateMode = process.env.WBRW_UPDATE_MODE || 'fixture';
const updateFixturePath = process.env.WBRW_UPDATE_FIXTURE || '';
const updateContentType = process.env.WBRW_UPDATE_CONTENT_TYPE || 'application/sky-mrp';

function readU16BE(buffer, offset) {
    return offset + 2 <= buffer.length ? buffer.readUInt16BE(offset) : undefined;
}

function readU32BE(buffer, offset) {
    return offset + 4 <= buffer.length ? buffer.readUInt32BE(offset) : undefined;
}

function writeU16BE(buffer, offset, value) {
    buffer.writeUInt16BE(value & 0xffff, offset);
}

function writeU32BE(buffer, offset, value) {
    buffer.writeUInt32BE(value >>> 0, offset);
}

function printable(buffer) {
    return [...buffer]
        .map(byte => (byte >= 0x20 && byte < 0x7f ? String.fromCharCode(byte) : '.'))
        .join('');
}

function readMrpEntry(mrpPath, entryName, options = {}) {
    if (!fs.existsSync(mrpPath)) return null;
    const data = fs.readFileSync(mrpPath);
    if (data.length < 240 || data.subarray(0, 4).toString('latin1') !== 'MRPG') return null;

    const fileStart = data.readUInt32LE(4) + 8;
    let offset = data.readUInt32LE(12);
    while (offset < fileStart && offset + 16 <= data.length) {
        const nameLength = data.readUInt32LE(offset);
        offset += 4;
        if (nameLength === 0 || nameLength > 512 || offset + nameLength + 12 > data.length) return null;
        const name = data.subarray(offset, offset + nameLength).toString('utf8').replace(/\0.*$/u, '');
        offset += nameLength;
        const fileOffset = data.readUInt32LE(offset);
        offset += 4;
        const packedLength = data.readUInt32LE(offset);
        offset += 4;
        offset += 4;

        if (name !== entryName) continue;
        const packed = data.subarray(fileOffset, fileOffset + packedLength);
        if (options.packed) return Buffer.from(packed);
        return packed[0] === 0x1f && packed[1] === 0x8b ? zlib.gunzipSync(packed) : Buffer.from(packed);
    }
    return null;
}

function readProxy2PageFixture() {
    if (proxy2FixturePath && fs.existsSync(proxy2FixturePath)) {
        return fs.readFileSync(proxy2FixturePath);
    }
    return readMrpEntry(proxy2MrpPath, 'mphome.sky', { packed: proxy2Page2Encoding === 'packed' });
}

function proxy2U8(value) {
    return Buffer.from([Number(value) & 0xff]);
}

function proxy2Page2Fields() {
    const fields = [];
    if (proxy2Page2Tag21 !== '' && !/^(?:false|none|off)$/iu.test(proxy2Page2Tag21)) {
        fields.push({ tag: 21, value: proxy2U8(proxy2Page2Tag21) });
    }
    if (proxy2Page2Tag33 !== '' && !/^(?:0|false|none|off)$/iu.test(proxy2Page2Tag33)) {
        fields.push({ tag: 33, value: proxy2U8(proxy2Page2Tag33) });
    }
    return fields;
}

function readUpdateFixture() {
    if (updateFixturePath && fs.existsSync(updateFixturePath)) {
        return fs.readFileSync(updateFixturePath);
    }
    return readMrpEntry(proxy2MrpPath, 'mphome.sky');
}

function makeProxy2ResponsePacket(payload, fields = []) {
    const fieldLength = fields.reduce((sum, field) => sum + 4 + field.value.length, 0);
    const headerLength = 17 + fieldLength;
    const totalLength = headerLength + payload.length;
    const packet = Buffer.alloc(totalLength);
    writeU32BE(packet, 0, 0x8aed9cf3);
    writeU16BE(packet, 4, proxy2PacketVersion);
    writeU16BE(packet, 6, headerLength);
    writeU16BE(packet, 8, proxy2PacketCode);
    writeU16BE(packet, 10, proxy2PacketStatus);
    writeU32BE(packet, 12, payload.length);
    packet[16] = fields.length & 0xff;

    let offset = 17;
    for (const field of fields) {
        writeU16BE(packet, offset, field.tag);
        writeU16BE(packet, offset + 2, field.value.length);
        field.value.copy(packet, offset + 4);
        offset += 4 + field.value.length;
    }
    payload.copy(packet, headerLength);
    return packet;
}

function parseFieldStream(body, offset, limit = body.length) {
    const fields = [];
    while (offset + 4 <= limit) {
        const tag = readU16BE(body, offset);
        const length = readU16BE(body, offset + 2);
        const valueOffset = offset + 4;
        const valueEnd = valueOffset + length;
        if (length > limit || valueEnd > limit) break;
        const value = body.subarray(valueOffset, valueEnd);
        fields.push({
            tag,
            length,
            ascii: printable(value).replace(/\.+$/u, ''),
            hex: value.length <= 8 ? value.toString('hex') : undefined,
        });
        offset = valueEnd;
    }
    return { fields, parsedLength: offset };
}

function parseProxy2Packet(body) {
    const packet = {
        magic: readU32BE(body, 0),
        version: readU16BE(body, 4),
        declaredLength: readU16BE(body, 6),
        service: readU16BE(body, 8),
        serviceLength: readU16BE(body, 10),
        fields: [],
    };

    if (packet.magic !== 0x8aed9cf3 || body.length < 8) {
        return packet;
    }

    let fieldOffset = 12 + (packet.serviceLength || 0);
    // Captured WBRW packets put a compact segment header before the TLV field stream.
    if (packet.service === 1 && packet.serviceLength === 1 && body[12] === 0) {
        fieldOffset = 17;
    } else if (packet.service === 5 && packet.serviceLength === 2) {
        fieldOffset = 17;
    }
    const fieldLimit = packet.declaredLength && packet.declaredLength <= body.length
        ? packet.declaredLength
        : body.length;
    const parsed = parseFieldStream(body, fieldOffset, fieldLimit);
    packet.fieldOffset = fieldOffset;
    packet.fields = parsed.fields;
    packet.parsedLength = parsed.parsedLength;
    packet.trailingLength = body.length - parsed.parsedLength;
    return packet;
}

function logProxy2Packet(req, body) {
    const packet = parseProxy2Packet(body);
    console.info('proxy2 packet:', JSON.stringify({
        path: req.url,
        bodyLength: body.length,
        magic: packet.magic == null ? null : `0x${packet.magic.toString(16)}`,
        version: packet.version,
        declaredLength: packet.declaredLength,
        service: packet.service,
        serviceLength: packet.serviceLength,
        fieldOffset: packet.fieldOffset,
        parsedLength: packet.parsedLength,
        trailingLength: packet.trailingLength,
        fields: packet.fields,
    }));
}

function sendBuffer(res, contentType, body) {
    console.info('res:', JSON.stringify({
        contentType,
        contentLength: body.length,
    }));
    res.writeHead(200, {
        'Content-Type': contentType,
        'Content-Length': body.length,
        Connection: proxy2Connection === 'keep-alive' ? 'keep-alive' : 'close',
    });
    res.end(body);
}

function sendRedirect(res, location) {
    console.info('res:', JSON.stringify({
        status: 302,
        location,
        contentLength: 0,
    }));
    res.writeHead(302, {
        Location: location,
        'Content-Length': 0,
        Connection: proxy2Connection === 'keep-alive' ? 'keep-alive' : 'close',
    });
    res.end();
}

function handleProxy2(req, res, body) {
    logProxy2Packet(req, body);

    // /sta统计上报始终返回纯文本retcode=0
    if (req.url === '/sta') {
        sendBuffer(res, 'text/plain; charset=utf-8', Buffer.from('retcode=0\n'));
        return true;
    }

    if (req.url === '/page2') {
        if (proxy2Mode === 'retcode') {
            sendBuffer(res, 'text/plain; charset=utf-8', Buffer.from('retcode=0\n'));
            return true;
        }
        const fixture = readProxy2PageFixture();
        if (fixture) {
            if (proxy2Mode === 'packet') {
                sendBuffer(res, proxy2Page2ContentType,
                    makeProxy2ResponsePacket(fixture, proxy2Page2Fields()));
                return true;
            }
            sendBuffer(res, proxy2Page2ContentType, fixture);
            return true;
        }
        sendBuffer(res, 'text/plain; charset=utf-8', Buffer.from('retcode=0\n'));
        return true;
    }

    return false;
}

function handleHelpProxy(req, res) {
    if (!req.url.startsWith('/update/?pageid=0')) {
        return false;
    }

    if (updateMode === 'redirect') {
        sendRedirect(res, 'http://proxy2.51mrp.com/page2');
        return true;
    }

    const fixture = readUpdateFixture();
    if (fixture) {
        sendBuffer(res, updateContentType, fixture);
        return true;
    }
    sendBuffer(res, 'text/plain; charset=utf-8', Buffer.from('retcode=0\n'));
    return true;
}

function sendProxyError(res, error) {
    console.error('proxy error:', error && error.stack ? error.stack : error);
    if (res.headersSent) {
        res.destroy(error);
        return;
    }

    const body = 'proxy error\n';
    res.writeHead(500, {
        'Content-Type': 'text/plain; charset=utf-8',
        'Content-Length': Buffer.byteLength(body),
        Connection: 'close',
    });
    res.end(body);
}

function handleRequestBody(req, res, body) {
    const host = (req.headers.host || '').split(':')[0];

    if (host === 'proxy2.51mrp.com' && handleProxy2(req, res, body)) {
        return;
    }

    if (host === 'help.proxy.51mrp.com' && handleHelpProxy(req, res)) {
        return;
    }

    // Print the complete request body for local HTTP debugging.
    console.info(body.toString('utf8'));

    res.writeHead(200, {
        'Content-Type': 'text/plain; charset=utf-8',
        'Content-Length': Buffer.byteLength(responseBody),
    });
    res.end(responseBody);
}

function handleRequest(req, res) {
    const chunks = [];
    console.info('req path:', req.url);

    req.on('data', chunk => {
        chunks.push(chunk);
    });

    req.on('error', error => {
        sendProxyError(res, error);
    });

    req.on('end', () => {
        try {
            handleRequestBody(req, res, Buffer.concat(chunks));
        } catch (error) {
            sendProxyError(res, error);
        }
    });
}

module.exports = handleRequest;
