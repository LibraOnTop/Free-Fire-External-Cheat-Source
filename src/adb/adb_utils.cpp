#include <src/adb/adb_utils.hpp>
#include <src/adb/Adb.hpp>
#include <Windows.h>
#include <iostream>
#include <sstream>
#include <thread>
#include <dwmapi.h>
#include <algorithm>
#include <TlHelp32.h>
#include <dxgi.h>
#include <d3dcompiler.h>
#include <filesystem>
#include <psapi.h>
#pragma comment(lib, "Psapi.lib")
#include <EspLines/Offsets.hpp>
#include <EspLines/Memory/Memory.hpp>

using namespace std;

namespace FWork {
    namespace ADB {

        std::wstring GetProcessPath(const std::wstring& processName) {
            PROCESSENTRY32W entry;
            entry.dwSize = sizeof(PROCESSENTRY32W);
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);

            if (snapshot == INVALID_HANDLE_VALUE) return L"";

            std::wstring processPath = L"";
            if (Process32FirstW(snapshot, &entry)) {
                do {
                    if (std::wstring(entry.szExeFile) == processName) {
                        HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION | PROCESS_VM_READ, FALSE, entry.th32ProcessID);
                        if (hProcess) {
                            wchar_t buffer[MAX_PATH];
                            if (GetModuleFileNameExW(hProcess, NULL, buffer, MAX_PATH)) {
                                processPath = buffer;
                            }
                            CloseHandle(hProcess);
                        }
                        break;
                    }
                } while (Process32NextW(snapshot, &entry));
            }

            CloseHandle(snapshot);
            return processPath;
        }

        std::string GetProcessPathAnsi(const std::wstring& processName) {
            std::wstring widePath = GetProcessPath(processName);
            if (widePath.empty()) return "";

            int size_needed = WideCharToMultiByte(CP_ACP, 0, widePath.c_str(), (int)widePath.size(), NULL, 0, NULL, NULL);
            std::string ansiPath(size_needed, 0);
            WideCharToMultiByte(CP_ACP, 0, widePath.c_str(), (int)widePath.size(), &ansiPath[0], size_needed, NULL, NULL);

            return ansiPath;
        }

        std::vector<std::string> GetAvailableDrives() {
            std::vector<std::string> drives;
            DWORD driveMask = GetLogicalDrives();

            for (char letter = 'A'; letter <= 'Z'; ++letter) {
                if (driveMask & (1 << (letter - 'A'))) {
                    std::string drive = { letter, ':', '\\' };
                    drives.push_back(drive);
                }
            }
            return drives;
        }

        std::string FindAdbPath(const std::string& emulatorFolder) {
            std::vector<std::string> drives = GetAvailableDrives();

            for (const auto& drive : drives) {
                std::string adbPath = drive + "Program Files\\" + emulatorFolder + "\\HD-Adb.exe";
                if (std::filesystem::exists(adbPath)) {
                    return adbPath;
                }
            }
            return "";
        }

        std::string GetAdbPath() {
            std::string hdPlayerPath = GetProcessPathAnsi(L"HD-Player.exe");
            std::string blueStacksPath = GetProcessPathAnsi(L"Bluestacks.exe");

            if (!hdPlayerPath.empty()) {
                if (hdPlayerPath.find("BlueStacks_msi5") != std::string::npos) {
                    return FindAdbPath("BlueStacks_msi5");
                }
                else if (hdPlayerPath.find("BlueStacks_msi2") != std::string::npos) {
                    return FindAdbPath("BlueStacks_msi2");
                }
                else if (hdPlayerPath.find("BlueStacks_nxt") != std::string::npos) {
                    return FindAdbPath("BlueStacks_nxt");
                }
                else if (hdPlayerPath.find("BlueStacks") != std::string::npos) {
                    return FindAdbPath("BlueStacks");
                }
            }

            if (!blueStacksPath.empty() && blueStacksPath.find("BlueStacks") != std::string::npos) {
                return FindAdbPath("BlueStacks");
            }
            std::string adbPathMSI5 = FindAdbPath("BlueStacks_msi5");
            std::string adbPathMSI2 = FindAdbPath("BlueStacks_msi2");
            std::string adbPathNXT = FindAdbPath("BlueStacks_nxt");
            std::string adbPathBlueStacks = FindAdbPath("BlueStacks");

            if (!adbPathMSI5.empty()) {
                return adbPathMSI5;
            }
            if (!adbPathMSI2.empty()) {
                return adbPathMSI2;
            }
            if (!adbPathNXT.empty()) {
                return adbPathNXT;
            }
            return adbPathBlueStacks;
        }

        bool InitializeADB() {
            std::cout << "[FreeFire-Cheat] Connecting ADB..." << std::endl;

            std::string adbPath = GetAdbPath();
            if (adbPath.empty()) {
                std::cerr << "Error: ADB path not found." << std::endl;
                return false;
            }

            FWork::Adb adb(adbPath);
            std::cout << "[FreeFire-Cheat] ADB Path Detected: " << adbPath << std::endl;

#ifdef FFMax
            Offsets::Il2Cpp = adb.Start("com.dts.freefiremax", "libil2cpp.so");
#else
            Offsets::Il2Cpp = adb.Start("com.dts.freefireth", "libil2cpp.so");
#endif
            if (Offsets::Il2Cpp == 0) {
                throw std::runtime_error("Error: Failed to find library address.");
            }

            std::cout << "[FreeFire-Cheat] Library Address: " << std::hex << Offsets::Il2Cpp << std::dec << std::endl;
           
            adb.Kill();

            std::cout << "[FreeFire-Cheat] Reading Library" << std::endl;
            unsigned char elf = Mem.Read<unsigned char>(Offsets::Il2Cpp);
            std::stringstream ss;
            ss << "ELF (Decimal): " << static_cast<unsigned int>(elf) << " | ELF (Hexadecimal): 0x" << std::hex << static_cast<unsigned int>(elf);

            std::cout << ss.str() << std::endl;
            return true;
        }
    }
}