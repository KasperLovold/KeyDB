//
// Created by kasper on 28/05/25.
//

#include "Session.h"
#include <iostream>
#include <asio/write.hpp>
#include <asio/read_until.hpp>

#include "../handlers/handleClientMessage.h"

Session::Session(asio::ip::tcp::socket socket, const ResponseCallback &callback)
    : socket_(std::move(socket)), callback_(callback) {
}

void Session::start() {
    doRead();
}

void Session::doRead() {
    auto self = shared_from_this();
    asio::async_read_until(socket_, streambuffer_, "\r\n",
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                std::istream is(&streambuffer_);
                std::string command;
                std::getline(is, command);

                if (!command.empty() && command.back() == '\r') {
                    command.pop_back();
                }

                callback_(command, [this, self](const std::string& response) {
                    send(response);
                });
                doRead();
            } else {
                socket_.close();
            }
        });
}

void Session::send(const std::string& message) {
    auto self = shared_from_this();
    asio::async_write(socket_, asio::buffer(message),
        [this, self](const std::error_code ec, std::size_t) {
            if (ec) {
                socket_.close();
            }
        });
}

