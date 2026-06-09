const fs = require('fs');
const net = require('net');

// tcpip.mr 里 dltype=3 默认连 13229；dltype=2 会连 13229。
const port = Number(process.env.PORT || 13229);
const tunnelAck = 'HTTP/1.1 200 Connection established\r\n\r\n';
const defaultDownloadPath = 'temp/test.mrp';

function u32le(value) {
    const buf = Buffer.alloc(4);
    buf.writeUInt32LE(value >>> 0, 0);
    return buf;
}

function fixedString(value, len) {
    const buf = Buffer.alloc(len);
    Buffer.from(value, 'ascii').copy(buf, 0, 0, len - 1);
    return buf;
}

function parseHandshake(line) {
    const result = {};
    const body = line.startsWith('set ') ? line.slice(4) : line;
    for (const kv of body.split(',')) {
        const eq = kv.indexOf('=');
        if (eq !== -1) {
            result[kv.slice(0, eq).trim()] = kv.slice(eq + 1).trim();
        }
    }
    return result;
}

function cleanMrpName(name) {
    const trimmed = String(name || '').trim().replace(/\\/g, '/').split('/').pop();
    const safe = trimmed.replace(/[^0-9A-Za-z_.-]/g, '');
    return safe || 'test';
}

function getDownloadTarget(fields) {
    const raw = process.env.DOWNLOAD_TARGET || fields.dlname || 'test';
    const name = cleanMrpName(raw);
    return name.toLowerCase().endsWith('.mrp') ? name : `${name}.mrp`;
}

function getDownloadFile() {
    return fs.readFileSync(process.env.DOWNLOAD_MRP_FILE || defaultDownloadPath);
}

