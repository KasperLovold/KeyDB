//
// Created by kasper on 25/05/25.
//

#include "InMemoryDB.h"

#include <ranges>
#include <variant>

std::optional<std::variant<int, double, std::string>> InMemoryDB::get(const std::string &key) {
    std::scoped_lock lock(mtx_);
    if (map_.contains(key))
        return map_[key];
    return std::nullopt;
}

void InMemoryDB::set(const ::std::string &key, const std::variant<int, double, std::string> &value) {
    std::scoped_lock lock(mtx_);
    map_[key] = value;
}

void InMemoryDB::del(const std::string &key) {
    std::scoped_lock lock(mtx_);
    map_.erase(key);
}

bool InMemoryDB::exists(const std::string &key) {
    std::scoped_lock lock(mtx_);
    return map_.contains(key);
}

void InMemoryDB::flush_all() {
    std::scoped_lock lock(mtx_);
    map_.clear();
}

std::vector<std::pair<std::string, std::variant<int, double, std::string>>> InMemoryDB::keys() const {
    std::vector<std::pair<std::string, std::variant<int, double, std::string>>> keys {};
    for (const auto &k: map_)
        keys.emplace_back(k);
    return keys;
}

Result InMemoryDB::incr(const std::string &key) {
    return adjust(key, 1);
}

Result InMemoryDB::decr(const std::string &key) {
    return adjust(key, -1);
}

Result InMemoryDB::adjust(const std::string &key, int delta) {
    std::scoped_lock lock(mtx_);

    const auto it = map_.find(key);
    if (it == map_.end()) {
        map_[key] = delta;
        return Result::ok(delta);
    }

    auto& val = it->second;
    if (const auto intVal = std::get_if<int>(&val)) {
        *intVal += delta;
        return Result::ok(*intVal);
    }

    return Result::err("ERR value is not an integer");
}




