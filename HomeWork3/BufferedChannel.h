#ifndef OC_DIKANDANILA_BUFFEREDCHANNEL_H
#define OC_DIKANDANILA_BUFFEREDCHANNEL_H

#include <queue>
#include <mutex>
#include <condition_variable>
#include <stdexcept>
#include <utility>

template <class T>
class BufferedChannel {
public:
    explicit BufferedChannel(int size) : buffer_size_(size), closed_(false) {}

    void Send(T value) {
        std::unique_lock<std::mutex> lock(mtx_);

        cv_not_full_.wait(lock, [this]() {
            return queue_.size() < buffer_size_ || closed_;
        });

        if (closed_) {
            throw std::runtime_error("Channel is closed");
        }

        queue_.push(std::move(value));

        cv_not_empty_.notify_one();
    }

    std::pair<T, bool> Recv() {
        std::unique_lock<std::mutex> lock(mtx_);

        cv_not_empty_.wait(lock, [this]() {
            return !queue_.empty() || closed_;
        });

        if (queue_.empty() && closed_) {
            return {T(), false};
        }

        T value = std::move(queue_.front());
        queue_.pop();

        cv_not_full_.notify_one();

        return {std::move(value), true};
    }

    void Close() {
        std::unique_lock<std::mutex> lock(mtx_);
        closed_ = true;

        cv_not_full_.notify_all();
        cv_not_empty_.notify_all();
    }

private:
    size_t buffer_size_;
    std::queue<T> queue_;
    bool closed_;
    std::mutex mtx_;
    std::condition_variable cv_not_full_;
    std::condition_variable cv_not_empty_;
};

#endif //OC_DIKANDANILA_BUFFEREDCHANNEL_H