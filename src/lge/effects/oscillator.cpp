// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/effects/oscillator.hpp>

#include <glm/ext/scalar_constants.hpp>
#include <glm/trigonometric.hpp>

namespace lge::effects {

auto update_oscillator(oscillator &osc, const float dt) -> float {
	static constexpr float two_pi = glm::pi<float>() * 2;
	// advance phase
	osc.phase += dt * (two_pi / osc.period);

	// normalized sine [0..1]
	const float normalized = (glm::sin(osc.phase) + 1.0F) * 0.5F;

	// map to [min, max]
	return osc.min + (normalized * (osc.max - osc.min));
}

} // namespace lge::effects