// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/events/collision.hpp>
#include <lge/interface/resources.hpp>
#include <lge/systems/system.hpp>

#include "dice.hpp"
#include "events.hpp"

#include <array>
#include <cstddef>
#include <entt/core/hashed_string.hpp>
#include <entt/entity/fwd.hpp>
#include <glm/ext/vector_float2.hpp>
#include <random>
#include <string_view>
#include <vector>

namespace examples {

using entt::literals::operator""_hs;

class dice_roller_system: public lge::system {
public:
	using system::system;

	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

private:
	// =============================================================================
	// Wall normals
	// =============================================================================

	static constexpr auto wall_normal_left = glm::vec2{1.0F, 0.0F};
	static constexpr auto wall_normal_right = glm::vec2{-1.0F, 0.0F};
	static constexpr auto wall_normal_top = glm::vec2{0.0F, 1.0F};
	static constexpr auto wall_normal_bottom = glm::vec2{0.0F, -1.0F};

	// =============================================================================
	// Tuning
	// =============================================================================

	static constexpr auto friction = 0.75F;
	static constexpr auto stop_threshold = 5.0F;
	static constexpr auto snap_step = 22.5F;
	static constexpr auto damping = 0.75F;
	static constexpr auto bounce_boost = 1.05F;
	static constexpr auto hit_cooldown_time = 0.1F;
	static constexpr auto throw_cooldown = 0.5F;

	static constexpr auto min_throw_speed = 650.0F;
	static constexpr auto max_throw_speed = 1200.0F;
	static constexpr auto min_rotation_speed = 120.0F;
	static constexpr auto max_rotation_speed = 400.0F;
	static constexpr auto dice_spawn_offset = 40.0F;
	static constexpr auto dice_spawn_jitter = 100.0F;
	static constexpr auto dice_spawn_delay = 0.15F;
	static constexpr auto board_area = 0.6F;

	// =============================================================================
	// Resources
	// =============================================================================

	static constexpr auto dices_path = "resources/game/sprites/dices.json";

	static constexpr auto hit_sound_paths = std::array<std::string_view, 2>{
		"resources/game/sounds/dice_hit1.wav",
		"resources/game/sounds/dice_hit2.wav",
	};

	static constexpr auto faces = std::array{
		"dice_1.png"_hs,
		"dice_2.png"_hs,
		"dice_3.png"_hs,
		"dice_4.png"_hs,
		"dice_5.png"_hs,
		"dice_6.png"_hs,
	};

	lge::sprite_sheet_handle sheet_{};
	glm::vec2 dice_size_{};
	std::array<lge::sound_handle, hit_sound_paths.size()> hit_sounds_{};
	size_t next_hit_sound_{0};
	float hit_cooldown_remaining_{0.0F};

	// =============================================================================
	// Roll tracking
	// =============================================================================

	int dice_in_flight_{0};
	std::vector<int> settled_values_;

	std::mt19937 rng_{std::random_device{}()};

	// =============================================================================
	// Internal methods
	// =============================================================================

	[[nodiscard]] auto on_dice_throw(const dice_throw &evt) -> lge::result<>;
	[[nodiscard]] auto on_collision(const lge::collision &col) -> lge::result<>;
	[[nodiscard]] auto apply_bounce(entt::entity entity, glm::vec2 normal) -> lge::result<>;
	[[nodiscard]] auto bounce_wall(entt::entity entity, lge::placement &plc, const dice &d, float half_w, float half_h)
		-> lge::result<>;
	[[nodiscard]] auto snap_to_rest(lge::placement &plc, dice &d) -> lge::result<>;
	[[nodiscard]] auto check_roll_complete() -> lge::result<>;

	auto spawn_die(glm::vec2 pos, glm::vec2 target_min, glm::vec2 target_max, float delay) -> void;
	auto randomise_face(entt::entity entity, dice &d) -> void;

	static auto apply_friction(dice &d, float dt) -> void;

	static constexpr auto dice_throw_sound_path = "resources/game/sounds/dice_throw.wav";
	lge::sound_handle dice_throw_sound_{};
};

} // namespace examples