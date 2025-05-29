//
// Created by kasper on 28/05/25.
//

#include "Session.h"
#include <iostream>
#include <asio/write.hpp>
#include <asio/read.hpp>
#include <asio/buffer.hpp>
#include <utility>
#include "../resp_parser/RespParser.h"

#include "../handlers/handleClientMessage.h"

Session::Session(asio::ip::tcp::socket socket, ResponseCallback callback)
    : socket_(std::move(socket)), buffer_(), callback_(std::move(callback)) {
}

void Session::start() {
    doRead();
}

void Session::doRead() {
    auto self = shared_from_this();
    socket_.async_read_some(asio::buffer(buffer_),
        [this, self](std::error_code ec, std::size_t length) {
            if (!ec) {
                rawBuffer_.append(buffer_.data(), length);
                callback_(rawBuffer_, [this, self](const std::string& response) {
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

