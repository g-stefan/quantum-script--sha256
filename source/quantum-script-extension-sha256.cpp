//
// Quantum Script Extension SHA256
//
// Copyright (c) 2020-2022 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "quantum-script-extension-sha256-license.hpp"
#include "quantum-script-extension-sha256.hpp"
#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_NO_VERSION
#	include "quantum-script-extension-sha256-version.hpp"
#endif

#include "quantum-script-variablestring.hpp"
//
#include "quantum-script-extension-buffer-variablebuffer.hpp"

//#define QUANTUM_SCRIPT_VM_DEBUG_RUNTIME

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace SHA256 {

				using namespace XYO;
				using namespace Quantum::Script;

				static TPointer<Variable> hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha256-hash\n");
#endif
					return VariableString::newVariable(XYO::SHA256::getHashString((arguments->index(0))->toString()));
				};

				static TPointer<Variable> hashToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha256-hash-to-buffer\n");
#endif
					TPointer<Variable> retV(Extension::Buffer::VariableBuffer::newVariable(32));
					((Extension::Buffer::VariableBuffer *)retV.value())->buffer.length = 32;
					XYO::SHA256::hashStringToU8((arguments->index(0))->toString(), ((Extension::Buffer::VariableBuffer *)retV.value())->buffer.buffer);
					return retV;
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("SHA256", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "SHA256\r\n";
					info << License::shortContent();

					executive->setExtensionName(extensionId, "SHA256");
					executive->setExtensionInfo(extensionId, info);
#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_NO_VERSION
					executive->setExtensionVersion(extensionId, Extension::SHA256::Version::versionWithBuild());
#endif
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX("Script.requireExtension(\"Buffer\");");
					executive->compileStringX("var SHA256={};");
					executive->setFunction2("SHA256.hash(str)", hash);
					executive->setFunction2("SHA256.hashToBuffer(str)", hashToBuffer);
				};

			};
		};
	};
};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" QUANTUM_SCRIPT_EXTENSION_SHA256_EXPORT void quantumScriptExtension(Quantum::Script::Executive *executive, void *extensionId) {
	Quantum::Script::Extension::SHA256::initExecutive(executive, extensionId);
};
#endif
