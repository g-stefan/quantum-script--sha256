//
// Quantum Script Extension SHA256
//
// Copyright (c) 2020 Grigore Stefan <g_stefan@yahoo.com>
// Created by Grigore Stefan <g_stefan@yahoo.com>
//
// MIT License (MIT) <http://opensource.org/licenses/MIT>
//

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_HPP
#define QUANTUM_SCRIPT_EXTENSION_SHA256_HPP

#ifndef QUANTUM_SCRIPT_HPP
#include "quantum-script.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256__EXPORT_HPP
#include "quantum-script-extension-sha256--export.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_COPYRIGHT_HPP
#include "quantum-script-extension-sha256-copyright.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_LICENSE_HPP
#include "quantum-script-extension-sha256-license.hpp"
#endif

#ifndef QUANTUM_SCRIPT_EXTENSION_SHA256_VERSION_HPP
#include "quantum-script-extension-sha256-version.hpp"
#endif

namespace Quantum {
	namespace Script {
		namespace Extension {
			namespace SHA256 {

				using namespace Quantum::Script;

				QUANTUM_SCRIPT_EXTENSION_SHA256_EXPORT void initExecutive(Executive *executive, void *extensionId);
				QUANTUM_SCRIPT_EXTENSION_SHA256_EXPORT void registerInternalExtension(Executive *executive);

			};
		};
	};
};

#endif

