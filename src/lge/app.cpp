// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/result.hpp>

#include <iostream>
#include <random>

namespace lge {

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::run() -> result<> { // NOLINT(*-convert-member-functions-to-static)

	std::mt19937 rng{std::random_device{}()};
	if(std::bernoulli_distribution fail_dist{0.5}; fail_dist(rng)) {
		return error{"random failure"};
	}

	std::cout << "Hello world\n";
	return true;
}

} // namespace lge
