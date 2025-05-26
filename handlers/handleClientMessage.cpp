//
// Created by kasper on 25/05/25.
//

#include "handleClientMessage.h"

#include <functional>
#include <iostream>
#include <sstream>
#include <bits/locale_facets_nonio.h>

#include "../command_parser/CommandParser.h"
#include "../inmem_database/InMemoryDB.h"
#include <sys/socket.h>

namespace handlers {
    void handle_client_message(const std::string &msg, const int clientSocket, InMemoryDB &db) {
        if (const auto command = CommandParser::parse(msg); command.has_value()) {
            switch (const DBPayload& payload = command.value(); payload.command) {
                case DBCommand::GET: {
                    if (const auto result = db.get(payload.key); result.has_value())
                        std::visit([&]<typename T0>(T0&& val) {
                            using T = std::decay_t<T0>;
                            std::string out;
                            if constexpr (std::is_same_v<T, std::string>)
                                out = "\"" + val + "\"" + "\r\n";
                            else
                                out = std::to_string(val) + "\r\n";
                            send(clientSocket, out.c_str(), out.size(), 0);
                    }, result.value()); else {
                        const std::string notFound = "nil\r\n";
                        send(clientSocket, notFound.c_str(), notFound.size(), 0);
                    }
                    break;
                }

                case DBCommand::SET: {
                    const auto data = payload.data.value();
                    db.set(payload.key, data);
                    const std::string ack = "OK\r\n";
                    send(clientSocket, ack.c_str(), ack.size(), 0);
                    break;
                }

                case DBCommand::DEL: {
                    db.del(payload.key);
                    const std::string ack = "OK\r\n";
                    send(clientSocket, ack.c_str(), ack.size(), 0);
                    break;
                }
                case DBCommand::EXISTS: {
                    if (db.exists(payload.key)) {
                        const std::string ack = "true\r\n";
                        send(clientSocket, ack.c_str(), ack.size(), 0);
                    } else {
                        const std::string notFound = "false\r\n";
                        send(clientSocket, notFound.c_str(), notFound.size(), 0);
                    }
                    break;
                }
                case DBCommand::KEYS: {
                    auto keys = db.keys();
                    std::ostringstream oss;
                    for (const auto& [k, v] : keys) {
                        size_t size;
                        std::visit([&]<typename T0>(const T0& val) {
                            using T = std::decay_t<T0>;
                            if constexpr (std::is_same_v<T, std::string>)
                                size = val.size();
                            else size = sizeof(T);
                        }, v);
                        oss << "$" << size << "\r\n" << k << "\r\n";
                    }

                    std::string keys_message = oss.str();
                    send(clientSocket, keys_message.c_str(), keys_message.size(), 0);
                    break;
                }
                case DBCommand::FLUSHALL: {
                    db.flush_all();
                    const std::string ack = "nil\r\n";
                    send(clientSocket, ack.c_str(), ack.size(), 0);
                    break;
                }

                case DBCommand::INCR:
                case DBCommand::DECR: {
                    Result value = (command->command == DBCommand::INCR)
                        ? db.incr(command->key)
                        : db.decr(command->key);

                    std::string ack = value.is_ok()
                        ? std::to_string(*value.value) + "\r\n"
                        : "(error) Cannot increment/decrement non-integer value\r\n";

                    send(clientSocket, ack.c_str(), ack.size(), 0);
                    break;
                }

                default: break;
            }
        } else {
            const std::string err = "ERR unknown command\r\n";
            send(clientSocket, err.c_str(), err.size(), 0);
        }
    }
}
// handlers