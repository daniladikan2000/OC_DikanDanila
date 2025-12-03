#include <iostream>
#include <string>
#include <windows.h>
#include <tlhelp32.h>

using namespace std;

const string KILLER_PATH = "killer.exe";

DWORD SpawnProcess(string program, string args, bool waitForFinish = false) {
    STARTUPINFOA si;
    PROCESS_INFORMATION pi;
    ZeroMemory(&si, sizeof(si));
    si.cb = sizeof(si);
    ZeroMemory(&pi, sizeof(pi));

    string cmdLine = program + " " + args;

    char* cmdCstr = _strdup(cmdLine.c_str());

    if (!CreateProcessA(NULL, cmdCstr, NULL, NULL, TRUE, CREATE_NEW_CONSOLE, NULL, NULL, &si, &pi)) {
        cerr << "[-] Ошибка CreateProcess: " << GetLastError() << endl;
        free(cmdCstr);
        return 0;
    }
    free(cmdCstr);

    if (waitForFinish) {
        WaitForSingleObject(pi.hProcess, INFINITE);
    }

    DWORD pid = pi.dwProcessId;
    CloseHandle(pi.hThread);
    CloseHandle(pi.hProcess);

    return pid;
}

bool IsProcessAlive(DWORD pid) {
    HANDLE hProcess = OpenProcess(PROCESS_QUERY_INFORMATION, FALSE, pid);
    if (hProcess == NULL) return false;

    DWORD exitCode;
    if (GetExitCodeProcess(hProcess, &exitCode)) {
        CloseHandle(hProcess);
        return (exitCode == STILL_ACTIVE);
    }

    CloseHandle(hProcess);
    return false;
}

int main() {
    setlocale(LC_ALL, "Russian");

    string victimExe = "timeout.exe";
    string victimArgs = "/T 100 /NOBREAK";

    cout << "ТЕСТ 1: Убийство по ID" << endl;

    DWORD pid1 = SpawnProcess(victimExe, victimArgs);
    cout << "User: Запущен sleep (PID: " << pid1 << ")" << endl;

    string killerArgs1 = "--id " + to_string(pid1);

    cout << "User: Запускаю killer --id..." << endl;
    SpawnProcess(KILLER_PATH, killerArgs1, true);

    if (!IsProcessAlive(pid1)) cout << "[OK] Процесс мертв." << endl;
    else cout << "[FAIL] Процесс жив." << endl;

    cout << "\nТЕСТ 2: Убийство по имени" << endl;

    DWORD pid2 = SpawnProcess(victimExe, victimArgs);
    cout << "User: Запущен sleep (PID: " << pid2 << ")" << endl;

    string killerArgs2 = "--name timeout.exe";

    cout << "User: Запускаю killer --name timeout.exe..." << endl;
    SpawnProcess(KILLER_PATH, killerArgs2, true);

    if (!IsProcessAlive(pid2)) cout << "[OK] Процесс мертв." << endl;
    else cout << "[FAIL] Процесс жив." << endl;


    cout << "\nТЕСТ 3: Переменная окружения ===" << endl;

    DWORD pid3 = SpawnProcess(victimExe, victimArgs);
    cout << "User: Запущен sleep (PID: " << pid3 << ")" << endl;

    SetEnvironmentVariableA("PROC_TO_KILL", "calc.exe,timeout.exe,paint.exe");
    cout << "User: Переменная PROC_TO_KILL установлена." << endl;

    SpawnProcess(KILLER_PATH, "", true);

    if (!IsProcessAlive(pid3)) cout << "[OK] Процесс мертв." << endl;
    else cout << "[FAIL] Процесс жив." << endl;

    SetEnvironmentVariableA("PROC_TO_KILL", NULL);
    cout << "User: Переменная PROC_TO_KILL удалена." << endl;

    system("pause");
    return 0;
}