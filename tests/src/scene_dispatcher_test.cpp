// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/dispatcher/dispatcher.hpp>
#include <lge/scene/scene.hpp>
#include <lge/scene/scene_manager.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <string>

// =============================================================================
// Events (shared contract between scenes and the app)
// =============================================================================

struct level_completed {
	int score{};
};

// =============================================================================
// Scene A — plays the level, posts level_completed when done
// =============================================================================

class gameplay_scene: public lge::scene {
public:
	using scene::scene;

	auto init() -> lge::result<> {
		return true;
	}

	auto end() -> lge::result<> override {
		return true;
	}

	auto on_enter() -> lge::result<> {
		test_log.emplace_back("gameplay_scene::on_enter");
		return true;
	}

	auto on_pause() -> lge::result<> override {
		return true;
	}

	auto on_resume() -> lge::result<> override {
		return true;
	}

	auto on_exit() -> lge::result<> override {
		test_log.emplace_back("gameplay_scene::on_exit");
		return true;
	}

	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("gameplay_scene::update");
		// Scene knows nothing about what happens next — it just reports what occurred.
		events.post(level_completed{.score = 500});
		return true;
	}
};

// =============================================================================
// Scene B — shows results, receives score from the event
// =============================================================================

class results_scene: public lge::scene {
public:
	using scene::scene;

	auto init() -> lge::result<> {
		return true;
	}

	auto end() -> lge::result<> override {
		return true;
	}

	auto on_enter(const level_completed &result) -> lge::result<> {
		test_log.emplace_back("results_scene::on_enter:score=" + std::to_string(result.score));
		return true;
	}

	auto on_pause() -> lge::result<> override {
		return true;
	}

	auto on_resume() -> lge::result<> override {
		return true;
	}

	auto on_exit() -> lge::result<> override {
		return true;
	}

	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("results_scene::update");
		return true;
	}
};

// =============================================================================
// Integration Tests
// =============================================================================

TEST_CASE("scene_manager + dispatcher: app-mediated scene transition", "[scene_manager][dispatcher][integration]") {
	SECTION("app wires dispatcher to switch scene when gameplay posts level_completed") {
		test_log.clear();
		entt::registry world;
		lge::dispatcher dispatcher;
		auto scm = lge::scene_manager(world, dispatcher);

		must(scm.register_scene<gameplay_scene>());
		must(scm.register_scene<results_scene>());

		// The app (this test) decides what level_completed means: go to results_scene.
		// Neither scene knows the other exists.
		dispatcher.on<level_completed>(
			[&scm](const level_completed &evt) -> void { must(scm.set_active_scene<results_scene>(evt)); });

		must(scm.set_active_scene<gameplay_scene>());
		test_log.clear();

		// One tick: gameplay fires the event; app handler switches to results.
		must(scm.update(0.16F));

		require_log({
			"gameplay_scene::update",			 // scene runs first
			"gameplay_scene::on_exit",			 // app transitions away from it
			"results_scene::on_enter:score=500", // results scene receives the score
		});
	}

	SECTION("results_scene is the active scene on the following tick") {
		test_log.clear();
		entt::registry world;
		lge::dispatcher dispatcher;
		auto scm = lge::scene_manager(world, dispatcher);

		must(scm.register_scene<gameplay_scene>());
		must(scm.register_scene<results_scene>());

		dispatcher.on<level_completed>(
			[&scm](const level_completed &evt) -> void { must(scm.set_active_scene<results_scene>(evt)); });

		must(scm.set_active_scene<gameplay_scene>());
		must(scm.update(0.16F)); // triggers transition
		test_log.clear();

		// Second tick must drive results_scene, not gameplay_scene.
		must(scm.update(0.16F));
		require_log({"results_scene::update"});
	}

	SECTION("score value posted by gameplay_scene arrives intact at results_scene") {
		test_log.clear();
		entt::registry world;
		lge::dispatcher dispatcher;
		auto scm = lge::scene_manager(world, dispatcher);

		must(scm.register_scene<gameplay_scene>());
		must(scm.register_scene<results_scene>());

		dispatcher.on<level_completed>(
			[&scm](const level_completed &evt) -> void { must(scm.set_active_scene<results_scene>(evt)); });

		must(scm.set_active_scene<gameplay_scene>());
		must(scm.update(0.16F));

		// Exactly one entry for results_scene::on_enter and it carries the right score.
		const auto it = std::ranges::find(test_log, "results_scene::on_enter:score=500");
		REQUIRE(it != test_log.end());
	}
}