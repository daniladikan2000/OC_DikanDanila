#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/wait.h>

using namespace std;


void SendViaEnvVar() {
    cout << "[Server] 1. Environment Variables" << endl;

    setenv("MY_IPC_DATA", "Hello_from_Linux_Env_Var!", 1);

    if (const pid_t pid = fork(); pid == 0) {
        execl("./Client", "Client", NULL);

        perror("[Server] Ошибка запуска Client");
        exit(1);
    } else if (pid > 0) {
        wait(nullptr);
        cout << "[Server] Дочерний процесс завершился." << endl;
    } else {
        perror("Fork failed");
    }
}

void SendViaNamedPipe() {
    cout << "[Server] 2. Named Pipes (FIFO)" << endl;
    const auto fifoPath = "/tmp/my_fifo";

    mkfifo(fifoPath, 0666);

    cout << "[Server] Жду подключения клиента..." << endl;
    const int fd = open(fifoPath, O_WRONLY);

    const string msg = "Hello from Linux FIFO!";
    write(fd, msg.c_str(), msg.size() + 1);

    cout << "[Server] Данные отправлены." << endl;
    close(fd);
    // unlink(fifoPath); // По идее можно удалить файл, но я решил оставить для теста
}

void SendViaSharedMemory() {
    cout << "[Server] 3. Shared Memory" << endl;
    const auto shmName = "/my_shm_test";
    constexpr int SIZE = 4096;

    const int shm_fd = shm_open(shmName, O_CREAT | O_RDWR, 0666);
    if (shm_fd == -1) {
        perror("Shared memory failed");
        return;
    }

    ftruncate(shm_fd, SIZE);
    void* ptr = mmap(nullptr, SIZE, PROT_WRITE, MAP_SHARED, shm_fd, 0);

    const auto msg = "Hello from Linux Shared Memory!";
    memcpy(ptr, msg, strlen(msg) + 1);

    cout << "[Server] Данные записаны в SHM. Нажми Enter, когда Клиент прочитает..." << endl;
    cin.get(); cin.get();

    munmap(ptr, SIZE);
    close(shm_fd);
    shm_unlink(shmName);
}

void SendViaSocket() {
    cout << "[Server] 4. Sockets" << endl;

    const int server_fd = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in address{};
    constexpr int opt = 1;
    int addrlen = sizeof(address);

    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    address.sin_family = AF_INET;
    address.sin_addr.s_addr = INADDR_ANY;
    address.sin_port = htons(8080);

    bind(server_fd, reinterpret_cast<struct sockaddr *>(&address), sizeof(address));
    listen(server_fd, 3);

    if (const int new_socket = accept(server_fd, reinterpret_cast<struct sockaddr *>(&address), reinterpret_cast<socklen_t *>(&addrlen)); new_socket >= 0) {
        cout << "[Server] Клиент подключен." << endl;
        const auto msg = "Hello from Linux Socket!";
        send(new_socket, msg, strlen(msg), 0);
        cout << "[Server] Отправлено." << endl;
        close(new_socket);
    }
    close(server_fd);
}

int main() {
    int choice;
    cout << "SERVER" << endl;
    cout << "1. Environment Variables" << endl;
    cout << "2. Named Pipes (FIFO)" << endl;
    cout << "3. Shared Memory" << endl;
    cout << "4. Sockets" << endl;
    cout << "Select: ";
    cin >> choice;

    switch(choice) {
        case 1: SendViaEnvVar(); break;
        case 2: SendViaNamedPipe(); break;
        case 3: SendViaSharedMemory(); break;
        case 4: SendViaSocket(); break;
    }

    return 0;
}