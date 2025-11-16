#ifndef SNAKESERVER_UTILS_H
#define SNAKESERVER_UTILS_H

#include <string>
#include <iostream>
#include <unordered_map>


// Basic Vector2
struct Vec2 {
    int x;
    int y;
    Vec2& operator=(const Vec2 &other) {
        x = other.x;
        y = other.y;
        return *this;
    }
    bool operator==(const Vec2 &other) const {
        return x == other.x && y == other.y;
    }
    friend std::ostream& operator<<(std::ostream& os, const Vec2& v) {
        os << v.x << ", " << v.y;
        return os;
    }
};


// hash overload for Vec2
template<> struct std::hash<Vec2> {
    size_t operator()(const Vec2& v) const noexcept {
        return (static_cast<size_t>(v.x) * 73856093u) ^ (static_cast<size_t>(v.y) * 19349663u);
    }
};


/* 'ol Json would like to a part of the party, too */

template<typename T>   // so ENUM can be converted to int
std::string toJSON(const std::unordered_map<T, std::string> &kv_pairs) {
    if (kv_pairs.size() == 0) return "{}";

    std::string json = "{";
    for (const auto &p : kv_pairs) {
        //             key                  value
        json += "\"" + std::to_string(static_cast<int>(p.first))
              + "\":\"" + p.second + "\",";
    }
    return json.substr(0, json.size() - 1) + "}";   // remove extra ',' char
}


template<typename T>
std::unordered_map<std::string, std::string> decodeJSON(T &json) {
    /*
     * Decode BASIC JSON string(_view)'s -> {"key":"value", "string": "1,2"}
     */
    // deal with using a template and invalid types
    std::cout << "json: " << json << std::endl;
    try {
        if (json.empty() || json.size() <= 2)   // yes this will probably crash if not a stringtype,
            return {};
    }
    catch (const std::exception& e) {
        std::cerr << "Invalid string type in \"decodeJSON\": "<< e.what() << std::endl;
        return {};
    }

    // handle remove JSON bracket { from string or string_view
    if constexpr (std::is_same_v<std::decay_t<T>, std::string>) {
        if (json[0] == '{')
            json.erase(0, 1);
    }
    else if constexpr (std::is_same_v<std::decay_t<T>, std::string_view>) {
        if (json.front() == '{')   // remove prefix
            json.remove_prefix(1);
    }

    std::unordered_map<std::string, std::string> map;

    // decode
    int start = 0;
    while (start < json.size() - 1) {
        int mid = json.find(':', start);
        int end = json.find("\",\"", mid);
        // at the end of the JSON string
        if (end == std::string::npos)
            end = json.size() - 2;

        auto key = std::string(json.substr(start + 1, mid - start - 2));
        auto val = std::string(json.substr(mid + 2, end - mid - 2));

        map[key] = val;
        start = end + 2;
        std::cout << key << " : " << val << "\n";   // debugging
    }
    return map;
}


inline Vec2 strToVec2(std::string str) {
    std::cout << "str: " << str << std::endl;
    int mid = str.find(',');
    if (mid == std::string::npos)
        throw std::runtime_error("Invalid Vec2: missing ','");
    if (mid == 0)
        throw std::runtime_error("Invalid Vec2: missing x or leading ','");

    int x = std::stoi(
        str.substr(0, mid)
    );
    int y = std::stoi(
        str.substr(mid + 1)
    );
    return {x, y};
}


#endif