// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/core/result.hpp>

#include "../../src/example.hpp"

namespace examples {

class scenes_game: public example {
public:
	explicit scenes_game(): example(example_title, kb_message, controller_message) {}
	[[nodiscard]] auto update(float dt) -> lge::result<> override;

protected:
	[[nodiscard]] auto init() -> lge::result<> override;
	[[nodiscard]] auto end() -> lge::result<> override;

private:
	static constexpr auto example_title = "scenes_game!";
	static constexpr auto kb_message = "xxx, F5 debug draw,\nF11: toggle fullscreen, Esc: exit";
	static constexpr auto controller_message = "xxx,\nSTART: debug draw, SELECT: toggle fullscreen, B: exit";
};

} // namespace examples
