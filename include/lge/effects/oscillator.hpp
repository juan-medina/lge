// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

namespace lge {

struct oscillator {
    float min_{};
    float max{};
    float period{};
    float phase = 0.0F;
};

namespace effects {
    auto update_oscillator(oscillator& osc, const float dt) -> float;
} // namespace effects



} // namespace lge