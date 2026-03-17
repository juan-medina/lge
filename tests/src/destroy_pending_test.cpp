// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/destroy_pending.hpp>
#include <lge/components/placement.hpp>
#include <lge/internal/systems/destroy_pending_system.hpp>
#include <lge/internal/systems/transform_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>

namespace {

using simple_fixture = system_fixture<lge::destroy_pending_system>;

struct hierarchy_fixture {
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
	lge::transform_system tsystem{lge::phase::global_update, ctx};
	lge::destroy_pending_system system{lge::phase::local_update, ctx};
};

} // namespace

// =============================================================================
// Deferred destruction
// =============================================================================

TEST_CASE("destroy_pending: entity is valid before processing", "[destroy_pending]") {
	simple_fixture f;
	const auto e = f.world.create();
	f.world.emplace<lge::destroy_pending>(e);
	REQUIRE(f.world.valid(e));
}

TEST_CASE("destroy_pending: entity is destroyed after processing", "[destroy_pending]") {
	simple_fixture f;
	const auto e = f.world.create();
	f.world.emplace<lge::destroy_pending>(e);
	must(f.system.update(0.F));
	REQUIRE(!f.world.valid(e));
}

// =============================================================================
// Hierarchy
// =============================================================================

TEST_CASE("destroy_pending: children are destroyed with pending parent", "[destroy_pending][hierarchy]") {
	hierarchy_fixture f;
	const auto parent = add_entity(f.world, lge::placement{0.F, 0.F});
	const auto child = add_child(f.world, parent, lge::placement{0.F, 0.F});

	f.world.emplace<lge::destroy_pending>(parent);
	REQUIRE(f.world.valid(child));

	must(f.system.update(0.F));

	REQUIRE(!f.world.valid(parent));
	REQUIRE(!f.world.valid(child));
}

TEST_CASE("destroy_pending: cascade-destroyed entity in pending list does not crash", "[destroy_pending]") {
	hierarchy_fixture f;
	const auto parent = add_entity(f.world, lge::placement{0.F, 0.F});
	const auto child = add_child(f.world, parent, lge::placement{0.F, 0.F});

	f.world.emplace<lge::destroy_pending>(parent);
	f.world.emplace<lge::destroy_pending>(child);

	REQUIRE_NOTHROW(f.system.update(0.F));
	REQUIRE(!f.world.valid(parent));
	REQUIRE(!f.world.valid(child));
}
