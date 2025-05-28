#include "connection/Connection.h"
#include "handlers/handleClientMessage.h"
#include "inmem_database/InMemoryDB.h"
#include <csignal>
#include <iostream>
#include <asio.hpp>

std::atomic shouldStop = false;

void signalHandler(int signum) {
    shouldStop = true;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    InMemoryDB db;
    asio::io_context io;

    Connection server(io, 8080);

    server.start(db);
    io.run();

    while (!shouldStop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    std::cout << "Shutting down..." << std::endl;
    return 0;
}
