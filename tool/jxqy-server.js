const fs = require("node:fs/promises");
const net = require("node:net");
const path = require("node:path");

const DEFAULT_HOST = "127.0.0.1";
const DEFAULT_PORT = 10005;
// The supplied temp package declares fileid 4001 and is incompatible with the
// wrapper request. Use the tracked, explicitly documented derived fixture.
const DEFAULT_RESOURCE_PATH = "test/fixtures/gjxqy-derived/res_1.mrp";
const ORIGINAL_HOST = "211.155.236.18:10005";
const REQUEST_MAGIC = Buffer.from("8aed9cf37e32b900", "hex");
const ENVELOPE_LENGTH = 44;
const EXPECTED_REQUEST_LENGTH = 161;
const DOWNLOAD_REQUEST = 102003;
const DOWNLOAD_RESPONSE = 202003;
const REQUEST_DESCRIPTOR_COUNT_OFFSET = 120;
const REQUEST_DESCRIPTOR_LENGTH = 13;
const EXPECTED_REQUEST_DESCRIPTOR_TYPES = [2, 11, 12];

function be16(value) {
  const result = Buffer.alloc(2);
  result.writeUInt16BE(value);
  return result;
}

function be32(value) {
  const result = Buffer.alloc(4);
  result.writeUInt32BE(value >>> 0);
  return result;
}

function parseHeaders(buffer, headerEnd) {
  const lines = buffer.subarray(0, headerEnd).toString("latin1").split("\r\n");
  if (lines.shift() !== "POST / HTTP/1.1") {
    throw new Error("expected POST / HTTP/1.1");
  }

  const headers = new Map();
  for (const line of lines) {
    const colon = line.indexOf(":");
    if (colon <= 0) throw new Error(`malformed HTTP header: ${line}`);
    const name = line.slice(0, colon).trim().toLowerCase();
    if (headers.has(name)) throw new Error(`duplicate HTTP header: ${name}`);
    headers.set(name, line.slice(colon + 1).trim());
  }
  if (headers.get("host") !== ORIGINAL_HOST || headers.get("x-online-host") !== ORIGINAL_HOST) {
    throw new Error("request host does not match the archived endpoint");
  }
  if (headers.get("content-type")?.toLowerCase() !== "application/octet-stream") {
    throw new Error("expected application/octet-stream request body");
  }

  const contentLengthText = headers.get("content-length");
  if (!contentLengthText || !/^\d+$/.test(contentLengthText)) {
    throw new Error("request has no valid Content-Length header");
  }
  const contentLength = Number(contentLengthText);
  if (contentLength !== EXPECTED_REQUEST_LENGTH) {
    throw new Error(`unexpected request body length: ${contentLength}`);
  }
  return contentLength;
}

function parseApplicationRequest(body) {
  if (body.length !== EXPECTED_REQUEST_LENGTH || !body.subarray(0, 8).equals(REQUEST_MAGIC)) {
    throw new Error("invalid application envelope");
  }

  const versionAndLength = body.readUInt32BE(8);
  if ((versionAndLength >>> 24) !== 1 || (versionAndLength & 0x00ffffff) !== body.length) {
    throw new Error("application envelope version or total length is invalid");
  }
  // cfunction.ext +0x2D94 builds this legacy protocol variant with fixed
  // channel/version words before the request's dynamic session fields.
  if (body.readUInt16BE(12) !== 100 || body.readUInt16BE(14) !== 301) {
    throw new Error("unsupported application envelope variant");
  }
  if (body.readUInt32BE(36) !== DOWNLOAD_REQUEST) {
    throw new Error("unsupported application command");
  }
  if (body.readUInt32BE(40) !== body.length - ENVELOPE_LENGTH) {
    throw new Error("application payload length does not match envelope");
  }

  const descriptorCount = body.readUInt16BE(REQUEST_DESCRIPTOR_COUNT_OFFSET);
  const descriptorsEnd = REQUEST_DESCRIPTOR_COUNT_OFFSET + 2
    + descriptorCount * REQUEST_DESCRIPTOR_LENGTH;
  if (descriptorCount !== EXPECTED_REQUEST_DESCRIPTOR_TYPES.length || descriptorsEnd !== body.length) {
    throw new Error("unexpected request descriptor section");
  }

  const descriptors = [];
  for (let index = 0; index < descriptorCount; index += 1) {
    const offset = REQUEST_DESCRIPTOR_COUNT_OFFSET + 2 + index * REQUEST_DESCRIPTOR_LENGTH;
    const descriptor = {
      type: body[offset],
      value1: body.readUInt32BE(offset + 1),
      value2: body.readUInt32BE(offset + 5),
      value3: body.readUInt32BE(offset + 9),
    };
    if (descriptor.type !== EXPECTED_REQUEST_DESCRIPTOR_TYPES[index]) {
      throw new Error(`unexpected request descriptor type: ${descriptor.type}`);
    }
    descriptors.push(descriptor);
  }

  // cfunction.ext +0x2EB8 serializes the request records, and +0x21AA
  // requires the response's value2 to echo the requested type-2 resource ID.
  return { body, resourceId: descriptors[0].value2 };
}

