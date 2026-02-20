// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include "../../src/example.hpp"
#include "entity/fwd.hpp"

#include <cstddef>
#include <entity/entity.hpp>
#include <string>

namespace examples {
class hello_world: public example {
public:
	explicit hello_world(): example(example_title, kb_message, controller_message) {}

	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;

private:
	static constexpr auto example_title = "Hello world!";
	static constexpr auto kb_message = "Press 1: hide hello+world, 2 hide: world, F5 debug draw,\n"
									   "F11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "Press controller A: hide hello+world, X: hide world,\n"
											   "START: debug draw, SELECT: toggle fullscreen, B: exit";

	static constexpr size_t hide_hello_world_action = total_base_actions + 0;
	static constexpr size_t hide_world_action = total_base_actions + 1;

	entt::entity center_ent_ = entt::null;
	entt::entity world_text_ent_ = entt::null;
};

} // namespace examples