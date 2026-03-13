// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "button_system.hpp"

#include <lge/components/button.hpp>
#include <lge/components/clickable.hpp>
#include <lge/core/result.hpp>
#include <lge/events/button_clicked.hpp>
#include <lge/events/click.hpp>
#include <lge/interface/input.hpp>
#include <lge/internal/components/effective_hidden.hpp>

#include <entt/entity/fwd.hpp>
#include <entt/entt.hpp>

namespace lge {

auto button_system::init() -> result<> {
	ctx.world.on_construct<button>().connect<&button_system::on_button_constructed>(this);

	click_sub_ = ctx.events.subscribe<click>([this](const click &c) -> result<> {
		if(!ctx.world.all_of<button>(c.entity)) {
			return true;
		}
		if(const auto err = ctx.events.post(button_clicked{.entity = c.entity}).unwrap(); err) [[unlikely]] {
			return error("failed to post button_clicked event", *err);
		}
		return true;
	});

	return true;
}

auto button_system::end() -> result<> {
	ctx.world.on_construct<button>().disconnect<&button_system::on_button_constructed>(this);

	if(const auto err = ctx.events.unsubscribe(click_sub_).unwrap(); err) [[unlikely]] {
		return error("failed to unsubscribe click event", *err);
	}

	return true;
}

auto button_system::update(const float /*dt*/) -> result<> {
	if(!ctx.actions.is_controller_available()) {
		return true;
	}

	for(const auto entity: ctx.world.view<button>(entt::exclude<effective_hidden>)) {
		const auto &btn = ctx.world.get<button>(entity);
		if(btn.controller_button == input::button::unknown) {
			continue;
		}
		if(ctx.actions.get_button_state(btn.controller_button).pressed) {
			if(const auto err = ctx.events.post(button_clicked{.entity = entity}).unwrap(); err) [[unlikely]] {
				return *err;
			}
		}
	}

	return true;
}

// NOLINTNEXTLINE(*-convert-member-functions-to-static)
auto button_system::on_button_constructed(entt::registry &registry, const entt::entity entity) -> void {
	registry.emplace_or_replace<clickable>(entity);
}

} // namespace lge