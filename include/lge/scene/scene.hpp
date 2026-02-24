// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/context.hpp>
#include <lge/core/result.hpp>
#include <lge/dispatcher/dispatcher.hpp>

#include <entt/core/fwd.hpp>
#include <entt/entity/entity.hpp>
#include <entt/entity/fwd.hpp>

namespace lge {

class scene {
public:
	explicit scene(context &the_context): ctx{the_context} {}
	virtual ~scene() = default;

	scene(const scene &) = delete;
	scene(scene &&) = delete;
	auto operator=(const scene &) -> scene & = delete;
	auto operator=(scene &&) -> scene & = delete;

	[[nodiscard]] virtual auto end() -> result<> {
		return true;
	}

	[[nodiscard]] virtual auto on_pause() -> result<> {
		return true;
	}
	[[nodiscard]] virtual auto on_resume() -> result<> {
		return true;
	}
	[[nodiscard]] virtual auto on_exit() -> result<> {
		return true;
	}

	[[nodiscard]] virtual auto update(float) -> result<> {
		return true;
	}

protected:
	context &ctx;

private:
};

} // namespace lge