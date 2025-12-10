#include <iostream>
#include <vector>
#include <pthread.h>
#include <chrono>
#include <cmath>
#include <iomanip>
#include <unistd.h>

using Matrix = std::vector<std::vector<int>>;

struct CommonData {
    const Matrix* A;
    const Matrix* B;
    Matrix* C;
    int N;
    int block_size;
    int num_blocks;
};

struct ThreadParams {
    CommonData* data;
    int thread_id;
    int num_threads;
};

void* worker_pthread(void* arg) {
    ThreadParams* params = static_cast<ThreadParams*>(arg);
    CommonData* data = params->data;

    int total_tasks = data->num_blocks * data->num_blocks;

    for (int task_idx = params->thread_id; task_idx < total_tasks; task_idx += params->num_threads) {
        int block_i = task_idx / data->num_blocks;
        int block_j = task_idx % data->num_blocks;

        int start_row_C = block_i * data->block_size;
        int start_col_C = block_j * data->block_size;
        int end_row_C = std::min(start_row_C + data->block_size, data->N);
        int end_col_C = std::min(start_col_C + data->block_size, data->N);

        for (int l = 0; l < data->num_blocks; ++l) {
            int start_col_A = l * data->block_size;
            int end_col_A = std::min(start_col_A + data->block_size, data->N);

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
    return nullptr;
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

    std::cout << " Pthread" << std::endl;
    int num_hardware_threads = static_cast<int>(sysconf(_SC_NPROCESSORS_ONLN));
    if (num_hardware_threads < 1) num_hardware_threads = 4;

    std::cout << std::setw(10) << "BlockSize" << std::setw(15) << "Time (ms)" << std::endl;

    for (int k = 1; k <= N; ++k) {
        int block_size = k;
        Matrix C_multi(N, std::vector<int>(N, 0));

        std::vector<pthread_t> threads(num_hardware_threads);
        std::vector<ThreadParams> paramsList(num_hardware_threads);

        int num_blocks = (N + block_size - 1) / block_size;
        CommonData common_data{ &A, &B, &C_multi, N, block_size, num_blocks };

        auto start_time = std::chrono::high_resolution_clock::now();

        for (int i = 0; i < num_hardware_threads; ++i) {
            paramsList[i] = { &common_data, i, num_hardware_threads };
            pthread_create(&threads[i], nullptr, worker_pthread, &paramsList[i]);
        }

        for (int i = 0; i < num_hardware_threads; ++i) {
            pthread_join(threads[i], nullptr);
        }

        auto end_time = std::chrono::high_resolution_clock::now();
        std::chrono::duration<double, std::milli> dur = end_time - start_time;
        std::cout << std::setw(10) << block_size << std::setw(15) << dur.count() << std::endl;
    }
    return 0;
}