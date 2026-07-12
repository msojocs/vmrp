const http = require('node:http');

const port = Number(process.env.PORT || 8090);
// The proxy serves local MRP fixtures; opt in explicitly before exposing it
// beyond the emulator host because HTTP Host is routing metadata, not auth.
const host = process.env.HOST || '0.0.0.0';
const keepAliveTimeoutMs = Number(process.env.KEEP_ALIVE_TIMEOUT_MS || 5000);

function loadProxyHandler() {
    delete require.cache[require.resolve('./proxy.js')];

    const proxy = require('./proxy.js');
    const handler = typeof proxy === 'function' ? proxy : proxy.handleRequest;
    if (typeof handler !== 'function') {
        throw new TypeError('tool/proxy.js must export a request handler function');
    }
    return handler;
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

const server = http.createServer((req, res) => {
    try {
        const result = loadProxyHandler()(req, res);
        if (result && typeof result.then === 'function') {
            result.catch(error => sendProxyError(res, error));
        }
    } catch (error) {
        sendProxyError(res, error);
    }
});

server.keepAliveTimeout = keepAliveTimeoutMs;

server.listen(port, host, () => {
    console.info(`HTTP server listening on ${host}:${port}`);
});
