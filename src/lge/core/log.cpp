// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#include <lge/core/log.hpp>

#include <spdlog/spdlog.h>

namespace lge {

auto log::init() noexcept -> void {
#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_TRACE
	spdlog::set_level(spdlog::level::trace);
#elif LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_DEBUG
	spdlog::set_level(spdlog::level::debug);
#elif LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_INFO
	spdlog::set_level(spdlog::level::info);
#elif LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_WARN
	spdlog::set_level(spdlog::level::warn);
#elif LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_ERROR
	spdlog::set_level(spdlog::level::err);
#else
	spdlog::set_level(spdlog::level::off);
#endif
	spdlog::set_pattern(empty_format);
	spdlog::info(banner);
	spdlog::set_pattern(color_line_format);
	spdlog::info("log initialized");
}

} // namespace lge