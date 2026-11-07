#ifndef KYTY_TESTS_FILE_TIME_TEST_UTIL_H_
#define KYTY_TESTS_FILE_TIME_TEST_UTIL_H_

#include <chrono>
#include <cstdint>
#include <filesystem>

// Sets a host file's write time to `unix_ns` nanoseconds since the Unix epoch.
inline void SetUnixWriteTime(const std::filesystem::path& path, int64_t unix_ns) {
#ifdef _MSVC_STL_VERSION
	// The MSVC file_clock counts 100 ns FILETIME units since 1601. clock_cast routes through
	// utc_clock and yields a negative time with some STL/tzdb versions, which last_write_time
	// rejects, so apply the FILETIME epoch offset the emulator itself uses.
	constexpr int64_t FILETIME_UNIX_EPOCH = 116444736000000000;
	const std::filesystem::file_time_type file_time {
	    std::filesystem::file_time_type::duration(FILETIME_UNIX_EPOCH + unix_ns / 100)};
	std::filesystem::last_write_time(path, file_time);
#else
	const std::chrono::sys_time<std::chrono::nanoseconds> time {std::chrono::nanoseconds(unix_ns)};
	const auto file_time = std::chrono::file_clock::from_sys(time);
	std::filesystem::last_write_time(
	    path, std::chrono::floor<std::filesystem::file_time_type::duration>(file_time));
#endif
}

#endif // KYTY_TESTS_FILE_TIME_TEST_UTIL_H_
