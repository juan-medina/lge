// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/interface/resources.hpp>
#include <lge/internal/resource_manager/sprite_sheet.hpp>

#include <entt/core/fwd.hpp>
#include <filesystem>
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

sprite_sheet::~sprite_sheet() {
	if(rm_ != nullptr && texture.is_valid()) {
		if(const auto err = rm_->unload_texture(texture).unwrap()) {
			log::error("failed to unload sprite sheet texture: {}", err->to_string());
		}
	}
}

auto sprite_sheet::load(const std::string_view uri, resource_manager &rm) -> result<> {
	rm_ = &rm;

	jsoncons::json root;
	if(const auto err = parse_sprite_sheet_json(uri).unwrap(root); err) [[unlikely]] {
		return error{"failed to parse sprite sheet JSON: " + std::string(uri), *err};
	}

	const auto base_path = std::filesystem::path(static_cast<std::string>(uri)).parent_path();

	std::string image_path;
	if(const auto err = parse_sprite_sheet_image_path(root, base_path).unwrap(image_path); err) [[unlikely]] {
		return error{"failed to parse sprite sheet image path: " + std::string(uri), *err};
	}

	if(const auto err = rm.load_texture(image_path).unwrap(texture); err) [[unlikely]] {
		return error{"failed to load sprite sheet texture", *err};
	}

	if(const auto err = parse_sprite_sheet_frames(root).unwrap(); err) [[unlikely]] {
		return error{"failed to parse sprite sheet frames: " + std::string(uri), *err};
	}

	return true;
}

auto sprite_sheet::parse_sprite_sheet_frames(const jsoncons::json &root) -> result<> {
	if(!root.contains("frames") || !root["frames"].is_object()) {
		return error{"missing sprite sheet frames"};
	}

	frames.clear();

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
		const auto key = entt::hashed_string{entry.key().data()}.value(); // NOLINT(*-suspicious-stringview-data-usage)
		frames.emplace(key,
					   sprite_sheet_frame{
						   .source_pos = {src_x, src_y},
						   .source_size = {src_w, src_h},
						   .pivot = pivot,
					   });

		if(frames.empty()) [[unlikely]] {
			return error{"sprite sheet contains no frame"};
		}

		log::debug("sprite sheet frame loaded: {}", entry.key());
	}

	return true;
}

auto sprite_sheet::parse_sprite_sheet_image_path(const jsoncons::json &root, const std::filesystem::path &base_path)
	-> result<std::string> {
	if(!root.contains("meta") || !root["meta"].is_object()) {
		return error{"missing sprite sheet meta"};
	}

	const auto image = root["meta"].get_value_or<std::string>("image", "");
	if(image.empty()) {
		return error{"sprite sheet meta missing image"};
	}
	return (base_path / image).string();
}

auto sprite_sheet::parse_sprite_sheet_json(const std::string_view uri) -> result<jsoncons::json> {
	std::ifstream const file(static_cast<std::string>(uri));
	if(!file.is_open()) {
		return error("failed to open sprite sheet JSON file: " + std::string(uri));
	}

	std::stringstream buffer;
	buffer << file.rdbuf();

	std::error_code error_code;
	jsoncons::json_decoder<jsoncons::json> decoder;
	jsoncons::json_stream_reader reader(buffer, decoder);
	reader.read(error_code);
	if(error_code) {
		return error("failed to parse sprite sheet JSON: " + std::string(uri) + ", error: " + error_code.message());
	}

	return decoder.get_result();
}

} // namespace lge
