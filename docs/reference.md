# API reference

## Script

| Symbol | Returns | Description |
|--------|---------|-------------|
| `Script.requireExtension("SHA256")` | | load the extension (also loads `Buffer`) |
| `SHA256` | `Object` | holder of the functions below, not a constructor |
| `SHA256.hash(data)` | `String` | SHA-256 of `data.toString()` bytes, 64 lowercase hex characters |
| `SHA256.hashToBuffer(data)` | `Buffer` | SHA-256 of `data.toString()` bytes, 32 raw bytes (`size` = `length` = 32) |
| `SHA256.fileHash(filename)` | `String` or `undefined` | SHA-256 of the file content, 64 lowercase hex characters; `undefined` if the file cannot be read completely |

Argument conversion for `hash` / `hashToBuffer`:

| Value | Hashed as |
|-------|-----------|
| `String` | its bytes |
| `Buffer` | its first `length` bytes |
| `Number`, `Boolean`, `Array`, `Object` | their text (`123` -> `"123"`, `true` -> `"true"`, `[1,2]` -> `"1,2"`, `{}` -> `"Object"`) |
| `null` | `"null"` |
| `undefined` / missing | `"undefined"` |

Known values:

| Input | `SHA256.hash(input)` |
|-------|----------------------|
| `""` | `e3b0c44298fc1c149afbf4c8996fb92427ae41e4649b934ca495991b7852b855` |
| `"abc"` | `ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad` |
| `"The quick brown fox jumps over the lazy dog"` | `d7a8fbb307d7809469ca9abcb0082e4f8d5651e46d3cdb762d02d0bf37c9e592` |
| missing argument (`"undefined"`) | `eb045d78d273107348b0300c01d29b7552d622abbc6faf81b3ec55359aa9950c` |

## C++ — `XYO::QuantumScript::Extension::SHA256`

| Symbol | Header | Description |
|--------|--------|-------------|
| `void registerInternalExtension(Executive *executive)` | `SHA256/Library.hpp` | register `SHA256` as an internal extension |
| `void initExecutive(Executive *executive, void *extensionId)` | `SHA256/Library.hpp` | extension initialization: metadata, `Buffer`, `SHA256` object and functions |
| `extern "C" void quantumScriptExtension(Executive *, void *)` | `SHA256/Library.cpp` | DLL entry point, forwards to `initExecutive` |
| `const char *Version::version()` / `build()` / `versionWithBuild()` / `datetime()` | `SHA256/Version.hpp` | version info from `version.json` |
| `std::string License::license()` / `shortLicense()` | `SHA256/License.hpp` | MIT license text |
| `const char *Copyright::copyright()` / `publisher()` / `company()` / `contact()` | `SHA256/Copyright.hpp` | copyright info |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA256_EXPORT` | `SHA256/Dependency.hpp` | import / export macro |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA256_INTERNAL` | | define when building the DLL |
| `XYO_QUANTUMSCRIPT_EXTENSION_SHA256_LIBRARY` | | define for static use: empty export macro, no DLL entry point |

Umbrella header: `<XYO/QuantumScript.Extension/SHA256.hpp>`.
Whole extension in one translation unit: `SHA256.Amalgam.cpp`.

## C++ — `XYO::Cryptography` (used by the extension)

| Symbol | Description |
|--------|-------------|
| `static String SHA256::hash(const String &toHash)` | one shot, lowercase hex |
| `static void SHA256::hashToU8(const String &toHash, uint8_t *buffer)` | one shot, 32 raw bytes into `buffer` |
| `SHA256()` / `processInit()` | start (or restart) a computation |
| `processU8(const uint8_t *toHash, size_t length)` | add bytes |
| `processDone()` | finalize, call once |
| `String getHashHex()` / `void toU8(uint8_t *buffer)` | read the result after `processDone()` |
| `copy(const SHA256 &in)` | duplicate a state |
| `bool Util::fileHashSHA256(const char *fileName, String &hash)` | hex digest of a file, `false` on any error |

## fabricare.json

| Project | Make | Depends on |
|---------|------|------------|
| `quantum-script--sha256` | `dll-or-lib` (DLL on dynamic platforms, static library on static ones) | `quantum-script`, `quantum-script--console`, `quantum-script--buffer`, `xyo-cryptography` |
| `test.01` | `exe`, category `test` | `quantum-script--sha256` |

## Related extensions

| Extension | Functions | Digest |
|-----------|-----------|--------|
| `SHA512` | `hash`, `hashToBuffer`, `fileHash` | 64 bytes / 128 hex characters |
| `MD5` | `hash`, `hashToBuffer` | 16 bytes / 32 hex characters (not for security) |
| `OpenSSL` | `OpenSSL.sha256(bufferIn, st, ln, hashOut)` | SHA-256 of a buffer range through OpenSSL |
| `Buffer` | the type returned by `hashToBuffer` | |
