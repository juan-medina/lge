// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/result.hpp>

#include <iostream>

namespace lge {

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::run() -> result<> { // NOLINT(*-convert-member-functions-to-static)
	std::cout << "Hello world\n";
	return true;
}

} // namespace lge
