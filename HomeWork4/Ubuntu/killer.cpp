#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <csignal>      // kill()
#include <sys/types.h>  // pid_t
#include <dirent.h>
#include <fstream>
#include <cstdlib>      // getenv

using namespace std;

void KillProcessByID(int pid) {
    if (kill(pid, SIGKILL) == 0) {
        cout << "[+] Процесс " << pid << " успешно убит (SIGKILL)." << endl;
    } else {
        perror("[-] Ошибка при убийстве процесса");
    }
}

void KillProcessByName(const string& processName) {
    DIR* procDir = opendir("/proc");
    if (!procDir) return;

    struct dirent* entry;
    while ((entry = readdir(procDir)) != nullptr) {

        if (!isdigit(*entry->d_name)) continue;

        int pid = stoi(entry->d_name);

        string commPath = string("/proc/") + entry->d_name + "/comm";

        ifstream commFile(commPath);
        if (commFile.is_open()) {
            string name;
            getline(commFile, name);

            if (name == processName) {
                cout << "[*] Найдено совпадение: " << name << " (PID: " << pid << ")" << endl;
                KillProcessByID(pid);
            }
            commFile.close();
        }
    }
    closedir(procDir);
}

void KillFromEnv() {
    const char* envVal = getenv("PROC_TO_KILL");
    if (envVal == nullptr) return;

    cout << "[*] Найдена переменная PROC_TO_KILL: " << envVal << endl;

    string s = envVal;
    string delimiter = ",";
    size_t pos = 0;
    string token;

    while ((pos = s.find(delimiter)) != string::npos) {
        token = s.substr(0, pos);
        if (!token.empty()) KillProcessByName(token);
        s.erase(0, pos + delimiter.length());
    }
    if (!s.empty()) KillProcessByName(s);
}

int main(int argc, char* argv[]) {
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