// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/result.hpp>

namespace lge {

class app {
public:
	// =============================================================================
	// Public API
	// =============================================================================
	explicit app() = default;
	virtual ~app() = default;

	// Disable copying and moving — apps are not copyable or movable
	app(const app &) = delete;
	app(app &&) = delete;
	auto operator=(const app &) -> app & = delete;
	auto operator=(app &&) -> app & = delete;

	[[nodiscard]] auto run() -> result<>;
};

} // namespace lge
