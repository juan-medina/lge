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

		dispatcher.on<event_a>([](const event_a &) -> void { test_log.emplace_back("event_a"); });

		dispatcher.post(event_a{});
		require_log({"event_a"});
	}

	SECTION("posting unsubscribed event does nothing") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.post(event_a{});
		require_log({});
	}

	SECTION("posting event passes data to handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_b>(
			[](const event_b &e) -> void { test_log.emplace_back("event_b:" + std::to_string(e.value)); });

		dispatcher.post(event_b{.value = 42});
		require_log({"event_b:42"});
	}

	SECTION("posting event passes string data to handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_c>([](const event_c &e) -> void { test_log.emplace_back("event_c:" + e.text); });

		dispatcher.post(event_c{.text = "hello"});
		require_log({"event_c:hello"});
	}
}

TEST_CASE("dispatcher: multiple event types", "[dispatcher]") {
	SECTION("each event type calls its own handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> void { test_log.emplace_back("event_a"); });
		dispatcher.on<event_b>(
			[](const event_b &e) -> void { test_log.emplace_back("event_b:" + std::to_string(e.value)); });

		dispatcher.post(event_a{});
		dispatcher.post(event_b{.value = 7});
		require_log({"event_a", "event_b:7"});
	}

	SECTION("posting one type does not trigger another type handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> void { test_log.emplace_back("event_a"); });
		dispatcher.on<event_b>([](const event_b &) -> void { test_log.emplace_back("event_b"); });

		dispatcher.post(event_a{});
		require_log({"event_a"});
	}
}

TEST_CASE("dispatcher: resubscription", "[dispatcher]") {
	SECTION("subscribing twice replaces the handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		dispatcher.on<event_a>([](const event_a &) -> void { test_log.emplace_back("first"); });
		dispatcher.on<event_a>([](const event_a &) -> void { test_log.emplace_back("second"); });

		dispatcher.post(event_a{});
		require_log({"second"});
	}
}