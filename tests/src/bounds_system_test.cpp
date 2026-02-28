// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/placement.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/systems/bounds_system.hpp>
#include <lge/internal/systems/transform_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <glm/ext/vector_float2.hpp>

using Catch::Approx;

namespace {

constexpr auto tolerance = 1e-3F;

// Fixture that runs transform_system before bounds_system, matching app.cpp order.
struct bounds_fixture {
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

	auto update() -> void {
		must(transforms.update(0.F));
		must(bounds.update(0.F));
	}
};

} // namespace

// =============================================================================
// Pivot offsets — position 0,0 / no rotation / scale 1,1
// Expected world-space corners are identical to old local values because
// the transform is identity. These tests verify pivot math is still correct.
// =============================================================================

TEST_CASE("bounds: pivot offsets at origin", "[bounds]") {
	bounds_fixture f;

	SECTION("center pivot produces centered quad") {
		add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::center}, {100.F, 200.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(-50.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(-100.F).margin(tolerance));
		REQUIRE(b.p1.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p1.y == Approx(-100.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(100.F).margin(tolerance));
		REQUIRE(b.p3.x == Approx(-50.F).margin(tolerance));
		REQUIRE(b.p3.y == Approx(100.F).margin(tolerance));
	}

	SECTION("top_left pivot produces quad starting at origin") {
		add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::top_left}, {100.F, 200.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(0.F).margin(tolerance));
		REQUIRE(b.p1.x == Approx(100.F).margin(tolerance));
		REQUIRE(b.p1.y == Approx(0.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(100.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(200.F).margin(tolerance));
		REQUIRE(b.p3.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p3.y == Approx(200.F).margin(tolerance));
	}

	SECTION("bottom_right pivot produces quad ending at origin") {
		add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::bottom_right}, {100.F, 200.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(-100.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(-200.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(0.F).margin(tolerance));
	}

	SECTION("zero size produces degenerate quad at pivot world position") {
		add_entity(f.world, lge::placement{50.F, 80.F, 0.F, {1.F, 1.F}, lge::pivot::center}, {0.F, 0.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(80.F).margin(tolerance));
		REQUIRE(b.p1.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p3.x == Approx(50.F).margin(tolerance));
	}
}

// =============================================================================
// World position — bounds translate with entity position
// =============================================================================

TEST_CASE("bounds: world position", "[bounds]") {
	bounds_fixture f;

	SECTION("top_left pivot entity at (100, 50) offsets all corners") {
		add_entity(f.world, lge::placement{100.F, 50.F, 0.F, {1.F, 1.F}, lge::pivot::top_left}, {40.F, 20.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(100.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(50.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(140.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(70.F).margin(tolerance));
	}

	SECTION("center pivot entity at (200, 100) centers quad on position") {
		add_entity(f.world, lge::placement{200.F, 100.F, 0.F, {1.F, 1.F}, lge::pivot::center}, {60.F, 40.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(170.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(80.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(230.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(120.F).margin(tolerance));
	}
}

// =============================================================================
// Scale — bounds grow with entity scale
// =============================================================================

TEST_CASE("bounds: scale", "[bounds]") {
	bounds_fixture f;

	SECTION("2x uniform scale doubles corner distances from pivot") {
		add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}, lge::pivot::top_left}, {50.F, 50.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(0.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(100.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(100.F).margin(tolerance));
	}

	SECTION("non-uniform scale stretches each axis independently") {
		add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {3.F, 1.F}, lge::pivot::top_left}, {10.F, 20.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p2.x == Approx(30.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(20.F).margin(tolerance));
	}
}

// =============================================================================
// Rotation — bounds corners rotate around entity pivot in screen space (CW positive)
// =============================================================================

TEST_CASE("bounds: rotation", "[bounds]") {
	bounds_fixture f;

	SECTION("90 degree CW rotation with top_left pivot swaps width/height into corners") {
		// top_left entity at origin, 90 CW rotation, size 100x0 (a horizontal line)
		// p1 starts at (100, 0) local; after 90 CW rotation it should land at (0, 100)
		add_entity(f.world, lge::placement{0.F, 0.F, 90.F, {1.F, 1.F}, lge::pivot::top_left}, {100.F, 0.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		// p0 is at pivot world = origin
		REQUIRE(b.p0.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(0.F).margin(tolerance));
		// p1 = local (100, 0) rotated 90 CW => (0, 100) in screen space
		REQUIRE(b.p1.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p1.y == Approx(100.F).margin(tolerance));
	}

	SECTION("180 degree rotation with top_left pivot flips corners to negative quadrant") {
		add_entity(f.world, lge::placement{0.F, 0.F, 180.F, {1.F, 1.F}, lge::pivot::top_left}, {100.F, 50.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(0.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(0.F).margin(tolerance));
		// p2 = local (100, 50) rotated 180 => (-100, -50)
		REQUIRE(b.p2.x == Approx(-100.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(-50.F).margin(tolerance));
	}

	SECTION("rotation with center pivot keeps geometric center fixed at entity position") {
		add_entity(f.world, lge::placement{100.F, 100.F, 90.F, {1.F, 1.F}, lge::pivot::center}, {60.F, 60.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		// center of quad = average of all four corners must equal (100, 100)
		const auto cx = (b.p0.x + b.p1.x + b.p2.x + b.p3.x) * 0.25F;
		const auto cy = (b.p0.y + b.p1.y + b.p2.y + b.p3.y) * 0.25F;
		REQUIRE(cx == Approx(100.F).margin(tolerance));
		REQUIRE(cy == Approx(100.F).margin(tolerance));
	}
}

// =============================================================================
// Missing components — bounds not created without all required components
// =============================================================================

TEST_CASE("bounds: missing components", "[bounds]") {
	bounds_fixture f;

	SECTION("entity without metrics does not get bounds") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F});
		f.update();
		REQUIRE(!f.world.all_of<lge::bounds>(e));
	}

	SECTION("entity without placement does not get bounds") {
		const auto e = f.world.create();
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 100.F}});
		f.update();
		REQUIRE(!f.world.all_of<lge::bounds>(e));
	}

	SECTION("entity without transform does not get bounds") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F});
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 100.F}});
		// no transform emplaced, bounds_system skips it
		must(f.bounds.update(0.F));
		REQUIRE(!f.world.all_of<lge::bounds>(e));
	}
}

TEST_CASE("bounds: parent with non-center pivot and metrics", "[bounds][regression]") {
	bounds_fixture f;

	SECTION("top_left pivot: bounds start at entity position") {
		// parent at (50,40), top_left pivot, size (100,60)
		// geometry starts at (50,40) — bounds must wrap it exactly there
		add_entity(f.world, lge::placement{50.F, 40.F, 0.F, {1.F, 1.F}, lge::pivot::top_left}, {100.F, 60.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(40.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(150.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(100.F).margin(tolerance));
	}

	SECTION("bottom_right pivot: bounds end at entity position") {
		// parent at (50,40), bottom_right pivot, size (100,60)
		// geometry ends at (50,40) — bounds must wrap it
		add_entity(f.world, lge::placement{50.F, 40.F, 0.F, {1.F, 1.F}, lge::pivot::bottom_right}, {100.F, 60.F});
		f.update();
		const auto &b = f.world.get<lge::bounds>(*f.world.view<lge::bounds>().begin());
		REQUIRE(b.p0.x == Approx(-50.F).margin(tolerance));
		REQUIRE(b.p0.y == Approx(-20.F).margin(tolerance));
		REQUIRE(b.p2.x == Approx(50.F).margin(tolerance));
		REQUIRE(b.p2.y == Approx(40.F).margin(tolerance));
	}
}