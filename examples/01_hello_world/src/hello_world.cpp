// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "hello_world.hpp"

#include <lge/main.hpp>
#include <lge/result.hpp>
#include <spdlog/spdlog.h>

#include <iostream>

LGE_MAIN(hello_world);

auto hello_world::run() -> lge::result<> {
	if(const auto err = app::run().unwrap(); err) {
		return lge::error("error init the app", *err);
	}

	SPDLOG_INFO("Hello world");
	return true;
}