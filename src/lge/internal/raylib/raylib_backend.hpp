// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/input.hpp>
#include <lge/interface/renderer.hpp>
#include <lge/interface/resource_manager.hpp>

#include <memory>

namespace lge {

struct backend_components {
	std::unique_ptr<resource_manager> resource_manager_ptr;
	std::unique_ptr<renderer> renderer_ptr;
	std::unique_ptr<input> input_ptr;
};

namespace raylib_backend {
[[nodiscard]] auto create() -> backend_components;
} // namespace raylib_backend

} // namespace lge