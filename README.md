# Quantum Script Extension SHA256

Quantum Script extension
- SHA-256 hashing for scripts: `SHA256.hash` returns the digest as 64
lowercase hex characters (the same text `sha256sum` prints).
- The raw 32 byte digest as a `Buffer` (`SHA256.hashToBuffer`).
- Checksums of files of any size, read in chunks (`SHA256.fileHash`),
`undefined` when the file cannot be read.
- Strings and buffers are hashed byte for byte, zero bytes included.

```javascript
Script.requireExtension("SHA256");

SHA256;
SHA256.hash(str);
SHA256.hashToBuffer(str);
SHA256.fileHash(filename);
```

Built on `quantum-script`, `quantum-script--buffer` and `xyo-cryptography`,
part of the XYO C++ SDK.

## Documentation

- [Overview](docs/README.md) - purpose and design
- [Getting started](docs/getting-started.md) - build, load from a script, register in a C++ host, static builds
- [Script API](docs/script-api.md) - every function: argument conversion, exact results, errors
- [Recipes](docs/recipes.md) - verify and write checksum files, hash folders and binary data, cache keys, what SHA-256 is not for
- [C++ API](docs/cpp-api.md) - `registerInternalExtension`, `initExecutive`, hashing from C++ with `xyo-cryptography`
- [API reference](docs/reference.md)

A Claude Code skill for this extension is in
[.claude/skills/quantum-script--sha256](.claude/skills/quantum-script--sha256/SKILL.md).

## License

Copyright (c) 2016-2026 Grigore Stefan
Licensed under the [MIT](LICENSE) license.
