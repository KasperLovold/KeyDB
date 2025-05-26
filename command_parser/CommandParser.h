//
// Created by kasper on 25/05/25.
//

#ifndef COMMANDPARSER_H
#define COMMANDPARSER_H
#include <optional>
#include <string>
#include <variant>

enum class DBCommand {
    SET,
    GET,
    DEL,
    EXISTS,
    KEYS,
    FLUSHALL,
    INCR,
    DECR
};

struct DBPayload {
    DBCommand command;
    std::string key;
    std::optional<std::variant<int, double, std::string>> data;
};

class CommandParser {
public:
    static std::optional<DBPayload> parse(const std::string &msg);

private:
    static std::variant<int, double, std::string> parseValue(const std::string &msg);
};



#endif //COMMANDPARSER_H
