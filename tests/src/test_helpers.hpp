// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/context.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/components/placement.hpp>
#include <lge/core/result.hpp>
#include <lge/dispatcher/dispatcher.hpp>
#include <lge/internal/components/metrics.hpp>
#include <lge/internal/components/transform.hpp>
#include <lge/internal/raylib/raylib_backend.hpp>
#include <lge/scene/scene_manager.hpp>
#include <lge/systems/system.hpp>

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <glm/ext/vector_float2.hpp>
#include <glm/geometric.hpp>
#include <string>
#include <vector>

inline std::vector<std::string> test_log;

inline auto require_log(const std::vector<std::string> &expected) -> void {
	std::string actual_str;
	for(const auto &e: test_log) {
		actual_str += "  " + e + "\n";
	}
	std::string expected_str;
	for(const auto &e: expected) {
		expected_str += "  " + e + "\n";
	}
	INFO("actual:\n" << actual_str);
	INFO("expected:\n" << expected_str);
	REQUIRE(test_log == expected);
}

// Asserts setup steps succeed without re-testing their logic
inline auto must(const lge::result<> &res) -> void {
	REQUIRE(res.has_value());
}

// =============================================================================
// Shared test fixtures
// =============================================================================

// Fixture for tests that exercise a single system.
// System_T must be constructible as: System_T{lge::phase, lge::context&}.
template<typename System_T>
struct system_fixture {
	lge::backend backend{lge::raylib_backend::create()};
	lge::dispatcher dispatcher{};
	entt::registry world{};
	lge::context ctx{
		.render = *backend.renderer_ptr,
		.actions = *backend.input_ptr,
		.resources = *backend.resource_manager_ptr,
		.world = world,
		.events = dispatcher,
	};
	System_T system{lge::phase::global_update, ctx};
};

// Fixture for tests that exercise the scene manager.
struct scene_fixture {
	lge::backend backend{lge::raylib_backend::create()};
	lge::dispatcher dispatcher{};
	entt::registry world{};
	lge::context ctx{
		.render = *backend.renderer_ptr,
		.actions = *backend.input_ptr,
		.resources = *backend.resource_manager_ptr,
		.world = world,
		.events = dispatcher,
	};
	lge::scene_manager scm{ctx};
};

// =============================================================================
// Entity helpers
// =============================================================================

// Creates a bare entity with no components.
[[nodiscard]] inline auto add_entity(entt::registry &world) -> entt::entity {
	return world.create();
}

// Creates an entity with a placement component.
[[nodiscard]] inline auto add_entity(entt::registry &world, const lge::placement &p) -> entt::entity {
	const auto e = world.create();
	world.emplace<lge::placement>(e, p);
	return e;
}

// Creates an entity with a placement and a metrics component.
[[nodiscard]] inline auto add_entity(entt::registry &world, const lge::placement &p, const glm::vec2 &size)
	-> entt::entity {
	const auto e = world.create();
	world.emplace<lge::placement>(e, p);
	world.emplace<lge::metrics>(e, lge::metrics{size});
	return e;
}

// Creates a child entity attached to parent (no placement).
[[nodiscard]] inline auto add_child(entt::registry &world, const entt::entity parent_entity) -> entt::entity {
	const auto child = world.create();
	lge::attach(world, parent_entity, child);
	return child;
}

// Creates a child entity with a placement, attached to parent.
[[nodiscard]] inline auto add_child(entt::registry &world, const entt::entity parent_entity, const lge::placement &p)
	-> entt::entity {
	const auto child = world.create();
	world.emplace<lge::placement>(child, p);
	lge::attach(world, parent_entity, child);
	return child;
}

// =============================================================================
// Transform inspection helpers
// =============================================================================

// Extracts the world-space position from a transform component.
[[nodiscard]] inline auto world_pos(const entt::registry &world, const entt::entity e) -> glm::vec2 {
	const auto &mat = world.get<lge::transform>(e).world;
	return {mat[2][0], mat[2][1]};
}

// Extracts the world-space scale from a transform component.
[[nodiscard]] inline auto world_scale(const entt::registry &world, const entt::entity e) -> glm::vec2 {
	const auto &mat = world.get<lge::transform>(e).world;
	return {glm::length(glm::vec2{mat[0][0], mat[0][1]}), glm::length(glm::vec2{mat[1][0], mat[1][1]})};
}