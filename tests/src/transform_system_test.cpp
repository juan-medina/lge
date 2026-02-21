// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/internal/systems/transform_system.hpp"
#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/systems/system.hpp>

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>
#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>

using Catch::Approx;

namespace {

constexpr auto tolerance = 1e-4F;

auto make_registry() -> entt::registry {
	return entt::registry{};
}

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
	auto world = make_registry();
	auto system = make_system(world);

	const auto e = add_entity(world, lge::placement{100.F, 200.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, e);
	REQUIRE(pos.x == Approx(100.F).margin(tolerance));
	REQUIRE(pos.y == Approx(200.F).margin(tolerance));
}

TEST_CASE("transform: root entity identity", "[transform]") {
	auto world = make_registry();
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
	auto world = make_registry();
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
	auto world = make_registry();
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{100.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{50.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(150.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
}

TEST_CASE("transform: child inherits parent scale", "[transform][hierarchy]") {
	auto world = make_registry();
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
	const auto child = add_child(world, parent, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}});

	REQUIRE(!system.update(0.F).has_error());

	const auto scale = world_scale(world, child);
	REQUIRE(scale.x == Approx(2.F).margin(tolerance));
	REQUIRE(scale.y == Approx(2.F).margin(tolerance));
}

TEST_CASE("transform: child position scaled by parent", "[transform][hierarchy]") {
	auto world = make_registry();
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(20.F).margin(tolerance));
	REQUIRE(pos.y == Approx(0.F).margin(tolerance));
}

TEST_CASE("transform: child inherits parent rotation", "[transform][hierarchy]") {
	auto world = make_registry();
	auto system = make_system(world);

	// Child at (10, 0) relative to parent rotated 90 degrees -> world pos should be (0, 10)
	const auto parent = add_entity(world, lge::placement{0.F, 0.F, 90.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	const auto pos = world_pos(world, child);
	REQUIRE(pos.x == Approx(0.F).margin(tolerance));
	REQUIRE(pos.y == Approx(10.F).margin(tolerance));
}

// =============================================================================
// Hierarchy events
// =============================================================================

TEST_CASE("transform: detaching child removes it from parent children list", "[transform][hierarchy]") {
	auto world = make_registry();
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	world.erase<lge::parent>(child);

	const auto &kids = world.get<lge::children>(parent).ids;
	REQUIRE(kids.empty());
}

TEST_CASE("transform: destroying parent destroys children", "[transform][hierarchy]") {
	auto world = make_registry();
	auto system = make_system(world);

	const auto parent = add_entity(world, lge::placement{0.F, 0.F});
	const auto child = add_child(world, parent, lge::placement{10.F, 0.F});

	REQUIRE(!system.update(0.F).has_error());

	world.destroy(parent);

	REQUIRE(!world.valid(child));
}