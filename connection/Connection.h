//
// Created by kasper on 25/05/25.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include <functional>
#include <thread>
#include <vector>
#include <queue>
#include <mutex>
#include <condition_variable>
#include <atomic>

class Connection {
public:
    explicit Connection(size_t port);
    using MessageCallback = std::function<void(const std::string&, int)>;
    void start(const MessageCallback& cb);
    void stop();


private:
    void acceptLoop(const MessageCallback& cb) noexcept;
    void workerLoop(const MessageCallback& cb);

    std::thread loop_;
    int serverSocket_{-1};
    size_t port_;
    std::thread acceptThread_;
    std::vector<std::thread> workers_;
    std::queue<int> clientQueue_;
    std::mutex queueMutex_;
    std::condition_variable queueCV_;
    std::atomic<bool> running_{false};

    const size_t NUM_WORKERS = 4;
};



#endif //CONNECTION_H
