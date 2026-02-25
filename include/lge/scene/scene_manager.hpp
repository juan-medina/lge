// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/context.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/core/types.hpp>
#include <lge/scene/scene.hpp>

#include <cstddef>
#include <cstdint>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <functional>
#include <memory>
#include <optional>
#include <unordered_map>

namespace lge {

class scene_manager {
public:
	explicit scene_manager(context &the_context): ctx_{the_context} {}
	~scene_manager() = default;

	scene_manager(const scene_manager &) = delete;
	scene_manager(scene_manager &&) = delete;
	auto operator=(const scene_manager &) -> scene_manager & = delete;
	auto operator=(scene_manager &&) -> scene_manager & = delete;

	[[nodiscard]] auto init() -> result<>;
	[[nodiscard]] auto end() -> result<>;

	[[nodiscard]] auto update(float dt) -> result<>;

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

	// Returns current overlay alpha (0–255); used by transition_system for rendering.
	[[nodiscard]] auto overlay_alpha() const noexcept -> uint8_t {
		return overlay_alpha_;
	}

	// =============================================================================
	// Scene registration
	// =============================================================================

	template<typename T, typename... Args>
		requires std::is_base_of_v<scene, T>
	[[nodiscard]] auto add(Args &&...args) -> result<> {
		const auto key = entt::type_hash<T>::value();
		if(scenes_.contains(key)) {
			const auto type_name = get_type_name<T>();
			return error(std::format("scene of type `{}` already registered", type_name));
		}
		auto new_scene = std::make_unique<T>(ctx_);
		const auto type_name = get_type_name<T>();
		if(const auto err = new_scene->init(std::forward<Args>(args)...).unwrap(); err) [[unlikely]] {
			return error(std::format("error initializing scene of type `{}`", type_name), *err);
		}
		scenes_.emplace(key, std::move(new_scene));
		log::debug("scene of type `{}` registered", type_name);
		return true;
	}

	// =============================================================================
	// Immediate activation (no transition)
	// =============================================================================

	template<typename T, typename... Args>
		requires std::is_base_of_v<scene, T>
	[[nodiscard]] auto activate(Args &&...args) -> result<> {
		const auto key = entt::type_hash<T>::value();
		const auto it = scenes_.find(key);
		if(it == scenes_.end()) {
			return error(std::format("scene of type `{}` not found", get_type_name<T>()));
		}
		if(current_scene_.has_value()) {
			if(const auto err = current_scene_->get().on_exit().unwrap(); err) [[unlikely]] {
				return error("error exiting current scene", *err);
			}
			current_scene_->get().clear_owned();
		}
		auto *concrete = static_cast<T *>(it->second.get());
		if(const auto err = concrete->on_enter(std::forward<Args>(args)...).unwrap(); err) [[unlikely]] {
			return error(std::format("error entering scene of type `{}`", get_type_name<T>()), *err);
		}
		current_scene_ = *it->second;
		log::debug("scene of type `{}` set as active", get_type_name<T>());
		return true;
	}

	// =============================================================================
	// Transition activation (fade-out → swap → fade-in)
	// No transition when there is no current scene (first activation).
	// =============================================================================

	template<typename T, typename... Args>
		requires std::is_base_of_v<scene, T>
	[[nodiscard]] auto transition_activate(Args &&...args) -> result<> {
		if(!current_scene_.has_value()) {
			// First scene — activate immediately with no effect.
			return activate<T>(std::forward<Args>(args)...);
		}

		const auto key = entt::type_hash<T>::value();
		if(!scenes_.contains(key)) {
			return error(std::format("scene of type `{}` not found", get_type_name<T>()));
		}

		// Capture the deferred activation as a type-erased callable.
		pending_activate_ = [this, key, ... captured_args = std::forward<Args>(args)]() mutable -> result<> {
			const auto it = scenes_.find(key);
			if(it == scenes_.end()) {
				return error("pending scene no longer registered");
			}
			// Exit old scene.
			if(current_scene_.has_value()) {
				if(const auto err = current_scene_->get().on_exit().unwrap(); err) [[unlikely]] {
					return error("error exiting current scene", *err);
				}
				current_scene_->get().clear_owned();
			}
			// Enter new scene (it starts paused — resumed when fade_in completes).
			auto *concrete = static_cast<T *>(it->second.get());
			if(const auto err = concrete->on_enter(std::move(captured_args)...).unwrap(); err) [[unlikely]] {
				return error(std::format("error entering scene of type `{}`", get_type_name<T>()), *err);
			}
			if(const auto err = concrete->on_pause().unwrap(); err) [[unlikely]] {
				return error("error pausing incoming scene", *err);
			}
			current_scene_ = *it->second;
			log::debug("scene of type `{}` swapped in (paused for fade-in)", get_type_name<T>());
			return true;
		};

		transition_duration_ = transition_duration;
		transition_elapsed_ = 0.0F;
		state_ = transition_state::fade_out;

		// Pause current scene during the fade-out.
		if(const auto err = current_scene_->get().on_pause().unwrap(); err) [[unlikely]] {
			return error("error pausing current scene before transition", *err);
		}

		log::debug("transition started: fade-out");
		return true;
	}

	[[nodiscard]] auto size() const -> size_t {
		return scenes_.size();
	}

private:
	// =============================================================================
	// Transition state
	// =============================================================================

	enum class transition_state : uint8_t { idle, fade_out, fade_in };

	static constexpr float transition_duration = 0.4F;

	context ctx_;
	std::optional<std::reference_wrapper<scene>> current_scene_;
	std::unordered_map<entt::id_type, std::unique_ptr<scene>> scenes_;

	transition_state state_ = transition_state::idle;
	float transition_duration_ = transition_duration;
	float transition_elapsed_ = 0.0F;
	uint8_t overlay_alpha_ = 0;
	std::function<result<>()> pending_activate_;
};

} // namespace lge