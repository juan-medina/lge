// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

#include <cstdint>
#include <entity/fwd.hpp>

namespace lge {

using system_id = uint32_t;

enum class phase : std::uint8_t { game_update, local_update, global_update, render, post_render };

class system {
public:
	explicit system(const phase p, entt::registry &w): world{w}, phase_{p} {}
	virtual ~system() = default;

	// Disable copying and moving — apps are not copyable or movable
	system(const system &) = delete;
	system(system &&) = delete;
	auto operator=(const system &) -> system & = delete;
	auto operator=(system &&) -> system & = delete;

	// Called every frame when the system is active
	virtual auto update(float dt) -> result<> = 0;

	[[nodiscard]] auto get_phase() const -> phase {
		return phase_;
	}

protected:
	entt::registry &world;

private:
	phase phase_;
};

} // namespace lge
