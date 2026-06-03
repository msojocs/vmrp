// tool/skymobi-pay-server.js
//
// netpay.mrp（skymobi 付费 SDK）在启动时会向 rop.skymobiapp.com:80 发起
// HTTP POST /payOneAsTlv，body 是一段 TLV（Type-Length-Value）二进制，携带
// 设备信息(IMEI/IMSI)、应用信息(包名/版本)、时间戳和 MD5 签名，用于校验
// 付费/授权。真机上服务器返回校验结果，netpay 据此放行游戏。该服务器已下线，
// gxdzc 等游戏因此卡在等待回复处（见 docs 分析）。
//
// 本脚本是一个本地假服务器。DNS 解析已在模拟器内部完成重定向（见
// src/network.c 的 my_getHostByNameSync：rop.skymobiapp.com -> 127.0.0.1，
// 无需改 /etc/hosts），netpay 的请求会直接落到本机。服务器做两件事：
//   1) 完整解析并打印请求的 HTTP 头与 TLV body —— 用于逆向 netpay 实际发送
//      的字段，进而推导出能让它放行的“成功”响应格式。
//   2) 返回一段可配置的 TLV 响应（默认是 best-effort 的“成功”占位，需要根据
//      netpay 对响应的解析行为继续打磨）。
//
// 用法：
//   sudo PORT=80 node tool/skymobi-pay-server.js
//   # netpay 写死连 80 端口。若不想用 root，可改连别的端口：先用
//   #   VMRP_PAY_HOST=127.0.0.1 启动 vmrp（IP 重定向），再配合端口转发，
//   #   例如  socat TCP-LISTEN:80,fork,reuseaddr TCP:127.0.0.1:8080
//   #   然后  PORT=8080 node tool/skymobi-pay-server.js
//
// 环境变量：
//   PORT            监听端口，默认 80（skymobi 请求里写死的是 80 端口）
//   RESP_TLV_FILE   指定一个文件，其内容作为响应 body 原样返回（覆盖默认响应）
//   RESP_HTTP_FILE  指定一个文件，其内容作为完整 HTTP 响应原样返回（连头部）
//   LOG_RAW         设为 "1" 时额外打印每个连接收到的原始字节 hex
//
// 依赖：仅 Node 内置 net 模块（与 tool/applist-server.js 同风格）。

const fs = require('fs');
const net = require('net');

const PORT = Number(process.env.PORT || 80);

/* ---------- TLV 编解码工具 ---------- */
/* skymobi payOneAsTlv 记录格式：4 字节 type(大端) + 4 字节 length(大端) +
 * length 字节 value。例如 0000045c 00000005 "gghjt"、00000452 00000006 "PREREG"。 */

function u32be(value) {
    const buf = Buffer.alloc(4);
    buf.writeUInt32BE(value >>> 0, 0);
    return buf;
}

function tlv(type, value) {
    const v = Buffer.isBuffer(value) ? value : Buffer.from(String(value), 'ascii');
    return Buffer.concat([u32be(type), u32be(v.length), v]);
}

/* 在 body 里从某个 offset 开始按 TLV 连续扫描。正常情况下 TLV 流从 body 起点
 * (offset 0) 开始，这里做容错扫描（选记录数最多的对齐方式），纯日志用途。 */
function scanTlv(body) {
    let best = { start: -1, records: [], consumed: 0, end: 0 };
    for (let start = 0; start < Math.min(body.length, 64); start++) {
        const records = [];
        let off = start;
        while (off + 8 <= body.length) {
            const type = body.readUInt32BE(off);
            const len = body.readUInt32BE(off + 4);
            // 长度异常则认为这里不是合法 TLV 流
            if (len > body.length - (off + 8)) break;
            const value = body.slice(off + 8, off + 8 + len);
            records.push({ type, len, value });
            off += 8 + len;
        }
        // 正确对齐会“恰好消费到 body 末尾且记录数最多”；优先选消费到末尾的，
        // 其次选记录数最多的。
        const reachedEnd = off === body.length;
        const better = reachedEnd
            ? (best.end !== body.length || records.length > best.records.length)
            : (best.end !== body.length && records.length > best.records.length);
        if (better) best = { start, records, consumed: off - start, end: off };
    }
    return best;
}

function preview(buf, max = 64) {
    const slice = buf.slice(0, max);
    const hex = slice.toString('hex').replace(/(..)/g, '$1 ').trim();
    const ascii = slice.toString('latin1').replace(/[^\x20-\x7e]/g, '.');
    return `${hex}${buf.length > max ? ' …' : ''}  |${ascii}|`;
}

/* ---------- HTTP 请求解析 ---------- */

