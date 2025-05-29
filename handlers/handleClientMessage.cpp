//
// Created by kasper on 25/05/25.
//

#include "handleClientMessage.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <ranges>
#include <sstream>
#include <bits/locale_facets_nonio.h>

#include "../command_parser/CommandParser.h"
#include "../inmem_database/InMemoryDB.h"
#include <sys/socket.h>

#include "../resp_parser/RespParser.h"

enum class CommandType;

namespace handlers {
    void handle_resp_message(const std::shared_ptr<RespObject> &commandObj,
        const std::function<void(const std::string &)> &sendResponse, InMemoryDB &db) {

        if (!commandObj || commandObj->type() != Type::Array) {
            sendResponse("-ERR expected array\r\n");
            return;
        }

        const auto elements = std::get<std::vector<std::shared_ptr<RespObject>>>(commandObj->value());
        if (elements.empty()) {
            sendResponse("-ERR empty command\r\n");
            return;
        }

        auto toString = [](const std::shared_ptr<RespObject> &obj) -> std::optional<std::string> {
            if (!obj) return std::nullopt;

            if ((obj->type() == Type::BulkString || obj->type() == Type::SimpleString) &&
                std::holds_alternative<std::string>(obj->value())) {
                return std::get<std::string>(obj->value());
            }
            return std::nullopt;
        };

        if (!elements[0]) {
            sendResponse("-ERR command must be a string\r\n");
            return;
        }

        const auto& cmd = std::get<std::string>(elements[0]->value());

        if (cmd == "PING") {
            sendResponse("+PONG\r\n");

        } else if (cmd == "GET" && elements.size() == 2) {
            auto keyOpt = toString(elements[1]);
            if (!keyOpt) {
                sendResponse("-ERR invalid key\r\n");
                return;
            }

            auto result = db.get(*keyOpt);
            if (!result.has_value()) {
                sendResponse("$-1\r\n");
                return;
            }

            std::visit([&]<typename T0>(T0&& val) {
                using T = std::decay_t<T0>;
                std::string str;

                if constexpr (std::is_same_v<T, std::string>) {
                    str = val;
                } else {
                    str = std::to_string(val);
                }

                sendResponse("$" + std::to_string(str.size()) + "\r\n" + str + "\r\n");
            }, *result);

        } else if (cmd == "SET" && elements.size() >= 3) {
            auto keyOpt = toString(elements[1]);
            auto valOpt = toString(elements[2]);
            if (!keyOpt || !valOpt) {
                sendResponse("-ERR invalid key or value\r\n");
                return;
            }

            std::optional<int> expiry = std::nullopt;
            if (elements.size() == 5) {
                auto exOpt = toString(elements[3]);
                auto ttlOpt = toString(elements[4]);
                if (exOpt && *exOpt == "EX" && ttlOpt) {
                    expiry = std::stoi(*ttlOpt);
                }
            }

            db.set(*keyOpt, CommandParser::parseValue(*valOpt), expiry);
            sendResponse("+OK\r\n");

        } else if (cmd == "DEL" && elements.size() == 2) {
            auto keyOpt = toString(elements[1]);
            if (!keyOpt) {
                sendResponse("-ERR invalid key\r\n");
                return;
            }
            db.del(*keyOpt);
            sendResponse("+OK\r\n");

        } else if (cmd == "EXISTS" && elements.size() == 2) {
            auto keyOpt = toString(elements[1]);
            if (!keyOpt) {
                sendResponse("-ERR invalid key\r\n");
                return;
            }
            sendResponse(db.exists(*keyOpt) ? ":1\r\n" : ":0\r\n");

        } else if (cmd == "KEYS") {
            auto entries = db.keys();
            std::ostringstream oss;
            oss << "*" << entries.size() << "\r\n";
            for (const auto &k: entries | std::views::keys) {
                oss << "$" << k.size() << "\r\n" << k << "\r\n";
            }
            sendResponse(oss.str());

        } else if (cmd == "FLUSHALL") {
            db.flush_all();
            sendResponse("+OK\r\n");

        } else if ((cmd == "INCR" || cmd == "DECR") && elements.size() == 2) {
            auto keyOpt = toString(elements[1]);
            if (!keyOpt) {
                sendResponse("-ERR invalid key\r\n");
                return;
            }

            auto result = (cmd == "INCR") ? db.incr(*keyOpt) : db.decr(*keyOpt);
            if (result.is_ok()) {
                sendResponse(":" + std::to_string(*result.value) + "\r\n");
            } else {
                sendResponse("-ERR value is not an integer\r\n");
            }

        } else {
            sendResponse("-ERR unknown or invalid command\r\n");
        }
    }
}
// handlers