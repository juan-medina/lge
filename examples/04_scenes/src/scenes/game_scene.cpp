// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT
#include "game_scene.hpp"

#include <lge/components/label.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

auto game_scene::init() -> lge::result<> { // NOLINT(*-convert-member-functions-to-static)
	return true;
}

auto game_scene::on_enter() -> lge::result<> {
	const auto text_label = create_entity();
	ctx.world.emplace<lge::label>(text_label, "Game Scene", lge::colors::magenta);
	ctx.world.emplace<lge::placement>(text_label);

	return true;
}
