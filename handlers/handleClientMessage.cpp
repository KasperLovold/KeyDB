//
// Created by kasper on 25/05/25.
//

#include "handleClientMessage.h"

#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <bits/locale_facets_nonio.h>

#include "../command_parser/CommandParser.h"
#include "../inmem_database/InMemoryDB.h"
#include <sys/socket.h>

namespace handlers {
    void handle_client_message(const std::string& msg, std::function<void(const std::string&)> sendResponse, InMemoryDB& db) {
        if (const auto command = CommandParser::parse(msg); command.has_value()) {
            switch (const DBPayload& payload = command.value(); payload.command) {
                case DBCommand::GET: {
                    if (const auto result = db.get(payload.key); result.has_value()) {
                        std::visit([&]<typename T0>(T0&& val) {
                            using T = std::decay_t<T0>;
                            std::ostringstream oss;
                            std::string valueStr;

                            if constexpr (std::is_same_v<T, std::string>)
                                valueStr = val;
                            else
                                valueStr = std::to_string(val);

                            oss << "$" << valueStr.size() << "\r\n" << valueStr << "\r\n";
                            sendResponse(oss.str());
                        }, result.value());
                    } else {
                        sendResponse("$-1\r\n"); // null bulk string
                    }
                    break;
                }

                case DBCommand::SET: {
                    const auto data = payload.data.value();
                    db.set(payload.key, data, payload.expiry);
                    const std::string ack = "+OK\r\n";
                    sendResponse(ack);
                    break;
                }
                case DBCommand::DEL: {
                    db.del(payload.key);
                    const std::string ack = "+OK\r\n";
                    sendResponse(ack);
                    break;
                }
                case DBCommand::EXISTS: {
                    sendResponse(db.exists(payload.key) ? ":1\r\n" : ":0\r\n");
                    break;
                }
                case DBCommand::KEYS: {
                    auto keys = db.keys();
                    std::ostringstream oss;
                    oss << keys.size() << "\r\n";
                    for (const auto &k: keys | std::views::keys) {
                        oss << "$" << k.size() << "\r\n" << k << "\r\n";
                    }

                    std::string keys_message = oss.str();
                    sendResponse(keys_message);
                    break;
                }
                case DBCommand::FLUSHALL: {
                    db.flush_all();
                    const std::string ack = "+OK\r\n";
                    sendResponse(ack);
                    break;
                }

                case DBCommand::INCR:
                case DBCommand::DECR: {
                    Result value = (command->command == DBCommand::INCR)
                        ? db.incr(command->key)
                        : db.decr(command->key);

                    std::string resp = value.is_ok()
                        ? ":" + std::to_string(*value.value) + "\r\n"
                        : "-ERR value is not an integer\r\n";
                    sendResponse(resp);
                    break;
                }
                case DBCommand::PING: {
                    std::string ack = "+PONG\r\n";
                    sendResponse(ack);
                }

                default: break;
            }
        } else {
            const std::string err = "-ERR unknown command\r\n";
            sendResponse(err);
        }
    }
}
// handlers