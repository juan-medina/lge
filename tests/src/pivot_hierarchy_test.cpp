// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/components/shapes.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/internal/systems/metrics_system.hpp>
#include <lge/internal/systems/transform_system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_approx.hpp>
#include <catch2/catch_test_macros.hpp>

using Catch::Approx;

namespace {

constexpr auto tolerance = 1e-3F;

struct pivot_hierarchy_fixture {
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
	lge::metrics_system metrics{lge::phase::game_update, ctx};
	lge::transform_system transforms{lge::phase::global_update, ctx};

	auto update() -> void {
		must(metrics.update(0.F));
		must(transforms.update(0.F));
	}

	[[nodiscard]] auto add_rect(const float x, const float y, const glm::vec2 &pivot, const glm::vec2 &size)
		-> entt::entity {
		const auto e = world.create();
		auto &r = world.emplace<lge::rect>(e);
		r.size = size;
		world.emplace<lge::placement>(e, x, y, 0.F, glm::vec2{1.F, 1.F}, pivot);
		return e;
	}

	[[nodiscard]] auto
	add_rect_child(const entt::entity parent_entity, const float x, const float y, const glm::vec2 &size)
		-> entt::entity {
		const auto e = world.create();
		auto &r = world.emplace<lge::rect>(e);
		r.size = size;
		world.emplace<lge::placement>(e, x, y, 0.F, glm::vec2{1.F, 1.F}, lge::pivot::center);
		lge::attach(world, parent_entity, e);
		return e;
	}
};

} // namespace

// =============================================================================
// Pivot + hierarchy regression
//
// Children anchor to the parent's logical position (0,0) regardless of pivot.
// The parent's pivot only affects where the parent geometry is drawn, not where
// children position themselves.
// =============================================================================

TEST_CASE("transform: child anchors to parent logical position regardless of parent pivot",
		  "[transform][pivot][hierarchy][regression]") {
	pivot_hierarchy_fixture f;

	SECTION("center pivot parent: child at local (0,0) lands at parent logical pos minus child half-size") {
		const auto parent = f.add_rect(0.F, 0.F, lge::pivot::center, {300.F, 60.F});
		const auto child = f.add_rect_child(parent, 0.F, 0.F, {50.F, 50.F});
		f.update();
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == Approx(-25.F).margin(tolerance));
		REQUIRE(pos.y == Approx(-25.F).margin(tolerance));
	}

	SECTION("top_left pivot parent: child at local (0,0) lands at same position as center pivot parent") {
		const auto parent = f.add_rect(0.F, 0.F, lge::pivot::top_left, {300.F, 60.F});
		const auto child = f.add_rect_child(parent, 0.F, 0.F, {50.F, 50.F});
		f.update();
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == Approx(-25.F).margin(tolerance));
		REQUIRE(pos.y == Approx(-25.F).margin(tolerance));
	}

	SECTION("bottom_right pivot parent: child at local (0,0) lands at same position as center pivot parent") {
		const auto parent = f.add_rect(0.F, 0.F, lge::pivot::bottom_right, {300.F, 60.F});
		const auto child = f.add_rect_child(parent, 0.F, 0.F, {50.F, 50.F});
		f.update();
		const auto pos = world_pos(f.world, child);
		REQUIRE(pos.x == Approx(-25.F).margin(tolerance));
		REQUIRE(pos.y == Approx(-25.F).margin(tolerance));
	}

	SECTION("child local offset is the same regardless of parent pivot") {
		const auto parent_c = f.add_rect(0.F, 0.F, lge::pivot::center, {300.F, 60.F});
		const auto child_c = f.add_rect_child(parent_c, 10.F, 5.F, {50.F, 50.F});
		const auto parent_tl = f.add_rect(0.F, 0.F, lge::pivot::top_left, {300.F, 60.F});
		const auto child_tl = f.add_rect_child(parent_tl, 10.F, 5.F, {50.F, 50.F});
		const auto parent_br = f.add_rect(0.F, 0.F, lge::pivot::bottom_right, {300.F, 60.F});
		const auto child_br = f.add_rect_child(parent_br, 10.F, 5.F, {50.F, 50.F});
		f.update();
		for(const auto child: {child_c, child_tl, child_br}) {
			const auto pos = world_pos(f.world, child);
			REQUIRE(pos.x == Approx(-15.F).margin(tolerance));
			REQUIRE(pos.y == Approx(-20.F).margin(tolerance));
		}
	}
}