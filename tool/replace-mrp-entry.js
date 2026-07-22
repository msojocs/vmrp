const fs = require("node:fs/promises");
const path = require("node:path");
const { gunzipSync, gzipSync } = require("node:zlib");

function parseMrpEntries(mrp) {
  if (mrp.length < 240 || mrp.subarray(0, 4).toString("ascii") !== "MRPG") {
    throw new Error("input is not an MRP package");
  }

  const indexStart = mrp.readUInt32LE(12);
  const indexEnd = mrp.readUInt32LE(4) + 8;
  const declaredLength = mrp.readUInt32LE(8);
  if (declaredLength !== mrp.length) {
    throw new Error(`MRP declares ${declaredLength} bytes but contains ${mrp.length}`);
  }
  if (indexStart < 16 || indexStart > indexEnd || indexEnd > mrp.length) {
    throw new Error("MRP index bounds are invalid");
  }

  const entries = [];
  let cursor = indexStart;
  while (cursor < indexEnd) {
    if (cursor + 4 > indexEnd) throw new Error("truncated MRP index entry");
    const nameLength = mrp.readUInt32LE(cursor);
    cursor += 4;
    if (nameLength < 2 || nameLength > 256 || cursor + nameLength + 12 > indexEnd) {
      throw new Error(`invalid MRP entry name length ${nameLength}`);
    }

    const encodedName = mrp.subarray(cursor, cursor + nameLength);
    if (encodedName[nameLength - 1] !== 0) throw new Error("MRP entry name is not NUL-terminated");
    const name = encodedName.subarray(0, nameLength - 1).toString("latin1");
    cursor += nameLength;
    const offset = mrp.readUInt32LE(cursor);
    const packedLength = mrp.readUInt32LE(cursor + 4);
    cursor += 12;
    if (offset > mrp.length || packedLength > mrp.length - offset) {
      throw new Error(`MRP entry ${name} points outside the package`);
    }
    entries.push({ name, offset, packedLength });
  }
  if (cursor !== indexEnd) throw new Error("MRP index does not end at its declared boundary");
  return entries;
}

function replaceMrpEntry(mrp, entryName, replacement) {
  const entries = parseMrpEntries(mrp);
  const matches = entries.filter(entry => entry.name === entryName);
  if (matches.length !== 1) {
    throw new Error(`expected one MRP entry named ${entryName}, found ${matches.length}`);
  }

  const entry = matches[0];
  const current = mrp.subarray(entry.offset, entry.offset + entry.packedLength);
  const isGzip = current.length >= 2 && current[0] === 0x1f && current[1] === 0x8b;
  const currentDecoded = isGzip ? gunzipSync(current) : current;
  if (currentDecoded.length !== replacement.length) {
    throw new Error(
      `replacement changes decoded length from ${currentDecoded.length} to ${replacement.length}`,
    );
  }

  // Preserve every container offset by allowing only a representation with the
  // same packed length. Deterministic gzip metadata makes the output reproducible.
  const packedReplacement = isGzip
    ? gzipSync(replacement, { level: 9, mtime: 0 })
    : replacement;
  if (packedReplacement.length !== entry.packedLength) {
    throw new Error(
      `replacement changes packed length from ${entry.packedLength} to ${packedReplacement.length}`,
    );
  }

  const result = Buffer.from(mrp);
  packedReplacement.copy(result, entry.offset);
  return result;
}

async function main(args) {
  if (args.length !== 4 || !/^(?:[0-9a-fA-F]{2})+$/.test(args[3])) {
    throw new Error(
      "usage: node tool/replace-mrp-entry.js <input.mrp> <output.mrp> <entry-name> <replacement-hex>",
    );
  }
  const [inputPath, outputPath, entryName, replacementHex] = args;
  const input = await fs.readFile(inputPath);
  const output = replaceMrpEntry(input, entryName, Buffer.from(replacementHex, "hex"));
  await fs.mkdir(path.dirname(outputPath), { recursive: true });
  await fs.writeFile(outputPath, output);
}

if (require.main === module) {
  main(process.argv.slice(2)).catch(error => {
    console.error(error.message);
    process.exitCode = 1;
  });
}

module.exports = { parseMrpEntries, replaceMrpEntry };
