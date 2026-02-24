// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <lge/interface/backend.hpp>

namespace lge::raylib_backend {

[[nodiscard]] auto create() -> backend;

} // namespace lge::raylib_backend
