// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_resource_manager.hpp"

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resource_manager.hpp>

#include <raylib.h>

#include <filesystem>
#include <format>
#include <fstream>
#include <glm/ext/vector_float2.hpp>
#include <jsoncons/basic_json.hpp>
#include <jsoncons/json_decoder.hpp>
#include <jsoncons/json_reader.hpp>
#include <memory>
#include <sstream>
#include <string>
#include <string_view>
#include <system_error>
#include <unordered_map>
#include <utility>

namespace lge {

// =============================================================================
// sprite_sheet_loader
// =============================================================================

namespace {

auto parse_frames(const jsoncons::json &root) -> std::unordered_map<std::string, sprite_sheet_frame> {
	if(!root.contains("frames")
	   || !root["frames"].is_object()) { // NOLINT(*-pro-bounds-avoid-unchecked-container-access)
		return {};
	}

	std::unordered_map<std::string, sprite_sheet_frame> frames;

	// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
	for(const auto &frames_node = root["frames"]; const auto &entry: frames_node.object_range()) {
		const auto &value = entry.value();

		// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
		if(!value.contains("frame") || !value["frame"].is_object()) {
			continue;
		}

		// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
		const auto &frame_node = value["frame"];
		const auto src_x = frame_node.get_value_or<float>("x", 0.F);
		const auto src_y = frame_node.get_value_or<float>("y", 0.F);
		const auto src_w = frame_node.get_value_or<float>("w", 0.F);
		const auto src_h = frame_node.get_value_or<float>("h", 0.F);

		glm::vec2 pivot{0.5F, 0.5F};
		// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
		if(value.contains("pivot") && value["pivot"].is_object()) {
			// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
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

auto parse_image_path(const jsoncons::json &root, const std::filesystem::path &base_path) -> std::string {
	// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
	if(!root.contains("meta") || !root["meta"].is_object()) {
		return {};
	}
	// NOLINTNEXTLINE(*-pro-bounds-avoid-unchecked-container-access)
	const auto image = root["meta"].get_value_or<std::string>("image", "");
	if(image.empty()) {
		return {};
	}
	return (base_path / image).string();
}

} // namespace

auto sprite_sheet_loader::operator()(const resource_uri &uri) const -> result_type {
	std::ifstream const file(static_cast<std::string>(uri));
	if(!file.is_open()) {
		log::error("sprite sheet file not found: {}", uri);
		return nullptr;
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::error_code error_code;
	jsoncons::json_decoder<jsoncons::json> decoder;
	jsoncons::json_stream_reader reader(buffer, decoder);
	reader.read(error_code);
	if(error_code) {
		log::error("sprite sheet JSON parse error: {}", error_code.message());
		return nullptr;
	}

	const auto &root = decoder.get_result();
	const auto base_path = std::filesystem::path(static_cast<std::string>(uri)).parent_path();

	const auto image_path = parse_image_path(root, base_path);
	if(image_path.empty()) {
		log::error("sprite sheet missing meta.image: {}", uri);
		return nullptr;
	}

	const auto rl_texture = LoadTexture(image_path.c_str());
	if(rl_texture.id == 0) {
		log::error("sprite sheet texture failed to load: {}", image_path);
		return nullptr;
	}
	SetTextureFilter(rl_texture, TEXTURE_FILTER_POINT);

	auto frames = parse_frames(root);

	return std::make_shared<sprite_sheet>(rl_texture, uri, std::move(frames));
}

// =============================================================================
// Init / end
// =============================================================================

auto raylib_resource_manager::init() -> result<> {
	log::debug("initializing raylib resource manager");
	return true;
}

auto raylib_resource_manager::end() -> result<> {
	log::debug("ending raylib resource manager");
	return true;
}

// =============================================================================
// Font
// =============================================================================

auto raylib_resource_manager::load_font(const resource_uri &uri) -> result<font_handle> {
	log::debug("loading font from uri `{}`", uri);

	if(!uri.exists()) [[unlikely]] {
		return error("font file does not exist: " + std::string(uri));
	}

	const auto key = uri_to_key(uri);
	auto [it, inserted] = font_cache_.load(key, uri);

	if(it == font_cache_.end() || !it->second) [[unlikely]] {
		return error("failed to load font from uri: " + std::string(uri));
	}

	return font_handle::from_id(key);
}

auto raylib_resource_manager::unload_font(const font_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid font handle");
	}

	const auto resource = font_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("font not found in cache");
	}

	log::debug("unloading font with id `{}`", handle);
	font_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::is_font_loaded(const font_handle handle) const noexcept -> bool {
	return handle.is_valid() && font_cache_[handle.raw()];
}

auto raylib_resource_manager::get_raylib_font(const font_handle handle) const -> result<Font> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid font handle");
	}

	const auto resource = font_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("font not found in cache");
	}

