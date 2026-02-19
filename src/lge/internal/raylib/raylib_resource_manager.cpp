// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_resource_manager.hpp"

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <raylib.h>

#include <string>
#include <string_view>

namespace lge {

// =============================================================================
// Init / end
// =============================================================================

auto raylib_resource_manager::init() -> result<> {
	log::debug("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	log::debug("ending raylib resource manager");
	return true;
}

} // namespace lge