# Script API

```javascript
Script.requireExtension("SHA256");

SHA256;                      // plain object holding the functions
SHA256.hash(data);           // String, 64 lowercase hex characters
SHA256.hashToBuffer(data);   // Buffer, 32 bytes
SHA256.fileHash(filename);   // String, 64 lowercase hex characters, or undefined
```

`SHA256` is a global variable created by the extension
(`var SHA256={};`) with three native functions. It is not a constructor and
has no state: every call starts a new SHA-256 computation and finishes it
before returning. There is no incremental (`update` / `digest`) interface; to
hash data that arrives in pieces, collect it first, or write it to a file and
use `fileHash`.

## Loading

```javascript
Script.requireExtension("SHA256");
```

- Loads `quantum-script--sha256` (external DLL, or an internal extension
  registered by the host) and runs its `initExecutive`.
- `initExecutive` runs `Script.requireExtension("Buffer")` first, so `Buffer`
  is available afterwards, then defines `SHA256` and its functions.
- Requiring it again does nothing.
- The extension is public, versioned and listed by `Script.getExtensionList()`
  with the name `SHA256`.

## How the argument is converted

`hash` and `hashToBuffer` take any value and hash the bytes of
`value.toString()` (the C++ `Variable::toString()`):

| Argument | Bytes hashed | Example |
|----------|--------------|---------|
| `String` | its bytes (UTF-8), zero bytes included | `SHA256.hash("ă")` hashes the 2 bytes `c4 83` |
| `Buffer` | its first `length` bytes, unchanged | `SHA256.hash(Buffer.fromHex("00ff00"))` hashes 3 bytes |
| `Number` | its text | `SHA256.hash(123) == SHA256.hash("123")`, `0.1` hashes `"0.1"` |
| `Boolean` | `"true"` / `"false"` | |
| `null` | `"null"` | **not** the empty input |
| `undefined`, missing argument | `"undefined"` | `SHA256.hash()` is `eb045d78...`, **not** `e3b0c442...` |
| `Array` | its text | `[1,2]` hashes `"1,2"` |
| `Object` | its text | `{}` hashes `"Object"` |

There is no character set conversion: a string is hashed exactly as stored.
Scripts, `Shell.fileGetContents` and most extensions produce UTF-8, which
matches what other tools hash for the same text. The digest of the empty
input is obtained with `SHA256.hash("")` or an empty buffer.

Extra arguments are ignored.

## SHA256.hash(data)

Returns the SHA-256 digest of `data` (converted as above) as a `String` of
**64 lowercase hexadecimal characters**, the same text as `sha256sum`,
`openssl dgst -sha256`, `Get-FileHash` (after lowercasing) or Python's
`hashlib.sha256(...).hexdigest()`.

```javascript
SHA256.hash("");      // "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
SHA256.hash("abc");   // "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
SHA256.hash("The quick brown fox jumps over the lazy dog");
                      // "d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592"
```

Never throws for a valid call; any value is accepted.

## SHA256.hashToBuffer(data)

Returns the digest of `data` (converted as above) as a new `Buffer` with
`size` 32 and `length` 32: the raw digest bytes, most significant byte of the
first word first (the standard SHA-256 byte order).

```javascript
var d = SHA256.hashToBuffer("abc");
d.length;               // 32
d.getU8(0);             // 186 (0xba)
d.toHex();              // same as SHA256.hash("abc")
SHA256.hash(d);         // SHA-256 of the 32 digest bytes (double SHA-256)
```

Use it when the bytes themselves are needed: as key material, inside a binary
file or packet (`file.writeFromBuffer(d)`), to feed another hash, or to
encode the digest differently (`Base64.encode(d)`).

Each call returns a new buffer; changing it does not affect anything else.

## SHA256.fileHash(filename)

Returns the SHA-256 digest of the file `filename` (converted to a string) as
64 lowercase hexadecimal characters, or `undefined` if the file cannot be
hashed.

```javascript
var h = SHA256.fileHash("release/archive.7z");
if (Script.isUndefined(h)) {
	throw(new Error("cannot hash release/archive.7z"));
};
```

- The path is relative to the current working directory of the process, not
  to the script. Use an absolute path, or build one from
  `Script.getIncludedFile()` / `Shell.getFilePath(...)`, when the script can
  be started from elsewhere.
- The file is opened read only and read in 32 KB chunks, so files of any size
  can be hashed without loading them into memory.
- After reading, the number of bytes hashed is compared with the file size
  measured before reading; if they differ (read error, file truncated while
  being read) the result is `undefined` rather than a wrong digest.
- `undefined` is also returned when the file does not exist, cannot be
  opened (permissions, locked), or the name is a directory or empty.
- It never throws. Always check the result: `undefined` concatenated into a
  string becomes the text `"undefined"`.
- An empty file gives the digest of the empty input,
  `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855`.

`fileHash` returns hex only. For the raw bytes use
`Buffer.fromHex(SHA256.fileHash(name))`, or for small files
`SHA256.hashToBuffer(Shell.fileGetContentsBuffer(name))`.

## Comparing digests

Digests from this extension are always lowercase. Text from elsewhere (a
`.sha256` file, a web page, `Get-FileHash` output) may be uppercase or carry
spaces and a file name; normalize before comparing:

```javascript
var expected = "BA7816BF8F01CFEA414140DE5DAE2223B00361A396177A9CB410FF61F20015AD";
SHA256.hash("abc") == expected.trim().toLowerCaseASCII();   // true
```

`==` on strings compares the whole text and stops at the first difference.
That is fine for integrity checks (checksums, cache keys); it is not a
constant time comparison for secrets.

## Errors

| Situation | Result |
|-----------|--------|
| `SHA256` used before `Script.requireExtension("SHA256")` | the usual undefined variable error |
| Extension library not found | `Script.requireExtension` throws `Unable to open "SHA256"` |
| `Buffer` extension not available (host registered `SHA256` but not `Buffer`, no Buffer DLL) | loading `SHA256` fails: its initialization requires `Buffer` |
| Any value passed to `hash` / `hashToBuffer` | hashed as its text, no error |
| File missing, unreadable, directory, short read | `fileHash` returns `undefined` |
