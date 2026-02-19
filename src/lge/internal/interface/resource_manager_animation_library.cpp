// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <filesystem>
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

auto parse_animations(const jsoncons::json &root) -> std::unordered_map<std::string, animation_library_anim> {
	if(!root.contains("animations") || !root["animations"].is_object()) {
		return {};
	}

	std::unordered_map<std::string, animation_library_anim> animations;

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
			anim.frames.push_back(frame_entry.as<std::string>());
		}

		log::debug("animation loaded: {}", entry.key());
		animations.emplace(entry.key(), std::move(anim));
	}

	return animations;
}

} // namespace

// =============================================================================
// Animation Libraries
// =============================================================================

auto resource_manager::load_animation_library(const std::string_view uri) -> result<animation_library_handle> {
	log::debug("loading animation library from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("animation library file does not exist: " + std::string(uri));
	}

	const auto root = parse_animation_library_json(uri);
	if(!root) [[unlikely]] {
		return error("failed to parse animation library JSON: " + std::string(uri));
	}

	const auto base_path = std::filesystem::path(static_cast<std::string>(uri)).parent_path();
	const auto sprite_sheet_path = parse_sprite_sheet_path(*root, base_path);
	if(sprite_sheet_path.empty()) [[unlikely]] {
		return error("animation library missing sprite sheet path: " + std::string(uri));
	}

	if(!exists(sprite_sheet_path)) [[unlikely]] {
		return error("animation library sprite sheet file does not exist: " + std::string(uri));
	}

	sprite_sheet_handle sprite_sheet;
	if(const auto err = load_sprite_sheet(sprite_sheet_path).unwrap(sprite_sheet); err) [[unlikely]] {
		return error("failed to load animation library sprite sheet", *err);
	}

	const auto key = uri_to_key(uri);
	animation_libraries_.emplace(key,
								 animation_library_data{
									 .sprite_sheet = sprite_sheet,
									 .animations = parse_animations(*root),
								 });

	return animation_library_handle::from_id(key);
}

auto resource_manager::unload_animation_library(const animation_library_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid animation library handle");
	}

	const auto it = animation_libraries_.find(handle.raw());
	if(it == animation_libraries_.end()) [[unlikely]] {
		return error("animation library not found");
	}

	if(const auto err = unload_sprite_sheet(it->second.sprite_sheet).unwrap(); err) [[unlikely]] {
		return error("failed to unload sprite sheet", *err);
	}

	log::debug("unloading animation library with id `{}`", handle);
	animation_libraries_.erase(it);

	return true;
}

auto resource_manager::get_animation(const animation_library_handle handle, const std::string_view anim_name) const
	-> result<animation_library_anim> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid animation library handle");
	}

	const auto it = animation_libraries_.find(handle.raw());
	if(it == animation_libraries_.end()) [[unlikely]] {
		return error("animation library not found");
	}

	const auto clip_it = it->second.animations.find(std::string(anim_name));
	if(clip_it == it->second.animations.end()) [[unlikely]] {
		return error(std::format("animation clip '{}' not found in animation library", anim_name));
	}

	return clip_it->second;
}

auto resource_manager::get_animation_sprite_sheet(animation_library_handle handle) const
	-> result<sprite_sheet_handle> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid animation library handle");
	}

	const auto it = animation_libraries_.find(handle.raw());
	if(it == animation_libraries_.end()) [[unlikely]] {
		return error("animation library not found");
	}

	return it->second.sprite_sheet;
}

} // namespace lge