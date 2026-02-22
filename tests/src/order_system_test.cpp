// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/order.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/internal/systems/order_system.hpp>

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>

namespace {

auto make_system(entt::registry &world) -> lge::order_system {
	return lge::order_system{lge::phase::global_update, world};
}

} // namespace

// =============================================================================
// Root entity
// =============================================================================

TEST_CASE("order: root entity", "[order]") {
	entt::registry world;
	auto system = make_system(world);

	SECTION("order component produces correct render_order") {
		const auto e = world.create();
		world.emplace<lge::order>(e, lge::order{.layer = 2, .index = 5});
		REQUIRE(!system.update(0.F).has_error());
		const auto &ro = world.get<lge::render_order>(e);
		REQUIRE(ro.layer == 2);
		REQUIRE(ro.index == 5);
	}

	SECTION("entity without order does not get render_order") {
		const auto e = world.create();
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!world.all_of<lge::render_order>(e));
	}
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("order: hierarchy", "[order][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	SECTION("child inherits parent layer") {
		const auto parent = world.create();
		world.emplace<lge::order>(parent, lge::order{.layer = 3, .index = 0});
		const auto child = world.create();
		world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 0});
		lge::attach(world, parent, child);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(world.get<lge::render_order>(child).layer == 3);
	}

	SECTION("child index adds to parent index") {
		const auto parent = world.create();
		world.emplace<lge::order>(parent, lge::order{.layer = 1, .index = 10});
		const auto child = world.create();
		world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 3});
		lge::attach(world, parent, child);
		REQUIRE(!system.update(0.F).has_error());
		const auto &ro = world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 1);
		REQUIRE(ro.index == 13);
	}

	SECTION("child without order component gets parent render_order") {
		const auto parent = world.create();
		world.emplace<lge::order>(parent, lge::order{.layer = 2, .index = 5});
		const auto child = world.create();
		lge::attach(world, parent, child);
		REQUIRE(!system.update(0.F).has_error());
		const auto &ro = world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 2);
		REQUIRE(ro.index == 5);
	}

	SECTION("grandchild accumulates index through full hierarchy") {
		const auto grandparent = world.create();
		world.emplace<lge::order>(grandparent, lge::order{.layer = 1, .index = 10});
		const auto parent = world.create();
		world.emplace<lge::order>(parent, lge::order{.layer = 0, .index = 5});
		lge::attach(world, grandparent, parent);
		const auto child = world.create();
		world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 2});
		lge::attach(world, parent, child);
		REQUIRE(!system.update(0.F).has_error());
		const auto &ro = world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 1);
		REQUIRE(ro.index == 17);
	}
}