function parseHttp(buf) {
    const headerEnd = buf.indexOf('\r\n\r\n');
    if (headerEnd < 0) return null; // 头部还没收全
    const head = buf.slice(0, headerEnd).toString('latin1');
    const lines = head.split('\r\n');
    const [method, path, version] = lines[0].split(' ');
    const headers = {};
    for (let i = 1; i < lines.length; i++) {
        const idx = lines[i].indexOf(':');
        if (idx > 0) headers[lines[i].slice(0, idx).trim().toLowerCase()] = lines[i].slice(idx + 1).trim();
    }
    const contentLen = Number(headers['content-length'] || 0);
    const bodyStart = headerEnd + 4;
    if (buf.length < bodyStart + contentLen) return null; // body 还没收全
    const body = buf.slice(bodyStart, bodyStart + contentLen);
    return { method, path, version, headers, body, consumed: bodyStart + contentLen };
}

/* ---------- 默认响应 ---------- */
/* 注意：skymobi payOneAsTlv 的“成功”响应 TLV 模式尚未确证。下面是 best-effort
 * 占位：HTTP 200 + 一段最小 TLV（一个常见的“结果码=0/成功”字段）。需要依据
 * netpay 对响应的实际解析行为（用本服务器的请求日志反推）继续调整字段类型与值。 */
function buildDefaultResponse(req) {
    if (process.env.RESP_HTTP_FILE) {
        return fs.readFileSync(process.env.RESP_HTTP_FILE);
    }
    let body;
    if (process.env.RESP_TLV_FILE) {
        body = fs.readFileSync(process.env.RESP_TLV_FILE);
    } else {
        // 占位“成功”响应：result(0x044f?)=0 等，后续按 netpay 解析行为修订
        body = Buffer.concat([
            tlv(0x03f1, '000000006'),                          // 回显事务/序列号占位
            tlv(0x044f, Buffer.from([0x00, 0x00, 0x00, 0x00])),// result code 占位 = 0(成功)
        ]);
    }
    const header = Buffer.from(
        'HTTP/1.1 200 OK\r\n' +
        'Content-Type: application/x-tar\r\n' +
        `Content-Length: ${body.length}\r\n` +
        'Connection: close\r\n' +
        '\r\n', 'latin1');
    return Buffer.concat([header, body]);
}

/* ---------- 服务器 ---------- */

let connSeq = 0;

const server = net.createServer((socket) => {
    const id = ++connSeq;
    const peer = `${socket.remoteAddress}:${socket.remotePort}`;
    console.log(`\n[#${id}] connection from ${peer}`);
    let buf = Buffer.alloc(0);

    socket.on('data', (chunk) => {
        buf = Buffer.concat([buf, chunk]);
        if (process.env.LOG_RAW === '1') {
            console.log(`[#${id}] raw +${chunk.length}B: ${preview(chunk, 96)}`);
        }
        const req = parseHttp(buf);
        if (!req) return; // 还没收全，继续等

        console.log(`[#${id}] ${req.method} ${req.path} ${req.version}`);
        for (const [k, v] of Object.entries(req.headers)) console.log(`[#${id}]   ${k}: ${v}`);
        console.log(`[#${id}] body ${req.body.length}B: ${preview(req.body, 96)}`);

        // 解析并打印 TLV body（容错扫描）
        const { start, records, end } = scanTlv(req.body);
        if (records.length) {
            console.log(`[#${id}] TLV stream @body+${start}..${end} (${records.length} records, ${req.body.length - (end || 0)}B trailing):`);
            for (const r of records) {
                const ascii = r.value.toString('latin1').replace(/[^\x20-\x7e]/g, '.');
                console.log(`[#${id}]   type=0x${r.type.toString(16).padStart(4, '0')} len=${r.len} value=${r.value.toString('hex')}  |${ascii}|`);
            }
        } else {
            console.log(`[#${id}] (no TLV stream found — full body hex)`);
            console.log(req.body.toString('hex'));
        }

        const resp = buildDefaultResponse(req);
        console.log(`[#${id}] -> reply ${resp.length}B`);
        socket.end(resp);
    });

    socket.on('error', (e) => console.log(`[#${id}] socket error: ${e.message}`));
    socket.on('close', () => console.log(`[#${id}] closed`));
});

server.on('error', (e) => {
    if (e.code === 'EACCES') {
        console.error(`监听 ${PORT} 失败：端口 <1024 需要 root。请用  sudo PORT=${PORT} node tool/skymobi-pay-server.js`);
    } else if (e.code === 'EADDRINUSE') {
        console.error(`端口 ${PORT} 已被占用。`);
    } else {
        console.error(`server error: ${e.message}`);
    }
    process.exit(1);
});

server.listen(PORT, () => {
    console.log(`skymobi fake pay-server listening on 0.0.0.0:${PORT}`);
    console.log(`DNS 已在 src/network.c 内重定向 rop.skymobiapp.com -> 127.0.0.1，`);
    console.log(`直接启动 vmrp 即可，netpay 的 POST /payOneAsTlv 会落到这里。`);
});
