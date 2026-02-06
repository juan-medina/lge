// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/label.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

#include <raylib.h>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto view = world.view<label, glm::vec2>(); const auto entity: view) {
		const auto &[label, position] = view.get<lge::label, glm::vec2>(entity);
		DrawText(label.text.c_str(),
				 static_cast<int>(position.x),
				 static_cast<int>(position.y),
				 static_cast<int>(label.size),
				 ColorFromGLM(label.color));
	}

	return true;
}

} // namespace lge