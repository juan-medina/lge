// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/scene/scene.hpp>
#include <lge/scene/scene_manager.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <utility>

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
		return ctx.events.post(level_completed{.score = 500});
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
		scene_fixture f;

		must(f.scm.add<gameplay_scene>());
		must(f.scm.add<results_scene>());

		// The app (this test) decides what level_completed means: go to results_scene.
		std::ignore = f.dispatcher.subscribe<level_completed>(
			[&](const level_completed &e) -> lge::result<> { return f.scm.activate<results_scene>(e); });

		must(f.scm.activate<gameplay_scene>());
		test_log.clear();

		// One update: gameplay_scene posts level_completed → dispatcher → results_scene activates
		must(f.scm.update(0.16F));
		require_log({"gameplay_scene::update", "gameplay_scene::on_exit", "results_scene::on_enter:score=500"});

		test_log.clear();

		// Next update runs results_scene
		must(f.scm.update(0.16F));
		require_log({"results_scene::update"});
	}
}