// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include "../../src/example.hpp"

#include <entity/entity.hpp>
#include <string>

class hello_world: public example {
public:
	explicit hello_world(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto example_title = "Hello world!";
	static constexpr auto kb_message = "press 1: hide hello+world, 2 hide: world, F5 to toggle debug draw, "
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "controller A: hide hello+world, X: hide world, START: debug draw, "
											   "SELECT: toggle fullscreen, B: exit";

	static constexpr auto hide_hello_world_action = "hide_hello_world";
	static constexpr auto hide_world_action = "hide_world";

	entt::entity center_ent_ = entt::null;
	entt::entity world_text_ent_ = entt::null;
};
