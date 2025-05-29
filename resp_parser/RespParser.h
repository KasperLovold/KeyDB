//
// Created by kasper on 29/05/25.
//

#ifndef RESPPARSER_H
#define RESPPARSER_H
#include <memory>
#include <optional>
#include <bits/stringfwd.h>
#include "RespObject.h"

enum class CommandType {
    PING,
    SET,
    GET,
    DEL,
    EXISTS,
    KEYS,
    FLUSHALL,
    INCR,
    DECR,
    UNKNOWN
};


class RespParser {
public:
    explicit RespParser(std::string& buffer);

    std::optional<std::shared_ptr<RespObject>> parse();
    static CommandType toCommandType(const std::string& cmd);

private:
    std::string& buffer_;

    [[nodiscard]] std::optional<std::string> readLine();
    [[nodiscard]] std::optional<std::shared_ptr<RespObject>> parseSimpleString();
    [[nodiscard]] std::optional<std::shared_ptr<RespObject>> parseError();
    [[nodiscard]] std::optional<std::shared_ptr<RespObject>> parseInteger();
    [[nodiscard]] std::optional<std::shared_ptr<RespObject>> parseBulkString();
    [[nodiscard]] std::optional<std::shared_ptr<RespObject>> parseArray() noexcept;
};
#endif //RESPPARSER_H
