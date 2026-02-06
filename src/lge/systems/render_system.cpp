// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/label.hpp>
#include <lge/components/position.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto view = world.view<label, global_position>(); const auto entity: view) {
		const auto &[label, position] = view.get<lge::label, global_position>(entity);
		renderer_.render_label(label, position.value);
	}

	return true;
}

} // namespace lge