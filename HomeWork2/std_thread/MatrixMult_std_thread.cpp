#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
#include <mutex>
#include <iomanip>

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
    std::mutex mtx;
    int next_block_i = 0;
    int next_block_j = 0;
};

void worker_std(CommonData* data, TaskManager* tasks) {
    while (true) {
        int block_i = -1;
        int block_j = -1;

        {
            std::lock_guard<std::mutex> lock(tasks->mtx);
            if (tasks->next_block_i < data->num_blocks) {
                block_i = tasks->next_block_i;
                block_j = tasks->next_block_j;

                tasks->next_block_j++;
                if (tasks->next_block_j >= data->num_blocks) {
                    tasks->next_block_j = 0;
                    tasks->next_block_i++;
                }
            }
            else {
                break;
            }
        }


        int start_row_C = block_i * data->block_size;
        int start_col_C = block_j * data->block_size;
        int end_row_C = std::min(start_row_C + data->block_size, data->N);
        int end_col_C = std::min(start_col_C + data->block_size, data->N);

        for (int l = 0; l < data->num_blocks; ++l) {
            int start_col_A = l * data->block_size;
            int end_col_A = std::min(start_col_A + data->block_size, data->N);
            int start_row_B = l * data->block_size;

            for (int i = start_row_C; i < end_row_C; ++i) {
                for (int j = start_col_C; j < end_col_C; ++j) {
                    for (int k = start_col_A; k < end_col_A; ++k) {   
                        (*data->C)[i][j] += (*data->A)[i][k] * (*data->B)[k][j];
                    }
                }
            }
        }
    }
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

    std::cout << "\n--- Многопоточное умножение (std::thread) ---" << std::endl;
    const int num_hardware_threads = std::thread::hardware_concurrency();
    std::cout << "Используем пул потоков: " << num_hardware_threads << " (ядер/потоков в системе)" << std::endl;
    std::cout << std::setw(10) << "k (блок) "
        << std::setw(15) << "Блоков (задач) "
        << std::setw(15) << "Потоков (размер пула) "
        << std::setw(15) << "Время (мс)" << std::endl;


    for (int k = 1; k <= N; ++k) {
        int block_size = k;
        Matrix C_multi(N, std::vector<int>(N, 0));
        std::vector<std::thread> threads;

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

        start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_hardware_threads; ++i) {
            threads.emplace_back(worker_std, &common_data, &task_manager);
        }

        for (auto& t : threads) {
            if (t.joinable()) {
                t.join();
            }
        }

        end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> multi_duration = end_time - start_time;

        // if (C_multi != C_single) {
        //     std::cout << "ОШИБКА при k = " << k << std::endl;
        // }

        std::cout << std::setw(10) << block_size
            << std::setw(15) << total_tasks
            << std::setw(15) << num_hardware_threads
            << std::setw(15) << multi_duration.count() << std::endl;
    }

    return 0;
}