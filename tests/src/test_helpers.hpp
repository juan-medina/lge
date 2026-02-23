// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include <catch2/catch_test_macros.hpp>
#include <string>
#include <vector>

inline std::vector<std::string> test_log;

inline auto require_log(const std::vector<std::string> &expected) -> void {
	std::string actual_str;
	for(const auto &e: test_log) {
		actual_str += "  " + e + "\n";
	}
	std::string expected_str;
	for(const auto &e: expected) {
		expected_str += "  " + e + "\n";
	}
	INFO("actual:\n" << actual_str);
	INFO("expected:\n" << expected_str);
	REQUIRE(test_log == expected);
}

// Asserts setup steps succeed without re-testing their logic
inline auto must(const lge::result<> &res) -> void {
	REQUIRE(res.has_value());
}
