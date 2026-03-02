// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>
#include <lge/dispatcher/subscription.hpp>

#include <algorithm>
#include <cstdint>
#include <entt/core/fwd.hpp>
#include <entt/core/type_info.hpp>
#include <functional>
#include <unordered_map>
#include <vector>

namespace lge {

class dispatcher {
public:
	template<typename Event>
	using handler = std::function<result<>(const Event &)>;

	template<typename Event>
	[[nodiscard]] auto subscribe(handler<Event> h) -> subscription {
		const auto key = entt::type_hash<Event>::value();
		const auto id = next_id_++;

		auto &list = handlers_[key];
		list.push_back(
			{id, [h = std::move(h)](const void *evt) -> result<> { return h(*static_cast<const Event *>(evt)); }});

		return subscription{.event_type = key, .id = id};
	}

	[[nodiscard]] auto unsubscribe(subscription token) -> result<> {
		const auto it = handlers_.find(token.event_type);
		if(it == handlers_.end()) [[unlikely]] {
			return error("dispatcher::off — no handlers registered for this event type");
		}

		if(const auto erased = std::erase_if(it->second, [&](const entry &e) -> bool { return e.id == token.id; });
		   erased == 0) [[unlikely]] {
			return error("dispatcher::off — subscription token not found (already removed?)");
		}

		return true;
	}

	template<typename Event>
	[[nodiscard]] auto post(const Event &event) const -> result<> {
		const auto key = entt::type_hash<Event>::value();
		const auto it = handlers_.find(key);
		if(it == handlers_.end()) [[unlikely]] {
			return true;
		}

		for(const auto &e: it->second) {
			if(const auto err = e.handler(&event).unwrap(); err) [[unlikely]] {
				return *err;
			}
		}

		return true;
	}

private:
	struct entry {
		uint32_t id;
		std::function<result<>(const void *)> handler;
	};

	using handler_list = std::vector<entry>;
	using handler_map = std::unordered_map<entt::id_type, handler_list>;

	handler_map handlers_;
	uint32_t next_id_ = 0;
};

} // namespace lge