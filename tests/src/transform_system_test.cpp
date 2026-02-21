// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/internal/systems/transform_system.hpp>
#include <lge/systems/system.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <cmath>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <glm/trigonometric.hpp>
#include <ranges>

using Catch::Approx;

namespace {

constexpr auto tolerance = 1e-4F;

auto make_system(entt::registry &world) -> lge::transform_system {
	return lge::transform_system{lge::phase::global_update, world};
}

auto add_entity(entt::registry &world, const lge::placement &p) -> entt::entity {
	const auto e = world.create();
	world.emplace<lge::placement>(e, p);
	return e;
}

auto add_child(entt::registry &world, const entt::entity parent_entity, const lge::placement &p) -> entt::entity {
	const auto child = world.create();
	world.emplace<lge::placement>(child, p);
	world.emplace<lge::parent>(child, parent_entity);

	if(!world.all_of<lge::children>(parent_entity)) {
		world.emplace<lge::children>(parent_entity);
	}
	world.get<lge::children>(parent_entity).ids.push_back(child);

	return child;
}

auto world_pos(const entt::registry &world, const entt::entity e) -> glm::vec2 {
	const auto &mat = world.get<lge::transform>(e).world;
	return {mat[2][0], mat[2][1]};
}

auto world_scale(const entt::registry &world, const entt::entity e) -> glm::vec2 {
	const auto &mat = world.get<lge::transform>(e).world;
	return {glm::length(glm::vec2{mat[0][0], mat[0][1]}), glm::length(glm::vec2{mat[1][0], mat[1][1]})};
}

} // namespace

// =============================================================================
// Root entity
// =============================================================================

TEST_CASE("transform: root entity position", "[transform]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{100.F, 200.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(100.F).margin(tolerance));
	REQUIRE(pos.y == Approx(200.F).margin(tolerance));
}

TEST_CASE("transform: root entity identity", "[transform]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, e);
	const auto scale = world_scale(world, e);
	REQUIRE(pos.x == Approx(0.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
	REQUIRE(scale.x == Approx(1.F).margin(tolerance));
	REQUIRE(scale.y == Approx(1.F).margin(tolerance));
}

TEST_CASE("transform: root entity scale", "[transform]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{0.F, 0.F, 0.F, {2.F, 3.F}});

	REQUIRE(!system.update(0.F).has_error());

	const auto scale = world_scale(world, e);
	REQUIRE(scale.x == Approx(2.F).margin(tolerance));
	REQUIRE(scale.y == Approx(3.F).margin(tolerance));
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("transform: child inherits parent position", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{100.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{50.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(150.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
}

TEST_CASE("transform: child inherits parent scale", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
	const auto child = add_child(world, parent, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}});

	REQUIRE(!system.update(0.F).has_error());

	const auto scale = world_scale(world, child);
	REQUIRE(scale.x == Approx(2.F).margin(tolerance));
	REQUIRE(scale.y == Approx(2.F).margin(tolerance));
}

TEST_CASE("transform: child position scaled by parent", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(20.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
}

TEST_CASE("transform: child inherits parent rotation", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	// Child at (10, 0) relative to parent rotated 90 degrees -> world pos should be (0, 10)
	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 90.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(0.F).margin(tolerance));
	REQUIRE(pos.y == Approx(10.F).margin(tolerance));
}

TEST_CASE("transform: root entity rotation", "[transform]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{0.f, 0.f, 45.f});

	REQUIRE(!system.update(0.f).has_error());

	const auto &mat = world.get<lge::transform>(e).world;
	const float angle = glm::degrees(std::atan2(mat[0][1], mat[0][0]));
	REQUIRE(angle == Approx(-45.f).margin(tolerance));
}

// =============================================================================
// Hierarchy events
// =============================================================================

TEST_CASE("transform: detaching child removes it from parent children list", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	world.erase<lge::parent>(child);

	const auto &kids = world.get<lge::children>(parent).ids;
	REQUIRE(kids.empty());
}

TEST_CASE("transform: destroying parent destroys children", "[transform][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	world.destroy(parent);

	REQUIRE(!world.valid(child));
}

TEST_CASE("transform: pivot offset with metrics", "[transform][pivot][metrics]") {
	using lge::metrics;
	using lge::placement;
	using lge::transform;

	entt::registry world;
	world.storage<metrics>();
	auto system = make_system(world);

	const auto e = world.create();
	world.emplace<placement>(e, placement{0.F, 0.F, 0.F, {1.F, 1.F}, {0.5F, 0.5F}});
	world.emplace<metrics>(e, metrics{glm::vec2{100.F, 200.F}});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(-50.F).margin(tolerance));
	REQUIRE(pos.y == Approx(-100.F).margin(tolerance));
}

TEST_CASE("transform: pivot offset with metrics (top_left)", "[transform][pivot][metrics]") {
	using lge::metrics;
	using lge::placement;
	entt::registry world;
	world.storage<metrics>();
	auto system = make_system(world);

	const auto e = world.create();
	world.emplace<placement>(e, placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::top_left});
	world.emplace<metrics>(e, metrics{glm::vec2{100.F, 200.F}});

	REQUIRE(!system.update(0.F).has_error());
	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(0.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
}

TEST_CASE("transform: pivot offset with metrics (bottom_right)", "[transform][pivot][metrics]") {
	using lge::metrics;
	using lge::placement;
	entt::registry world;
	world.storage<metrics>();
	auto system = make_system(world);

	const auto e = world.create();
	world.emplace<placement>(e, placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::bottom_right});
	world.emplace<metrics>(e, metrics{glm::vec2{100.F, 200.F}});

	REQUIRE(!system.update(0.F).has_error());
	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(-100.F).margin(tolerance));
	REQUIRE(pos.y == Approx(-200.F).margin(tolerance));
}

TEST_CASE("transform: entity without metrics uses zero pivot offset", "[transform][pivot][metrics]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{10.F, 20.F, 0.F, {1.F, 1.F}, lge::pivot::top_right});
	// No metrics component
	REQUIRE(!system.update(0.F).has_error());
	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(10.F).margin(tolerance));
	REQUIRE(pos.y == Approx(20.F).margin(tolerance));
}

