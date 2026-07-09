const fs = require('node:fs');
const http = require('node:http');
const https = require('node:https');
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

function makeProxy2ResponsePacket(payload, fields = [], status = proxy2PacketStatus) {
    const fieldLength = fields.reduce((sum, field) => sum + 4 + field.value.length, 0);
    const headerLength = 17 + fieldLength;
    const totalLength = headerLength + payload.length;
    const packet = Buffer.alloc(totalLength);
    writeU32BE(packet, 0, 0x8aed9cf3);
    writeU16BE(packet, 4, proxy2PacketVersion);
    writeU16BE(packet, 6, headerLength);
    writeU16BE(packet, 8, proxy2PacketCode);
    writeU16BE(packet, 10, status);
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
// 从proxy2请求包中提取tag1(路径)和tag2(目标主机)
function extractProxy2RequestUrl(body) {
    const packet = parseProxy2Packet(body);
    let urlPath = '';
    let host = '';
    for (const field of packet.fields) {
        if (field.tag === 1) urlPath = field.ascii;
        if (field.tag === 2) host = field.ascii;
    }
    return { host, path: urlPath };
}

function buildProxy2TargetUrl(host, urlPath, defaultProtocol = 'http:') {
    // WBRW proxy2 requests carry the target host in tag2 and usually carry only
    // the origin-form path in tag1.  Some callers may pass an absolute URL in
    // tag1, so normalize both shapes through URL instead of string splicing.
    if (urlPath && /^https?:\/\//iu.test(urlPath)) {
        return new URL(urlPath).toString();
    }
    const pathPart = urlPath && urlPath.startsWith('/') ? urlPath : `/${urlPath || ''}`;
    return new URL(pathPart, `${defaultProtocol}//${host}`).toString();
}

const proxy2FetchTimeoutMs = Number(process.env.PROXY2_FETCH_TIMEOUT_MS || 15000);
const proxy2FetchMaxRedirects = Number(process.env.PROXY2_FETCH_MAX_REDIRECTS || 5);

// 发起HTTP GET请求，跟随重定向，返回响应body
function fetchUrl(url, callback, redirectCount) {
    redirectCount = redirectCount || 0;
    if (redirectCount > proxy2FetchMaxRedirects) {
        callback(new Error('too many redirects'));
        return;
    }

    const mod = url.startsWith('https://') ? https : http;
    let parsed;
    try {
        parsed = new URL(url);
    } catch (e) {
        callback(e);
        return;
    }

    const options = {
        hostname: parsed.hostname,
        port: parsed.port || (parsed.protocol === 'https:' ? 443 : 80),
        path: parsed.pathname + parsed.search,
        method: 'GET',
        headers: {
            'Host': parsed.host,
            'User-Agent': 'Sky-Wapproxy(MTK3210)',
            'Accept': 'text/vnd.wap.wml,text/html,*/*',
            'Accept-Charset': 'utf-8',
            'Connection': 'close',
        },
        timeout: proxy2FetchTimeoutMs,
    };

    console.info('proxy2 fetch: %s', url);
    const req = mod.request(options, (res) => {
        // 跟随3xx重定向; URL resolves absolute, root-relative, and
        // protocol-relative Location values consistently.
        if (res.statusCode >= 300 && res.statusCode < 400 && res.headers.location) {
            const redirectUrl = new URL(res.headers.location, parsed).toString();
            console.info('proxy2 redirect %d -> %s', res.statusCode, redirectUrl);
            res.resume();
            fetchUrl(redirectUrl, callback, redirectCount + 1);
            return;
        }
        const chunks = [];
        res.on('data', chunk => chunks.push(chunk));
        res.on('end', () => {
            callback(null, Buffer.concat(chunks), res.statusCode, res.headers);
        });
    });

    req.on('error', err => callback(err));
    req.on('timeout', () => {
        req.destroy();
        callback(new Error('fetch timeout'));
    });
    req.end();
}

// WAP网关代理将桌面HTML转换为WBRW可渲染的精简页面;
// WBRW recv缓冲区有限,proxy2响应payload需控制在此字节内
const proxy2MaxPayloadBytes = Number(process.env.PROXY2_MAX_PAYLOAD_BYTES || 7000);
function decodeHtmlEntities(text) {
    return text
        // 分号可省略:移动站点的图标字体标记常写成 &#xe750 无分号形式
        .replace(/&#(\d+);?/g, (_, value) => String.fromCodePoint(Number(value)))
        .replace(/&#x([0-9a-f]+);?/giu, (_, value) => String.fromCodePoint(Number.parseInt(value, 16)))
        .replace(/&nbsp;/g, ' ')
        .replace(/&lt;/g, '<')
        .replace(/&gt;/g, '>')
        .replace(/&amp;/g, '&')
        .replace(/&quot;/g, '"')
        .replace(/&apos;/g, "'")
        .replace(/&copy;/g, '©')
        .replace(/&reg;/g, '®')
        .replace(/&middot;/g, '·')
        .replace(/&mdash;/g, '—')
        .replace(/&hellip;/g, '…')
        // 私用区码点是站点自带图标字体,目标设备没有对应字形,渲染为乱码,直接去掉
        .replace(/[\u{e000}-\u{f8ff}]/gu, '');
}

function extractHtmlPageModel(htmlBuf, requestUrl) {
    const html = htmlBuf.toString('utf8');

    const titleMatch = html.match(/<title[^>]*>([\s\S]*?)<\/title>/i);
    const title = titleMatch ? decodeHtmlEntities(titleMatch[1]).replace(/\s+/g, ' ').trim() : '';

    // 移除script/style/head块,保留body内纯文本和链接
    let body = html
        .replace(/<script[\s\S]*?<\/script>/gi, '')
        .replace(/<style[\s\S]*?<\/style>/gi, '')
        .replace(/<head[\s\S]*?<\/head>/gi, '')
        .replace(/<!--[\s\S]*?-->/g, '');

    // 保留<a>链接结构(重写为独占一行的规范形式,并在通用剥标签时跳过),其余标签剥离
    body = body
        .replace(/<a\s[^>]*href="([^"]*)"[^>]*>([\s\S]*?)<\/a>/gi, (_, href, text) => {
            const linkText = text.replace(/<[^>]+>/g, '').trim();
            if (!linkText) return '';
            return `\n<a href="${href}">${linkText}</a>\n`;
        })
        .replace(/<br\s*\/?>/gi, '\n')
        .replace(/<\/?(p|div|li|tr|td|h[1-6])[^>]*>/gi, '\n')
        .replace(/<(?!a href=|\/a>)[^>]+>/g, '')
        .replace(/&[^;\s]{1,32};/g, entity => decodeHtmlEntities(entity))
        .replace(/[ \t]+/g, ' ')
        .replace(/\n{3,}/g, '\n\n')
        .trim();

    // 每行提取为 {text, href}: 只有源HTML里真正的<a>行才带链接目标,纯文本行 href=null。
    // 正文只含页面标题与内容行;请求URL不进正文(它只是地址,重复且非页面内容)。
    const items = [];
    const seen = new Set();
    for (const line of [title, ...body.split(/[\r\n]+/u)]) {
        let text = line;
        let href = null;
        const linkMatch = line.trim().match(/^<a href="([^"]*)">([\s\S]*)<\/a>$/u);
        if (linkMatch) {
            // 解析失败就让异常冒出走 /page2 的 404 错误包路径;静默替换成当前页 URL
            // 会把解析 bug 变成"点击后原地不动"的隐蔽故障。
            href = new URL(linkMatch[1], requestUrl).toString();
            text = linkMatch[2];
        }
        text = text.replace(/<[^>]+>/g, '').replace(/\s+/g, ' ').trim();
        if (!text || seen.has(text)) continue;
        seen.add(text);
        items.push({ text, href });
        // 行数只做失控保护;实际内容量由 generateSkyPage 按 payload 字节预算裁剪
        if (items.length >= 64) break;
    }

    return {
        title: title || new URL(requestUrl).host,
        items,
    };
}

