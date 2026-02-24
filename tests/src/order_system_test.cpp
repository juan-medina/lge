// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/app/context.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/order.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/internal/raylib/raylib_backend.hpp>
#include <lge/internal/systems/order_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>

namespace {

auto backend = lge::raylib_backend::create();
auto dispatcher = lge::dispatcher{};

struct test_fixture {
	entt::registry world;
	lge::context ctx;
	lge::order_system system;

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

} // namespace

// =============================================================================
// Root entity
// =============================================================================

TEST_CASE("order: root entity", "[order]") {
	test_fixture f;

	SECTION("order component produces correct render_order") {
		const auto e = f.world.create();
		f.world.emplace<lge::order>(e, lge::order{.layer = 2, .index = 5});
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &ro = f.world.get<lge::render_order>(e);
		REQUIRE(ro.layer == 2);
		REQUIRE(ro.index == 5);
	}

	SECTION("entity without order does not get render_order") {
		const auto e = f.world.create();
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!f.world.all_of<lge::render_order>(e));
	}
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("order: hierarchy", "[order][hierarchy]") {
	test_fixture f;

	SECTION("child inherits parent layer") {
		const auto parent = f.world.create();
		f.world.emplace<lge::order>(parent, lge::order{.layer = 3, .index = 0});
		const auto child = f.world.create();
		f.world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 0});
		lge::attach(f.world, parent, child);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(f.world.get<lge::render_order>(child).layer == 3);
	}

	SECTION("child index adds to parent index") {
		const auto parent = f.world.create();
		f.world.emplace<lge::order>(parent, lge::order{.layer = 1, .index = 10});
		const auto child = f.world.create();
		f.world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 3});
		lge::attach(f.world, parent, child);
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &ro = f.world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 1);
		REQUIRE(ro.index == 13);
	}

	SECTION("child without order component gets parent render_order") {
		const auto parent = f.world.create();
		f.world.emplace<lge::order>(parent, lge::order{.layer = 2, .index = 5});
		const auto child = f.world.create();
		lge::attach(f.world, parent, child);
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &ro = f.world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 2);
		REQUIRE(ro.index == 5);
	}

	SECTION("grandchild accumulates index through full hierarchy") {
		const auto grandparent = f.world.create();
		f.world.emplace<lge::order>(grandparent, lge::order{.layer = 1, .index = 10});
		const auto parent = f.world.create();
		f.world.emplace<lge::order>(parent, lge::order{.layer = 0, .index = 5});
		lge::attach(f.world, grandparent, parent);
		const auto child = f.world.create();
		f.world.emplace<lge::order>(child, lge::order{.layer = 0, .index = 2});
		lge::attach(f.world, parent, child);
		REQUIRE(!f.system.update(0.F).has_error());
		const auto &ro = f.world.get<lge::render_order>(child);
		REQUIRE(ro.layer == 1);
		REQUIRE(ro.index == 17);
	}
}