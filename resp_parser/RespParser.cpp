//
// Created by kasper on 29/05/25.
//

#include "RespParser.h"

#include <iostream>
#include <stack>
#include <bits/ostream.tcc>


RespParser::RespParser(std::string& buffer) : buffer_(buffer) {}

std::optional<std::shared_ptr<RespObject>> RespParser::parse() {
    if (buffer_.empty()) return std::nullopt;

    switch (buffer_[0]) {
        case '+': return parseSimpleString();
        case '-': return parseError();
        case ':': return parseInteger();
        case '$': return parseBulkString();
        case '*': return parseArray();
        default: return std::nullopt;
    }
}

CommandType RespParser::toCommandType(const std::string &cmd) {
    if (cmd == "PING") return CommandType::PING;
    if (cmd == "SET") return CommandType::SET;
    if (cmd == "GET") return CommandType::GET;
    if (cmd == "DEL") return CommandType::DEL;
    if (cmd == "EXISTS") return CommandType::EXISTS;
    if (cmd == "KEYS") return CommandType::KEYS;
    if (cmd == "FLUSHALL") return CommandType::FLUSHALL;
    if (cmd == "INCR") return CommandType::INCR;
    if (cmd == "DECR") return CommandType::DECR;
    return CommandType::UNKNOWN;
}

std::optional<std::string> RespParser::readLine() {
    size_t pos = buffer_.find("\r\n");
    if (pos == std::string::npos) return std::nullopt;
    std::string line = buffer_.substr(0, pos);
    buffer_.erase(0, pos + 2);
    return line;
}

std::optional<std::shared_ptr<RespObject>> RespParser::parseSimpleString() {
    buffer_.erase(0, 1);
    const auto line = readLine();
    if (!line) return std::nullopt;
    return RespObject::make(*line);
}

std::optional<std::shared_ptr<RespObject>> RespParser::parseError() {
    buffer_.erase(0, 1);
    auto line = readLine();
    if (!line) return std::nullopt;
    return RespObject::make(*line, false, true);
}

std::optional<std::shared_ptr<RespObject>> RespParser::parseInteger() {
    buffer_.erase(0, 1);
    const auto line = readLine();
    if (!line) return std::nullopt;
    return RespObject::make(std::stoll(*line));
}

std::optional<std::shared_ptr<RespObject>> RespParser::parseBulkString() {
    buffer_.erase(0, 1);
    const auto line = readLine();
    if (!line) return std::nullopt;
    const int len = std::stoi(*line);
    if (len == -1) return RespObject::make(std::string{}); // Null

    if (buffer_.size() < static_cast<size_t>(len + 2)) return std::nullopt;
    const std::string value = buffer_.substr(0, len);
    buffer_.erase(0, len + 2);
    return RespObject::make(value, true);
}

std::optional<std::shared_ptr<RespObject>> RespParser::parseArray() noexcept {
    buffer_.erase(0, 1);

    auto line = readLine();
    if (!line) return std::nullopt;

    int initialCount;
    try {
        initialCount = std::stoi(*line);
    } catch (...) {
        return std::nullopt;
    }

    if (initialCount == -1)
        return RespObject::makeNullArray();

    std::stack<StackFrame> stack;
    stack.push({initialCount, {}});

    while (!stack.empty()) {
        if (auto&[remainingElements, e] = stack.top(); remainingElements == 0) {
            auto completed = RespObject::make(e);
            stack.pop();

            if (stack.empty()) return completed;
            stack.top().elements.push_back(completed);
            stack.top().remainingElements--;
        } else {
            auto obj = parse();
            if (!obj) return std::nullopt;
            e.push_back(*obj);
            remainingElements--;
        }
    }

    return std::nullopt;
}