function tryParseRequest(buffer) {
  const headerEnd = buffer.indexOf("\r\n\r\n");
  if (headerEnd < 0) {
    if (buffer.length > 16 * 1024) throw new Error("HTTP headers exceed 16 KiB");
    return undefined;
  }

  const contentLength = parseHeaders(buffer, headerEnd);
  const requestLength = headerEnd + 4 + contentLength;
  if (buffer.length < requestLength) return undefined;
  if (buffer.length !== requestLength) throw new Error("request has trailing bytes");
  return parseApplicationRequest(buffer.subarray(headerEnd + 4));
}

function buildResponseFrames(request, resource) {
  // cfunction.ext +0x2CE8 scans for a NUL after the four-byte status, while
  // +0x2FC4 decodes those first four bytes as a big-endian HTTP-like status.
  const status = Buffer.concat([be32(200), Buffer.from([0])]);
  const count = be16(1);
  // cfunction.ext +0x2F70 consumes this 31-byte descriptor before state 9
  // streams exactly the declared resource length into the selected local file.
  const descriptor = Buffer.concat([
    Buffer.from([2]),
    be32(0),
    be32(request.resourceId),
    be32(200),
    be32(0),
    Buffer.from("res_1.mrp\0", "ascii"),
    be32(resource.length),
  ]);
  const payloadLength = status.length + count.length + descriptor.length + resource.length;
  if (ENVELOPE_LENGTH + payloadLength > 0x00ffffff) {
    throw new Error("response is too large for the 24-bit envelope length");
  }

  const envelope = Buffer.alloc(ENVELOPE_LENGTH);
  // The first 36 bytes contain protocol version, message ID, and session data.
  // Echoing them from the request preserves the archived server's transaction.
  request.body.copy(envelope, 0, 0, 36);
  envelope.writeUInt32BE((0x01000000 | ENVELOPE_LENGTH + payloadLength) >>> 0, 8);
  envelope.writeUInt32BE(DOWNLOAD_RESPONSE, 36);
  envelope.writeUInt32BE(payloadLength, 40);

  const bodyLength = ENVELOPE_LENGTH + payloadLength;
  const http = Buffer.from(
    `HTTP/1.1 200 OK\r\nContent-Type: application/octet-stream\r\nContent-Length: ${bodyLength}\r\nConnection: close\r\n\r\n`,
    "ascii",
  );
  return { http, envelope, status, count, descriptor, resource };
}

async function sendResponse(socket, frames) {
  // cfunction.ext +0x506C4..+0x50750 repeatedly invokes the current state's
  // length callback and advances a ring-buffer read pointer.  One TCP read can
  // therefore contain every logical frame without relying on packet timing.
  const response = Buffer.concat([
    frames.http,
    frames.envelope,
    frames.status,
    frames.count,
    frames.descriptor,
    frames.resource,
  ]);
  await new Promise((resolve, reject) => {
    if (socket.destroyed) {
      reject(new Error("client closed before the response was sent"));
      return;
    }
    socket.end(response, error => error ? reject(error) : resolve());
  });
}

/**
 * Start the archived gjxqy download endpoint and return an awaitable lifecycle.
 * The server handles exactly one validated download transaction; protocol
 * errors reject `completed` instead of falling back to a guessed response.
 */
