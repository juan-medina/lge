// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_backend.hpp"

#include <lge/interface/backend.hpp>
#include <lge/internal/raylib/raylib_input.hpp>
#include <lge/internal/raylib/raylib_renderer.hpp>
#include <lge/internal/raylib/raylib_resource_manager.hpp>

#include <memory>

namespace lge::raylib_backend {

auto create() -> backend {
	auto resource_manager = std::make_unique<raylib_resource_manager>();
	auto renderer = std::make_unique<raylib_renderer>(*resource_manager);
	auto input = std::make_unique<raylib_input>();

	return backend{
		.renderer_ptr = std::move(renderer),
		.input_ptr = std::move(input),
		.resource_manager_ptr = std::move(resource_manager),
	};
}

} // namespace lge::raylib_backend