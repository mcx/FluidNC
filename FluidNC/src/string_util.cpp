#include "string_util.h"
#include <string>
#include <cstdlib>
#include <cctype>
#include <charconv>

namespace string_util {
    char tolower(char c) {
        if (c >= 'A' && c <= 'Z') {
            return c + ('a' - 'A');
        }
        return c;
    }

    // cppcheck-suppress unusedFunction
    bool equal_ignore_case(std::string_view a, std::string_view b) {
        return std::equal(a.begin(), a.end(), b.begin(), b.end(), [](auto a, auto b) { return tolower(a) == tolower(b); });
    }

    // cppcheck-suppress unusedFunction
    bool starts_with_ignore_case(std::string_view a, std::string_view b) {
        return std::equal(a.begin(), a.begin() + b.size(), b.begin(), b.end(), [](auto a, auto b) { return tolower(a) == tolower(b); });
    }

    // cppcheck-suppress unusedFunction
    const std::string_view trim(std::string_view s) {
        auto start = s.find_first_not_of(" \t\n\r\f\v");
        if (start == std::string_view::npos) {
            return "";
        }
        auto end = s.find_last_not_of(" \t\n\r\f\v");
        if (end == std::string_view::npos) {
            return s.substr(start);
        }
        return s.substr(start, end - start + 1);
    }

    bool from_xdigit(char c, uint8_t& value) {
        if (isdigit(c)) {
            value = c - '0';
            return true;
        }
        c = tolower(c);
        if (c >= 'a' && c <= 'f') {
            value = 10 + c - 'a';
            return true;
        }
        return false;
    }

    bool from_hex(std::string_view str, uint8_t& value) {
        value = 0;
        if (str.size() == 0 || str.size() > 2) {
            return false;
        }
        uint8_t x;
        while (str.size()) {
            value <<= 4;
            if (!from_xdigit(str[0], x)) {
                return false;
            }
            value += x;
            str = str.substr(1);
        }
        return true;
    }
    bool from_decimal(std::string_view str, uint32_t& value) {
        value = 0;
        if (str.size() == 0) {
            return false;
        }
        while (str.size()) {
            if (!isdigit(str[0])) {
                return false;
            }
            value = value * 10 + str[0] - '0';
            str   = str.substr(1);
        }
        return true;
    }
    bool from_decimal(std::string_view sv, int32_t& value) {
        auto [ptr, ec] = std::from_chars(sv.data(), sv.data() + sv.size(), value);

        if (ec == std::errc::invalid_argument || ec == std::errc::result_out_of_range || ptr != sv.data() + sv.size()) {
            return false;
        }

        return true;
    }

    bool from_float(std::string_view sv, float& value) {
        // std::from_chars() does not work for float arguments on ESP32
        std::string s(sv);
        char*       floatEnd;
        const char* str = s.c_str();
        value           = strtof(str, &floatEnd);
        if (floatEnd != (str + sv.length())) {
            return false;
        }

        return true;
    }

    bool split(std::string_view& input, std::string_view& next, char delim) {
        auto pos = input.find_first_of(delim);
        if (pos != std::string_view::npos) {
            next  = input.substr(pos + 1);
            input = input.substr(0, pos);
            return true;
        }
        next = "";
        return false;
    }
    bool split_prefix(std::string_view& rest, std::string_view& prefix, char delim) {
        if (rest.empty()) {
            return false;
        }
        auto pos = rest.find_first_of(delim);
        if (pos != std::string_view::npos) {
            prefix = rest.substr(0, pos);
            rest   = rest.substr(pos + 1);
        } else {
            prefix = rest;
            rest   = "";
        }
        return true;
    }
}
