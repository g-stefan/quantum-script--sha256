---
name: quantum-script--sha256
description: >-
  How to use the Quantum Script SHA256 extension (quantum-script--sha256),
  the SHA-256 hash functions loaded with Script.requireExtension("SHA256"):
  SHA256.hash(data) (64 lowercase hex characters), SHA256.hashToBuffer(data)
  (32 byte Buffer), SHA256.fileHash(filename) (hex, or undefined on error);
  how arguments are converted (bytes of toString(): Buffer first length bytes,
  numbers as text, null -> "null", missing argument -> "undefined", not
  empty); checksum files, verifying downloads, cache keys, hashing binary
  data, comparing with sha256sum / Get-FileHash; what it is not for
  (passwords, HMAC); fabricare does not embed SHA256 (use SHA512 there); the
  C++ side (registerInternalExtension, initExecutive, Buffer registered
  first, XYO::Cryptography::SHA256 hash / hashToU8 / processU8 /
  processDone, Util::fileHashSHA256). Use when writing or reviewing Quantum
  Script code that calls SHA256, C++ code that includes
  <XYO/QuantumScript.Extension/SHA256.hpp>, a fabricare.json depending on
  "quantum-script--sha256", or when working inside the
  quantum-script--sha256 repository.
---

# quantum-script--sha256

SHA-256 extension of Quantum Script (see the `quantum-script` skill for the
language and its differences from JavaScript, and the
`quantum-script--buffer` skill for the `Buffer` type; their rules apply).
Purpose: **checksums and content identifiers in scripts** — verify
downloads, write release checksum files, detect changed content, cache keys
— with the same digests as `sha256sum`.

Full documentation: `docs/` in the quantum-script--sha256 repository
(`X:\Storage\XYO\Gitea\CPP\quantum-script--sha256\docs` on this machine):
README, getting-started, **script-api** (argument conversion, exact
results, errors), **recipes** (verify / write checksum files, folders,
binary data, cache keys, what not to use it for), cpp-api, reference. Read
the matching page when you need more than this summary. The whole
implementation is `source/XYO/QuantumScript.Extension/SHA256/Library.cpp`
(~75 lines) on top of `xyo-cryptography`.

## Script API

```javascript
Script.requireExtension("SHA256");          // also loads Buffer

SHA256.hash("abc");                         // "ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad"
SHA256.hash("");                            // "e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855"
SHA256.hash(buffer);                        // first buffer.length bytes, zero bytes included
var d = SHA256.hashToBuffer("abc");         // Buffer, size == length == 32; d.toHex() == SHA256.hash("abc")
var h = SHA256.fileHash("archive.7z");      // hex String, or undefined
if (Script.isUndefined(h)) { throw(new Error("cannot hash archive.7z")); };
```

## Hard rules

1. **Only three functions**: `hash`, `hashToBuffer`, `fileHash`. No
   `update` / `digest` streaming, no HMAC, no `SHA256(...)` call, no
   `new SHA256`. `SHA256` is a plain object (`typeof(SHA256) == "Object"`).
   For data in pieces: join first (`parts.join("")`), or write a file and
   use `fileHash`.
2. **The argument is hashed as `toString()` bytes.** `SHA256.hash(123)` ==
   `SHA256.hash("123")`; `true` -> `"true"`; `[1,2]` -> `"1,2"`; `{}` ->
   `"Object"`; `null` -> `"null"`; **`SHA256.hash()` / `undefined` hash the
   text `"undefined"`** (`eb045d78...`), not the empty input. Guard optional
   values; use `SHA256.hash("")` for the empty digest.
3. **Bytes, no transcoding.** Strings are hashed as stored (UTF-8 from
   scripts and `Shell.fileGetContents`), so `SHA256.hash("ă")` matches
   `printf 'ă' | sha256sum`. Buffers hash their first `length` bytes, not
   `size`.
4. **`fileHash` returns `undefined` on any failure** (missing file,
   directory, empty name, no permission, short read) and never throws.
   Always test with `Script.isUndefined(h)` before using it — `"" + undefined`
   is the text `"undefined"`. Paths are relative to the process working
   directory, not the script.
5. **Output is lowercase hex.** Normalize external digests before `==`:
   `expected.trim().toLowerCaseASCII()`. `.sha256` files hold
   `<hex> *<name>` or `<hex>  <name>`: take `substring(0, 64)`.
