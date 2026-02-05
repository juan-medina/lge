// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/app.hpp"

#include <lge/result.hpp>

#include <spdlog/spdlog.h>

namespace lge {

auto app::run() -> result<> {
	if(const auto err = init().unwrap(); err) [[unlikely]] {
		return error("failed to init the application", *err);
	}

	return true;
}

auto app::init() -> result<> {
	if(const auto err = setup_log().unwrap(); err) [[unlikely]] {
		return error("failed to setup log", *err);
	}

	SPDLOG_INFO("application initialized successfully");

	return true;
}

// ReSharper disable once CppMemberFunctionMayBeStatic
auto app::setup_log() -> result<> { // NOLINT(*-convert-member-functions-to-static)
#ifdef NDEBUG
	spdlog::set_level(spdlog::level::err);
#else
	spdlog::set_level(spdlog::level::debug);
#endif
	spdlog::set_pattern(empty_format);
	SPDLOG_INFO(banner);
	spdlog::set_pattern(color_line_format);

	SPDLOG_INFO("log setup complete");
	return true;
}

} // namespace lge
