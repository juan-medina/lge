// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "buttons.hpp"

#include <lge/app/main.hpp>
#include <lge/components/button.hpp>
#include <lge/components/label.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/panel.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
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

	const auto panel = ctx.world.create();
	ctx.world.emplace<lge::panel>(panel, ui_sheet_, "tile_0017.png"_hs, glm::vec2{300.0F, 100.0F}, 15.0F);
	ctx.world.emplace<lge::placement>(panel, 0.0F, 0.0F);

	const auto label = ctx.world.create();
	ctx.world.emplace<lge::label>(label, "Press Buttons", lge::colors::white, 32.0F);
	ctx.world.emplace<lge::placement>(label, 0.0F, -20.0F);
	lge::attach(ctx.world, panel, label);

	const auto button_1 = ctx.world.create();
	ctx.world.emplace<lge::button>(
		button_1, ui_sheet_, "tile_0003.png"_hs, glm::vec2{70.0F, 30.0F}, 15.0F, "Ok", lge::colors::green);
	ctx.world.emplace<lge::placement>(button_1, -40.0F, 20.0F);
	lge::attach(ctx.world, panel, button_1);

	const auto button_2 = ctx.world.create();
	ctx.world.emplace<lge::button>(
		button_2, ui_sheet_, "tile_0003.png"_hs, glm::vec2{70.0F, 30.0F}, 15.0F, "Cancel", lge::colors::red);
	ctx.world.emplace<lge::placement>(button_2, 40.0F, 20.0F);
	lge::attach(ctx.world, panel, button_2);

	return true;
}

auto buttons::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_sprite_sheet(ui_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload ui sprite sheet", *err);
	}

	return example::end();
}

} // namespace examples
