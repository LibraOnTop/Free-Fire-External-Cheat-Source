#include "Render.hpp"
#include <Windows.h>
#include <string>
#include <algorithm>
#include <cctype>
#include <vector>
#include <TlHelp32.h>
#include <iostream>

HWND RenderWindow;

namespace Render
{
    std::vector<DWORD> GetProcessIdsByName(const std::wstring& ProcessName)
    {
        std::vector<DWORD> processIds;

        HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (hSnapshot == INVALID_HANDLE_VALUE) {
            return processIds;
        }

        PROCESSENTRY32 processEntry;
        processEntry.dwSize = sizeof(PROCESSENTRY32);

        if (Process32First(hSnapshot, &processEntry))
        {
            do {
                if (ProcessName == processEntry.szExeFile)
                {
                    processIds.push_back(processEntry.th32ProcessID);
                }
            } while (Process32Next(hSnapshot, &processEntry));
        }

        CloseHandle(hSnapshot);

        return processIds;
    }

    inline BOOL CALLBACK EnumChildWindowsProc(HWND hWnd, LPARAM lParam)
    {
        char windowName[256];
        GetWindowTextA(hWnd, windowName, sizeof(windowName));
        std::string name(windowName);

        char className[256];
        GetClassNameA(hWnd, className, sizeof(className));
        std::string class_str(className);

        std::cout << "[INFO] Verificando janela filha: " << name << " | Classe: " << class_str << std::endl;

        if (name == "_ctl.Window" && class_str.find("BlueStacksApp") != std::wstring::npos)
        {
            RenderWindow = hWnd;
            std::cout << "[INFO] Janela Render encontrada: " << name << std::endl;
            return FALSE;
        }
        else if (name == "HD-Player" && class_str.find("Qt") != std::wstring::npos)
        {
            RenderWindow = hWnd;
            std::cout << "[INFO] Janela Render encontrada: " << name << std::endl;
            return FALSE;
        }

        return TRUE;
    }

    inline BOOL CALLBACK EnumWindowsProc(HWND hWnd, LPARAM lParam)
    {
        DWORD processId = *reinterpret_cast<DWORD*>(lParam);

        DWORD windowProcessId;
        GetWindowThreadProcessId(hWnd, &windowProcessId);

        if (windowProcessId == processId)
        {
            EnumChildWindows(hWnd, EnumChildWindowsProc, lParam);
        }

        return TRUE;
    }

    HWND Render::FindRenderWindow()
    {
        auto BlueStacksProcesses = Render::GetProcessIdsByName(L"Bluestacks.exe");
        if (BlueStacksProcesses.empty()) {
            BlueStacksProcesses = Render::GetProcessIdsByName(L"HD-Player.exe");
        }

        if (BlueStacksProcesses.empty()) {
            std::cout << "[ERROR] Nenhum processo do BlueStacks encontrado!" << std::endl;
            return nullptr;
        }

        for (const auto& ProcessId : BlueStacksProcesses)
        {
            HANDLE ProcessHandle = OpenProcess(PROCESS_ALL_ACCESS, FALSE, ProcessId);
            if (ProcessHandle)
            {
                EnumWindows(EnumWindowsProc, reinterpret_cast<LPARAM>(&ProcessId));

                CloseHandle(ProcessHandle);
            }
        }

        return RenderWindow;
    }
}