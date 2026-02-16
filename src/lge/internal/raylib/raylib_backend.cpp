// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include "raylib_backend.hpp"

#include <lge/internal/raylib/raylib_input.hpp>
#include <lge/internal/raylib/raylib_renderer.hpp>
#include <lge/internal/raylib/raylib_resource_manager.hpp>

#include <memory>
#include <utility>

namespace lge::raylib_backend {

auto create() -> backend_components {
	auto resource_manager = std::make_unique<raylib_resource_manager>();
	auto renderer = std::make_unique<raylib_renderer>(*resource_manager);
	auto input = std::make_unique<raylib_input>();

	return backend_components{
		.resource_manager = std::move(resource_manager),
		.renderer = std::move(renderer),
		.input = std::move(input),
	};
}

} // namespace lge::raylib_backend