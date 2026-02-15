// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <spdlog/common.h>
#include <spdlog/spdlog.h>

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_DEBUG && !defined(CLANGD_ACTIVE)
#	define LGE_DEBUG SPDLOG_DEBUG
#else
#	define LGE_DEBUG(fmt_str, ...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_INFO && !defined(CLANGD_ACTIVE)
#	define LGE_INFO SPDLOG_INFO
#else
#	define LGE_INFO(fmt_str, ...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_TRACE && !defined(CLANGD_ACTIVE)
#	define LGE_TRACE SPDLOG_TRACE
#else
#	define LGE_TRACE(fmt_str, ...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_WARN && !defined(CLANGD_ACTIVE)
#	define LGE_WARN SPDLOG_WARN
#else
#	define LGE_WARN(fmt_str, ...) (void)0
#endif

#if SPDLOG_ACTIVE_LEVEL <= SPDLOG_LEVEL_ERROR && !defined(CLANGD_ACTIVE)
#	define LGE_ERROR SPDLOG_ERROR
#else
#	define LGE_ERROR(fmt_str, ...) (void)0
#endif