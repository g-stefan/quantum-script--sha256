# Recipes

All examples assume:

```javascript
Script.requireExtension("Console");
Script.requireExtension("SHA256");
```

## Print the checksum of a file

```javascript
var name = "archive.7z";
var h = SHA256.fileHash(name);
if (Script.isUndefined(h)) {
	Console.writeLn("error: cannot read " + name);
	Script.setExitCode(1);
} else {
	Console.writeLn(h + " *" + name);   // the sha256sum binary mode line format
};
```

## Verify a file against a published checksum

A `.sha256` file usually holds `<64 hex chars> *<file name>` or
`<64 hex chars>  <file name>`, sometimes in uppercase:

```javascript
Script.requireExtension("Shell");

function verifySHA256(fileName, checksumFile) {
	var text = Shell.fileGetContents(checksumFile);
	if (!text) {
		return false;
	};
	var expected = text.trim().substring(0, 64).toLowerCaseASCII();
	var actual = SHA256.fileHash(fileName);
	if (Script.isUndefined(actual)) {
		return false;
	};
	return actual == expected;
};

if (!verifySHA256("archive.7z", "archive.7z.sha256")) {
	throw(new Error("checksum mismatch: archive.7z"));
};
```

## Write a checksum file for a release

```javascript
Script.requireExtension("Shell");

var files = ["app-1.0.0-win64.7z", "app-1.0.0-linux64.tar.gz"];
var out = "";
for (var name of files) {
	var h = SHA256.fileHash(name);
	if (Script.isUndefined(h)) {
		throw(new Error("cannot hash " + name));
	};
	out += h + " *" + name + "\n";
};
Shell.filePutContents("SHA256SUMS", out);
```

## Hash every file in a folder

```javascript
Script.requireExtension("Shell");

var list = Shell.getFileList("data/*");
for (var name of list) {
	var h = SHA256.fileHash(name);
	Console.writeLn((Script.isUndefined(h) ? "<error>" : h) + "  " + name);
};
```

## Hash binary data

Strings and buffers are hashed byte for byte, so binary content gives the
same digest as other tools:

```javascript
Script.requireExtension("Shell");

var data = Shell.fileGetContentsBuffer("logo.png");          // Buffer, or undefined
if (data) {
	Console.writeLn(SHA256.hash(data));                       // == SHA256.fileHash("logo.png")
};

var packet = Buffer.fromHex("0102030400ff");
Console.writeLn(SHA256.hash(packet));                         // 6 bytes hashed, zero byte included
```

For large files prefer `SHA256.fileHash`: it reads in chunks instead of
loading the whole file.

## Hash data built in pieces

There is no incremental interface. Join the pieces first:

```javascript
var parts = [header, body, footer];
var digest = SHA256.hash(parts.join(""));
```

The pieces are concatenated without separators, so `["ab", "c"]` and
`["a", "bc"]` give the same digest. When the structure matters (cache keys
built from several fields), add a separator or a length prefix:

```javascript
function cacheKey(fields) {
	var text = "";
	for (var field of fields) {
		var s = "" + field;
		text += s.length + ":" + s + ";";
	};
	return SHA256.hash(text);
};
```

## Content changed? (build stamps, caches)

```javascript
Script.requireExtension("Shell");

var stampFile = "temp/config.json.sha256";
var current = SHA256.fileHash("config.json");
var previous = Shell.fileGetContents(stampFile);
if (current != previous) {
	Console.writeLn("config.json changed, regenerating ...");
	// ... regenerate ...
	Shell.filePutContents(stampFile, current);
};
```

## Raw digest, other encodings

```javascript
Script.requireExtension("Base64");

var d = SHA256.hashToBuffer("abc");      // 32 bytes
Console.writeLn(d.toHex());              // hex, same as SHA256.hash("abc")
Console.writeLn(Base64.encode(d));       // base64, as used by Subresource Integrity ("sha256-" + ...)
Console.writeLn(SHA256.hash(d));         // double SHA-256: SHA256(SHA256(x))
```

`Buffer.fromHex(SHA256.hash(x))` and `SHA256.hashToBuffer(x)` hold the same
32 bytes.

## What SHA-256 is not for

SHA-256 is a fast, unkeyed hash. It is the right tool for integrity checks,
content identifiers and cache keys. It is the wrong tool for:

- **Passwords.** A plain `SHA256.hash(password)` (with or without a salt) can
  be brute forced quickly. Use a slow password hash (Argon2, scrypt, bcrypt,
  PBKDF2); no Quantum Script extension provides one, so use a native library
  or an external tool.
- **Message authentication.** `SHA256.hash(secret + message)` is open to
  length extension attacks. Use HMAC-SHA256 instead; this extension does not
  provide it.
- **Encryption.** A hash cannot be reversed; to encrypt data see the `Crypt`
  or `OpenSSL` extensions.
- **Secret comparison.** String `==` is not constant time; do not use it to
  check secret tokens where timing can be observed.
