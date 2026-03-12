// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include "../../src/example.hpp"

namespace examples {

class buttons: public example {
public:
	explicit buttons(): example(example_title, kb_message, controller_message) {}

private:
	static constexpr auto example_title = "Buttons!";
	static constexpr auto kb_message =
		"Press {#FFFF00}F5{#}: debug draw, {#FFFF00}F11{#}: toggle fullscreen, {#FFFF00}Esc{#}: exit";
	static constexpr auto controller_message =
		"Press {#FFFF00}START{#}: debug draw, {#FFFF00}SELECT{#}: toggle fullscreen, {#FFFF00}B{#}: exit";
};

} // namespace examples