function encodeUcs2Be(text) {
    const value = Buffer.alloc(text.length * 2);
    for (let index = 0; index < text.length; index++) {
        value.writeUInt16BE(text.charCodeAt(index), index * 2);
    }
    return value;
}

// 组装 .sky 容器: 10字节头 + 重新计算长度前缀的字符串池 + 显示列表。
// header[3..5]=池条目数, header[8..10]=poolEnd-10 (池解析的真正边界,见格式文档§2)。
function emitSkyContainer(header, entries, displayList) {
    const poolParts = [];
    for (const value of entries) {
        const lengthPrefix = Buffer.alloc(2);
        lengthPrefix.writeUInt16BE(value.length, 0);
        poolParts.push(lengthPrefix, value);
    }
    const pool = Buffer.concat(poolParts);

    const page = Buffer.concat([header, pool, displayList]);
    page.writeUInt16BE(entries.length, 3);
    page.writeUInt16BE(pool.length, 8);
    return page;
}

// ---- .sky 从零生成(不读任何模板文件) ----
// DL 记录帧(反汇编 0xE582/0xDC60 验证, docs/wbrw-sky-format.md §4b-3):
//   [a:be16][flag:be16][flag&0x4000时 e2:be16][op:u8][len:u8][payload]
// flag 低14位 = 本记录id(全页递增); a = 父记录id(父为元素节点时带0x4000位)。
function skyRecord(a, id, e2, op, payload = Buffer.alloc(0)) {
    const head = Buffer.alloc(e2 == null ? 6 : 8);
    head.writeUInt16BE(a, 0);
    head.writeUInt16BE(e2 == null ? id : 0x4000 | id, 2);
    if (e2 != null) head.writeUInt16BE(e2, 4);
    head[head.length - 2] = op;
    head[head.length - 1] = payload.length;
    return Buffer.concat([head, payload]);
}

