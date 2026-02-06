// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app_config.hpp>
#include <lge/renderer.hpp>
#include <lge/result.hpp>
#include <lge/systems/system.hpp>
#include <lge/types.hpp>

#include <entt/entt.hpp>
#include <spdlog/spdlog.h>

namespace lge {

class app {
public:
	explicit app() = default;
	virtual ~app() = default;

	// Disable copying and moving — apps are not copyable or movable
	app(const app &) = delete;
	app(app &&) = delete;
	auto operator=(const app &) -> app & = delete;
	auto operator=(app &&) -> app & = delete;

	[[nodiscard]] auto run() -> result<>;

	[[nodiscard]] auto get_world() -> entt::registry & {
		return world_;
	}

	[[nodiscard]] auto get_world() const -> const entt::registry & {
		return world_;
	}

	[[nodiscard]] virtual auto configure() -> app_config {
		return app_config{};
	}

protected:
	virtual auto init() -> result<>;

	template<typename T, typename... Args>
		requires std::is_base_of_v<system, T>
	void register_system(Args &&...args) {
		auto system = std::make_unique<T>(world_, std::forward<Args>(args)...);
		const auto type_name = get_type_name<T>();
		systems_.push_back(std::move(system));
		SPDLOG_DEBUG("system of type `{}` registered", type_name);
	}

private:
	auto setup_log() -> result<>;
	[[nodiscard]] auto end() -> result<>;
	[[nodiscard]] auto main_loop() -> result<>;

	static constexpr auto empty_format = "%v";
	static constexpr auto color_line_format = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v %@";
	static constexpr auto banner = "\033[38;2;0;255;0ml\033[38;2;128;128;128m[\033[38;2;0;0;255mg\033[38;2;128;128;"
								   "128m]\033[38;2;255;0;0me\033[0m";

#ifndef __EMSCRIPTEN__
	bool should_exit_ = false;
#endif

	renderer renderer_;

	entt::registry world_;
	std::vector<std::unique_ptr<system>> systems_;

	[[nodiscard]] auto update_system(phase p, float dt) const -> result<>;
};

} // namespace lge
