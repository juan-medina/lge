// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

namespace lge {

class raylib_resource_manager final: public resource_manager {
public:
	[[nodiscard]] auto init() -> result<> override;
	[[nodiscard]] auto end() -> result<> override;
	[[nodiscard]] auto load_font(resource_uri uri) -> result<font_id> override;
	[[nodiscard]] auto unload_font(font_id id) -> result<> override;
};

} // namespace lge