function skyU16(value) {
    const buffer = Buffer.alloc(2);
    buffer.writeUInt16BE(value, 0);
    return buffer;
}

// 显示列表: 外层 `03 <A:be32>` + 内层 `<1:be32><C=A-8:be32>` + 记录流(字节总和必须精确等于C)。
// 记录结构复刻真机列表页骨架(179603.sky 逐记录解码):
//   根描述符(0x31) → 文档节点(0x4E,e2=3) → 正文节点(0x4E,e2=4) → 头部文本(0x54 绑标题)
//   → 内容节(0x4E,e2=3),其下每项一个行节点(0x4E,e2=3,child型)——独立行节点才会换行,
//     行内 = 0x38 绑caption + 0x54(e2=3) companion。
// 不发射 logo 图片元素: 图片加载永不完成会让标题栏卡在"加载图片"状态。
function generateSkyDisplayList(titleIndex, pairs) {
    const records = [
        skyRecord(0x4001, 2, null, 0x31, Buffer.from('000200000000000000000000', 'hex')),
        skyRecord(0x4001, 2, 3, 0x4e, Buffer.from('010000', 'hex')),
        skyRecord(0x4002, 3, 4, 0x4e, Buffer.from('000000', 'hex')),
        // 页头标题必须用不可聚焦的 0x54 纯文本绑定,不能用 0x38:
        // 焦点移动(game.ext 0x366c)是"全 DOM 几何搜索最近的可聚焦框",可聚焦判据只看
        // op==0x38/0x37(0x275B4)。标题若绑 0x38 会成为几何最靠上的焦点序列首项,而它
        // 标题前一池槽不是链接目标,回车激活(0x1edf8 browser_load_url)无法得到有效URL;
        // 表现为"第一次 DOWN 后回车毫无反应",且焦点序列与可见链接错位一格。
        // 0x54 文本绑定渲染 pool[operand-1](见 wbrw-sky-format.md §4b-5),故 operand=titleIndex+1。
        skyRecord(0x4003, 4, 6, 0x54, Buffer.concat([skyU16(titleIndex + 1), Buffer.from('00000000', 'hex')])),
        skyRecord(0x4001, 5, 3, 0x4e, Buffer.from('010000', 'hex')),
    ];
    const sectionId = 5;
    let id = 6;
    for (const pair of pairs) {
        const rowId = id;
        records.push(skyRecord(0x4000 | sectionId, rowId, 3, 0x4e, Buffer.from('000000', 'hex')));
        const childId = id + 1;
        id += 2;
        // 渲染层判定(反汇编 0x275b4/0x28718 确认):元素可聚焦当且仅当 op==0x38('8');
        // 文本框含 0x38 子元素→链接色+可选中,否则→黑色+不可选。e2 不参与该判定。
        // 链接行 = 0x38(caption,成为可聚焦锚点) + 0x54(e2=3) companion。
        // 真机320个锚点均把目标URL放在caption前一池槽;激活路径按
        // pool[captionIndex-1]取href,并不把后置0x54的操作数当作当前href。
        // 纯文本行 = 仅 0x54(文本run,无0x38子元素)→黑色不可选(真机"©Powered"同为 54(e2=6))。
        // 0x38直接引用caption;0x54 companion/纯文本均使用caption后一槽的操作数,
        // 让文本组装器回退到caption,该槽本身只保留空白占位。
        if (pair.hrefIndex != null) {
            records.push(skyRecord(0x4000 | rowId, childId, null, 0x38,
                Buffer.concat([skyU16(pair.captionIndex), Buffer.from('0000ff', 'hex')])));
            records.push(skyRecord(childId, id, 3, 0x54,
                Buffer.concat([skyU16(pair.companionOperand), Buffer.from('00000000', 'hex')])));
            id += 1;
        } else {
            records.push(skyRecord(0x4000 | rowId, childId, 6, 0x54,
                Buffer.concat([skyU16(pair.textOperand), Buffer.from('00000000', 'hex')])));
        }
    }
    const nodeRegion = Buffer.concat(records);
    // 记录流必须由 0x0F 状态令牌引导(切换到记录分发状态,才会产出文档节点);
    // 其后 be32 X = 从此处到流尾的字节数(含2字节保留字段),真机页面均满足。
    const section = Buffer.alloc(7);
    section[0] = 0x0f;
    section.writeUInt32BE(nodeRegion.length + 2, 1);
    const body = Buffer.concat([section, nodeRegion]);
    const envelope = Buffer.alloc(13);
    envelope[0] = 0x03;
    envelope.writeUInt32BE(body.length + 8, 1);
    envelope.writeUInt32BE(1, 5);
    envelope.writeUInt32BE(body.length, 9);
    return Buffer.concat([envelope, body]);
}