	return resource->raylib_font;
}

// =============================================================================
// Texture
// =============================================================================

auto raylib_resource_manager::load_texture(const resource_uri &uri) -> result<texture_handle> {
	log::debug("loading texture from uri `{}`", uri);

	if(!uri.exists()) [[unlikely]] {
		return error("texture file does not exist: " + std::string(uri));
	}

	const auto key = uri_to_key(uri);
	auto [it, inserted] = texture_cache_.load(key, uri);

	if(it == texture_cache_.end() || !it->second) [[unlikely]] {
		return error("failed to load texture from uri: " + std::string(uri));
	}

	return texture_handle::from_id(key);
}

auto raylib_resource_manager::unload_texture(const texture_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid texture handle");
	}

	const auto resource = texture_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("texture not found in cache");
	}

	log::debug("unloading texture with id `{}`", handle);
	texture_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::is_texture_loaded(const texture_handle handle) const noexcept -> bool {
	return handle.is_valid() && texture_cache_[handle.raw()];
}

auto raylib_resource_manager::get_raylib_texture(const texture_handle handle) const -> result<Texture2D> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid texture handle");
	}

	const auto resource = texture_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("texture not found in cache");
	}

	return resource->raylib_texture;
}

// =============================================================================
// Sprite sheet
// =============================================================================

auto raylib_resource_manager::load_sprite_sheet(const resource_uri &uri) -> result<sprite_sheet_handle> {
	log::debug("loading sprite sheet from uri `{}`", uri);

	if(!uri.exists()) [[unlikely]] {
		return error("sprite sheet file does not exist: " + std::string(uri));
	}

	const auto key = uri_to_key(uri);
	auto [it, inserted] = sprite_sheet_cache_.load(key, uri);

	if(it == sprite_sheet_cache_.end() || !it->second) [[unlikely]] {
		return error("failed to load sprite sheet from uri: " + std::string(uri));
	}

	return sprite_sheet_handle::from_id(key);
}

auto raylib_resource_manager::unload_sprite_sheet(const sprite_sheet_handle handle) -> result<> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto resource = sprite_sheet_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("sprite sheet not found in cache");
	}

	log::debug("unloading sprite sheet with id `{}`", handle);
	sprite_sheet_cache_.erase(handle.raw());

	return true;
}

auto raylib_resource_manager::is_sprite_sheet_loaded(const sprite_sheet_handle handle) const noexcept -> bool {
	return handle.is_valid() && sprite_sheet_cache_[handle.raw()];
}

auto raylib_resource_manager::get_sprite_sheet_frame(const sprite_sheet_handle handle,
													 const std::string_view frame_name) const
	-> result<sprite_sheet_frame> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto resource = sprite_sheet_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("sprite sheet not found in cache");
	}

	const auto it = resource->frames.find(std::string(frame_name));
	if(it == resource->frames.end()) [[unlikely]] {
		return error(std::format("frame '{}' not found in sprite sheet", frame_name));
	}

	return it->second;
}

auto raylib_resource_manager::get_sprite_sheet_texture(const sprite_sheet_handle handle) const -> result<Texture2D> {
	if(!handle.is_valid()) [[unlikely]] {
		return error("invalid sprite sheet handle");
	}

	const auto resource = sprite_sheet_cache_[handle.raw()];
	if(!resource) [[unlikely]] {
		return error("sprite sheet not found in cache");
	}

	return resource->raylib_texture;
}

} // namespace lge