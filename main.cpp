#include "connection/Connection.h"
#include "handlers/handleClientMessage.h"
#include "inmem_database/InMemoryDB.h"
#include <csignal>
#include <iostream>

std::atomic<bool> shouldStop = false;

void signalHandler(int signum) {
    shouldStop = true;
}

int main() {
    signal(SIGINT, signalHandler);
    signal(SIGTERM, signalHandler);
    InMemoryDB db;
    Connection conn(8080);
    conn.start([&db](const std::string& msg, const int clientSocket) {
        handlers::handle_client_message(msg, clientSocket, db);
    });

    while (!shouldStop) {
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
    }

    conn.stop();
    std::cout << "Shutting down..." << std::endl;
    return 0;
}