// 从零构造整页。池布局约定(与真机页面一致):
//   [0]=页面URL(ASCII)  [1]=文档标题(UTF-8, DL不引用——浏览器chrome/缓存索引层读取)
//   [2]="./"基准路径    [3]=标题(UCS-2BE, 页面头部元素文本)
//   [4..]链接项 = href(ASCII) + caption(UCS-2BE) + companion占位;
//         纯文本项 = caption(UCS-2BE) + companion占位。
// 可见文本必须UCS-2BE(渲染器无条件按UCS-2BE解码, game.ext 0x1230C);URL/引用保持ASCII。
function generateSkyPage(htmlBuf, requestUrl) {
    const model = extractHtmlPageModel(htmlBuf, requestUrl);
    // 标题已经在池头部槽位显示,正文里不重复渲染
    const items = model.items.filter(item => item.text !== model.title);

    const build = () => {
        const entries = [
            Buffer.from(requestUrl, 'latin1'),
            Buffer.from(model.title, 'utf8'),
            Buffer.from('./', 'latin1'),
            encodeUcs2Be(model.title),
        ];
        const pairs = [];
        for (const item of items) {
            const pair = {
                captionIndex: null,
                hrefIndex: null,
                companionOperand: null,
                textOperand: null,
            };
            if (item.href) {
                // WBRW从0x38 caption的前一池槽取链接目标;按真机顺序先放href。
                pair.hrefIndex = entries.length;
                entries.push(Buffer.from(item.href, 'latin1'));
                pair.captionIndex = entries.length;
                entries.push(encodeUcs2Be(item.text));
                pair.companionOperand = entries.length;
                entries.push(encodeUcs2Be(' '));
            } else {
                pair.captionIndex = entries.length;
                entries.push(encodeUcs2Be(item.text));
                // 纯文本行的 0x54 文本绑定实测渲染 pool[operand-1],故在 caption 后放一个占位
                // 条目,operand 指占位位,渲染回退一位正好命中 caption。链接的 0x38 无此偏移。
                pair.textOperand = entries.length;
                entries.push(encodeUcs2Be(' '));
            }
            pairs.push(pair);
        }
        const header = Buffer.from([0x02, 0x02, 0x01, 0x00, 0x00, 0x02, 0x00, 0x00, 0x00, 0x00]);
        return emitSkyContainer(header, entries, generateSkyDisplayList(3, pairs));
    };

    // 响应payload受WBRW接收缓冲限制,超限时从尾部裁剪内容行
    let page = build();
    while (page.length > proxy2MaxPayloadBytes && items.length > 0) {
        items.pop();
        page = build();
    }
    return page;
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
        const { host, path: urlPath } = extractProxy2RequestUrl(body);

        // help.proxy.51mrp.com 更新检查走内部处理
        if (host === 'help.proxy.51mrp.com') {
            const fakeReq = { url: urlPath, headers: { host } };
            if (handleHelpProxy(fakeReq, res)) return true;
        }

        // tag2含目标主机时，作为WAP网关代理抓取并转换页面
        if (host && host !== 'proxy2.51mrp.com') {
            const targetUrl = buildProxy2TargetUrl(host, urlPath);
            fetchUrl(targetUrl, (err, pageBody, statusCode) => {
                if (err) {
                    console.error('proxy2 page2 fetch error:', err.message);
                    // 抓取失败必须回合法proxy2包: WBRW的响应处理器(game.ext 0x41C60)
                    // 只在收到完整proxy2包后才驱动完成回调——status!=200时置错误码74
                    // 并提示"获取页面错误"。回text/plain不是合法信封,recv层无法组包,
                    // 状态机收不到完成事件,标题栏会永远停在"数据请求"。
                    sendBuffer(res, proxy2Page2ContentType,
                        makeProxy2ResponsePacket(Buffer.alloc(0), [], 404));
                    return;
                }
                console.info('proxy2 page2 fetched %d bytes, status=%d', pageBody.length, statusCode);
                // WAP网关将抓取的HTML按 .sky 结构从零生成为WBRW可渲染页面
                const page = generateSkyPage(pageBody, targetUrl);
                console.info('proxy2 page2 sky payload %d bytes', page.length);
                // PROXY2_PAGE2_WEB_TAG33 控制网页响应的tag33值(默认3=.sky格式)
                const webTag33 = Number(process.env.PROXY2_PAGE2_WEB_TAG33 ?? 3);
                const fields = [{ tag: 33, value: proxy2U8(webTag33) }];
                sendBuffer(res, proxy2Page2ContentType,
                    makeProxy2ResponsePacket(page, fields));
            });
            return true;
        }

        // 无目标主机时返回首页内容(mphome.sky)
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

    // /image: 代理抓取内联图片
    if (req.url === '/image') {
        const { host, path: urlPath } = extractProxy2RequestUrl(body);
        if (host) {
            const targetUrl = buildProxy2TargetUrl(host, urlPath);
            fetchUrl(targetUrl, (err, imageBody, statusCode) => {
                if (err) {
                    console.error('proxy2 image fetch error:', err.message);
                    // 与/page2同理: 必须回合法proxy2包(status!=200),否则图片加载
                    // 状态机等不到recv完成事件
                    sendBuffer(res, proxy2Page2ContentType,
                        makeProxy2ResponsePacket(Buffer.alloc(0), [], 404));
                    return;
                }
                console.info('proxy2 image fetched %d bytes, status=%d', imageBody.length, statusCode);
                sendBuffer(res, proxy2Page2ContentType,
                    makeProxy2ResponsePacket(imageBody, []));
            });
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
