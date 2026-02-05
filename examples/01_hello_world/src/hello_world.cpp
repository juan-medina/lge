// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/main.hpp>
#include <lge/result.hpp>

#include <iostream>
#include <random>

LGE_MAIN(hello_world);

auto hello_world::run() -> lge::result<> {
	if(const auto err = app::run().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	// Simulate a random failure with 50% chance
	std::mt19937 rng{std::random_device{}()};
	if(std::bernoulli_distribution fail_dist{0.5}; fail_dist(rng)) {
		return lge::error{"random failure"};
	}

	std::cout << "Hello world\n";

	return true;
}