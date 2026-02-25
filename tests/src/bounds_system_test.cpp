// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/placement.hpp>
#include <lge/internal/components/bounds.hpp>
#include <lge/internal/systems/bounds_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

using fixture = system_fixture<lge::bounds_system>;

} // namespace

// =============================================================================
// Bounds calculation
// =============================================================================

TEST_CASE("bounds: pivot offsets", "[bounds]") {
	fixture f;

	SECTION("center pivot produces centered quad") {
		const auto e =
			add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::center}, {100.F, 200.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &b = f.world.get<lge::bounds>(e);
		REQUIRE(b.p0 == glm::vec2{-50.F, -100.F});
		REQUIRE(b.p1 == glm::vec2{50.F, -100.F});
		REQUIRE(b.p2 == glm::vec2{50.F, 100.F});
		REQUIRE(b.p3 == glm::vec2{-50.F, 100.F});
	}

	SECTION("top_left pivot produces quad starting at origin") {
		const auto e =
			add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::top_left}, {100.F, 200.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &b = f.world.get<lge::bounds>(e);
		REQUIRE(b.p0 == glm::vec2{0.F, 0.F});
		REQUIRE(b.p1 == glm::vec2{100.F, 0.F});
		REQUIRE(b.p2 == glm::vec2{100.F, 200.F});
		REQUIRE(b.p3 == glm::vec2{0.F, 200.F});
	}

	SECTION("bottom_right pivot produces quad ending at origin") {
		const auto e =
			add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::bottom_right}, {100.F, 200.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &b = f.world.get<lge::bounds>(e);
		REQUIRE(b.p0 == glm::vec2{-100.F, -200.F});
		REQUIRE(b.p1 == glm::vec2{0.F, -200.F});
		REQUIRE(b.p2 == glm::vec2{0.F, 0.F});
		REQUIRE(b.p3 == glm::vec2{-100.F, 0.F});
	}

	SECTION("zero size produces degenerate quad at pivot") {
		const auto e = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::center}, {0.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &b = f.world.get<lge::bounds>(e);
		REQUIRE(b.p0 == glm::vec2{0.F, 0.F});
		REQUIRE(b.p1 == glm::vec2{0.F, 0.F});
		REQUIRE(b.p2 == glm::vec2{0.F, 0.F});
		REQUIRE(b.p3 == glm::vec2{0.F, 0.F});
	}
}

TEST_CASE("bounds: missing components", "[bounds]") {
	fixture f;

	SECTION("entity without metrics does not get bounds") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!f.world.all_of<lge::bounds>(e));
	}

	SECTION("entity without placement does not get bounds") {
		const auto e = f.world.create();
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 100.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!f.world.all_of<lge::bounds>(e));
	}
}