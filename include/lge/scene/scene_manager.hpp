// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/core/types.hpp>
#include <lge/scene/scene.hpp>

#include <cstddef>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <entt/entity/fwd.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace lge {

class scene_manager {
public:
	explicit scene_manager(entt::registry &world): world_{world} {}
	~scene_manager() = default;

	scene_manager(const scene_manager &) = delete;
	scene_manager(scene_manager &&) = delete;
	auto operator=(const scene_manager &) -> scene_manager & = delete;
	auto operator=(scene_manager &&) -> scene_manager & = delete;

	[[nodiscard]] auto init() -> result<>;
	[[nodiscard]] auto end() -> result<>;

	[[nodiscard]] auto update(float dt) -> result<> {
		if(!current_scene_.has_value()) {
			return true;
		}
		return current_scene_->get().update(dt);
	}

	[[nodiscard]] auto pause_active_scene() -> result<> {
		if(!current_scene_.has_value()) {
			return true;
		}
		return current_scene_->get().on_pause();
	}

	[[nodiscard]] auto resume_active_scene() -> result<> {
		if(!current_scene_.has_value()) {
			return true;
		}
		return current_scene_->get().on_resume();
	}

	template<typename T, typename... Args>
		requires std::is_base_of_v<scene, T>
	[[nodiscard]] auto register_scene(Args &&...args) -> result<> {
		const auto key = entt::type_hash<T>::value();
		if(scenes_.contains(key)) {
			const auto type_name = get_type_name<T>();
			return error(std::format("scene of type `{}` already registered", type_name));
		}
		auto new_scene = std::make_unique<T>(key, world_);
		const auto type_name = get_type_name<T>();
		if(const auto err = new_scene->init(std::forward<Args>(args)...).unwrap(); err) [[unlikely]] {
			return error(std::format("error initializing scene of type `{}`", type_name), *err);
		}
		scenes_.emplace(key, std::move(new_scene));
		log::debug("scene of type `{}` registered", type_name);
		return true;
	}

	template<typename T, typename... Args>
		requires std::is_base_of_v<scene, T>
	[[nodiscard]] auto set_active_scene(Args &&...args) -> result<> {
		const auto key = entt::type_hash<T>::value();
		const auto it = scenes_.find(key);
		if(it == scenes_.end()) {
			return error(std::format("scene of type `{}` not found", get_type_name<T>()));
		}
		if(current_scene_.has_value()) {
			if(const auto err = current_scene_->get().on_exit().unwrap(); err) [[unlikely]] {
				return error("error exiting current scene", *err);
			}
		}
		auto *concrete = static_cast<T *>(it->second.get());
		if(const auto err = concrete->on_enter(std::forward<Args>(args)...).unwrap(); err) [[unlikely]] {
			return error(std::format("error entering scene of type `{}`", get_type_name<T>()), *err);
		}
		current_scene_ = *it->second;
		log::debug("scene of type `{}` set as active", get_type_name<T>());
		return true;
	}

	[[nodiscard]] auto size() const -> size_t {
		return scenes_.size();
	}

private:
	entt::registry &world_;
	std::optional<std::reference_wrapper<scene>> current_scene_;
	std::unordered_map<entt::id_type, std::unique_ptr<scene>> scenes_;
};

} // namespace lge