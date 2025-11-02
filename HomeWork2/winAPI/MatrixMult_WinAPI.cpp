#include <iostream>
#include <vector>
#include <windows.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <algorithm>
#include <string>

using Matrix = std::vector<std::vector<int>>;

struct CommonData {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    int N;
    int block_size;
    int num_blocks;
};

struct TaskManager {
    CRITICAL_SECTION mtx;
    int next_block_i = 0;
    int next_block_j = 0;
};

struct ThreadFuncParams {
    CommonData* data;
    TaskManager* tasks;
};

DWORD WINAPI worker_winapi(LPVOID lpParam) {
    ThreadFuncParams* params = static_cast<ThreadFuncParams*>(lpParam);
    CommonData* data = params->data;
    TaskManager* tasks = params->tasks;

    while (TRUE) {
        int block_i = -1;
        int block_j = -1;

        EnterCriticalSection(&tasks->mtx);

        if (tasks->next_block_i < data->num_blocks) {
            block_i = tasks->next_block_i;
            block_j = tasks->next_block_j;

            tasks->next_block_j++;
            if (tasks->next_block_j >= data->num_blocks) {
                tasks->next_block_j = 0;
                tasks->next_block_i++;
            }
        }

        LeaveCriticalSection(&tasks->mtx);

        if (block_i == -1) {
            break;
        }

        int start_row_C = block_i * data->block_size;
        int start_col_C = block_j * data->block_size;

        int end_row_C = (std::min)(start_row_C + data->block_size, data->N);
        int end_col_C = (std::min)(start_col_C + data->block_size, data->N);

        for (int l = 0; l < data->num_blocks; ++l) {
            int start_col_A = l * data->block_size;
            int end_col_A = (std::min)(start_col_A + data->block_size, data->N);

            for (int i = start_row_C; i < end_row_C; ++i) {
                for (int j = start_col_C; j < end_col_C; ++j) {
                    for (int k = start_col_A; k < end_col_A; ++k) {
                        (*data->C)[i][j] += (*data->A)[i][k] * (*data->B)[k][j];
                    }
                }
            }
        }
    }
    return 0;
}

int main() {
    setlocale(LC_ALL, "Russian");

    const int N = 48;
    Matrix A(N, std::vector<int>(N));
    Matrix B(N, std::vector<int>(N));

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            A[i][j] = rand() % 10;
            B[i][j] = rand() % 10;
        }
    }

    std::cout << "--- Однопоточное умножение ---" << std::endl;
    Matrix C_single(N, std::vector<int>(N, 0));
    auto start_time = std::chrono::high_resolution_clock::now();

    for (int i = 0; i < N; ++i) {
        for (int j = 0; j < N; ++j) {
            for (int k = 0; k < N; ++k) {
                C_single[i][j] += A[i][k] * B[k][j];
            }
        }
    }

    auto end_time = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double, std::milli> single_duration = end_time - start_time;
    std::cout << "Время выполнения: " << single_duration.count() << " мс" << std::endl;

    std::cout << "\n--- Многопоточное умножение (windows.h) ---" << std::endl;

    SYSTEM_INFO sysInfo;
    GetSystemInfo(&sysInfo);
    const int num_hardware_threads = sysInfo.dwNumberOfProcessors;

    std::cout << "Используем пул потоков: " << num_hardware_threads << " (ядер/потоков в системе)" << std::endl;
    std::cout << std::setw(10) << "k (блок) "
        << std::setw(15) << "Блоков (задач) "
        << std::setw(15) << "Потоков (размер пула) "
        << std::setw(15) << "Время (мс)" << std::endl;

    for (int k = 1; k <= N; ++k) {
        int block_size = k;
        Matrix C_multi(N, std::vector<int>(N, 0));

        std::vector<HANDLE> threadHandles(num_hardware_threads);

        int num_blocks = static_cast<int>(ceil(static_cast<double>(N) / block_size));
        int total_tasks = num_blocks * num_blocks;

        CommonData common_data;
        common_data.A = &A;
        common_data.B = &B;
        common_data.C = &C_multi;
        common_data.N = N;
        common_data.block_size = block_size;
        common_data.num_blocks = num_blocks;

        TaskManager task_manager;
        task_manager.next_block_i = 0;
        task_manager.next_block_j = 0;

        InitializeCriticalSection(&task_manager.mtx);

        ThreadFuncParams params = { &common_data, &task_manager };

        start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_hardware_threads; ++i) {
            threadHandles[i] = CreateThread(
                NULL,
                0,
                worker_winapi,
                &params,
                0,
                NULL
            );
        }

        WaitForMultipleObjects(
            num_hardware_threads,
            threadHandles.data(),
            TRUE,
            INFINITE
        );

        for (int i = 0; i < num_hardware_threads; ++i) {
            CloseHandle(threadHandles[i]);
        }

        end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> multi_duration = end_time - start_time;

        DeleteCriticalSection(&task_manager.mtx);

        std::cout << std::setw(10) << block_size
            << std::setw(15) << total_tasks
            << std::setw(15) << num_hardware_threads
            << std::setw(15) << multi_duration.count() << std::endl;
    }

    return 0;
}