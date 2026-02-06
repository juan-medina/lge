// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/components/label.hpp>
#include <lge/result.hpp>
#include <lge/systems/render_system.hpp>

#include <raylib.h>

#include <glm/ext/vector_float2.hpp>

namespace lge {

auto render_system::update(const float /*dt*/) -> result<> {
	for(const auto view = world.view<label, glm::vec2>(); const auto entity: view) {
		const auto &[label, position] = view.get<lge::label, glm::vec2>(entity);
		DrawText(label.text.c_str(),
				 static_cast<int>(position.x),
				 static_cast<int>(position.y),
				 static_cast<int>(label.size),
				 Color{
					 .r = static_cast<unsigned char>(label.color.r * 255),
					 .g = static_cast<unsigned char>(label.color.g * 255),
					 .b = static_cast<unsigned char>(label.color.b * 255),
					 .a = static_cast<unsigned char>(label.color.a * 255),
				 });
	}

	return true;
}

} // namespace lge