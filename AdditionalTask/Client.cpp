#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <sys/mman.h>

using namespace std;

void ReadViaEnvVar() {
    if (const char* val = getenv("MY_IPC_DATA")) {
        cout << "[Client] УСПЕХ: " << val << endl;
    } else {
        cout << "[Client] Переменная пуста (запустите через Server)." << endl;
    }
}

void ReadViaNamedPipe() {
    cout << "[Client] 2. FIFO..." << endl;
    const auto fifoPath = "/tmp/my_fifo";

    const int fd = open(fifoPath, O_RDONLY);
    if (fd == -1) {
        perror("Open FIFO failed");
        return;
    }

    char buf[128];
    const int bytes = read(fd, buf, sizeof(buf));
    buf[bytes] = '\0';

    cout << "[Client] УСПЕХ: " << buf << endl;
    close(fd);
}

void ReadViaSharedMemory() {
    cout << "[Client] 3. Shared Memory..." << endl;
    const auto shmName = "/my_shm_test";
    constexpr int SIZE = 4096;

    const int shm_fd = shm_open(shmName, O_RDONLY, 0666);
    if (shm_fd == -1) {
        perror("SHM open failed (Run server first!)");
        return;
    }

    void* ptr = mmap(nullptr, SIZE, PROT_READ, MAP_SHARED, shm_fd, 0);
    cout << "[Client] УСПЕХ: " << static_cast<char *>(ptr) << endl;

    munmap(ptr, SIZE);
    close(shm_fd);
}

void ReadViaSocket() {
    cout << "[Client] 4. Socket..." << endl;
    const int sock = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serv_addr{};

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(8080);

    inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr);

    if (connect(sock, reinterpret_cast<struct sockaddr *>(&serv_addr), sizeof(serv_addr)) < 0) {
        perror("Connection Failed");
        return;
    }

    char buffer[1024] = {0};
    read(sock, buffer, 1024);
    cout << "[Client] УСПЕХ: " << buffer << endl;
    close(sock);
}

int main() {
    int choice;
    cout << "CLIENT" << endl;
    cout << "1. Environment Variables" << endl;
    cout << "2. Named Pipes" << endl;
    cout << "3. Shared Memory" << endl;
    cout << "4. Sockets" << endl;
    cout << "Select: ";
    cin >> choice;

    switch(choice) {
        case 1: ReadViaEnvVar(); break;
        case 2: ReadViaNamedPipe(); break;
        case 3: ReadViaSharedMemory(); break;
        case 4: ReadViaSocket(); break;
    }

    return 0;
}