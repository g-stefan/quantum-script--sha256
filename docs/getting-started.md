# Getting started

## 1. Build and install

The extension is built with [fabricare](https://github.com/g-stefan/fabricare),
the build tool used by all XYO C++ projects. `quantum-script` (and everything
below it: `xyo-system`, `xyo-encoding`, ...), `quantum-script--console`,
`quantum-script--buffer` and `xyo-cryptography` must be installed to the SDK
first. From the repository root:

```bash
fabricare make       # build into output/
fabricare test       # build and run test/test.01 (run make first)
fabricare install    # copy output/{bin,include,lib} to ~/.fabricare/<platform>
fabricare clean      # remove output/ and temp/
```

The library project is `quantum-script--sha256` with `"make": "dll-or-lib"`:

| Platform | Result | Use it when |
|----------|--------|-------------|
| dynamic (`~/.fabricare/win64-msvc-2026`, ...) | `quantum-script--sha256.dll` / `libquantum-script--sha256.so` | scripts run by `quantum-script`, or a host using the engine DLL |
| static (`~/.fabricare/win64-msvc-2026.static`, ...) | static library | self-contained hosts that register the extension as internal |

After `fabricare install` on a dynamic platform the DLL sits in the SDK `bin`
folder next to `quantum-script.exe`, which is where
`Script.requireExtension("SHA256")` finds it.

## 2. Use it from a script

```javascript
Script.requireExtension("Console");
Script.requireExtension("SHA256");

Console.writeLn(SHA256.hash("abc"));
// ba7816bf8f01cfea414140de5dae2223b00361a396177a9cb410ff61f20015ad

var digest = SHA256.hashToBuffer("abc");
Console.writeLn(digest.length);                  // 32
Console.writeLn(digest.toHex());                 // same text as SHA256.hash("abc")

var fileDigest = SHA256.fileHash("hello.js");
if (Script.isUndefined(fileDigest)) {
	Console.writeLn("cannot read hello.js");
} else {
	Console.writeLn(fileDigest + "  hello.js");
};
```

Run it with:

```bash
quantum-script hello-sha256.js
```

`Script.requireExtension("SHA256")` looks for an external
`quantum-script--sha256` library first (the file as named, then every include
path folder: next to the interpreter, next to the script), then for an
internal extension registered by the host. Loading twice does nothing. A
missing extension throws `Unable to open "SHA256"`.

Loading `SHA256` also loads `Buffer` (the extension runs
`Script.requireExtension("Buffer")` while it initializes), so the `Buffer`
global exists afterwards.

### fabricare build scripts

`fabricare` is a static executable: it cannot load extension DLLs and it does
**not** embed `SHA256` (it embeds `SHA512`). In fabricare scripts use
`Script.requireExtension("SHA512")` — same three functions, 128 character
digests — or run a separate `quantum-script` process for SHA-256.

## 3. Register it in a C++ host

A host that embeds Quantum Script makes `SHA256` available as an internal
extension by registering it, together with `Buffer`, in the init callback
(this is what `test/test.01.cpp` does):

```cpp
#include <XYO/QuantumScript.hpp>
#include <XYO/QuantumScript.Extension/Console.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>
#include <XYO/QuantumScript.Extension/SHA256.hpp>

using namespace XYO::QuantumScript;

void initExecutive(Executive *executive) {
	Extension::Console::registerInternalExtension(executive);
	Extension::Buffer::registerInternalExtension(executive);
	Extension::SHA256::registerInternalExtension(executive);
};

int main(int cmdN, char *cmdS[]) {
	if (ExecutiveX::initExecutive(cmdN, cmdS, initExecutive)) {
		if (!ExecutiveX::executeString(
		        "Script.requireExtension(\"Console\");"
		        "Script.requireExtension(\"SHA256\");"
		        "Console.writeLn(SHA256.hash(\"abc\"));")) {
			printf("%s\n", (ExecutiveX::getError()).value());
			printf("%s", (ExecutiveX::getStackTrace()).value());
		};
		ExecutiveX::endProcessing();
	};
	return 0;
};
```

`Buffer` must be registered too (or be loadable as a DLL): `SHA256` requires
it during its own initialization.

Registering only makes the extension *available*: scripts still call
`Script.requireExtension("SHA256")`. With the DLL build of the engine an
external `quantum-script--sha256.dll` found on the include path wins over the
internal one for `requireExtension`; use
`Script.requireInternalExtension("SHA256")` to force the internal one.

In the host's `fabricare.json`:

```json
{
	"name": "my-host",
	"make": "exe",
	"sourcePath": "XYO/MyHost",
	"dependency": [
		"quantum-script--sha256"
	]
}
```

`quantum-script--sha256` brings `quantum-script`, `quantum-script--console`,
`quantum-script--buffer` and `xyo-cryptography` with it.

## 4. Static builds

There is no separate `quantum-script--sha256.static` project. On a static
fabricare platform the same `quantum-script--sha256` project is built as a
static library (`dll-or-lib`), the `quantumScriptExtension` DLL entry point is
left out (it is compiled only with `XYO_PLATFORM_COMPILE_DYNAMIC_LIBRARY`),
and the host registers the extension with `registerInternalExtension`
(section 3). `quantum-script--magnet` is an example of a host that does this.

## 5. Threads

Each thread that runs scripts has its own engine, so every thread loads the
extension itself with `Script.requireExtension("SHA256")`. The functions keep
no state between calls; each call creates its own hash context, so there is
nothing to share or lock.
