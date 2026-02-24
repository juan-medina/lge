// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/dispatcher/dispatcher.hpp>

#include "test_helpers.hpp"

// =============================================================================
// Test Events
// =============================================================================

struct event_a {};

struct event_b {
	int value{};
};

struct event_c {
	std::string text;
};

// =============================================================================
// Subscription Tests
// =============================================================================

TEST_CASE("dispatcher: posting events", "[dispatcher]") {
	SECTION("posting subscribed event calls handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("event_a");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"event_a"});
	}

	SECTION("posting unsubscribed event does nothing") {
		test_log.clear();
		lge::dispatcher dispatcher;

		must(dispatcher.post(event_a{}));
		require_log({});
	}

	SECTION("posting event passes data to handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_b>([](const event_b &e) -> lge::result<> {
			test_log.emplace_back("event_b:" + std::to_string(e.value));
			return true;
		});

		must(dispatcher.post(event_b{.value = 42}));
		require_log({"event_b:42"});
	}

	SECTION("posting event passes string data to handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_c>([](const event_c &e) -> lge::result<> {
			test_log.emplace_back("event_c:" + e.text);
			return true;
		});

		must(dispatcher.post(event_c{.text = "hello"}));
		require_log({"event_c:hello"});
	}
}

TEST_CASE("dispatcher: multiple event types", "[dispatcher]") {
	SECTION("each event type calls its own handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("event_a");
			return true;
		});
		dispatcher.on<event_b>([](const event_b &e) -> lge::result<> {
			test_log.emplace_back("event_b:" + std::to_string(e.value));
			return true;
		});

		must(dispatcher.post(event_a{}));
		must(dispatcher.post(event_b{.value = 7}));
		require_log({"event_a", "event_b:7"});
	}

	SECTION("posting one type does not trigger another type handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("event_a");
			return true;
		});
		dispatcher.on<event_b>([](const event_b &) -> lge::result<> {
			test_log.emplace_back("event_b");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"event_a"});
	}
}

TEST_CASE("dispatcher: resubscription", "[dispatcher]") {
	SECTION("subscribing twice replaces the handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("first");
			return true;
		});
		dispatcher.on<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("second");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"second"});
	}
}

TEST_CASE("dispatcher: error propagation", "[dispatcher]") {
	SECTION("error returned from handler is propagated by post") {
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> lge::result<> { return lge::error("handler failed"); });

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_error());
	}

	SECTION("post with no handler returns success") {
		lge::dispatcher dispatcher;

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_value());
	}
}