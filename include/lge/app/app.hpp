// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app/app_config.hpp>
#include <lge/core/log.hpp>
#include <lge/core/result.hpp>
#include <lge/core/types.hpp>
#include <lge/interface/input.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/interface/resource_manager.hpp>
#include <lge/systems/system.hpp>

#include <entity/fwd.hpp>
#include <entt/entt.hpp>
#include <memory>
#include <vector>

namespace lge {

class app {
public:
	explicit app();
	virtual ~app() = default;

	app(const app &) = delete;
	app(app &&) = delete;
	auto operator=(const app &) -> app & = delete;
	auto operator=(app &&) -> app & = delete;

	[[nodiscard]] auto run() -> result<>;

	[[nodiscard]] auto get_world() noexcept -> entt::registry & {
		return world_;
	}

	[[nodiscard]] auto get_world() const noexcept -> const entt::registry & {
		return world_;
	}

	[[nodiscard]] virtual auto configure() -> app_config {
		return app_config{};
	}

	[[nodiscard]] virtual auto update(float /*dt*/) -> result<> {
		return true;
	}

protected:
	// =============================================================================
	// Lifecycle
	// =============================================================================

	[[nodiscard]] virtual auto init() -> result<>;
	[[nodiscard]] virtual auto end() -> result<>;

	// =============================================================================
	// System registration
	// =============================================================================

	template<typename T, typename... Args>
		requires std::is_base_of_v<system, T>
	void register_system(phase p, Args &&...args) {
		auto system = std::make_unique<T>(p, world_, std::forward<Args>(args)...);
		const auto type_name = get_type_name<T>();
		systems_.push_back(std::move(system));
		LGE_DEBUG("system of type `{}` registered", type_name);
	}

	// =============================================================================
	// Subsystem access
	// =============================================================================

	[[nodiscard]] auto get_renderer() noexcept -> renderer & {
		return *renderer_;
	}

	[[nodiscard]] auto get_renderer() const noexcept -> const renderer & {
		return *renderer_;
	}

	[[nodiscard]] auto get_input() noexcept -> input & {
		return *input_;
	}

	[[nodiscard]] auto get_input() const noexcept -> const input & {
		return *input_;
	}

	[[nodiscard]] auto get_resource_manager() noexcept -> resource_manager & {
		return *resource_manager_;
	}

	[[nodiscard]] auto get_resource_manager() const noexcept -> const resource_manager & {
		return *resource_manager_;
	}

	// =============================================================================
	// Application control
	// =============================================================================

	auto exit() noexcept -> void {
#ifndef __EMSCRIPTEN__
		should_exit_ = true;
#endif
	}

private:
	// =============================================================================
	// Subsystems
	// =============================================================================

	std::unique_ptr<renderer> renderer_;
	std::shared_ptr<input> input_;
	std::shared_ptr<resource_manager> resource_manager_;

	// =============================================================================
	// Core state
	// =============================================================================

	entt::registry world_;
	std::vector<std::unique_ptr<system>> systems_;

#ifndef __EMSCRIPTEN__
	bool should_exit_ = false;
#endif

	// =============================================================================
	// Internal methods
	// =============================================================================

	[[nodiscard]] auto setup_log() -> result<>;
	[[nodiscard]] auto main_loop() -> result<>;
	[[nodiscard]] auto update_system(phase p, float dt) const -> result<>;

	// =============================================================================
	// Constants
	// =============================================================================

	static constexpr auto empty_format = "%v";
	static constexpr auto color_line_format = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v %@";
	static constexpr auto banner = "\033[38;2;0;255;0ml\033[38;2;128;128;128m[\033[38;2;0;0;255mg\033[38;2;128;128;"
								   "128m]\033[38;2;255;0;0me\033[0m";
};

} // namespace lge