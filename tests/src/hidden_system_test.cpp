// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/internal/systems/hidden_system.hpp>

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>

namespace {

auto make_system(entt::registry &world) -> lge::hidden_system {
	return lge::hidden_system{lge::phase::game_update, world};
}

auto add_entity(entt::registry &world) -> entt::entity {
	return world.create();
}

auto add_child(entt::registry &world, const entt::entity parent_entity) -> entt::entity {
	const auto child = world.create();
	lge::attach(world, parent_entity, child);
	return child;
}

auto is_hidden(const entt::registry &world, const entt::entity e) -> bool {
	return world.all_of<lge::effective_hidden>(e);
}

} // namespace

// =============================================================================
// Root entity
// =============================================================================

TEST_CASE("hidden: root entity", "[hidden]") {
	entt::registry world;
	auto system = make_system(world);

	SECTION("visible entity is not effectively hidden") {
		const auto e = add_entity(world);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, e));
	}

	SECTION("hidden entity is effectively hidden") {
		const auto e = add_entity(world);
		world.emplace<lge::hidden>(e);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(is_hidden(world, e));
	}

	SECTION("removing hidden makes entity visible again") {
		const auto e = add_entity(world);
		world.emplace<lge::hidden>(e);
		REQUIRE(!system.update(0.F).has_error());
		world.remove<lge::hidden>(e);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, e));
	}
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("hidden: hierarchy", "[hidden][hierarchy]") {
	entt::registry world;
	auto system = make_system(world);

	SECTION("visible parent does not hide visible child") {
		const auto parent = add_entity(world);
		const auto child = add_child(world, parent);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, parent));
		REQUIRE(!is_hidden(world, child));
	}

	SECTION("hidden parent hides visible child") {
		const auto parent = add_entity(world);
		const auto child = add_child(world, parent);
		world.emplace<lge::hidden>(parent);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(is_hidden(world, child));
	}

	SECTION("visible parent does not override hidden child") {
		const auto parent = add_entity(world);
		const auto child = add_child(world, parent);
		world.emplace<lge::hidden>(child);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, parent));
		REQUIRE(is_hidden(world, child));
	}

	SECTION("unhiding parent makes child visible again") {
		const auto parent = add_entity(world);
		const auto child = add_child(world, parent);
		world.emplace<lge::hidden>(parent);
		REQUIRE(!system.update(0.F).has_error());
		world.remove<lge::hidden>(parent);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, child));
	}
}

// =============================================================================
// Deep hierarchy
// =============================================================================

TEST_CASE("hidden: deep hierarchy", "[hidden][hierarchy][deep]") {
	entt::registry world;
	auto system = make_system(world);

	SECTION("hidden grandparent hides all descendants") {
		const auto grandparent = add_entity(world);
		const auto parent = add_child(world, grandparent);
		const auto child = add_child(world, parent);
		world.emplace<lge::hidden>(grandparent);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(is_hidden(world, grandparent));
		REQUIRE(is_hidden(world, parent));
		REQUIRE(is_hidden(world, child));
	}

	SECTION("hidden middle node hides descendants but not ancestors") {
		const auto grandparent = add_entity(world);
		const auto parent = add_child(world, grandparent);
		const auto child = add_child(world, parent);
		world.emplace<lge::hidden>(parent);
		REQUIRE(!system.update(0.F).has_error());
		REQUIRE(!is_hidden(world, grandparent));
		REQUIRE(is_hidden(world, parent));
		REQUIRE(is_hidden(world, child));
	}
}