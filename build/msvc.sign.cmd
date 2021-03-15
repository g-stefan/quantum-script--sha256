@echo off
rem Public domain
rem http://unlicense.org/
rem Created by Grigore Stefan <g_stefan@yahoo.com>

echo -^> sign quantum-script-extension-sha256

pushd output
for /r %%i in (*.dll) do call grigore-stefan.sign "Quantum Script Extension SHA256" "%%i"
popd
