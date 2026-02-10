// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app.hpp>
#include <lge/app_config.hpp>
#include <lge/result.hpp>

#include <entity/entity.hpp>
#include <entity/fwd.hpp>

class hello_world: public lge::app {
public:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	static constexpr auto game_res = glm::vec2{640, 360}; // world expands horizontally keep the vertical size fixed
	static constexpr auto clear_color = glm::vec4{0, 0, 0, 1}; // black background
	static constexpr auto game_title = "Hello world!";
	static constexpr auto kb_message = "press 1: hide hello+world, 2 hide: world, F5 to toggle debug draw, "
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "controller A: hide hello+world, X: hide world, START: debug draw, "
											   "SELECT: toggle fullscreen, B: exit";

	static constexpr auto debug_action = "debug";
	static constexpr auto fullscreen_action = "fullscreen";
	static constexpr auto exit_action = "exit";
	static constexpr auto hide_hello_world_action = "hide_hello_world";
	static constexpr auto hide_world_action = "hide_world";

	bool was_in_controller_mode_ = false;
	entt::entity message_ent_ = entt::null;
	entt::entity center_ent_ = entt::null;
	entt::entity world_text_ent_ = entt::null;
};
