//
// Created by kasper on 25/05/25.
//

#ifndef HANDLECLIENTMESSAGE_H
#define HANDLECLIENTMESSAGE_H
#include <functional>
#include <memory>
#include <string>
#include "../resp_parser/RespObject.h"

class InMemoryDB;

namespace handlers {
    void handle_client_message(const std::string& msg, std::function<void(const std::string&)> sendResponse, InMemoryDB& db);
    void handle_resp_message(const std::shared_ptr<RespObject>& commandObj, const std::function<void(const std::string&)> &sendResponse, InMemoryDB& db);
} // handlers

#endif //HANDLECLIENTMESSAGE_H
