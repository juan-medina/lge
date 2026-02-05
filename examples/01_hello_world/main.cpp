#include <lge/app.hpp>

#include <cstdio>
#include <exception>

auto main() -> int {
	try {
		auto app = lge::app{};
		if(const auto err = app.run().unwrap(); err) [[unlikely]] {
			std::fputs("lge error in main: ", stderr);
			std::fputs(err->to_string().c_str(), stderr);
			std::fputc('\n', stderr);
			return 1;
		}
	} catch(const std::exception &e) {
		std::fputs("unhandled exception in main: ", stderr);
		std::fputs(e.what(), stderr);
		std::fputc('\n', stderr);
	} catch(...) {
		std::fputs("unhandled non-standard exception in main\n", stderr);
	}

	return 0;
}
