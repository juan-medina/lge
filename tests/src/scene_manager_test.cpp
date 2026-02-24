// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/hierarchy.hpp>
#include <lge/components/order.hpp>
#include <lge/dispatcher/dispatcher.hpp>
#include <lge/internal/components/render_order.hpp>
#include <lge/internal/raylib/raylib_backend.hpp>
#include <lge/scene/scene.hpp>
#include <lge/scene/scene_manager.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <entt/entt.hpp>
#include <string>
#include <vector>

namespace {

auto backend = lge::raylib_backend::create();
auto dispatcher = lge::dispatcher{};

struct test_fixture {
	entt::registry world;
	lge::context ctx;
	lge::scene_manager scm;

	explicit test_fixture()
		: ctx{
			  .render = *backend.renderer_ptr,
			  .actions = *backend.input_ptr,
			  .resources = *backend.resource_manager_ptr,
			  .world = world,
			  .events = dispatcher,
		  },
		  scm{ctx} {}
};

} // namespace
// =============================================================================
// Test Scenes
// =============================================================================

class test_scene1: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		test_log.emplace_back("test_scene1::init");
		return true;
	}
	auto end() -> lge::result<> override {
		test_log.emplace_back("test_scene1::end");
		return true;
	}
	auto on_enter() -> lge::result<> {
		test_log.emplace_back("test_scene1::on_enter");
		return true;
	}
	auto on_pause() -> lge::result<> override {
		test_log.emplace_back("test_scene1::on_pause");
		return true;
	}
	auto on_resume() -> lge::result<> override {
		test_log.emplace_back("test_scene1::on_resume");
		return true;
	}
	auto on_exit() -> lge::result<> override {
		test_log.emplace_back("test_scene1::on_exit");
		return true;
	}
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("test_scene1::update");
		return true;
	}
};

class test_scene2: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		test_log.emplace_back("test_scene2::init");
		return true;
	}
	auto end() -> lge::result<> override {
		test_log.emplace_back("test_scene2::end");
		return true;
	}
	auto on_enter() -> lge::result<> {
		test_log.emplace_back("test_scene2::on_enter");
		return true;
	}
	auto on_pause() -> lge::result<> override {
		test_log.emplace_back("test_scene2::on_pause");
		return true;
	}
	auto on_resume() -> lge::result<> override {
		test_log.emplace_back("test_scene2::on_resume");
		return true;
	}
	auto on_exit() -> lge::result<> override {
		test_log.emplace_back("test_scene2::on_exit");
		return true;
	}
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("test_scene2::update");
		return true;
	}
};

struct enter_data {
	int value{};
};

class test_scene_with_data: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		test_log.emplace_back("test_scene_with_data::init");
		return true;
	}
	auto end() -> lge::result<> override {
		test_log.emplace_back("test_scene_with_data::end");
		return true;
	}
	auto on_enter(const enter_data &data) -> lge::result<> {
		test_log.emplace_back("test_scene_with_data::on_enter:" + std::to_string(data.value));
		return true;
	}
	auto on_pause() -> lge::result<> override {
		return true;
	}
	auto on_resume() -> lge::result<> override {
		return true;
	}
	auto on_exit() -> lge::result<> override {
		test_log.emplace_back("test_scene_with_data::on_exit");
		return true;
	}
	auto update(float /*dt*/) -> lge::result<> override {
		return true;
	}
};

// =============================================================================
// Registration Tests
// =============================================================================

TEST_CASE("scene_manager: registering scenes", "[scene_manager]") {
	SECTION("registering one scene succeeds and calls init") {
		test_log.clear();
		test_fixture f;

		const auto res = f.scm.add<test_scene1>();
		REQUIRE(res.has_value());
		REQUIRE(f.scm.size() == 1);
		require_log({"test_scene1::init"});
	}

	SECTION("registering multiple scenes calls init on each") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		const auto res = f.scm.add<test_scene2>();
		REQUIRE(res.has_value());
		REQUIRE(f.scm.size() == 2);
		require_log({"test_scene1::init", "test_scene2::init"});
	}

	SECTION("registering same scene twice returns error") {
		test_log.clear();
		test_fixture f;
		;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.add<test_scene1>();
		REQUIRE(res.has_error());
		REQUIRE(f.scm.size() == 1);
		require_log({});
	}
}

// =============================================================================
// Scene Switching Tests
// =============================================================================

TEST_CASE("scene_manager: changing scene", "[scene_manager]") {
	SECTION("first activation calls on_enter") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.activate<test_scene1>();
		REQUIRE(res.has_value());
		require_log({"test_scene1::on_enter"});
	}

	SECTION("switching scenes calls on_exit then on_enter") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		must(f.scm.add<test_scene2>());
		must(f.scm.activate<test_scene1>());
		test_log.clear();

		const auto res = f.scm.activate<test_scene2>();
		REQUIRE(res.has_value());
		require_log({"test_scene1::on_exit", "test_scene2::on_enter"});
	}

	SECTION("switching to non-existing scene returns error") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.activate<test_scene2>();
		REQUIRE(res.has_error());
		require_log({});
	}

	SECTION("on_enter receives data passed to activate") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene_with_data>());
		test_log.clear();

		const auto res = f.scm.activate<test_scene_with_data>(enter_data{.value = 42});
		REQUIRE(res.has_value());
		require_log({"test_scene_with_data::on_enter:42"});
	}
}

// =============================================================================
// Update Tests
// =============================================================================

TEST_CASE("scene_manager: update", "[scene_manager]") {
	SECTION("update calls update on active scene only") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		must(f.scm.add<test_scene2>());
		must(f.scm.activate<test_scene1>());
		test_log.clear();

		const auto res = f.scm.update(0.16F);
		REQUIRE(res.has_value());
		require_log({"test_scene1::update"});
	}

	SECTION("update with no active scene does nothing") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.update(0.16F);
		REQUIRE(res.has_value());
		require_log({});
	}
}

// =============================================================================
// Pause / Resume Tests
// =============================================================================

TEST_CASE("scene_manager: pause and resume", "[scene_manager]") {
	SECTION("pause calls on_pause on active scene") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		must(f.scm.activate<test_scene1>());
		test_log.clear();

		const auto res = f.scm.pause_active_scene();
		REQUIRE(res.has_value());
		require_log({"test_scene1::on_pause"});
	}

	SECTION("resume calls on_resume on active scene") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		must(f.scm.activate<test_scene1>());
		test_log.clear();

		const auto res = f.scm.resume_active_scene();
		REQUIRE(res.has_value());
		require_log({"test_scene1::on_resume"});
	}

	SECTION("pause with no active scene does nothing") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.pause_active_scene();
		REQUIRE(res.has_value());
		require_log({});
	}

	SECTION("resume with no active scene does nothing") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		test_log.clear();

		const auto res = f.scm.resume_active_scene();
		REQUIRE(res.has_value());
		require_log({});
	}
}

// =============================================================================
// End Tests
// =============================================================================

TEST_CASE("scene_manager: end", "[scene_manager]") {
	SECTION("end calls end on all registered scenes") {
		test_log.clear();
		test_fixture f;

		must(f.scm.add<test_scene1>());
		must(f.scm.add<test_scene2>());
		must(f.scm.activate<test_scene1>());
		test_log.clear();

		const auto res = f.scm.end();
		REQUIRE(res.has_value());
		REQUIRE(test_log.size() == 2);
		REQUIRE(std::ranges::find(test_log, "test_scene1::end") != test_log.end());
		REQUIRE(std::ranges::find(test_log, "test_scene2::end") != test_log.end());
	}
}