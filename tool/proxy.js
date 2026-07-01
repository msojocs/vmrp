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
const proxy2Magic = 0x8aed9cf3;
const proxy2FixedHeaderLength = 17;

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

function asciiValue(buffer) {
    return printable(buffer).replace(/\.+$/u, '');
}

function parseParams(text) {
    const query = text.includes('?') ? text.slice(text.indexOf('?') + 1) : text;
    if (!query.includes('=')) return undefined;

    const params = {};
    for (const [key, value] of new URLSearchParams(query)) {
        if (!/^[A-Za-z0-9_.~-]+$/u.test(key)) {
            return undefined;
        }
        if (!Object.prototype.hasOwnProperty.call(params, key)) {
            params[key] = value;
            continue;
        }
        params[key] = Array.isArray(params[key]) ? [...params[key], value] : [params[key], value];
    }
    return Object.keys(params).length ? params : undefined;
}

function fieldInfo(tag, length, value, offset) {
    const text = asciiValue(value);
    const field = {
        tag,
        length,
        offset,
        ascii: text,
    };
    if (value.length <= 16) {
        field.hex = value.toString('hex');
    }
    if (value.length === 1) {
        field.u8 = value[0];
    } else if (value.length === 2) {
        field.u16 = value.readUInt16BE(0);
    } else if (value.length === 4) {
        field.u32 = value.readUInt32BE(0);
    }

    const params = parseParams(text);
    if (params) {
        field.params = params;
    }
    return field;
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

function parseFieldStream(body, offset, limit = body.length, expectedCount = undefined) {
    const fields = [];
    const end = Math.min(limit, body.length);
    while (offset + 4 <= end && (expectedCount == null || fields.length < expectedCount)) {
        const tag = readU16BE(body, offset);
        const length = readU16BE(body, offset + 2);
        const valueOffset = offset + 4;
        const valueEnd = valueOffset + length;
        if (valueEnd > end) {
            return {
                fields,
                parsedLength: offset,
                error: `field ${fields.length} tag ${tag} length ${length} exceeds limit ${end - valueOffset}`,
            };
        }
        const value = body.subarray(valueOffset, valueEnd);
        fields.push(fieldInfo(tag, length, value, offset));
        offset = valueEnd;
    }
    const error = expectedCount != null && fields.length < expectedCount
        ? `expected ${expectedCount} fields, parsed ${fields.length}`
        : undefined;
    return { fields, parsedLength: offset, error };
}

function summarizeBytes(buffer) {
    const summary = {
        length: buffer.length,
        ascii: asciiValue(buffer),
    };
    if (buffer.length <= 64) {
        summary.hex = buffer.toString('hex');
    }
    const params = parseParams(summary.ascii);
    if (params) {
        summary.params = params;
    }
    return summary;
}

function parseProxy2Payload(body, offset, declaredLength) {
    const length = declaredLength || 0;
    const end = Math.min(offset + length, body.length);
    const payloadBuffer = body.subarray(offset, end);
    const payload = {
        offset,
        declaredLength: length,
        ...summarizeBytes(payloadBuffer),
    };
    if (payloadBuffer.length < length) {
        payload.truncatedLength = length - payloadBuffer.length;
    }
    if (payloadBuffer.length < 5) {
        return payload;
    }

    // /sta captures use one byte of payload type followed by a TLV stream.
    const parsed = parseFieldStream(payloadBuffer, 1, payloadBuffer.length);
    if (parsed.fields.length > 0) {
        payload.format = 'u8-tlv';
        payload.type = payloadBuffer[0];
        payload.fields = parsed.fields;
        payload.parsedLength = parsed.parsedLength;
        payload.trailingLength = payloadBuffer.length - parsed.parsedLength;
        if (payload.trailingLength > 0 && payload.trailingLength <= 16) {
            payload.trailingHex = payloadBuffer.subarray(parsed.parsedLength).toString('hex');
        }
        if (parsed.error) {
            payload.error = parsed.error;
        }
    }
    return payload;
}

function parseProxy2Packet(body) {
    const headerLength = readU16BE(body, 6);
    const code = readU16BE(body, 8);
    const status = readU16BE(body, 10);
    const payloadLength = readU32BE(body, 12);
    const packet = {
        magic: readU32BE(body, 0),
        version: readU16BE(body, 4),
        headerLength,
        declaredLength: headerLength,
        code,
        status,
        payloadLength,
        fieldCount: body.length > 16 ? body[16] : undefined,
        fields: [],
    };
    packet.service = packet.code;
    packet.serviceLength = packet.status;

    if (packet.magic !== proxy2Magic || body.length < proxy2FixedHeaderLength) {
        return packet;
    }

    const fieldOffset = proxy2FixedHeaderLength;
    const fieldLimit = packet.headerLength && packet.headerLength <= body.length
        ? packet.headerLength
        : body.length;
    const parsed = parseFieldStream(body, fieldOffset, fieldLimit, packet.fieldCount);
    packet.fieldOffset = fieldOffset;
    packet.fields = parsed.fields;
    packet.parsedLength = parsed.parsedLength;
    packet.headerTrailingLength = Math.max(0, fieldLimit - parsed.parsedLength);
    if (parsed.error) {
        packet.error = parsed.error;
    }

    const packetLength = (packet.headerLength || 0) + (packet.payloadLength || 0);
    packet.packetLength = packetLength;
    packet.payload = parseProxy2Payload(body, packet.headerLength || body.length, packet.payloadLength || 0);
    packet.trailingLength = packetLength < body.length ? body.length - packetLength : 0;
    packet.truncatedLength = packetLength > body.length ? packetLength - body.length : 0;
    return packet;
}
function logProxy2Packet(req, body) {
    const tempDir = path.resolve(__dirname, './temp');
    fs.mkdirSync(tempDir, { recursive: true });
    fs.writeFileSync(path.resolve(tempDir, req.url.replace(/\//g, '_') + `_${Date.now()}.bin`), body);
    const packet = parseProxy2Packet(body);
    console.info('proxy2 packet:', JSON.stringify({
        path: req.url,
        bodyLength: body.length,
        magic: packet.magic == null ? null : `0x${packet.magic.toString(16)}`,
        version: packet.version,
        declaredLength: packet.declaredLength,
        headerLength: packet.headerLength,
        code: packet.code,
        status: packet.status,
        payloadLength: packet.payloadLength,
        fieldCount: packet.fieldCount,
        fieldOffset: packet.fieldOffset,
        parsedLength: packet.parsedLength,
        headerTrailingLength: packet.headerTrailingLength,
        packetLength: packet.packetLength,
        trailingLength: packet.trailingLength,
        truncatedLength: packet.truncatedLength,
        error: packet.error,
        fields: packet.fields,
        payload: packet.payload,
    }));
    console.info('\n');
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
