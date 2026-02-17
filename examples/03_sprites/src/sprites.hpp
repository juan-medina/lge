// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include "../../src/example.hpp"

#include <entity/entity.hpp>
#include <entity/fwd.hpp>
#include <random>

class sprites: public example {
public:
	explicit sprites(): example(example_title, kb_message, controller_message) {}
	auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;

private:
	static constexpr auto example_title = "Sprites!";
	static constexpr auto kb_message = "draws a single sprite in the center";
	static constexpr auto controller_message = "draws a single sprite in the center";

	entt::entity rect_entity_{entt::null};
};
