// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/collidable.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/components/overlapping.hpp>
#include <lge/internal/systems/bounds_system.hpp>
#include <lge/internal/systems/collision_system.hpp>
#include <lge/internal/systems/transform_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

// Runs the full pipeline that app.cpp uses:
//   transform_system -> bounds_system -> collision_system
struct collision_fixture {
	lge::backend backend{lge::raylib_backend::create()};
	lge::dispatcher dispatcher{};
	entt::registry world{};
	lge::context ctx{
		.render = *backend.renderer_ptr,
		.actions = *backend.input_ptr,
		.resources = *backend.resource_manager_ptr,
		.audio = *backend.audio_manager_ptr,
		.world = world,
		.events = dispatcher,
	};
	lge::transform_system transforms{lge::phase::global_update, ctx};
	lge::bounds_system bounds{lge::phase::global_update, ctx};
	lge::collision_system collisions{lge::phase::global_update, ctx};

	auto update() -> void {
		must(transforms.update(0.F));
		must(bounds.update(0.F));
		must(collisions.update(0.F));
	}

	// Creates a collidable entity at the given position, rotation and size (center pivot).
	[[nodiscard]] auto add_collidable(const float x, const float y, const float rotation, const glm::vec2 &size)
		-> entt::entity {
		const auto e = add_entity(world, lge::placement{x, y, rotation, {1.F, 1.F}, lge::pivot::center}, size);
		world.emplace<lge::collidable>(e);
		return e;
	}

	[[nodiscard]] auto is_overlapping(const entt::entity e) const -> bool {
		return world.all_of<lge::overlapping>(e);
	}
};

} // namespace

// =============================================================================
// Rotation sign regression
//
// Bug: two overlapping objects both at +45 deg were detected as colliding,
// but one at +45 and one at -45 were not — negative rotation was handled
// incorrectly, placing the quad in the wrong world position.
// =============================================================================

TEST_CASE("collision: rotation sign symmetry", "[collision][regression]") {
	collision_fixture f;

	SECTION("+45 and +45 overlapping at same position collide") {
		const auto a = f.add_collidable(0.F, 0.F, 45.F, {60.F, 60.F});
		const auto b = f.add_collidable(0.F, 0.F, 45.F, {60.F, 60.F});
		f.update();
		REQUIRE(f.is_overlapping(a));
		REQUIRE(f.is_overlapping(b));
	}

	SECTION("+45 and -45 overlapping at same position collide") {
		const auto a = f.add_collidable(0.F, 0.F, 45.F, {60.F, 60.F});
		const auto b = f.add_collidable(0.F, 0.F, -45.F, {60.F, 60.F});
		f.update();
		REQUIRE(f.is_overlapping(a));
		REQUIRE(f.is_overlapping(b));
	}

	SECTION("+45 and -45 clearly separated do not collide") {
		const auto a = f.add_collidable(-200.F, 0.F, 45.F, {60.F, 60.F});
		const auto b = f.add_collidable(200.F, 0.F, -45.F, {60.F, 60.F});
		f.update();
		REQUIRE(!f.is_overlapping(a));
		REQUIRE(!f.is_overlapping(b));
	}

	SECTION("+45 and 0 overlapping at same position collide") {
		const auto a = f.add_collidable(0.F, 0.F, 45.F, {60.F, 60.F});
		const auto b = f.add_collidable(0.F, 0.F, 0.F, {60.F, 60.F});
		f.update();
		REQUIRE(f.is_overlapping(a));
		REQUIRE(f.is_overlapping(b));
	}

	SECTION("-45 and 0 overlapping at same position collide") {
		const auto a = f.add_collidable(0.F, 0.F, -45.F, {60.F, 60.F});
		const auto b = f.add_collidable(0.F, 0.F, 0.F, {60.F, 60.F});
		f.update();
		REQUIRE(f.is_overlapping(a));
		REQUIRE(f.is_overlapping(b));
	}

	SECTION("collision is symmetric: +45/-45 result matches -45/+45") {
		// order of entity creation must not affect the outcome
		const auto a1 = f.add_collidable(10.F, 0.F, 45.F, {60.F, 60.F});
		const auto b1 = f.add_collidable(10.F, 0.F, -45.F, {60.F, 60.F});
		f.update();
		const auto both_overlap = f.is_overlapping(a1) && f.is_overlapping(b1);

		// reset
		f.world.destroy(a1);
		f.world.destroy(b1);

		const auto a2 = f.add_collidable(10.F, 0.F, -45.F, {60.F, 60.F});
		const auto b2 = f.add_collidable(10.F, 0.F, 45.F, {60.F, 60.F});
		f.update();
		const auto both_overlap_reversed = f.is_overlapping(a2) && f.is_overlapping(b2);

		REQUIRE(both_overlap == both_overlap_reversed);
	}
}