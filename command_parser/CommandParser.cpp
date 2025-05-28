//
// Created by kasper on 25/05/25.
//

#include "CommandParser.h"

#include <algorithm>
#include <ranges>
#include <sstream>

std::optional<DBPayload> CommandParser::parse(const std::string &msg) {
    std::istringstream iss(msg);
    std::string op, key;
    iss >> op >> key;

    std::ranges::transform(op, op.begin(), ::toupper);

    if (op == "GET") {
        return DBPayload{ DBCommand::GET, key, std::nullopt };
    }
    if (op == "SET") {
        std::string rawValue;
        iss >> std::ws;
        std::getline(iss, rawValue, ' ');

        std::string next;
        std::optional<int> expiry = std::nullopt;

        iss >> next;
        if (next == "EX") {
            int seconds;
            iss >> seconds;
            expiry = seconds;
        }

        return DBPayload{ DBCommand::SET, key, parseValue(rawValue), expiry };
    }
    if (op == "DEL") {
        return DBPayload{ DBCommand::DEL, key, std::nullopt };
    }
    if (op == "EXISTS") {
        return DBPayload{ DBCommand::EXISTS, key, std::nullopt };
    }
    if (op == "KEYS") {
        return DBPayload{ DBCommand::KEYS, key, std::nullopt };
    }

    if (op == "FLUSHALL") {
        return DBPayload{ DBCommand::FLUSHALL, key, std::nullopt };
    }

    if (op == "INCR") {
        return DBPayload{ DBCommand::INCR, key, std::nullopt };
    }

    if (op == "DECR") {
        return DBPayload { DBCommand::DECR, key, std::nullopt };
    }

    if (op == "PING")
        return DBPayload{ DBCommand::PING, key, std::nullopt };

    return std::nullopt;
}

std::variant<int, double, std::string> CommandParser::parseValue(const std::string &msg) {
    if (msg.empty()) return msg;

    char* end = nullptr;

    long intValue = std::strtol(msg.c_str(), &end, 10);
    if (*end == '\0') {
        return static_cast<int>(intValue);
    }

    double doubleValue = std::strtod(msg.c_str(), &end);
    if (*end == '\0') {
        return doubleValue;
    }

    return msg;
}
