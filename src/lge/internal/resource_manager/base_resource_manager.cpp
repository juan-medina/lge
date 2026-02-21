// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/internal/resource_manager/base_resource_manager.hpp>

#include <filesystem>
#include <string_view>

namespace lge {

// =============================================================================
// Common
// =============================================================================

auto base_resource_manager::exists(const std::string_view uri) const -> bool {
	return std::filesystem::exists(uri);
}

} // namespace lge