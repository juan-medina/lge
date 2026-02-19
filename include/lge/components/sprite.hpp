// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/resource_manager.hpp>

#include <string>

namespace lge {

struct sprite {
	sprite_sheet_handle sheet;
	std::string frame;
};

} // namespace lge