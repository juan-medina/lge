// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include <concepts>
#include <entt/core/fwd.hpp>
#include <entt/entt.hpp>
#include <format>
#include <memory>
#include <string>
#include <string_view>

namespace lge {

template<typename T, typename... Args>
concept ResourceData = std::default_initializable<T> && requires(T &t, std::string_view uri, Args &&...args) {
	{ t.load(uri, std::forward<Args>(args)...) } -> std::same_as<result<>>;
};

template<typename Key>
concept ResourceKey = requires(entt::id_type id, const Key &key) {
	{ Key::from_id(id) } -> std::same_as<Key>;
	{ key.raw() } -> std::same_as<entt::id_type>;
};

template<typename T, ResourceKey Key>
class resource_store {
public:
	template<typename... Args>
		requires ResourceData<T, Args...>
	[[nodiscard]] auto load(std::string_view uri, Args &&...args) -> result<Key> {
		const auto key = entt::hashed_string{uri.data()}.value(); // NOLINT(*-suspicious-stringview-data-usage)
		if(const auto it = entries_.find(key); it != entries_.end()) {
			++it->second.ref_count;
			return Key::from_id(key);
		}
		auto res_ptr = std::make_unique<T>();
		if(const auto err = res_ptr->load(uri, std::forward<Args>(args)...).unwrap(); err) [[unlikely]] {
			return error{"failed to load resource: " + std::string(uri), *err};
		}
		entries_.emplace(key, entry{.resource = std::move(res_ptr), .ref_count = 1});
		return Key::from_id(key);
	}

	[[nodiscard]] auto unload(Key key) -> result<> {
		const auto it = entries_.find(key.raw());
		if(it == entries_.end()) [[unlikely]] {
			return error{std::format("attempted to unload non-existent resource with key: {}", key)};
		}
		if(--it->second.ref_count <= 0) {
			entries_.erase(it);
		}
		return true;
	}

	[[nodiscard]] auto get(Key key) const -> result<const T *> {
		const auto it = entries_.find(key.raw());
		if(it == entries_.end()) [[unlikely]] {
			return error{std::format("resource not found with key: {}", key)};
		}
		return it->second.resource.get();
	}

	[[nodiscard]] auto get(Key key) noexcept -> result<T *> {
		const auto it = entries_.find(key.raw());
		if(it == entries_.end()) [[unlikely]] {
			return error{std::format("resource not found with key: {}", key)};
		}
		return it->second.resource.get();
	}

private:
	struct entry {
		std::unique_ptr<T> resource;
		int ref_count = 0;
	};
	std::unordered_map<entt::id_type, entry> entries_;
};

} // namespace lge