6. **`fileHash` for files, not `hash(Shell.fileGetContents(...))`**: it
   streams 32 KB chunks and detects incomplete reads. Read the file into a
   buffer only when the bytes are needed anyway
   (`SHA256.hash(Shell.fileGetContentsBuffer(name))` gives the same digest).
7. **Raw bytes**: `hashToBuffer` returns a new 32 byte `Buffer` each call;
   `Buffer.fromHex(SHA256.fileHash(name))` for a file's raw digest;
   `Base64.encode(d)` for base64 (Subresource Integrity); `SHA256.hash(d)` is
   double SHA-256.
8. **Concatenation is ambiguous**: `hash("ab" + "c") == hash("a" + "bc")`.
   For keys from several fields add separators or length prefixes
   (`s.length + ":" + s + ";"`).
9. **Not for passwords, MACs or secrets**: plain / salted SHA-256 is too fast
   for passwords (no Quantum Script extension provides Argon2 / scrypt /
   bcrypt / PBKDF2); `hash(secret + message)` is not a MAC (length
   extension, no HMAC here); string `==` is not constant time.
10. **fabricare scripts cannot use it**: fabricare is static and embeds
    `SHA512`, not `SHA256` — use `SHA512.hash` / `hashToBuffer` /
    `fileHash` (128 hex characters) there, or run `quantum-script`.
11. **One engine per thread**: each thread requires `SHA256` itself; the
    functions keep no state.

## Related

- `SHA512`: same three functions, 64 byte digest. `MD5`: `hash` /
  `hashToBuffer` only, 16 bytes, not for security.
- `OpenSSL.sha256(bufferIn, st, ln, hashOut)`: SHA-256 of a buffer range
  through OpenSSL.

## C++

```cpp
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/SHA256.hpp>
using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {                 // host init callback
	Extension::Buffer::registerInternalExtension(executive);   // required: SHA256 requires Buffer while initializing
	Extension::SHA256::registerInternalExtension(executive);   // scripts still call requireExtension("SHA256")
};
```

Hashing in C++ without a script (`#include <XYO/Cryptography.hpp>`,
dependency `xyo-cryptography`):

```cpp
String hex = XYO::Cryptography::SHA256::hash(str);                // lowercase hex
uint8_t digest[32]; XYO::Cryptography::SHA256::hashToU8(str, digest);
String fileHex; bool ok = XYO::Cryptography::Util::fileHashSHA256("f.bin", fileHex);
XYO::Cryptography::SHA256 sha;                                    // constructor = processInit()
sha.processU8(data, size); sha.processDone();                     // processDone once, then:
sha.getHashHex(); sha.toU8(digest); sha.processInit();            // processInit before reuse
```

- `fabricare.json` dependency: `"quantum-script--sha256"` (brings
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer`,
  `xyo-cryptography`). There is no `.static` project: the `dll-or-lib`
  project builds a static library on static platforms; register it as
  internal there.
- `XYO_QUANTUMSCRIPT_EXTENSION_SHA256_LIBRARY`: empty export macro, no
  `quantumScriptExtension` entry point. `..._INTERNAL`: building the DLL.

## Working in this repository

- Build: `fabricare make`, `fabricare test` (runs `test/test.01`, a host
  registering Console, Buffer and SHA256 as internal and running
  `test/test.01.js`, which checks `SHA256.hash` against known digests; run
  `make` first), `fabricare install` (see the `fabricare` skill).
  `quantum-script`, `quantum-script--console`, `quantum-script--buffer` and
  `xyo-cryptography` must be installed first.
- Quick check with the installed DLL:
  `quantum-script script.js` where the script requires `SHA256`; compare
  with `printf 'abc' | sha256sum`.
- Native functions live in `SHA256/Library.cpp` as
  `static TPointer<Variable> name(VariableFunction *, Variable *this_, VariableArray *arguments)`
  and are registered in `initExecutive` with
  `executive->setFunction2("SHA256.name(args)", name)`.
- Changing the API: update `README.md`, `docs/script-api.md`,
  `docs/reference.md`, `docs/recipes.md` when relevant, and this skill. Keep
  it in step with `quantum-script--sha512` (same API shape).
- Code style: tabs (width 8), `.clang-format`, CRLF, statements and blocks
  end with `};`, camelCase. SPDX header: MIT for `source/` and `docs/`,
  Unlicense for `test/` and `.claude/` (see `.reuse/dep5`).
