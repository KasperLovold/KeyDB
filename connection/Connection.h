//
// Created by kasper on 25/05/25.
//

#ifndef CONNECTION_H
#define CONNECTION_H
#include <asio.hpp>
#include "../session/Session.h"
#include "../inmem_database/InMemoryDB.h"


class Connection {
public:
    Connection(asio::io_context& io, uint16_t port);

    void start(InMemoryDB &db);


private:
    void doAccept(InMemoryDB &db);

    asio::ip::tcp::acceptor acceptor_;
    asio::io_context& io_;
};


#endif //CONNECTION_H
