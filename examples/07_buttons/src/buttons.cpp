// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "buttons.hpp"

#include <lge/app/main.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/sprite.hpp>
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

	ui_sprite_ = ctx.world.create();
	ctx.world.emplace<lge::sprite>(ui_sprite_, ui_sheet_, "tile_0017.png"_hs);
	ctx.world.emplace<lge::placement>(ui_sprite_, 0.0F, 0.0F);

	return true;
}

auto buttons::end() -> lge::result<> {
	if(const auto err = ctx.resources.unload_sprite_sheet(ui_sheet_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unload ui sprite sheet", *err);
	}

	return example::end();
}

} // namespace examples