async function startGjxqyServer(options = {}) {
  const host = options.host ?? DEFAULT_HOST;
  const port = options.port ?? DEFAULT_PORT;
  const resourcePath = path.resolve(options.resourcePath ?? DEFAULT_RESOURCE_PATH);

  const resource = await fs.readFile(resourcePath);
  if (resource.length < 16 || resource.subarray(0, 4).toString("ascii") !== "MRPG") {
    throw new Error(`resource is not an MRP package: ${resourcePath}`);
  }
  // The client streams the declared representation length into its installer.
  // Reject a truncated or overlong fixture before accepting any transaction.
  if (resource.readUInt32LE(8) !== resource.length) {
    throw new Error(`resource length does not match its MRP header: ${resourcePath}`);
  }

  const sockets = new Set();
  let handled = false;
  let closed = false;
  let completedPending = true;
  let settleCompleted;
  let rejectCompleted;
  const completed = new Promise((resolve, reject) => {
    settleCompleted = resolve;
    rejectCompleted = reject;
  });
  // A CLI caller may only observe stderr; attaching a handler prevents a
  // protocol failure from becoming an unrelated unhandled-rejection warning.
  completed.catch(() => {});
  const complete = result => {
    if (!completedPending) return;
    completedPending = false;
    settleCompleted(result);
  };
  const fail = error => {
    if (!completedPending) return;
    completedPending = false;
    rejectCompleted(error);
  };

  const server = net.createServer(socket => {
    sockets.add(socket);
    socket.setNoDelay(true);
    socket.once("close", () => sockets.delete(socket));
    let requestBuffer = Buffer.alloc(0);
    let replying = false;

    socket.on("data", chunk => {
      // Once another socket owns the validated transaction, isolate this
      // connection without changing the owner's completion promise.
      if (handled && !replying) {
        socket.destroy(new Error("server accepts exactly one download request"));
        return;
      }
      if (replying) {
        fail(new Error("client sent data after the complete request"));
        socket.destroy();
        return;
      }
      requestBuffer = Buffer.concat([requestBuffer, chunk]);
      try {
        const request = tryParseRequest(requestBuffer);
        if (!request) return;
        // Claim the one-shot transaction only after a complete request has
        // passed every protocol check.  An empty TCP health probe must not
        // consume the archived endpoint before the game connects.
        handled = true;
        replying = true;
        socket.pause();
        const frames = buildResponseFrames(request, resource);
        sendResponse(socket, frames).then(() => {
          complete({
            requestLength: request.body.length,
            resourceLength: resource.length,
            responseBodyLength: ENVELOPE_LENGTH + frames.status.length + frames.count.length
              + frames.descriptor.length + frames.resource.length,
            frameLengths: [
              frames.http.length,
              frames.envelope.length,
              frames.status.length,
              frames.count.length,
              frames.descriptor.length,
              frames.resource.length,
            ],
          });
        }, fail);
      } catch (error) {
        handled = true;
        fail(error);
        socket.destroy();
      }
    });
    socket.once("end", () => {
      // A nonempty prefix followed by EOF can never become a valid request;
      // reject it explicitly instead of leaving `completed` pending forever.
      if (!replying && !handled && requestBuffer.length > 0) {
        handled = true;
        fail(new Error("client ended before sending a complete request"));
      }
    });
    socket.once("error", error => {
      if (replying || (!handled && requestBuffer.length > 0)) fail(error);
    });
  });

  await new Promise((resolve, reject) => {
    const onError = error => reject(error);
    server.once("error", onError);
    server.listen(port, host, () => {
      server.off("error", onError);
      resolve();
    });
  });
  server.on("error", error => fail(error));
  const address = server.address();
  if (!address || typeof address === "string") {
    await new Promise(resolve => server.close(resolve));
    throw new Error("server did not expose a TCP listening address");
  }

  return {
    host,
    port: address.port,
    resourcePath,
    completed,
    async close() {
      if (closed) return;
      closed = true;
      fail(new Error("server closed before completing a download request"));
      for (const socket of sockets) socket.destroy();
      await new Promise((resolve, reject) => {
        server.close(error => error ? reject(error) : resolve());
      });
    },
  };
}

async function runCli() {
  const instance = await startGjxqyServer({
    host: process.env.GJXQY_SERVER_HOST || DEFAULT_HOST,
    port: Number(process.env.GJXQY_SERVER_PORT || DEFAULT_PORT),
    resourcePath: process.env.GJXQY_RESOURCE_PATH || DEFAULT_RESOURCE_PATH,
  });
  console.log(`[gjxqy-server] listening on ${instance.host}:${instance.port}; resource=${instance.resourcePath}`);

  const close = async () => {
    await instance.close();
  };
  process.once("SIGINT", close);
  process.once("SIGTERM", close);
  try {
    const result = await instance.completed;
    console.log(`[gjxqy-server] sent ${result.resourceLength} resource bytes`);
  } catch (error) {
    console.error(`[gjxqy-server] ${error.message}`);
    process.exitCode = 1;
  } finally {
    // The CLI is a one-transaction fixture server, so release the listening
    // socket after either a completed response or a protocol failure.
    await instance.close();
  }
}

module.exports = { startGjxqyServer };

if (require.main === module) {
  runCli().catch(error => {
    console.error(`[gjxqy-server] ${error.message}`);
    process.exitCode = 1;
  });
}
