//
// Created by kasper on 26/05/25.
//

#ifndef RESULT_H
#define RESULT_H
#include <optional>
#include <string>


struct Result {
    std::optional<int> value;
    std::string error;

    static Result ok(int v) { return Result{v, ""}; }
    static Result err(std::string e) { return Result{std::nullopt, std::move(e)}; }
    [[nodiscard]] bool is_ok() const { return value.has_value(); }
};


#endif //RESULT_H
