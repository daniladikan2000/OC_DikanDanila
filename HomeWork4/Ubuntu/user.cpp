#include <iostream>
#include <unistd.h>     // fork, exec, sleep
#include <sys/wait.h>   // waitpid
#include <string>
#include <csignal>      // kill

using namespace std;

pid_t SpawnProcess(const char* program, char* const args[], bool waitForFinish = false) {
    pid_t pid = fork();

    if (pid == -1) {
        perror("fork failed");
        return -1;
    } else if (pid == 0) {
        execvp(program, args);

        perror("execvp failed");
        exit(1);
    } else {
        if (waitForFinish) {
            int status;
            waitpid(pid, &status, 0);
        }
        return pid;
    }
}

bool IsProcessAlive(pid_t pid) {
    int status;
    pid_t result = waitpid(pid, &status, WNOHANG);

    if (result == 0) {
        return true;
    } else if (result == pid) {
        return false;
    } else {
        return false;
    }
}

int main() {
    string killerExe = "./killer";

    char* victimArgs[] = { (char*)"sleep", (char*)"1000", NULL };

    cout << "ТЕСТ 1: Убийство по ID" << endl;

    pid_t pid1 = SpawnProcess("sleep", victimArgs);
    cout << "User: Запущен sleep (PID: " << pid1 << ")" << endl;

    string pidStr = to_string(pid1);
    char* killerArgs1[] = { (char*)"killer", (char*)"--id", (char*)pidStr.c_str(), NULL };

    cout << "User: Запускаю killer --id..." << endl;
    SpawnProcess(killerExe.c_str(), killerArgs1, true);

    if (!IsProcessAlive(pid1)) cout << "[OK] Процесс мертв." << endl;
    else {
        cout << "[FAIL] Процесс жив. Добиваем..." << endl;
        kill(pid1, SIGKILL);
    }

    cout << "\nТЕСТ 2: Убийство по имени" << endl;
    pid_t pid2 = SpawnProcess("sleep", victimArgs);
    cout << "User: Запущен sleep (PID: " << pid2 << ")" << endl;

    char* killerArgs2[] = { (char*)"killer", (char*)"--name", (char*)"sleep", NULL };

    cout << "User: Запускаю killer --name sleep..." << endl;
    SpawnProcess(killerExe.c_str(), killerArgs2, true);

    if (!IsProcessAlive(pid2)) cout << "[OK] Процесс мертв." << endl;
    else {
        cout << "[FAIL] Процесс жив. Добиваем..." << endl;
        kill(pid2, SIGKILL);
    }


    cout << "\nТЕСТ 3: Переменная окружения" << endl;
    pid_t pid3 = SpawnProcess("sleep", victimArgs);

    cout << "User: Запущен sleep (PID: " << pid3 << ")" << endl;

    setenv("PROC_TO_KILL", "sleep,vlc", 1);
    cout << "User: Переменная PROC_TO_KILL установлена." << endl;

    char* killerArgs3[] = { (char*)"killer", NULL };
    SpawnProcess(killerExe.c_str(), killerArgs3, true);

    if (!IsProcessAlive(pid3)) cout << "[OK] Процесс мертв." << endl;
    else {
        cout << "[FAIL] Процесс жив." << endl;
        kill(pid3, SIGKILL);
    }

    unsetenv("PROC_TO_KILL");
    cout << "User: Переменная PROC_TO_KILL удалена." << endl;

    return 0;
}