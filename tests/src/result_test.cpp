// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <optional>
#include <string>

// =============================================================================
// result<bool>
// =============================================================================

TEST_CASE("result: true value has no error", "[result]") {
    const lge::result<> r = true;
    REQUIRE(!r.has_error());
    REQUIRE(r.has_value());
}

TEST_CASE("result: error value has error", "[result]") {
    const lge::result<> r = lge::error("something went wrong");
    REQUIRE(r.has_error());
    REQUIRE(!r.has_value());
}

TEST_CASE("result: unwrap returns nullopt on success", "[result]") {
    const lge::result<> r = true;
    REQUIRE(!r.unwrap().has_value());
}

TEST_CASE("result: unwrap returns error on failure", "[result]") {
    const lge::result<> r = lge::error("fail");
    const auto err = r.unwrap();
    REQUIRE(err.has_value());
    REQUIRE(err->get_message() == "fail");
}

TEST_CASE("result: unwrap with value extracts on success", "[result]") {
    const lge::result<int> r = 42;
    int value = 0;
    const auto err = r.unwrap(value);
    REQUIRE(!err.has_value());
    REQUIRE(value == 42);
}

TEST_CASE("result: unwrap with value returns error on failure", "[result]") {
    const lge::result<int> r = lge::error("fail");
    int value = 0;
    const auto err = r.unwrap(value);
    REQUIRE(err.has_value());
    REQUIRE(value == 0);
}

// =============================================================================
// error
// =============================================================================

TEST_CASE("error: message is preserved", "[error]") {
    const lge::error e("something failed");
    REQUIRE(e.get_message() == "something failed");
}

TEST_CASE("error: chained error preserves top message", "[error]") {
    const lge::error root("root cause");
    const lge::error chained("higher level", root);
    REQUIRE(chained.get_message() == "higher level");
}

TEST_CASE("error: chained error to_string contains both messages", "[error]") {
    const lge::error root("root cause");
    const lge::error chained("higher level", root);
    const auto str = chained.to_string();
    REQUIRE(str.find("higher level") != std::string::npos);
    REQUIRE(str.find("root cause") != std::string::npos);
}

TEST_CASE("error: chained from optional with value preserves cause", "[error]") {
    const std::optional<lge::error> opt = lge::error("root cause");
    const lge::error chained("higher level", opt);
    const auto str = chained.to_string();
    REQUIRE(str.find("root cause") != std::string::npos);
}

TEST_CASE("error: chained from empty optional has no cause", "[error]") {
    const std::optional<lge::error> opt = std::nullopt;
    const lge::error chained("higher level", opt);
    const auto str = chained.to_string();
    REQUIRE(str.find("higher level") != std::string::npos);
    REQUIRE(str.find("caused by") == std::string::npos);
}