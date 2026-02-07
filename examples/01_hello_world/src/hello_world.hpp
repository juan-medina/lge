// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/app.hpp>
#include <lge/app_config.hpp>

#include <lge/result.hpp>

class hello_world: public lge::app {
public:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto configure() -> lge::app_config override;
	[[nodiscard]] auto update(float dt) -> lge::result<> override;
};
