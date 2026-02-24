// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/internal/raylib/raylib_backend.hpp>
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

auto backend = lge::raylib_backend::create();
auto dispatcher = lge::dispatcher{};

struct test_fixture {
	entt::registry world;
	lge::context ctx;
	lge::transform_system system;

	explicit test_fixture()
		: ctx{
			  .render = *backend.renderer_ptr,
			  .actions = *backend.input_ptr,
			  .resources = *backend.resource_manager_ptr,
			  .world = world,
			  .events = dispatcher,
		  },
		  system{lge::phase::global_update, ctx} {}
};

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

TEST_CASE("transform: root entity", "[transform]") {
	test_fixture f;

	SECTION("position is applied directly") {
		const auto e = add_entity(f.world, lge::placement{100.F, 200.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == 100.F);
		REQUIRE(pos.y == 200.F);
	}

	SECTION("scale is applied directly") {
		const auto e = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {2.F, 3.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto scale = world_scale(f.world, e);
		REQUIRE(scale.x == 2.F);
		REQUIRE(scale.y == 3.F);
	}

	SECTION("rotation is applied directly") {
		const auto e = add_entity(f.world, lge::placement{0.F, 0.F, 45.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &mat = f.world.get<lge::transform>(e).world;
		const float angle = glm::degrees(std::atan2(mat[0][1], mat[0][0]));
		REQUIRE(angle == -45.F);
	}

	SECTION("negative scale does not produce NaN") {
		const auto e = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {-2.F, 3.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto scale = world_scale(f.world, e);
		REQUIRE(scale.x == 2.F);
		REQUIRE(scale.y == 3.F);
	}

	SECTION("zero scale produces finite position") {
		const auto e = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {0.F, 0.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(std::isfinite(pos.x));
		REQUIRE(std::isfinite(pos.y));
	}
}

// =============================================================================
// Pivot with metrics
// =============================================================================

TEST_CASE("transform: pivot with metrics", "[transform][pivot]") {
	test_fixture f;

	SECTION("center pivot offsets by half size") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, {0.5F, 0.5F}});
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 200.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == -50.F);
		REQUIRE(pos.y == -100.F);
	}

	SECTION("top_left pivot produces no offset") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::top_left});
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 200.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == 0.F);
		REQUIRE(pos.y == 0.F);
	}

	SECTION("bottom_right pivot offsets by full size") {
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, lge::pivot::bottom_right});
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 200.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == -100.F);
		REQUIRE(pos.y == -200.F);
	}

	SECTION("manual pivot offsets by fractional size") {
		constexpr glm::vec2 manual_pivot{0.3F, 0.7F};
		const auto e = f.world.create();
		f.world.emplace<lge::placement>(e, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}, manual_pivot});
		f.world.emplace<lge::metrics>(e, lge::metrics{glm::vec2{100.F, 200.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == Approx(-30.F).margin(tolerance));
		REQUIRE(pos.y == Approx(-140.F).margin(tolerance));
	}

	SECTION("entity without metrics uses zero pivot offset") {
		const auto e = add_entity(f.world, lge::placement{10.F, 20.F, 0.F, {1.F, 1.F}, lge::pivot::top_right});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, e);
		REQUIRE(pos.x == 10.F);
		REQUIRE(pos.y == 20.F);
	}
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("transform: child inherits parent transform", "[transform][hierarchy]") {
	test_fixture f;

	SECTION("child position adds to parent position") {
		const auto parent = add_entity(f.world, lge::placement{100.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{50.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == 150.F);
		REQUIRE(pos.y == 0.F);
	}

	SECTION("child scale multiplies parent scale") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
		const auto child = add_child(f.world, parent, lge::placement{0.F, 0.F, 0.F, {1.F, 1.F}});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto scale = world_scale(f.world, child);
		REQUIRE(scale.x == 2.F);
		REQUIRE(scale.y == 2.F);
	}

	SECTION("child position is scaled by parent scale") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F, 0.F, {2.F, 2.F}});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == 20.F);
		REQUIRE(pos.y == 0.F);
	}

	SECTION("child position is rotated by parent rotation") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F, 90.F});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == Approx(0.F).margin(tolerance));
		REQUIRE(pos.y == Approx(10.F).margin(tolerance));
	}

	SECTION("parent with multiple children each get correct position") {
		const auto parent = add_entity(f.world, lge::placement{5.F, 5.F});
		const auto child1 = add_child(f.world, parent, lge::placement{1.F, 0.F});
		const auto child2 = add_child(f.world, parent, lge::placement{0.F, 2.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto pos1 = world_pos(f.world, child1);
		const auto pos2 = world_pos(f.world, child2);
		REQUIRE(pos1.x == 6.F);
		REQUIRE(pos1.y == 5.F);
		REQUIRE(pos2.x == 5.F);
		REQUIRE(pos2.y == 7.F);
	}
}

// =============================================================================
// Deep hierarchy
// =============================================================================

TEST_CASE("transform: deep hierarchy", "[transform][hierarchy][deep]") {
	test_fixture f;

	SECTION("grandchild accumulates all ancestor positions") {
		const auto grandparent = add_entity(f.world, lge::placement{100.F, 0.F});
		const auto parent = add_child(f.world, grandparent, lge::placement{50.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{25.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(world_pos(f.world, grandparent).x == 100.F);
		REQUIRE(world_pos(f.world, parent).x == 150.F);
		REQUIRE(world_pos(f.world, child).x == 175.F);
	}

	SECTION("grandchild accumulates rotation through hierarchy") {
		const auto grandparent = add_entity(f.world, lge::placement{0.F, 0.F, 90.F});
		const auto parent = add_child(f.world, grandparent, lge::placement{10.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto parent_pos = world_pos(f.world, parent);
		REQUIRE(parent_pos.x == Approx(0.F).margin(tolerance));
		REQUIRE(parent_pos.y == Approx(10.F).margin(tolerance));
		const auto child_pos = world_pos(f.world, child);
		REQUIRE(child_pos.x == Approx(0.F).margin(tolerance));
		REQUIRE(child_pos.y == Approx(20.F).margin(tolerance));
	}
}

// =============================================================================
// Hierarchy mutation
// =============================================================================

TEST_CASE("transform: hierarchy mutation", "[transform][hierarchy]") {
	test_fixture f;

	SECTION("detaching child removes it from parent children list") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		f.world.erase<lge::parent>(child);
		REQUIRE(f.world.get<lge::children>(parent).ids.empty());
	}

	SECTION("destroying parent destroys children") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		f.world.destroy(parent);
		REQUIRE(!f.world.valid(child));
	}

	SECTION("destroying child removes it from parent children list") {
		const auto parent = add_entity(f.world, lge::placement{0.F, 0.F});
		const auto child = add_child(f.world, parent, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		f.world.destroy(child);
		const auto &kids = f.world.get<lge::children>(parent).ids;
		REQUIRE(std::ranges::find(kids, child) == kids.end());
	}

	SECTION("child reattached to different parent uses new parent transform") {
		const auto parent1 = add_entity(f.world, lge::placement{0.F, 0.F});
		const auto parent2 = add_entity(f.world, lge::placement{100.F, 0.F});
		const auto child = add_child(f.world, parent1, lge::placement{10.F, 0.F});
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(world_pos(f.world, child).x == 10.F);

		f.world.erase<lge::parent>(child);
		f.world.emplace<lge::parent>(child, parent2);
		if(!f.world.all_of<lge::children>(parent2)) {
			f.world.emplace<lge::children>(parent2);
		}
		f.world.get<lge::children>(parent2).ids.push_back(child);

		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(world_pos(f.world, child).x == 110.F);
	}
}