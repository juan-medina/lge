// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <vector>

namespace examples {

struct dice_throw {
	int count{};
};

struct dice_roll_result {
	std::vector<int> values;
	int total{};
};

} // namespace examples