// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <string>

// =============================================================================
// result<>
// =============================================================================

TEST_CASE("result: success and failure states", "[result]") {
	SECTION("true value has value and no error") {
		const lge::result<> r = true;
		REQUIRE(r.has_value());
		REQUIRE(!r.has_error());
	}

	SECTION("error value has error and no value") {
		const lge::result<> r = lge::error("something went wrong");
		REQUIRE(r.has_error());
		REQUIRE(!r.has_value());
	}
}

TEST_CASE("result: unwrap on result<>", "[result]") {
	SECTION("unwrap returns nullopt on success") {
		const lge::result<> r = true;
		REQUIRE(!r.unwrap().has_value());
	}

	SECTION("unwrap returns error on failure") {
		const lge::result<> r = lge::error("fail");
		const auto err = r.unwrap();
		REQUIRE(err.has_value());
		REQUIRE(err->get_message() == "fail");
	}
}

TEST_CASE("result: unwrap on result<T>", "[result]") {
	SECTION("unwrap extracts value on success") {
		const lge::result<int> r = 42;
		int value = 0;
		const auto err = r.unwrap(value);
		REQUIRE(!err.has_value());
		REQUIRE(value == 42);
	}

	SECTION("unwrap returns error and leaves value unchanged on failure") {
		const lge::result<int> r = lge::error("fail");
		int value = 0;
		const auto err = r.unwrap(value);
		REQUIRE(err.has_value());
		REQUIRE(value == 0);
	}
}

// =============================================================================
// error
// =============================================================================

TEST_CASE("error: message", "[error]") {
	SECTION("message is preserved") {
		const lge::error e("something failed");
		REQUIRE(e.get_message() == "something failed");
	}

	SECTION("chained error preserves own message") {
		const lge::error root("root cause");
		const lge::error chained("higher level", root);
		REQUIRE(chained.get_message() == "higher level");
	}
}

TEST_CASE("error: to_string with chain", "[error]") {
	SECTION("chained error contains both messages") {
		const lge::error root("root cause");
		const lge::error chained("higher level", root);
		const auto str = chained.to_string();
		REQUIRE(str.find("higher level") != std::string::npos);
		REQUIRE(str.find("root cause") != std::string::npos);
	}

	SECTION("chained from optional with value contains cause") {
		const std::optional<lge::error> opt = lge::error("root cause");
		const lge::error chained("higher level", opt);
		REQUIRE(chained.to_string().find("root cause") != std::string::npos);
	}

	SECTION("chained from empty optional has no cause") {
		const std::optional<lge::error> opt = std::nullopt;
		const lge::error chained("higher level", opt);
		const auto str = chained.to_string();
		REQUIRE(str.find("higher level") != std::string::npos);
		REQUIRE(str.find("caused by") == std::string::npos);
	}
}