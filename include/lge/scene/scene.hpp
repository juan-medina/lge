// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include <entt/core/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace lge {

class scene {
public:
	explicit scene(const entt::id_type id, entt::registry &the_world): id_{id}, world{the_world} {};
	virtual ~scene() = default;

	scene(const scene &) = delete;
	scene(scene &&) = delete;
	auto operator=(const scene &) -> scene & = delete;
	auto operator=(scene &&) -> scene & = delete;

	[[nodiscard]] virtual auto end() -> result<> = 0;

	[[nodiscard]] virtual auto on_pause() -> result<> = 0;
	[[nodiscard]] virtual auto on_resume() -> result<> = 0;
	[[nodiscard]] virtual auto on_exit() -> result<> = 0;

	[[nodiscard]] virtual auto update(float dt) -> result<> = 0;

	[[nodiscard]] auto get_id() const noexcept -> entt::id_type {
		return id_;
	}

protected:
	entt::registry &world;

private:
	entt::id_type id_ = entt::null;
};

} // namespace lge