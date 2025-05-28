#include "InMemoryDB.h"

#include <ranges>
#include <variant>

std::optional<std::variant<int, double, std::string>> InMemoryDB::get(const std::string &key) {
    std::scoped_lock lock(mtx_);
    const auto it = map_.find(key);
    if (it == map_.end()) return std::nullopt;

    if (it->second.expiry && std::chrono::steady_clock::now() > *it->second.expiry) {
        map_.erase(it);
        return std::nullopt;
    }

    return it->second.value;
}

void InMemoryDB::set(const std::string& key, const Value& value, std::optional<int> expirySeconds) {
    std::scoped_lock lock(mtx_);
    std::optional<TimePoint> expiry = std::nullopt;

    if (expirySeconds && *expirySeconds > 0)
        expiry = std::chrono::steady_clock::now() + std::chrono::seconds(*expirySeconds);

    map_[key] = Entry{value, expiry};
}

void InMemoryDB::del(const std::string &key) {
    std::scoped_lock lock(mtx_);
    map_.erase(key);
}

bool InMemoryDB::exists(const std::string &key) {
    std::scoped_lock lock(mtx_);
    const auto it = map_.find(key);
    if (it == map_.end()) return false;

    if (it->second.expiry && std::chrono::steady_clock::now() > *it->second.expiry) {
        map_.erase(it);
        return false;
    }

    return true;
}

void InMemoryDB::flush_all() {
    std::scoped_lock lock(mtx_);
    map_.clear();
}

std::vector<std::pair<std::string, std::variant<int, double, std::string>>> InMemoryDB::keys() {
    std::scoped_lock lock(mtx_);
    std::vector<std::pair<std::string, std::variant<int, double, std::string>>> keys{};

    const auto now = std::chrono::steady_clock::now();

    for (const auto& [k, v] : map_) {
        if (!v.expiry || now <= *v.expiry) {
            keys.emplace_back(k, v.value);
        }
    }

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
        map_[key] = Entry{delta, std::nullopt};
        return Result::ok(delta);
    }

    if (it->second.expiry && std::chrono::steady_clock::now() > *it->second.expiry) {
        map_.erase(it);
        map_[key] = Entry{delta, std::nullopt};
        return Result::ok(delta);
    }

    auto& val = it->second.value;
    if (const auto intVal = std::get_if<int>(&val)) {
        *intVal += delta;
        return Result::ok(*intVal);
    }

    return Result::err("ERR value is not an integer");
}
