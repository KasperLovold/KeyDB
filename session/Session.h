//
// Created by kasper on 28/05/25.
//

#ifndef SESSION_H
#define SESSION_H
#include <functional>
#include <memory>
#include <string>
#include <asio/streambuf.hpp>
#include <asio/ip/tcp.hpp>

using MessageCallback = std::function<void(const std::string&, int)>;

class Session : public std::enable_shared_from_this<Session> {
public:
    using ResponseCallback = std::function<void(std::string& msg,
                                                std::function<void(const std::string&)> send)>;
    Session(asio::ip::tcp::socket socket, ResponseCallback callback);

    void start();


private:
    void doRead();

    void send(const std::string &message);

    asio::ip::tcp::socket socket_;
    std::string rawBuffer_;
    std::array<char, 4096> buffer_;
    ResponseCallback callback_;
};


#endif //SESSION_H
