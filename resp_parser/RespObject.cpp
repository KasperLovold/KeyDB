//
// Created by kasper on 29/05/25.
//

#include "RespObject.h"

std::shared_ptr<RespObject> RespObject::makeNullArray() {
    return std::make_shared<RespObject>(Type::NullArray);
}

std::shared_ptr<RespObject> RespObject::make(const std::string& str, bool isBulk, bool isError) {
    if (isError)
        return std::make_shared<RespObject>(Type::Error, str);
    if (isBulk)
        return std::make_shared<RespObject>(Type::BulkString, str);

    return std::make_shared<RespObject>(Type::SimpleString, str);
}

std::shared_ptr<RespObject> RespObject::make(int64_t integer) {
    return std::make_shared<RespObject>(Type::Integer, integer);
}

std::shared_ptr<RespObject> RespObject::make(const std::vector<std::shared_ptr<RespObject>>& elements) {
    return std::make_shared<RespObject>(Type::Array, elements);
}

std::shared_ptr<RespObject> RespObject::makeNullBulkString() {
    return std::make_shared<RespObject>(Type::NullBulkString);
}