function escapeMrSourceString(value) {
    return String(value).replace(/\\/g, '\\\\').replace(/"/g, '\\"');
}

function getDownloadInitPayload(fields) {
    const target = getDownloadTarget(fields);
    const total = getDownloadFile().length;
    const safeTarget = escapeMrSourceString(target);

    return Buffer.from(
        [
            `print("download init: ${safeTarget} bytes=${total}")`,
            `dl_name = "${safeTarget}"`,
            `dl_total = ${total}`,
            'dl_written = 0',
            'dl_f = nil',
            'dl_ok = 0',
            '',
            'def progress(data)',
            '  p = tonumber(data)',
            '  if p == nil then',
            '    p = 0',
            '  end',
            '  if g_dialog then',
            '    if g_dialog.update then',
            '      g_dialog.update(g_dialog, nil, data .. "%", p)',
            '    end',
            '  end',
            '  if win then',
            '    if win.refresh then',
            '      win.refresh()',
            '    end',
            '  end',
            'end',
            '',
            'def dl_begin()',
            '  if sys then',
            '    if sys.rm then',
            '      sys.rm(dl_name)',
            '    end',
            '  end',
            '  dl_f = file.open(dl_name, 10)',
            '  if dl_f then',
            '    dl_ok = 1',
            '  else',
            '    dl_ok = 0',
            '    if net then',
            '      if net.fail then',
            '        net.fail()',
            '      end',
            '    end',
            '  end',
            'end',
            '',
            'def dl_file(data)',
            '  if dl_f then',
            '    dl_f.write(dl_f, data)',
            '    dl_written = dl_written + string.len(data)',
            '  end',
            'end',
            '',
            'def dl_done(data)',
            '  if dl_f then',
            '    dl_f.close(dl_f)',
            '  end',
            '  if g_dialog then',
            '    if g_dialog.update then',
            '      g_dialog.update(g_dialog, nil, "100%", 100)',
            '    end',
            '  end',
            '  if win then',
            '    if win.refresh then',
            '      win.refresh()',
            '    end',
            '  end',
            '  if dl_ok == 1 then',
            '    if dl_written == dl_total then',
            '      if recreateApplist then',
            '        recreateApplist(1)',
            '      else',
            '        if net then',
            '          if net.successexit then',
            '            net.successexit()',
            '          end',
            '        end',
            '      end',
            '    else',
            '      if net then',
            '        if net.fail then',
            '          net.fail()',
            '        end',
            '      end',
            '    end',
            '  else',
            '    if net then',
            '      if net.fail then',
            '        net.fail()',
            '      end',
            '    end',
            '  end',
            'end',
            '',
            'cmd.progress = progress',
            'cmd.file = dl_file',
            'cmd.done = dl_done',
            'dl_begin()',
        ].join('\n'),
        'utf8',
    );
}
function escapeMrString(buf) {
    let out = '';
    for (const byte of buf) {
        out += `\\${byte.toString(10).padStart(3, '0')}`;
    }
    return out;
}

function chunkBuffer(buf, size) {
    const chunks = [];
    for (let offset = 0; offset < buf.length; offset += size) {
        chunks.push(buf.slice(offset, offset + size));
    }
    return chunks;
}

function hasCustomCodePayload() {
    return Boolean(process.env.CODE_PAYLOAD_FILE || process.env.CODE_PAYLOAD_HEX || process.env.CODE_PAYLOAD);
}

function getCodePayload(fields) {
    if (process.env.CODE_PAYLOAD_FILE) {
        return fs.readFileSync(process.env.CODE_PAYLOAD_FILE);
    }
    if (process.env.CODE_PAYLOAD_HEX) {
        return Buffer.from(process.env.CODE_PAYLOAD_HEX.replace(/\s+/g, ''), 'hex');
    }
    if (process.env.CODE_PAYLOAD) {
        return Buffer.from(process.env.CODE_PAYLOAD, 'utf8');
    }
    if (String(fields.dltype) === '2') {
        return getDownloadInitPayload(fields);
    }

    // tcpip.mr 的 cmd 表只在本文件里注册了 cmd.code。
    // 真实服务端的 code 数据会经 TestCom1(300, data) 处理后 _loads(data) 执行。
    // 默认 payload 用来确认分发路径，并注册 progress 命令。
    return Buffer.from(
        [
            `print("code frame received: dltype=${fields.dltype}, listver=${fields.listver}")`,
            'def progress(data)',
            '  p = tonumber(data)',
            '  if p == nil then',
            '    p = 0',
            '  end',
            '  if g_dialog then',
            '    if g_dialog.update then',
            '      g_dialog.update(g_dialog, nil, data .. "%", p)',
            '    end',
            '  end',
            '  if win then',
            '    if win.refresh then',
            '      win.refresh()',
            '    end',
            '  end',
            'end',
            'cmd.progress = progress',
        ].join('\n'),
        'utf8',
    );
}



// CmdRun 解析格式：
//   total = byte(recvbuf, cmdpos) * 256 + byte(recvbuf, cmdpos + 1) + 2
//   space = string.find(recvbuf, " ", cmdpos + 2)
//   thiscmd = string.sub(recvbuf, cmdpos + 2, space - 1)
//   data = string.sub(recvbuf, space + 1, cmdpos + total - 1)
function makeFrame(command, data) {
    const commandBuf = Buffer.from(`${command} `, 'ascii');
    const dataBuf = Buffer.isBuffer(data) ? data : Buffer.from(String(data), 'utf8');
    const len = commandBuf.length + dataBuf.length;

    if (len > 0xffff) {
        throw new RangeError(`frame too large: ${len} bytes`);
    }

    const frame = Buffer.alloc(2 + len);
    frame.writeUInt16BE(len, 0);
    commandBuf.copy(frame, 2);
    dataBuf.copy(frame, 2 + commandBuf.length);
    return frame;
}

function preview(buf, max = 96) {
    const head = buf.slice(0, max);
    const text = head.toString('utf8').replace(/\r/g, '\\r').replace(/\n/g, '\\n');
    return `${buf.length} bytes hex=${head.toString('hex')} text="${text}"`;
}

function sendFrame(socket, command, data) {
    const frame = makeFrame(command, data);
    socket.write(frame);
    const payloadLen = frame.length - 2;
    console.log(`[SEND FRAME] ${command} payload=${payloadLen} total=${frame.length}`);
}

function sendDownloadFrames(socket, fields, startDelay) {
    const filePath = process.env.DOWNLOAD_MRP_FILE || defaultDownloadPath;
    const file = getDownloadFile();
    const target = getDownloadTarget(fields);
    const requestedChunkSize = Number(process.env.DOWNLOAD_CHUNK_SIZE || 8192);
    const requestedChunkDelay = Number(process.env.DOWNLOAD_CHUNK_DELAY_MS || 40);
    const chunkSize = Number.isFinite(requestedChunkSize)
        ? Math.max(1, Math.min(0xffff - Buffer.byteLength('file ', 'ascii'), Math.floor(requestedChunkSize)))
        : 8192;
    const chunkDelay = Number.isFinite(requestedChunkDelay)
        ? Math.max(0, Math.floor(requestedChunkDelay))
        : 40;
    const chunks = chunkBuffer(file, chunkSize);
    let lastPercent = -1;

    console.log(`[DOWNLOAD] ${filePath} -> ${target} size=${file.length} chunks=${chunks.length} chunkSize=${chunkSize}`);

    chunks.forEach((chunk, index) => {
        setTimeout(() => {
            if (socket.destroyed) return;

            sendFrame(socket, 'file', chunk);

            const percent = Math.min(99, Math.floor(((index + 1) * 100) / chunks.length));
            if (percent !== lastPercent) {
                lastPercent = percent;
                sendFrame(socket, 'progress', String(percent));
            }
        }, startDelay + chunkDelay * index);
    });

    setTimeout(() => {
        if (!socket.destroyed) {
            sendFrame(socket, 'progress', '100');
            sendFrame(socket, 'done', '1');
        }
    }, startDelay + chunkDelay * chunks.length);
}

function sendInitialResponse(socket, fields) {
    const payload = getCodePayload(fields);
    const frameDelay = Number(process.env.FRAME_DELAY_MS || 300);

    // 稍微延迟，避免代理 ACK、握手响应和业务帧被客户端一次 receive 到。
    // tcpip.mr 的 CMWAP 首包处理会先找 "\r\n\r\n"，业务帧放后续包最稳。
    setTimeout(() => {
        if (!socket.destroyed) {
            sendFrame(socket, 'code', payload);
        }
    }, frameDelay);

    if (hasCustomCodePayload()) {
        return;
    }

    if (String(fields.dltype) === '2') {
        sendDownloadFrames(socket, fields, frameDelay + Number(process.env.DOWNLOAD_START_DELAY_MS || 300));
        return;
    }

}

net.createServer((socket) => {
    console.log(`\n[CONNECT] ${socket.remoteAddress}:${socket.remotePort}`);
    socket.setNoDelay(true);

    // detect: 自动识别 CMWAP CONNECT 或直接 set 握手。
    // handshake: 等待 "set dltype=..." 行。
    // data: 握手后记录客户端额外数据。
    let state = 'detect';
    let buf = Buffer.alloc(0);

    function handleHandshakeLine(line) {
        console.log(`[HANDSHAKE] ${line}`);

        if (!line.startsWith('set ')) {
            console.error(`[ERROR] 非法握手包，期望 "set ..."，收到: "${line}"`);
            socket.end();
            return false;
        }

        const fields = parseHandshake(line);
        const required = ['dltype', 'listver', 'scrsize', 'IMEI', 'IMSI'];
        const missing = required.filter(k => !(k in fields));
        if (missing.length > 0) {
            console.error(`[ERROR] 缺少字段: ${missing.join(', ')}`);
            socket.end();
            return false;
        }

        console.log('[FIELDS]', fields);
        console.log(`[OK] 握手成功 dltype=${fields.dltype} listver=${fields.listver} scrsize=${fields.scrsize}`);
        state = 'data';
        sendInitialResponse(socket, fields);
        return true;
    }

    function pump() {
        while (!socket.destroyed) {
            if (state === 'detect') {
                const head = buf.slice(0, Math.min(buf.length, 16)).toString('ascii');

                if (head.startsWith('CONNECT ')) {
                    const eoh = buf.indexOf('\r\n\r\n');
                    if (eoh === -1) return;

                    const firstLine = buf.slice(0, buf.indexOf('\r\n')).toString('utf8');
                    console.log(`[TUNNEL REQ] ${firstLine}`);
                    socket.write(tunnelAck);
                    console.log(`[TUNNEL ACK] ${tunnelAck.trim().replace(/\r\n/g, ' | ')}`);
                    buf = buf.slice(eoh + 4);
                    state = 'handshake';
                    continue;
                }

                if (head.startsWith('set ')) {
                    state = 'handshake';
                    continue;
                }

                if (buf.length < 8) return;

                console.error(`[ERROR] 未识别的首包: ${preview(buf)}`);
                socket.end();
                return;
            }

            if (state === 'handshake') {
                const nl = buf.indexOf('\n');
                if (nl === -1) return;

                const line = buf.slice(0, nl).toString('utf8').replace(/\r$/, '').trim();
                buf = buf.slice(nl + 1);
                if (line.length === 0) continue;

                if (!handleHandshakeLine(line)) return;
                continue;
            }

            if (state === 'data') {
                if (buf.length > 0) {
                    console.log(`[CLIENT DATA] ${preview(buf)}`);
                    buf = Buffer.alloc(0);
                }
                return;
            }
        }
    }

    socket.on('data', (chunk) => {
        buf = Buffer.concat([buf, chunk]);
        pump();
    });

    socket.on('end', () => console.log('[DISCONNECT]'));
    socket.on('error', err => console.error(`[SOCKET ERROR] ${err.message}`));

}).listen(port, () => {
    console.log(`TCP server running at localhost:${port}`);
    console.log('Use CODE_PAYLOAD_FILE, CODE_PAYLOAD_HEX, or CODE_PAYLOAD to replace the default code payload.');
});
