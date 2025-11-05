#include <string>
#include <vector>

#ifndef ADB_UTILS_HPP
#define ADB_UTILS_HPP

namespace FWork {
	namespace ADB {
		std::wstring GetProcessPath(const std::wstring& processName);
		std::vector<std::string> GetAvailableDrives();
		std::string FindAdbPath(const std::string& emulatorFolder);
		std::string GetAdbPath();
		bool InitializeADB();
	};
}
#endif