// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/colors.hpp>
#include <lge/internal/text/rich_text.hpp>

#include <catch2/catch_test_macros.hpp>

// =============================================================================
// has_rich_tags
// =============================================================================

TEST_CASE("rich_text: has_rich_tags detects tags correctly", "[rich_text]") {
	SECTION("plain text returns false") {
		REQUIRE(!lge::has_rich_tags("hello world"));
	}

	SECTION("empty string returns false") {
		REQUIRE(!lge::has_rich_tags(""));
	}

	SECTION("valid reset tag returns true") {
		REQUIRE(lge::has_rich_tags("{#}"));
	}

	SECTION("valid rgb tag returns true") {
		REQUIRE(lge::has_rich_tags("{#FF0000}red"));
	}

	SECTION("valid rgba tag returns true") {
		REQUIRE(lge::has_rich_tags("{#FF00001A}faint red"));
	}

	SECTION("lone open brace with no hash returns false") {
		REQUIRE(!lge::has_rich_tags("{hello}"));
	}
}

// =============================================================================
// strip_rich_tags
// =============================================================================

TEST_CASE("rich_text: strip_rich_tags removes all tags", "[rich_text]") {
	SECTION("plain text is returned unchanged") {
		REQUIRE(lge::strip_rich_tags("hello") == "hello");
	}

	SECTION("reset tag is removed") {
		REQUIRE(lge::strip_rich_tags("a{#}b") == "ab");
	}

	SECTION("rgb tag is removed") {
		REQUIRE(lge::strip_rich_tags("{#FF0000}red") == "red");
	}

	SECTION("rgba tag is removed") {
		REQUIRE(lge::strip_rich_tags("{#FF00001A}faint") == "faint");
	}

	SECTION("multiple tags are all removed") {
		REQUIRE(lge::strip_rich_tags("a{#FF0000}b{#}c") == "abc");
	}

	SECTION("malformed tag (wrong hex length) is kept as literal") {
		REQUIRE(lge::strip_rich_tags("{#FF}text") == "{#FF}text");
	}

	SECTION("malformed tag (invalid hex chars) is kept as literal") {
		REQUIRE(lge::strip_rich_tags("{#ZZZZZZ}text") == "{#ZZZZZZ}text");
	}

	SECTION("empty string returns empty") {
		REQUIRE(lge::strip_rich_tags("") == "");
	}

	SECTION("multiline text strips correctly") {
		REQUIRE(lge::strip_rich_tags("{#FF0000}line1\n{#}line2") == "line1\nline2");
	}
}

// =============================================================================
// parse_rich_text
// =============================================================================

TEST_CASE("rich_text: parse_rich_text produces correct segments", "[rich_text]") {
	constexpr auto default_color = lge::colors::white;

	SECTION("plain text produces one segment with default color") {
		const auto segs = lge::parse_rich_text("hello", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "hello");
		REQUIRE(segs[0].segment_color == default_color);
	}

	SECTION("empty string produces no segments") {
		REQUIRE(lge::parse_rich_text("", default_color).empty());
	}

	SECTION("reset tag with no prior color uses default") {
		const auto segs = lge::parse_rich_text("{#}hello", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "hello");
		REQUIRE(segs[0].segment_color == default_color);
	}

	SECTION("rgb tag produces correct color") {
		const auto segs = lge::parse_rich_text("{#FF0000}red", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "red");
		REQUIRE(segs[0].segment_color == lge::color::from_hex(0xFF0000FF));
	}

	SECTION("rgba tag produces correct color with alpha") {
		const auto segs = lge::parse_rich_text("{#FF00001A}faint", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "faint");
		REQUIRE(segs[0].segment_color == lge::color::from_hex(0xFF00001A));
	}

	SECTION("text before tag is first segment with default color") {
		const auto segs = lge::parse_rich_text("normal {#FF0000}red", default_color);
		REQUIRE(segs.size() == 2);
		REQUIRE(segs[0].text == "normal ");
		REQUIRE(segs[0].segment_color == default_color);
		REQUIRE(segs[1].text == "red");
		REQUIRE(segs[1].segment_color == lge::color::from_hex(0xFF0000FF));
	}

	SECTION("reset tag after color restores default") {
		const auto segs = lge::parse_rich_text("{#FF0000}red{#}normal", default_color);
		REQUIRE(segs.size() == 2);
		REQUIRE(segs[0].segment_color == lge::color::from_hex(0xFF0000FF));
		REQUIRE(segs[1].text == "normal");
		REQUIRE(segs[1].segment_color == default_color);
	}

	SECTION("multiple color tags produce multiple segments") {
		// "normal color, this is {#FF0000}solid red, {#FF00001A}transparent red {#}, and normal again"
		const auto text =
			"normal, {#FF0000}solid red, {#FF00001A}transparent red {#}normal again";
		const auto segs = lge::parse_rich_text(text, default_color);
		REQUIRE(segs.size() == 4);
		REQUIRE(segs[0].segment_color == default_color);
		REQUIRE(segs[1].segment_color == lge::color::from_hex(0xFF0000FF));
		REQUIRE(segs[2].segment_color == lge::color::from_hex(0xFF00001A));
		REQUIRE(segs[3].segment_color == default_color);
	}

	SECTION("malformed tag (wrong length) is treated as literal text") {
		const auto segs = lge::parse_rich_text("{#FF}bad", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "{#FF}bad");
		REQUIRE(segs[0].segment_color == default_color);
	}

	SECTION("malformed tag (invalid hex) is treated as literal text") {
		const auto segs = lge::parse_rich_text("{#ZZZZZZ}bad", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "{#ZZZZZZ}bad");
		REQUIRE(segs[0].segment_color == default_color);
	}

	SECTION("there is no tag nesting — reset always goes to default") {
		// {#FF0000}red {#00FF00}green {#} — the reset goes to default, not back to red
		const auto segs = lge::parse_rich_text("{#FF0000}red {#00FF00}green {#}back", default_color);
		REQUIRE(segs.size() == 3);
		REQUIRE(segs[2].segment_color == default_color);
		REQUIRE(segs[2].text == "back");
	}

	SECTION("multiline text in a single segment is preserved") {
		const auto segs = lge::parse_rich_text("line1\nline2", default_color);
		REQUIRE(segs.size() == 1);
		REQUIRE(segs[0].text == "line1\nline2");
	}

	SECTION("multiline text across multiple color segments is preserved") {
		const auto segs = lge::parse_rich_text("{#FF0000}line1\n{#}line2", default_color);
		REQUIRE(segs.size() == 2);
		REQUIRE(segs[0].text == "line1\n");
		REQUIRE(segs[1].text == "line2");
	}
}