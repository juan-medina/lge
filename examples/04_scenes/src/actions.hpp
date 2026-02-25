// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/input.hpp>

#include "../../src/actions.hpp"

namespace examples::actions {

// menu actions
static constexpr lge::input::id left_action = total_base_actions + 0;
static constexpr lge::input::id right_action = total_base_actions + 1;
static constexpr lge::input::id go_to_game_action = total_base_actions + 2;

// game actions
static constexpr lge::input::id go_to_menu_action = go_to_game_action + 1;

} // namespace examples::actions
