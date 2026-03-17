// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>
#include <lge/scene/scene.hpp>
#include <lge/systems/system.hpp>

#include "test_helpers.hpp"

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

// =============================================================================
// Test systems
// =============================================================================

class tracking_system_a: public lge::system {
public:
	using system::system;
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("tracking_system_a::update");
		return true;
	}
	auto end() -> lge::result<> override {
		test_log.emplace_back("tracking_system_a::end");
		return true;
	}
};

class tracking_system_b: public lge::system {
public:
	using system::system;
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("tracking_system_b::update");
		return true;
	}
	auto end() -> lge::result<> override {
		test_log.emplace_back("tracking_system_b::end");
		return true;
	}
};

class error_system: public lge::system {
public:
	using system::system;
	auto update(float /*dt*/) -> lge::result<> override {
		return lge::error("error_system::update failed");
	}
};

struct ctx_marker {};

class ctx_access_system: public lge::system {
public:
	using system::system;
	auto update(float /*dt*/) -> lge::result<> override {
		const auto e = ctx.world.create();
		ctx.world.emplace<ctx_marker>(e);
		test_log.emplace_back("ctx_access_system::update");
		return true;
	}
};

// =============================================================================
// Test scenes
// =============================================================================

class tracking_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		return true;
	}
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("tracking_scene::update");
		return true;
	}
};

class system_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		if(const auto err = register_system<tracking_system_a>().unwrap(); err) [[unlikely]] {
			return lge::error("failed to register tracking_system_a", *err);
		}
		return true;
	}
};

class multi_system_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		if(const auto err = register_system<tracking_system_a>().unwrap(); err) [[unlikely]] {
			return lge::error("failed to register tracking_system_a", *err);
		}
		if(const auto err = register_system<tracking_system_b>().unwrap(); err) [[unlikely]] {
			return lge::error("failed to register tracking_system_b", *err);
		}
		return true;
	}
};

class ordering_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		return register_system<tracking_system_a>();
	}
	auto update(float /*dt*/) -> lge::result<> override {
		test_log.emplace_back("ordering_scene::update");
		return true;
	}
};

class error_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		return register_system<error_system>();
	}
};

class ctx_scene: public lge::scene {
public:
	using scene::scene;
	auto init() -> lge::result<> {
		return register_system<ctx_access_system>();
	}
};

// =============================================================================
// System is updated when tick() is called
// =============================================================================

TEST_CASE("scene_system: system is updated when tick() is called", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	system_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	const auto res = s.tick(0.16F);
	REQUIRE(res.has_value());
	REQUIRE(!test_log.empty());
	REQUIRE(test_log.front() == "tracking_system_a::update");
}

// =============================================================================
// System is not updated after end() is called
// =============================================================================

TEST_CASE("scene_system: system is not updated after end() is called", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	system_scene s{f.ctx};
	REQUIRE(!s.init().has_error());
	REQUIRE(!s.end().has_error());

	test_log.clear();
	const auto res = s.tick(0.16F);
	REQUIRE(res.has_value());
	REQUIRE(test_log.empty());
}

// =============================================================================
// Systems run before scene update()
// =============================================================================

TEST_CASE("scene_system: systems run before scene update()", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	ordering_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	REQUIRE(!s.tick(0.16F).has_error());

	REQUIRE(test_log.size() == 2);
	REQUIRE(test_log.at(0) == "tracking_system_a::update");
	REQUIRE(test_log.at(1) == "ordering_scene::update");
}

// =============================================================================
// Multiple systems run in registration order
// =============================================================================

TEST_CASE("scene_system: multiple systems run in registration order", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	multi_system_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	REQUIRE(!s.tick(0.16F).has_error());

	REQUIRE(test_log.size() >= 2);
	REQUIRE(test_log.at(0) == "tracking_system_a::update");
	REQUIRE(test_log.at(1) == "tracking_system_b::update");
}

// =============================================================================
// Systems are torn down in reverse registration order on end()
// =============================================================================

TEST_CASE("scene_system: systems are torn down in reverse order on end()", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	multi_system_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	test_log.clear();
	REQUIRE(!s.end().has_error());

	REQUIRE(test_log.size() == 2);
	REQUIRE(test_log.at(0) == "tracking_system_b::end");
	REQUIRE(test_log.at(1) == "tracking_system_a::end");
}

// =============================================================================
// Scene-owned system has access to ctx
// =============================================================================

TEST_CASE("scene_system: system has access to ctx", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	ctx_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	REQUIRE(!s.tick(0.16F).has_error());

	REQUIRE(test_log == std::vector<std::string>{"ctx_access_system::update"});
	REQUIRE(!f.world.view<ctx_marker>().empty());
}

// =============================================================================
// Error from system update propagates through tick()
// =============================================================================

TEST_CASE("scene_system: error from system update propagates through tick()", "[scene_system]") {
	test_log.clear();
	scene_fixture f;

	error_scene s{f.ctx};
	REQUIRE(!s.init().has_error());

	const auto res = s.tick(0.16F);
	REQUIRE(res.has_error());
}
