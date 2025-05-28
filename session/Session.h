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
    using ResponseCallback = std::function<void(const std::string& msg,
                                                std::function<void(const std::string&)> send)>;
    Session(asio::ip::tcp::socket socket, const ResponseCallback &callback);

    void start();


private:
    void doRead();

    void send(const std::string &message);

    asio::ip::tcp::socket socket_;
    asio::streambuf streambuffer_;
    ResponseCallback callback_;
};


#endif //SESSION_H
