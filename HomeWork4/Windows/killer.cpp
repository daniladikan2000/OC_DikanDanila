#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

void KillProcessByID(int pid) {
    HANDLE hProcess = OpenProcess(PROCESS_TERMINATE, FALSE, pid);
    if (hProcess == NULL) {
        cerr << "[-] Ошибка: Не удалось открыть процесс PID " << pid << " (" << GetLastError() << ")" << endl;
        return;
    }

    if (TerminateProcess(hProcess, 1)) {
        cout << "[+] Процесс " << pid << " успешно убит (TerminateProcess)." << endl;
    }
    else {
        cerr << "[-] Ошибка при убийстве процесса" << endl;
    }
    CloseHandle(hProcess);
}

void KillProcessByName(const string& processName) {
    HANDLE hSnapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
    if (hSnapshot == INVALID_HANDLE_VALUE) return;

    PROCESSENTRY32 pe;
    pe.dwSize = sizeof(PROCESSENTRY32);

    if (Process32First(hSnapshot, &pe)) {
        do {
            char currentName[MAX_PATH];
            size_t num;
            wcstombs_s(&num, currentName, pe.szExeFile, MAX_PATH);

            if (string(currentName) == processName) {
                cout << "[*] Найдено совпадение: " << currentName << " (PID: " << pe.th32ProcessID << ")" << endl;
                KillProcessByID(pe.th32ProcessID);
            }
        } while (Process32Next(hSnapshot, &pe));
    }
    CloseHandle(hSnapshot);
}

void KillFromEnv() {
    char* envVal = nullptr;
    size_t len = 0;
    if (_dupenv_s(&envVal, &len, "PROC_TO_KILL") == 0 && envVal != nullptr) {
        cout << "[*] Найдена переменная PROC_TO_KILL: " << envVal << endl;

        string s(envVal);
        string delimiter = ",";
        size_t pos = 0;
        string token;

        while ((pos = s.find(delimiter)) != string::npos) {
            token = s.substr(0, pos);
            if (!token.empty()) KillProcessByName(token);
            s.erase(0, pos + delimiter.length());
        }
        if (!s.empty()) KillProcessByName(s);

        free(envVal);
    }
}

int main(int argc, char* argv[]) {
    setlocale(LC_ALL, "Russian");

    KillFromEnv();

    for (int i = 1; i < argc; ++i) {
        string arg = argv[i];

        if (arg == "--id" && i + 1 < argc) {
            int pid = stoi(argv[++i]);
            KillProcessByID(pid);
        }
        else if (arg == "--name" && i + 1 < argc) {
            string name = argv[++i];
            KillProcessByName(name);
        }
    }

    return 0;
}