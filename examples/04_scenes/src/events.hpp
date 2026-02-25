// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once
#include <cstdint>

namespace examples {

enum class game_type : std::uint8_t {
	red,
	blue,
};

struct go_to_game {
	game_type type;
};

struct go_to_menu {};

} // namespace examples