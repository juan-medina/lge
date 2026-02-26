// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include "audio_manager.hpp"
#include "input.hpp"
#include "renderer.hpp"
#include "resource_manager.hpp"

#include <memory>

namespace lge {

struct backend {
	std::unique_ptr<renderer> renderer_ptr;
	std::shared_ptr<input> input_ptr;
	std::shared_ptr<resource_manager> resource_manager_ptr;
	std::shared_ptr<audio_manager> audio_manager_ptr;
};

} // namespace lge