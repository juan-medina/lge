// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app.hpp>

class hello_world: public lge::app {
public:
	[[nodiscard]] auto init() -> lge::result<> override;
};