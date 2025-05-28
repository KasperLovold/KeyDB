//
// Created by kasper on 25/05/25.
//

#include "Connection.h"
#include "../handlers/handleClientMessage.h"

Connection::Connection(asio::io_context& io, const uint16_t port)
    : acceptor_(io, asio::ip::tcp::endpoint(asio::ip::tcp::v4(), port)), io_(io) {
}

void Connection::start(InMemoryDB& db) {
    doAccept(db);
}

void Connection::doAccept(InMemoryDB& db) {
    acceptor_.async_accept([this, &db](const std::error_code ec, asio::ip::tcp::socket socket) {
        if (!ec) {
            const auto session = std::make_shared<Session>(
                std::move(socket),
                [&db](const std::string& msg, std::function<void(const std::string&)> send) {
                    handlers::handle_client_message(msg, std::move(send), db);
                });
            session->start();
        }
        doAccept(db);
    });
}
