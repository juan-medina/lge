// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hidden.hpp>
#include <lge/internal/components/effective_hidden.hpp>
#include <lge/internal/systems/hidden_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

using fixture = system_fixture<lge::hidden_system>;

auto is_hidden(const entt::registry &world, const entt::entity e) -> bool {
	return world.all_of<lge::effective_hidden>(e);
}

} // namespace

// =============================================================================
// Root entity
// =============================================================================

TEST_CASE("hidden: root entity", "[hidden]") {
	fixture f;

	SECTION("visible entity is not effectively hidden") {
		const auto e = add_entity(f.world);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, e));
	}

	SECTION("hidden entity is effectively hidden") {
		const auto e = add_entity(f.world);
		f.world.emplace<lge::hidden>(e);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(is_hidden(f.world, e));
	}

	SECTION("removing hidden makes entity visible again") {
		const auto e = add_entity(f.world);
		f.world.emplace<lge::hidden>(e);
		REQUIRE(!f.system.update(0.F).has_error());
		f.world.remove<lge::hidden>(e);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, e));
	}
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("hidden: hierarchy", "[hidden][hierarchy]") {
	fixture f;

	SECTION("visible parent does not hide visible child") {
		const auto parent = add_entity(f.world);
		const auto child = add_child(f.world, parent);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, parent));
		REQUIRE(!is_hidden(f.world, child));
	}

	SECTION("hidden parent hides visible child") {
		const auto parent = add_entity(f.world);
		const auto child = add_child(f.world, parent);
		f.world.emplace<lge::hidden>(parent);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(is_hidden(f.world, child));
	}

	SECTION("visible parent does not override hidden child") {
		const auto parent = add_entity(f.world);
		const auto child = add_child(f.world, parent);
		f.world.emplace<lge::hidden>(child);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, parent));
		REQUIRE(is_hidden(f.world, child));
	}

	SECTION("unhiding parent makes child visible again") {
		const auto parent = add_entity(f.world);
		const auto child = add_child(f.world, parent);
		f.world.emplace<lge::hidden>(parent);
		REQUIRE(!f.system.update(0.F).has_error());
		f.world.remove<lge::hidden>(parent);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, child));
	}
}

// =============================================================================
// Deep hierarchy
// =============================================================================

TEST_CASE("hidden: deep hierarchy", "[hidden][hierarchy][deep]") {
	fixture f;

	SECTION("hidden grandparent hides all descendants") {
		const auto grandparent = add_entity(f.world);
		const auto parent = add_child(f.world, grandparent);
		const auto child = add_child(f.world, parent);
		f.world.emplace<lge::hidden>(grandparent);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(is_hidden(f.world, grandparent));
		REQUIRE(is_hidden(f.world, parent));
		REQUIRE(is_hidden(f.world, child));
	}

	SECTION("hidden middle node hides descendants but not ancestors") {
		const auto grandparent = add_entity(f.world);
		const auto parent = add_child(f.world, grandparent);
		const auto child = add_child(f.world, parent);
		f.world.emplace<lge::hidden>(parent);
		REQUIRE(!f.system.update(0.F).has_error());
		REQUIRE(!is_hidden(f.world, grandparent));
		REQUIRE(is_hidden(f.world, parent));
		REQUIRE(is_hidden(f.world, child));
	}
}