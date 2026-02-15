// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_resource_manager.hpp"

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <string>

namespace lge {

auto raylib_resource_manager::init() -> result<> {
	LGE_DEBUG("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	LGE_DEBUG("ending raylib resource manager");
	return true;
}

auto raylib_resource_manager::load_font(const resource_uri uri) -> result<font_id> {
	LGE_DEBUG("loading font from uri `{}`", uri);
	if(!uri.exists()) [[unlikely]] {
		return error("font file does not exist: " + std::string(uri));
	}

	return empty_resource;
}

auto raylib_resource_manager::unload_font(font_id /*id*/) -> result<> {
	return true;
}

} // namespace lge