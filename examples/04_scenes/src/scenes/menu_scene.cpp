// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "menu_scene.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

auto menu_scene::init() -> lge::result<> { // NOLINT(*-convert-member-functions-to-static)
	return true;
}

auto menu_scene::on_enter() -> lge::result<> {
	const auto text_label = ctx.world.create();
	ctx.world.emplace<lge::label>(text_label, "Menu Scene", lge::colors::cyan);
	ctx.world.emplace<lge::placement>(text_label);

	return true;
}
