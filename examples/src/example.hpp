// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/app.hpp>
#include <lge/app/app_config.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>

#include <cstddef>
#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
#include <string>
#include <utility>

namespace examples {

class example: public lge::app {
public:
	explicit example(
		std::string game_title = "Example",
		std::string kb_message = "change the keyboard message in the constructor of your example subclass",
		std::string controller_message = "change the controller message in the constructor of your example subclass",
		const lge::color clear_color = default_clear_color)
		: game_title_(std::move(game_title)), kb_message_(std::move(kb_message)),
		  controller_message_(std::move(controller_message)), clear_color_(clear_color) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;

	[[nodiscard]] static constexpr auto get_game_res() -> glm::vec2 {
		return game_res;
	}
	static constexpr std::size_t fullscreen_action = 0;
	static constexpr std::size_t exit_action = 1;
	static constexpr std::size_t debug_action = 2;
	static constexpr std::size_t total_base_actions = 3;

private:
	static constexpr auto default_clear_color = lge::colors::black;
	lge::color clear_color_ = default_clear_color;
	static constexpr auto game_res = glm::vec2{640, 360}; // world expands horizontally keep the vertical size fixed

	bool was_in_controller_mode_ = false;
	entt::entity message_ent_ = entt::null;

	auto bind_common_actions() -> void;
	auto handle_common_input() -> void;
	auto update_controller_mode_message() -> void;

	std::string game_title_;
	std::string kb_message_;
	std::string controller_message_;
};

} // namespace examples
