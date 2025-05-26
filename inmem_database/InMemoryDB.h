//
// Created by kasper on 25/05/25.
//

#ifndef INMEMDB_H
#define INMEMDB_H
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <variant>
#include <vector>

#include "../result/Result.h"


class InMemoryDB {
public:
    std::optional<std::variant<int, double, std::string>> get(const std::string &key);
    void set(const std::string& key, const std::variant<int, double, std::string> &value);
    void del(const std::string &key);
    bool exists(const std::string &key);
    void flush_all();
    std::vector<std::pair<std::string, std::variant<int, double, std::string>>> keys() const;
    Result incr(const std::string& key);
    Result decr(const std::string& key);

private:
    std::unordered_map<std::string, std::variant<int, double, std::string>> map_;
    std::mutex mtx_;
    Result adjust(const std::string& key, int delta);
};



#endif //INMEMDB_H
