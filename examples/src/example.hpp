// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/app.hpp>
#include <lge/app/app_config.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <utility>

class example: public lge::app {
public:
	explicit example(
		std::string game_title = "Example",
		std::string kb_message = "change the keyboard message in the constructor of your example subclass",
		std::string controller_message = "change the controller message in the constructor of your example subclass")
		: game_title_(std::move(game_title)), kb_message_(std::move(kb_message)),
		  controller_message_(std::move(controller_message)) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

	[[nodiscard]] static constexpr auto get_game_res() -> glm::vec2 {
		return game_res;
	}

	[[nodiscard]] auto get_font_id() const -> lge::font_handle {
		return font_id_;
	}

private:
	static constexpr auto clear_color = glm::vec4{0, 0, 0, 1}; // black background
	static constexpr auto game_res = glm::vec2{640, 360}; // world expands horizontally keep the vertical size fixed

	static constexpr auto fullscreen_action = "fullscreen";
	static constexpr auto exit_action = "exit";
	static constexpr auto debug_action = "debug";

	bool was_in_controller_mode_ = false;
	entt::entity message_ent_ = entt::null;

	auto bind_common_actions() -> void;
	auto handle_common_input() -> void;
	auto update_controller_mode_message() -> void;

	std::string game_title_;
	std::string kb_message_;
	std::string controller_message_;

	lge::font_handle font_id_ = lge::invalid_font;
};
