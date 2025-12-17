#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <cmath>
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

void worker_std(CommonData* data, int thread_id, int num_threads) {
    int total_tasks = data->num_blocks * data->num_blocks;

    for (int task_idx = thread_id; task_idx < total_tasks; task_idx += num_threads) {

        int block_i = task_idx / data->num_blocks;
        int block_j = task_idx % data->num_blocks;

        int start_row_C = block_i * data->block_size;
        int start_col_C = block_j * data->block_size;
        int end_row_C = std::min(start_row_C + data->block_size, data->N);
        int end_col_C = std::min(start_col_C + data->block_size, data->N);

        for (int l = 0; l < data->num_blocks; ++l) {
            int start_col_A = l * data->block_size;
            int end_col_A = std::min(start_col_A + data->block_size, data->N);

            // start_row_B совпадает с start_col_A

            for (int i = start_row_C; i < end_row_C; ++i) {
                for (int j = start_col_C; j < end_col_C; ++j) {
                    int sum = 0;
                    for (int k = start_col_A; k < end_col_A; ++k) {
                        sum += (*data->A)[i][k] * (*data->B)[k][j];
                    }
                    (*data->C)[i][j] += sum;
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

    std::cout << "Многопоточное умножение" << std::endl;
    const int num_hardware_threads = std::thread::hardware_concurrency();
    std::cout << "Потоков: " << num_hardware_threads << std::endl;
    std::cout << std::setw(10) << "BlockSize" << std::setw(15) << "TotalTasks" << std::setw(15) << "Time (ms)" << std::endl;

    for (int k = 1; k <= N; ++k) {
        int block_size = k;
        Matrix C_multi(N, std::vector<int>(N, 0));
        std::vector<std::thread> threads;

        int num_blocks = (N + block_size - 1) / block_size;
        int total_tasks = num_blocks * num_blocks;

        CommonData common_data{ &A, &B, &C_multi, N, block_size, num_blocks };

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_hardware_threads; ++i) {
            threads.emplace_back(worker_std, &common_data, i, num_hardware_threads);
        }

        for (auto& t : threads) {
            if (t.joinable()) t.join();
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> duration = end_time - start_time;

        std::cout << std::setw(10) << block_size
                  << std::setw(15) << total_tasks
                  << std::setw(15) << duration.count() << std::endl;
    }
    return 0;
}
