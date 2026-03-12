// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "buttons.hpp"

#include <lge/app/main.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include "../../src/example.hpp"

#include <entt/core/hashed_string.hpp>

LGE_MAIN(examples::buttons);

namespace examples {

using entt::literals::operator""_hs;

auto buttons::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	if(const auto err = ctx.resources.load_sprite_sheet(ui_sheet_path).unwrap(ui_sheet_); err) [[unlikely]] {
		return lge::error("failed to load ui sprite sheet", *err);
	}

	panel1_ = ctx.world.create();
	ctx.world.emplace<lge::panel>(panel1_, ui_sheet_, "tile_0017.png"_hs, glm::vec2{60.0F, 30.0F}, 15.0F);
	ctx.world.emplace<lge::placement>(panel1_, -80.0F, 0.0F);

	panel2_ = ctx.world.create();
	ctx.world.emplace<lge::panel>(panel2_, ui_sheet_, "tile_0017.png"_hs, glm::vec2{30.0F, 60.0F}, 15.0F);
	ctx.world.emplace<lge::placement>(panel2_, 80.0F, 0.0F);

	return true;
}

auto buttons::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_sprite_sheet(ui_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload ui sprite sheet", *err);
	}

	return example::end();
}

} // namespace examples
