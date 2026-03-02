// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/dispatcher/dispatcher.hpp>

#include "test_helpers.hpp"

#include <utility>

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
// Posting Events
// =============================================================================

TEST_CASE("dispatcher: posting events", "[dispatcher]") {
	SECTION("posting subscribed event calls handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
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

		std::ignore = dispatcher.subscribe<event_b>([](const event_b &e) -> lge::result<> {
			test_log.emplace_back("event_b:" + std::to_string(e.value));
			return true;
		});

		must(dispatcher.post(event_b{.value = 42}));
		require_log({"event_b:42"});
	}

	SECTION("posting event passes string data to handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_c>([](const event_c &e) -> lge::result<> {
			test_log.emplace_back("event_c:" + e.text);
			return true;
		});

		must(dispatcher.post(event_c{.text = "hello"}));
		require_log({"event_c:hello"});
	}
}

// =============================================================================
// Multiple Event Types
// =============================================================================

TEST_CASE("dispatcher: multiple event types", "[dispatcher]") {
	SECTION("each event type calls its own handler") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("event_a");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_b>([](const event_b &e) -> lge::result<> {
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

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("event_a");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_b>([](const event_b &) -> lge::result<> {
			test_log.emplace_back("event_b");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"event_a"});
	}
}

// =============================================================================
// Multiple Subscribers
// =============================================================================

TEST_CASE("dispatcher: multiple subscribers", "[dispatcher]") {
	SECTION("subscribing twice calls both handlers in order") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("first");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("second");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"first", "second"});
	}

	SECTION("three subscribers are all called in subscription order") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("one");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("two");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("three");
			return true;
		});

		must(dispatcher.post(event_a{}));
		require_log({"one", "two", "three"});
	}

	SECTION("multiple subscribers on different event types are independent") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("a:first");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("a:second");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_b>([](const event_b &) -> lge::result<> {
			test_log.emplace_back("b:first");
			return true;
		});

		must(dispatcher.post(event_a{}));
		must(dispatcher.post(event_b{}));
		require_log({"a:first", "a:second", "b:first"});
	}
}

// =============================================================================
// Unsubscribe
// =============================================================================

TEST_CASE("dispatcher: unsubscribe", "[dispatcher]") {
	SECTION("off removes the specific handler, others still fire") {
		test_log.clear();
		lge::dispatcher dispatcher;

		const auto token = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("first");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("second");
			return true;
		});

		must(dispatcher.unsubscribe(token));
		must(dispatcher.post(event_a{}));
		require_log({"second"});
	}

	SECTION("unsubscribe last handler leaves no subscribers, post is silent") {
		test_log.clear();
		lge::dispatcher dispatcher;

		const auto token = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("handler");
			return true;
		});

		must(dispatcher.unsubscribe(token));
		must(dispatcher.post(event_a{}));
		require_log({});
	}

	SECTION("off middle handler, first and third still fire in order") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("first");
			return true;
		});
		const auto middle = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("second");
			return true;
		});
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("third");
			return true;
		});

		must(dispatcher.unsubscribe(middle));
		must(dispatcher.post(event_a{}));
		require_log({"first", "third"});
	}

	SECTION("off with already-removed token returns error") {
		lge::dispatcher dispatcher;

		const auto token = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> { return true; });

		must(dispatcher.unsubscribe(token));

		const auto result = dispatcher.unsubscribe(token);
		REQUIRE(result.has_error());
	}

	SECTION("off with token for unknown event type returns error") {
		lge::dispatcher dispatcher;

		// Build a token that was never issued by this dispatcher
		constexpr lge::subscription bogus{.event_type = entt::type_hash<event_c>::value(), .id = 9999};

		const auto result = dispatcher.unsubscribe(bogus);
		REQUIRE(result.has_error());
	}
}

// =============================================================================
// Error Propagation
// =============================================================================

TEST_CASE("dispatcher: error propagation", "[dispatcher]") {
	SECTION("error returned from handler is propagated by post") {
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>(
			[](const event_a &) -> lge::result<> { return lge::error("handler failed"); });

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_error());
	}

	SECTION("post with no handler returns success") {
		lge::dispatcher dispatcher;

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_value());
	}

	SECTION("first handler error stops propagation, second handler not called") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore =
			dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> { return lge::error("first failed"); });
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("second");
			return true;
		});

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_error());
		require_log({});
	}

	SECTION("second handler error stops propagation, third handler not called") {
		test_log.clear();
		lge::dispatcher dispatcher;

		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("first");
			return true;
		});
		std::ignore =
			dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> { return lge::error("second failed"); });
		std::ignore = dispatcher.subscribe<event_a>([](const event_a &) -> lge::result<> {
			test_log.emplace_back("third");
			return true;
		});

		const auto result = dispatcher.post(event_a{});
		REQUIRE(result.has_error());
		require_log({"first"});
	}
}