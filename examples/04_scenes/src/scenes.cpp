// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "scenes.hpp"

#include <lge/app/main.hpp>
#include <lge/components/order.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/components/sprite.hpp>
#include <lge/components/sprite_animation.hpp>
#include <lge/core/colors.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/interface/resources.hpp>

#include "../../src/example.hpp"

#include <cassert>
#include <entt/entt.hpp>

LGE_MAIN(examples::scenes);

namespace examples {

auto scenes::init() -> lge::result<> {
	if(const auto err = example::init().unwrap(); err) [[unlikely]] {
		return lge::error("error init the app", *err);
	}

	return true;
}

auto scenes::update(const float dt) -> lge::result<> {
	return example::update(dt);
}

auto scenes::end() -> lge::result<> {
	return example::end();
}

} // namespace examples

