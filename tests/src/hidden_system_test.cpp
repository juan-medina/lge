// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "lge/internal/systems/hidden_system.hpp"

#include <lge/components/hidden.hpp>
#include <lge/components/hierarchy.hpp>
#include <lge/internal/components/effective_hidden.hpp>

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

TEST_CASE("hidden: visible entity is not effectively hidden", "[hidden]") {
    entt::registry world;
    auto system = make_system(world);

    const auto e = add_entity(world);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, e));
}

TEST_CASE("hidden: hidden entity is effectively hidden", "[hidden]") {
    entt::registry world;
    auto system = make_system(world);

    const auto e = add_entity(world);
    world.emplace<lge::hidden>(e);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, e));
}

TEST_CASE("hidden: removing hidden makes entity visible again", "[hidden]") {
    entt::registry world;
    auto system = make_system(world);

    const auto e = add_entity(world);
    world.emplace<lge::hidden>(e);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, e));

    world.remove<lge::hidden>(e);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, e));
}

// =============================================================================
// Parent / child hierarchy
// =============================================================================

TEST_CASE("hidden: visible parent does not hide visible child", "[hidden][hierarchy]") {
    entt::registry world;
    auto system = make_system(world);

    const auto parent = add_entity(world);
    const auto child  = add_child(world, parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, parent));
    REQUIRE(!is_hidden(world, child));
}

TEST_CASE("hidden: hidden parent hides visible child", "[hidden][hierarchy]") {
    entt::registry world;
    auto system = make_system(world);

    const auto parent = add_entity(world);
    const auto child  = add_child(world, parent);
    world.emplace<lge::hidden>(parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, parent));
    REQUIRE(is_hidden(world, child));
}

TEST_CASE("hidden: visible parent does not override hidden child", "[hidden][hierarchy]") {
    entt::registry world;
    auto system = make_system(world);

    const auto parent = add_entity(world);
    const auto child  = add_child(world, parent);
    world.emplace<lge::hidden>(child);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, parent));
    REQUIRE(is_hidden(world, child));
}

TEST_CASE("hidden: hidden parent overrides visible child", "[hidden][hierarchy]") {
    entt::registry world;
    auto system = make_system(world);

    const auto parent = add_entity(world);
    const auto child  = add_child(world, parent);
    world.emplace<lge::hidden>(parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, child));
}

TEST_CASE("hidden: unhiding parent makes child visible again", "[hidden][hierarchy]") {
    entt::registry world;
    auto system = make_system(world);

    const auto parent = add_entity(world);
    const auto child  = add_child(world, parent);
    world.emplace<lge::hidden>(parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, child));

    world.remove<lge::hidden>(parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, child));
}

// =============================================================================
// Deep hierarchy
// =============================================================================

TEST_CASE("hidden: hidden grandparent hides grandchild", "[hidden][hierarchy][deep]") {
    entt::registry world;
    auto system = make_system(world);

    const auto grandparent = add_entity(world);
    const auto parent      = add_child(world, grandparent);
    const auto child       = add_child(world, parent);
    world.emplace<lge::hidden>(grandparent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(is_hidden(world, grandparent));
    REQUIRE(is_hidden(world, parent));
    REQUIRE(is_hidden(world, child));
}

TEST_CASE("hidden: hidden middle node hides grandchild but not grandparent", "[hidden][hierarchy][deep]") {
    entt::registry world;
    auto system = make_system(world);

    const auto grandparent = add_entity(world);
    const auto parent      = add_child(world, grandparent);
    const auto child       = add_child(world, parent);
    world.emplace<lge::hidden>(parent);

    REQUIRE(!system.update(0.F).has_error());
    REQUIRE(!is_hidden(world, grandparent));
    REQUIRE(is_hidden(world, parent));
    REQUIRE(is_hidden(world, child));
}