// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/base_resource_manager.hpp>
#include <lge/internal/resource_manager/animation_library.hpp>

#include "core/fwd.hpp"

#include <entt/core/hashed_string.hpp>
#include <filesystem>
#include <format>
#include <fstream>
#include <jsoncons/basic_json.hpp>
#include <jsoncons/json_decoder.hpp>
#include <jsoncons/json_reader.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>

namespace lge {

namespace {

auto parse_animation_library_json(const std::string_view uri) -> std::optional<jsoncons::json> {
	std::ifstream const file(static_cast<std::string>(uri));
	if(!file.is_open()) {
		return std::nullopt;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::error_code error_code;
	jsoncons::json_decoder<jsoncons::json> decoder;
	jsoncons::json_stream_reader reader(buffer, decoder);
	reader.read(error_code);
	if(error_code) {
		return std::nullopt;
	}

	return decoder.get_result();
}

auto parse_sprite_sheet_path(const jsoncons::json &root, const std::filesystem::path &base_path) -> std::string {
	if(!root.contains("sheet")) {
		return {};
	}

	const auto sheet = root.get_value_or<std::string>("sheet", "");
	if(sheet.empty()) {
		return {};
	}
	return (base_path / sheet).string();
}

auto parse_animations(const jsoncons::json &root) -> std::unordered_map<entt::id_type, animation_library_anim> {
	if(!root.contains("animations") || !root["animations"].is_object()) {
		return {};
	}

	std::unordered_map<entt::id_type, animation_library_anim> animations;

	for(const auto &frames_node = root["animations"]; const auto &entry: frames_node.object_range()) {
		const auto &value = entry.value();
		if(!value.is_object()) {
			continue;
		}

		if(!value.contains("fps") || !value["fps"].is_number()) {
			continue;
		}

		if(!value.contains("frames") || !value["frames"].is_array()) {
			continue;
		}

		animation_library_anim anim;
		anim.fps = value.get_value_or<float>("fps", 0.0F);

		for(const auto &frame_entry: value["frames"].array_range()) {
			auto str = frame_entry.as<std::string>();
			auto key = entt::hashed_string(str.data()); // NOLINT(*-suspicious-stringview-data-usage)
			anim.frames.push_back(key);
		}

		auto key_str = entry.key();
		log::debug("animation loaded: {}", key_str);
		auto key = entt::hashed_string(key_str.data()); // NOLINT(*-suspicious-stringview-data-usage)
		animations.emplace(key, std::move(anim));
	}

	return animations;
}

} // namespace

// =============================================================================
// Animation Libraries
// =============================================================================

animation_library::~animation_library() {
	if(rm_ != nullptr && sprite_sheet.is_valid()) {
		if(const auto err = rm_->unload_sprite_sheet(sprite_sheet).unwrap()) {
			log::error("failed to unload animation library sprite sheet: {}", err->to_string());
		}
	}
}

auto animation_library::load(const std::string_view uri, resource_manager &rm) -> result<> {
	log::debug("loading animation library from uri `{}`", uri);

	const auto root = parse_animation_library_json(uri);
	if(!root) [[unlikely]] {
		return error("failed to parse animation library JSON: " + std::string(uri));
	}

	const auto base_path = std::filesystem::path(static_cast<std::string>(uri)).parent_path();
	const auto sprite_sheet_path = parse_sprite_sheet_path(*root, base_path);
	if(sprite_sheet_path.empty()) [[unlikely]] {
		return error("animation library missing sprite sheet path: " + std::string(uri));
	}

	if(const auto err = rm.load_sprite_sheet(sprite_sheet_path).unwrap(sprite_sheet); err) [[unlikely]] {
		return error("failed to load animation library sprite sheet", *err);
	}

	animations = parse_animations(*root);
	rm_ = &rm;
	return true;
}

} // namespace lge