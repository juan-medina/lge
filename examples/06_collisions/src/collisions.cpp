// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "collisions.hpp"

#include <lge/app/main.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/systems/system.hpp>

#include "../../src/example.hpp"
#include "dice_roller_system.hpp"
#include "events.hpp"

#include <string>

LGE_MAIN(examples::collisions);

namespace examples {

auto collisions::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	ctx.actions.bind(throw_action,
					 {
						 .keys = {lge::input::key::space, lge::input::key::enter},
						 .buttons = {lge::input::button::left_face_down},
					 });

	if(const auto err = register_system<dice_roller_system>(lge::phase::game_update).unwrap(); err) [[unlikely]] {
		return lge::error("failed to register dice_roller_system", *err);
	}

	dice_roll_result_sub_ = ctx.events.subscribe<dice_roll_result>(
		[](const dice_roll_result &r) -> lge::result<> { return on_dice_roll_result(r); });

	return true;
}

auto collisions::update(const float dt) -> lge::result<> {
	if(ctx.actions.get(throw_action).pressed) {
		ctx.audio.stop_all();
		if(const auto err = ctx.events.post(dice_throw{.count = 10}).unwrap(); err) [[unlikely]] {
			return lge::error("failed to post dice throw event", *err);
		}
	}

	return example::update(dt);
}

auto collisions::end() -> lge::result<> {
	if(const auto err = ctx.events.unsubscribe(dice_roll_result_sub_).unwrap(); err) [[unlikely]] {
		return lge::error("failed to unsubscribe from dice roll result events", *err);
	}

	return example::end();
}

auto collisions::on_dice_roll_result(const dice_roll_result &result) -> lge::result<> {
	std::string values_str;
	for(const auto value: result.values) {
		if(!values_str.empty()) [[likely]] {
			values_str += " + ";
		}
		values_str += std::to_string(value);
	}
	lge::log::debug("We got: {} | Total: {}", values_str, result.total);
	return true;
}

} // namespace examples