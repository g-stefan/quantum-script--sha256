# Quantum Script Extension SHA256 — Documentation

`quantum-script--sha256` is the **SHA-256 hash extension of Quantum Script**.
Loaded with `Script.requireExtension("SHA256")`, it adds a `SHA256` object to
scripts with three functions: hash a string or a buffer to hexadecimal text,
hash it to a 32 byte `Buffer`, and hash a whole file.

The hashing itself is `XYO::Cryptography::SHA256` from `xyo-cryptography`
(FIPS 180-4 SHA-256); this extension is the thin script binding on top of it.

- **`SHA256.hash(data)`** — the digest as 64 lowercase hexadecimal
  characters, the same text `sha256sum` prints.
- **`SHA256.hashToBuffer(data)`** — the raw 32 byte digest as a `Buffer`
  (`quantum-script--buffer`), for binary protocols, key material or further
  hashing.
- **`SHA256.fileHash(filename)`** — the hex digest of a file, read in 32 KB
  chunks (any size, no memory spike); `undefined` if the file cannot be read
  completely.

```
scripts: quantum-script .js, tools and hosts that embed the engine
quantum-script--sha256    <-- this extension: SHA256.hash / hashToBuffer / fileHash
quantum-script--buffer    (the Buffer type returned by hashToBuffer)
quantum-script            (Executive, Variable, Context)
xyo-cryptography          (XYO::Cryptography::SHA256, Util::fileHashSHA256)
xyo-system, xyo-encoding, xyo-multithreading, xyo-data-structures, xyo-managed-memory, xyo-platform
```

## Why it exists

| Need | What `SHA256` gives |
|------|---------------------|
| Checksum a download or a release archive | `SHA256.fileHash("archive.7z")`, compare with the published `.sha256` |
| Detect that content changed (cache keys, build stamps) | `SHA256.hash(text)`, a fixed 64 character key for any input |
| Identify data by content (deduplication, content addressing) | same input, same digest, on every platform |
| Raw digest bytes for another algorithm or a binary format | `SHA256.hashToBuffer(data)`, 32 bytes |
| Hash binary data, not only text | pass a `Buffer`: its bytes are hashed unchanged, zero bytes included |

## Concepts at a glance

| Need | Use | Notes |
|------|-----|-------|
| Load the extension | `Script.requireExtension("SHA256");` | also loads `Buffer` |
| Hex digest of a string | `SHA256.hash("abc")` | `"ba7816bf...f20015ad"`, 64 lowercase hex characters |
| Hex digest of a buffer | `SHA256.hash(buffer)` | the first `length` bytes of the buffer |
| Raw digest | `SHA256.hashToBuffer("abc")` | `Buffer`, `size` = `length` = 32 |
| Raw digest as hex | `SHA256.hashToBuffer(x).toHex()` | equal to `SHA256.hash(x)` |
| Hash of a file | `SHA256.fileHash("file.bin")` | hex string, or `undefined` on any error |
| Compare digests | `a == b` | both lowercase; lowercase external text first if needed |
| Other algorithms | `SHA512`, `MD5` extensions | `SHA512`: same three functions, 64 byte digest; `MD5`: `hash` / `hashToBuffer` only, 16 bytes |

Arguments are converted to a string first, so numbers, booleans and objects
are hashed as their text: `SHA256.hash(123)` is the hash of `"123"`, and a
missing argument is the hash of the text `"undefined"`, **not** of the empty
string. See [Script API](script-api.md).

## Contents

| Document | What it covers |
|----------|----------------|
| [Getting started](getting-started.md) | Build and install, load the extension from a script, register it in a C++ host, static builds |
| [Script API](script-api.md) | Every function: arguments, how values are converted, exact results, errors |
| [Recipes](recipes.md) | Verify a checksum file, hash many files, hash binary data, cache keys, what SHA-256 is not for |
| [C++ API](cpp-api.md) | `registerInternalExtension`, `initExecutive`, version info, hashing from C++ with `xyo-cryptography` |
| [API reference](reference.md) | Every script and C++ symbol on one page |

Quantum Script itself (the language, `Script.requireExtension`, embedding,
writing extensions) is documented in the `quantum-script` repository,
`docs/`; the `Buffer` type in the `quantum-script--buffer` repository,
`docs/`.

## Source map

```
source/XYO/QuantumScript.Extension/SHA256.hpp            umbrella header, include this from C++
source/XYO/QuantumScript.Extension/SHA256.Amalgam.cpp    the whole extension in one translation unit
source/XYO/QuantumScript.Extension/SHA256/
    Dependency.hpp                                       <XYO/QuantumScript.hpp>, export macro
    Library[.hpp/.cpp]                                   initExecutive, registerInternalExtension,
                                                         hash / hashToBuffer / fileHash
    Copyright / License / Version                        library metadata
test/test.01.cpp                                         C++ host registering Console, Buffer and SHA256 as internal
test/test.01.js                                          SHA256.hash against known digests
```

## AI assistant skill

A Claude Code skill describing how to use this extension lives in
[`.claude/skills/quantum-script--sha256/`](../.claude/skills/quantum-script--sha256/SKILL.md).
It is picked up automatically inside this repository; copy the folder to
`~/.claude/skills/` to have it available in the projects that use `SHA256`
(Quantum Script tools, hosts, other extensions).
