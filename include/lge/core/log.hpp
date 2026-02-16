// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <spdlog/spdlog.h>

#define LGE_LOG_LEVEL_TRACE 0 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_DEBUG 1 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_INFO 2  // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_WARN 3  // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_ERROR 4 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_OFF 5	  // NOLINT(*-macro-usage)

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_DEBUG // NOLINT(*-include-cleaner)
#	define LGE_DEBUG SPDLOG_DEBUG
#else
#	define LGE_DEBUG(fmt_str, ...) (void)0
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_INFO
#	define LGE_INFO SPDLOG_INFO
#else
#	define LGE_INFO(fmt_str, ...) (void)0
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_TRACE
#	define LGE_TRACE SPDLOG_TRACE
#else
#	define LGE_TRACE(fmt_str, ...) (void)0
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_ERROR
#	define LGE_WARN SPDLOG_WARN
#else
#	define LGE_WARN(fmt_str, ...) (void)0
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_ERROR
#	define LGE_ERROR SPDLOG_ERROR
#else
#	define LGE_ERROR(fmt_str, ...) (void)0
#endif