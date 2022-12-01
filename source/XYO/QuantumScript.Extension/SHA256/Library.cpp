// Quantum Script Extension SHA256
// Copyright (c) 2022 Grigore Stefan <g_stefan@yahoo.com>
// MIT License (MIT) <http://opensource.org/licenses/MIT>
// SPDX-FileCopyrightText: 2022 Grigore Stefan <g_stefan@yahoo.com>
// SPDX-License-Identifier: MIT

#include <XYO/QuantumScript.Extension/SHA256/Library.hpp>
#include <XYO/QuantumScript.Extension/SHA256/Copyright.hpp>
#include <XYO/QuantumScript.Extension/SHA256/License.hpp>
#include <XYO/QuantumScript.Extension/SHA256/Version.hpp>
#include <XYO/QuantumScript.Extension/Buffer.hpp>

#include <XYO/Cryptography.hpp>

namespace XYO::QuantumScript::Extension::SHA256 {

				static TPointer<Variable> hash(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha256-hash\n");
#endif
					return VariableString::newVariable(XYO::Cryptography::SHA256::hash((arguments->index(0))->toString()));
				};

				static TPointer<Variable> hashToBuffer(VariableFunction *function, Variable *this_, VariableArray *arguments) {
#ifdef QUANTUM_SCRIPT_DEBUG_RUNTIME
					printf("- sha256-hash-to-buffer\n");
#endif
					TPointer<Variable> retV(Extension::Buffer::VariableBuffer::newVariable(32));
					((Extension::Buffer::VariableBuffer *)retV.value())->buffer.length = 32;
					XYO::Cryptography::SHA256::hashToU8((arguments->index(0))->toString(), ((Extension::Buffer::VariableBuffer *)retV.value())->buffer.buffer);
					return retV;
				};

				void registerInternalExtension(Executive *executive) {
					executive->registerInternalExtension("SHA256", initExecutive);
				};

				void initExecutive(Executive *executive, void *extensionId) {

					String info = "SHA256\r\n";
					info << License::shortLicense();

					executive->setExtensionName(extensionId, "SHA256");
					executive->setExtensionInfo(extensionId, info);
					executive->setExtensionVersion(extensionId, Extension::SHA256::Version::versionWithBuild());
					executive->setExtensionPublic(extensionId, true);

					executive->compileStringX("Script.requireExtension(\"Buffer\");");
					executive->compileStringX("var SHA256={};");
					executive->setFunction2("SHA256.hash(str)", hash);
					executive->setFunction2("SHA256.hashToBuffer(str)", hashToBuffer);
				};


};

#ifdef XYO_COMPILE_DYNAMIC_LIBRARY
extern "C" void quantumScriptExtension(XYO::QuantumScript::Executive *executive, void *extensionId) {
	XYO::QuantumScript::Extension::SHA256::initExecutive(executive, extensionId);
};
#endif
