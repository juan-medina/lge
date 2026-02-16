// SPDX-FileCopyrightText: 2026 Juan Medina
// SPDX-License-Identifier: MIT

#pragma once

#include <source_location>
#include <spdlog/common.h>
#include <spdlog/spdlog.h>

namespace lge {

// Log level constants
#define LGE_LOG_LEVEL_TRACE 0 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_DEBUG 1 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_INFO 2  // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_WARN 3  // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_ERROR 4 // NOLINT(*-macro-usage)
#define LGE_LOG_LEVEL_OFF 5	  // NOLINT(*-macro-usage)

// Default to DEBUG if not set
#ifndef LGE_ACTIVE_LOG_LEVEL
#	define LGE_ACTIVE_LOG_LEVEL LGE_LOG_LEVEL_DEBUG // NOLINT(*-macro-usage)
#endif

class log final {
public:
	log() = delete;
	log(const log &) = delete;
	log(log &&) = delete;
	auto operator=(const log &) -> log & = delete;
	auto operator=(log &&) -> log & = delete;
	~log() = delete;

	// =============================================================================
	// Initialization
	// =============================================================================

	static auto init() noexcept -> void;

	// =============================================================================
	// Logging methods
	// =============================================================================

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_TRACE
	// trace
	template<typename... Args>
	struct trace {
		explicit trace(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			internal_log(spdlog::level::trace, location, fmt, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	trace(spdlog::format_string_t<Args...> fmt, Args &&...) -> trace<Args...>;
#else
	template<typename... Args>
	struct trace {
		explicit trace(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			(void)fmt;
			(void)location;
			(void(args), ...);
		}
	};
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_DEBUG
	// debug
	template<typename... Args>
	struct debug {
		explicit debug(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			internal_log(spdlog::level::debug, location, fmt, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	debug(spdlog::format_string_t<Args...> fmt, Args &&...) -> debug<Args...>;
#else
	template<typename... Args>
	struct debug {
		explicit debug(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			(void)fmt;
			(void)location;
			(void(args), ...);
		}
	};
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_INFO
	// info
	template<typename... Args>
	struct info {
		explicit info(spdlog::format_string_t<Args...> fmt,
					  Args &&...args,
					  const std::source_location &location = std::source_location::current()) {
			internal_log(spdlog::level::info, location, fmt, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	info(spdlog::format_string_t<Args...> fmt, Args &&...) -> info<Args...>;
#else
	template<typename... Args>
	struct info {
		explicit info(spdlog::format_string_t<Args...> fmt,
					  Args &&...args,
					  const std::source_location &location = std::source_location::current()) {
			(void)fmt;
			(void)location;
			(void(args), ...);
		}
	};
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_WARN
	// warn
	template<typename... Args>
	struct warn {
		explicit warn(spdlog::format_string_t<Args...> fmt,
					  Args &&...args,
					  const std::source_location &location = std::source_location::current()) {
			internal_log(spdlog::level::warn, location, fmt, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	warn(spdlog::format_string_t<Args...> fmt, Args &&...) -> warn<Args...>;
#else
	template<typename... Args>
	struct warn {
		explicit warn(spdlog::format_string_t<Args...> fmt,
					  Args &&...args,
					  const std::source_location &location = std::source_location::current()) {
			(void)fmt;
			(void)location;
			(void(args), ...);
		}
	};
#endif

#if LGE_ACTIVE_LOG_LEVEL <= LGE_LOG_LEVEL_ERROR
	// error
	template<typename... Args>
	struct error {
		explicit error(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			internal_log(spdlog::level::err, location, fmt, std::forward<Args>(args)...);
		}
	};

	template<typename... Args>
	error(spdlog::format_string_t<Args...> fmt, Args &&...) -> error<Args...>;
#else
	template<typename... Args>
	struct error {
		explicit error(spdlog::format_string_t<Args...> fmt,
					   Args &&...args,
					   const std::source_location &location = std::source_location::current()) {
			(void)fmt;
			(void)location;
			(void(args), ...);
		}
	};
#endif

private:
	// =============================================================================
	// Internal logging method
	// =============================================================================

	template<typename... Args>
	static auto internal_log(const spdlog::level::level_enum level,
							 const std::source_location &source,
							 spdlog::format_string_t<Args...> fmt,
							 Args &&...args) -> void {
		const auto location =
			spdlog::source_loc{source.file_name(), static_cast<int>(source.line()), source.function_name()};
		spdlog::log(location, level, fmt, std::forward<Args>(args)...);
	}

	// =============================================================================
	// Constants
	// =============================================================================

	static constexpr auto empty_format = "%v";
	static constexpr auto color_line_format = "[%Y-%m-%d %H:%M:%S.%e] [%^%l%$] %v %@";
	static constexpr auto banner = "\033[38;2;0;255;0ml\033[38;2;128;128;128m[\033[38;2;0;0;255mg\033[38;2;128;128;"
								   "128m]\033[38;2;255;0;0me\033[0m";
};

} // namespace lge