//
// Created by kasper on 25/05/25.
//

#include "Connection.h"
#include <cstring>
#include <iostream>
#include <netinet/in.h>
#include <sys/socket.h>
#include <unistd.h>


Connection::Connection(const size_t port) : port_(port) {}

void Connection::start(const MessageCallback& cb) {
    serverSocket_ = socket(AF_INET, SOCK_STREAM, 0);
    sockaddr_in serverAddr { AF_INET, htons(port_), htonl(INADDR_ANY) };

    if (bind(serverSocket_, reinterpret_cast<sockaddr*>(&serverAddr), sizeof(serverAddr)) < 0) {
        std::cerr << "Failed to bind server socket\n";
        return;
    }

    listen(serverSocket_, 5);
    running_ = true;

    for (size_t i = 0; i < NUM_WORKERS; ++i) {
        workers_.emplace_back(&Connection::workerLoop, this, cb);
    }

    acceptThread_ = std::thread(&Connection::acceptLoop, this, cb);}

void Connection::stop() {
    running_ = false;
    queueCV_.notify_all();

    if (serverSocket_ >= 0) {
        shutdown(serverSocket_, SHUT_RDWR);
        close(serverSocket_);
        serverSocket_ = -1;
    }

    if (acceptThread_.joinable()) acceptThread_.join();

    for (auto& worker : workers_) {
        if (worker.joinable()) worker.join();
    }
}


void Connection::acceptLoop(const MessageCallback& cb) {
    while (running_) {
        sockaddr_in clientAddr{};
        socklen_t clientLen = sizeof(clientAddr);
        int clientSocket = accept(serverSocket_, reinterpret_cast<sockaddr*>(&clientAddr), &clientLen);
        if (clientSocket < 0) continue;

        {
            std::lock_guard lock(queueMutex_);
            clientQueue_.push(clientSocket);
        }
        queueCV_.notify_one();
    }
}

void Connection::workerLoop(const MessageCallback& cb) {
    while (running_) {
        int clientSocket;
        {
            std::unique_lock lock(queueMutex_);
            queueCV_.wait(lock, [&] { return !clientQueue_.empty() || !running_; });

            if (!running_ && clientQueue_.empty()) break;

            clientSocket = clientQueue_.front();
            clientQueue_.pop();
        }

        char buffer[1024] = {};
        ssize_t bytesRead = recv(clientSocket, buffer, sizeof(buffer), 0);

        if (bytesRead > 0) {
            std::string msg(buffer, bytesRead);
            cb(msg, clientSocket);
        }

        close(clientSocket);
    }
}