TEST_CASE("transform: parent with multiple children", "[transform][hierarchy][children]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{5.F, 5.F});
	const auto child1 = add_child(world, parent, lge::placement{1.F, 0.F});
	const auto child2 = add_child(world, parent, lge::placement{0.F, 2.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos1 = world_pos(world, child1);
	const auto pos2 = world_pos(world, child2);
	REQUIRE(pos1.x == Approx(6.F).margin(tolerance));
	REQUIRE(pos1.y == Approx(5.F).margin(tolerance));
	REQUIRE(pos2.x == Approx(5.F).margin(tolerance));
	REQUIRE(pos2.y == Approx(7.F).margin(tolerance));
}

TEST_CASE("transform: detach child and reattach to different parent", "[transform][hierarchy][reattach]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent1 = add_entity(world, lge::placement{0.F, 0.F});
	const auto parent2 = add_entity(world, lge::placement{100.F, 0.F});
	const auto child = add_child(world, parent1, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());
	REQUIRE(world_pos(world, child).x == Approx(10.F).margin(tolerance));

	// Detach from parent1
	world.erase<lge::parent>(child);
	// Attach to parent2
	world.emplace<lge::parent>(child, parent2);
	if(!world.all_of<lge::children>(parent2)) {
		world.emplace<lge::children>(parent2);
	}
	world.get<lge::children>(parent2).ids.push_back(child);

	REQUIRE(!system.update(0.F).has_error());
	REQUIRE(world_pos(world, child).x == Approx(110.F).margin(tolerance));
}

TEST_CASE("transform: destroying child updates parent's children list", "[transform][hierarchy][destroy]") {
	entt::registry world;
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());
	world.destroy(child);
	const auto &kids = world.get<lge::children>(parent).ids;
	REQUIRE(std::ranges::find(kids, child) == kids.end());
}

TEST_CASE("transform: negative scale (mirroring)", "[transform][scale][negative]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{0.F, 0.F, 0.F, {-2.F, 3.F}});
	REQUIRE(!system.update(0.F).has_error());
	const auto scale = world_scale(world, e);
	REQUIRE(scale.x == Approx(2.F).margin(tolerance));
	REQUIRE(scale.y == Approx(3.F).margin(tolerance));
}

TEST_CASE("transform: zero scale does not produce NaN", "[transform][scale][zero]") {
	entt::registry world;
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{0.F, 0.F, 0.F, {0.F, 0.F}});
	REQUIRE(!system.update(0.F).has_error());
	const auto scale = world_scale(world, e);
	REQUIRE(scale.x == Approx(0.F).margin(tolerance));
	REQUIRE(scale.y == Approx(0.F).margin(tolerance));
	const auto pos = world_pos(world, e);
	REQUIRE(std::isfinite(pos.x));
	REQUIRE(std::isfinite(pos.y));
}

TEST_CASE("transform: pivot offset with metrics (manual pivot)", "[transform][pivot][metrics]") {
	using lge::metrics;
	using lge::placement;

	entt::registry world;
	world.storage<metrics>();
	auto system = make_system(world);

	constexpr glm::vec2 manual_pivot{0.3F, 0.7F};
	const auto e = world.create();
	world.emplace<placement>(e, placement{0.F, 0.F, 0.F, {1.F, 1.F}, manual_pivot});
	world.emplace<metrics>(e, metrics{glm::vec2{100.F, 200.F}});

	REQUIRE(!system.update(0.F).has_error());
	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(-30.F).margin(tolerance));
	REQUIRE(pos.y == Approx(-140.F).margin(tolerance));
}
