// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

namespace lge {

class app {
public:
	explicit app() = default;
	virtual ~app() = default;

	// Disable copying and moving — apps are not copyable or movable
	app(const app &) = delete;
	app(app &&) = delete;
	auto operator=(const app &) -> app & = delete;
	auto operator=(app &&) -> app & = delete;
};

} // namespace lge
