// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <entt/core/type_info.hpp>
#include <functional>
#include <unordered_map>

namespace lge {

class dispatcher {
public:
	template<typename Event>
	using handler = std::function<void(const Event &)>;

	template<typename Event>
	auto on(handler<Event> h) -> void {
		const auto key = entt::type_hash<Event>::value();
		handlers_[key] = [h = std::move(h)](const void *evt) -> auto { h(*static_cast<const Event *>(evt)); };
	}

	template<typename Event>
	auto post(const Event &event) const -> void {
		const auto key = entt::type_hash<Event>::value();
		if(const auto it = handlers_.find(key); it != handlers_.end()) {
			it->second(&event);
		}
	}

private:
	using type_erased_handler = std::function<void(const void *)>;
	using handler_map = std::unordered_map<entt::id_type, type_erased_handler>;

	handler_map handlers_;
};

} // namespace lge