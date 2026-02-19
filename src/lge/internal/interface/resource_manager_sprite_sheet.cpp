// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <filesystem>
#include <format>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <jsoncons/basic_json.hpp>
#include <jsoncons/json_decoder.hpp>
#include <jsoncons/json_reader.hpp>
#include <optional>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>

namespace lge {

// =============================================================================
// Internal JSON parsing helpers
// =============================================================================

namespace {

auto parse_sprite_sheet_frames(const jsoncons::json &root) -> std::unordered_map<std::string, sprite_sheet_frame> {
	if(!root.contains("frames") || !root["frames"].is_object()) {
		return {};
	}

	std::unordered_map<std::string, sprite_sheet_frame> frames;

	for(const auto &frames_node = root["frames"]; const auto &entry: frames_node.object_range()) {
		const auto &value = entry.value();

		if(!value.contains("frame") || !value["frame"].is_object()) {
			continue;
		}

		const auto &frame_node = value["frame"];
		const auto src_x = frame_node.get_value_or<float>("x", 0.F);
		const auto src_y = frame_node.get_value_or<float>("y", 0.F);
		const auto src_w = frame_node.get_value_or<float>("w", 0.F);
		const auto src_h = frame_node.get_value_or<float>("h", 0.F);

		glm::vec2 pivot{0.5F, 0.5F};
		if(value.contains("pivot") && value["pivot"].is_object()) {
			const auto &pivot_node = value["pivot"];
			pivot.x = pivot_node.get_value_or<float>("x", 0.5F);
			pivot.y = pivot_node.get_value_or<float>("y", 0.5F);
		}

		frames.emplace(entry.key(),
					   sprite_sheet_frame{
						   .source_pos = {src_x, src_y},
						   .source_size = {src_w, src_h},
						   .pivot = pivot,
					   });

		log::debug("sprite sheet frame loaded: {}", entry.key());
	}

	return frames;
}

auto parse_sprite_sheet_image_path(const jsoncons::json &root, const std::filesystem::path &base_path) -> std::string {
	if(!root.contains("meta") || !root["meta"].is_object()) {
		return {};
	}

	const auto image = root["meta"].get_value_or<std::string>("image", "");
	if(image.empty()) {
		return {};
	}
	return (base_path / image).string();
}

auto parse_sprite_sheet_json(const std::string_view uri) -> std::optional<jsoncons::json> {
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

} // namespace

// =============================================================================
// Sprite Sheet
// =============================================================================

auto resource_manager::load_sprite_sheet(const std::string_view uri) -> result<sprite_sheet_handle> {
	log::debug("loading sprite sheet from uri `{}`", uri);

	if(!exists(uri)) [[unlikely]] {
		return error("sprite sheet file does not exist: " + std::string(uri));
	}

	const auto root = parse_sprite_sheet_json(uri);
	if(!root) [[unlikely]] {
		return error("failed to parse sprite sheet JSON: " + std::string(uri));
	}

	const auto base_path = std::filesystem::path(static_cast<std::string>(uri)).parent_path();
	const auto image_path = parse_sprite_sheet_image_path(*root, base_path);
	if(image_path.empty()) [[unlikely]] {
		return error("sprite sheet missing meta.image: " + std::string(uri));
	}

	texture_handle texture;
	if(const auto err = load_texture(image_path).unwrap(texture); err) [[unlikely]] {
		return error("failed to load sprite sheet texture", *err);
	}

	const auto key = uri_to_key(uri);
	sprite_sheets_.emplace(key, sprite_sheet_data{.texture = texture, .frames = parse_sprite_sheet_frames(*root)});

	return sprite_sheet_handle::from_id(key);
}

auto resource_manager::unload_sprite_sheet(const sprite_sheet_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto it = sprite_sheets_.find(handle.raw());
	if(it == sprite_sheets_.end()) [[unlikely]] {
		return error("sprite sheet not found");
	}

	if(const auto err = unload_texture(it->second.texture).unwrap(); err) [[unlikely]] {
		return error("failed to unload sprite sheet texture", *err);
	}

	log::debug("unloading sprite sheet with id `{}`", handle);
	sprite_sheets_.erase(it);

	return true;
}

auto resource_manager::is_sprite_sheet_loaded(const sprite_sheet_handle handle) const noexcept -> bool {
	return handle.is_valid() && sprite_sheets_.contains(handle.raw());
}

auto resource_manager::get_sprite_sheet_frame(const sprite_sheet_handle handle, const std::string_view frame_name) const
	-> result<sprite_sheet_frame> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto it = sprite_sheets_.find(handle.raw());
	if(it == sprite_sheets_.end()) [[unlikely]] {
		return error("sprite sheet not found");
	}

	const auto frame_it = it->second.frames.find(std::string(frame_name));
	if(frame_it == it->second.frames.end()) [[unlikely]] {
		return error(std::format("frame '{}' not found in sprite sheet", frame_name));
	}

	return frame_it->second;
}

auto resource_manager::get_sprite_sheet_texture(const sprite_sheet_handle handle) const -> result<texture_handle> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto it = sprite_sheets_.find(handle.raw());
	if(it == sprite_sheets_.end()) [[unlikely]] {
		return error("sprite sheet not found");
	}

	return it->second.texture;
}

} // namespace lge