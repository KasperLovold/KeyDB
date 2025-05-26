//
// Created by kasper on 25/05/25.
//

#ifndef HANDLECLIENTMESSAGE_H
#define HANDLECLIENTMESSAGE_H
#include <string>

class InMemoryDB;

namespace handlers {
    void handle_client_message(const std::string& msg, int clientSocket, InMemoryDB& db);
} // handlers

#endif //HANDLECLIENTMESSAGE_H
