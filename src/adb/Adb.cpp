#include <src/adb/Adb.hpp>
#include <iostream>
using namespace Poco;

namespace FWork {
    Adb::Adb(const std::string& path) : _path(path) {}

    void Adb::Kill() {
        std::cout << "[FreeFire-ADB] Finalizando processos ADB..." << std::endl;

        auto killByName = [](const std::wstring& name) {
            HANDLE snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
            PROCESSENTRY32 entry;
            entry.dwSize = sizeof(PROCESSENTRY32);

            if (Process32First(snapshot, &entry)) {
                do {
                    if (std::wstring(entry.szExeFile) == name) {
                        HANDLE process = OpenProcess(PROCESS_TERMINATE, FALSE, entry.th32ProcessID);
                        if (process) {
                            TerminateProcess(process, 0);
                            CloseHandle(process);
                            std::wcout << L"[FreeFire - ADB] Processo " << name.c_str() << L" finalizado" << std::endl;
                        }
                    }
                } while (Process32Next(snapshot, &entry));
            }
            CloseHandle(snapshot);
            };

        killByName(L"adb.exe");
        killByName(L"HD-Adb.exe");
    }

    uint32_t Adb::Start(std::string processName, std::string moduleName) {
        std::cout << "[FreeFire-ADB] Iniciando ADB..." << std::endl;

        ExecuteAdbCommand("kill-server");
        ExecuteAdbCommand("devices");

        Pipe inPipe, outPipe, errPipe;
        Process::Args args;
        args.push_back("shell");
        args.push_back("getprop ro.secure ; /boot/android/android/system/xbin/bstk/su");

        std::cout << "[FreeFire-ADB] Tentando obter acesso root..." << std::endl;

        Process::launch(_path, args, &inPipe, &outPipe, &errPipe);

        PipeInputStream pis(outPipe);
        PipeInputStream pisErr(errPipe);
        PipeOutputStream pos(inPipe);

        std::string line;
        bool rootAccessGranted = false;
        bool foundPid = false;
        std::string pid;

        int rootCheckAttempts = 0;
        const int maxRootCheckAttempts = 5;

        while (std::getline(pis, line)) {
            if (!rootAccessGranted) {
                if (line.find("0") == 0) {
                    rootAccessGranted = true;
                    std::cout << "[FreeFire-ADB] Acesso root concedido com sucesso!" << std::endl;
                    pos << "ps" << std::endl;
                    pos.flush();
                }
                else if (++rootCheckAttempts >= maxRootCheckAttempts) {
                    std::cerr << "[FreeFire-ADB] Falha ao obter acesso root" << std::endl;
                    return 0;
                }
                continue;
            }

            if (!foundPid && line.find(processName) != std::string::npos) {
                auto parts = split(line, ' ');

                parts.erase(std::remove_if(parts.begin(), parts.end(),
                    [](const std::string& s) { return s.empty(); }), parts.end());

                if (parts.size() >= 5) {
                    pid = parts[1];

                    if (!pid.empty() && pid.find_first_not_of("0123456789") == std::string::npos) {
                        std::cout << "[FreeFire-ADB] PID encontrado: " << pid << std::endl;
                        foundPid = true;

                        std::string mapInput = "cat /proc/" + pid + "/maps | grep " + moduleName;
                        pos << mapInput << std::endl;
                        pos.flush();
                    }
                }
            }

            if (foundPid && line.find(moduleName) != std::string::npos) {
                auto moduleParts = split(line, '-');
                if (!moduleParts.empty()) {
                    std::cout << "[FreeFire-ADB] Módulo encontrado: " << moduleParts[0] << std::endl;
                    return std::stoul(moduleParts[0], nullptr, 16);
                }
            }
        }

        if (!rootAccessGranted) {
            std::cerr << "[FreeFire-ADB] Nenhuma resposta de acesso root recebida" << std::endl;
        }

        std::cerr << "[FreeFire-ADB] Módulo não encontrado" << std::endl;
        return 0;
    }

    void Adb::ExecuteAdbCommand(const std::string& command) {
        Process::Args args;
        args.push_back(command);
        Pipe inPipe, outPipe, errPipe;

        ProcessHandle proc = Process::launch(
            _path,
            args,
            &inPipe,
            &outPipe,
            &errPipe
        );

        PipeInputStream pis(outPipe);
        PipeInputStream pisErr(errPipe);
        PipeOutputStream pos(inPipe);

        proc.wait();
    }

    std::vector<std::string> Adb::split(const std::string& s, char delimiter) {
        std::vector<std::string> tokens;
        std::string token;
        std::istringstream tokenStream(s);
        while (std::getline(tokenStream, token, delimiter)) {
            tokens.push_back(token);
        }
        return tokens;
    }
}