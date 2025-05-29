//
// Created by kasper on 29/05/25.
//

#ifndef RESPOBJECT_H
#define RESPOBJECT_H

#include <memory>
#include <string>
#include <variant>
#include <vector>

struct RespObject;

enum class Type {
    SimpleString,
    Error,
    Integer,
    BulkString,
    Array,
    NullArray,
    NullBulkString
};

struct StackFrame {
    int remainingElements;
    std::vector<std::shared_ptr<RespObject>> elements;
};

using RespValue = std::variant<
    std::string,
    int64_t,
    std::vector<std::shared_ptr<RespObject>>
>;

struct RespObject {
    RespObject(Type type, std::variant<std::string, int64_t, std::vector<std::shared_ptr<RespObject>>> val)
        : type_(type), value_(std::move(val)) {}
    explicit RespObject(Type nullType) : type_(nullType) {}

    [[nodiscard]] Type type() const { return type_; }
    [[nodiscard]] const std::variant<std::string, int64_t, std::vector<std::shared_ptr<RespObject>>>& value() const { return value_; }

    static std::shared_ptr<RespObject> make(const std::string& str, bool isBulk = false, bool isError = false);
    static std::shared_ptr<RespObject> make(int64_t integer);
    static std::shared_ptr<RespObject> make(const std::vector<std::shared_ptr<RespObject>>& elements);
    static std::shared_ptr<RespObject> makeNullBulkString();
    static std::shared_ptr<RespObject> makeNullArray();

private:
    Type type_;
    std::variant<std::string, int64_t, std::vector<std::shared_ptr<RespObject>>> value_;


};


#endif //RESPOBJECT_H
