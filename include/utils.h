#ifndef SNAKESERVER_UTILS_H
#define SNAKESERVER_UTILS_H
#include <string>


struct Vec2 {
    int x;
    int y;
    bool operator==(const Vec2 &other) const {
        return x == other.x && y == other.y;
    }
};


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


std::unordered_map<std::string, std::string> decodeJSON(std::string_view &json) {
    /*
     * Decode BASIC JSON string(_view)'s -> {"key":"value", "string": "1"}
     */
    if (json.empty() || json.size() <= 2)
        return {};
    if (json.front()=='{')   // remove prefix
        json.remove_prefix(1);

    std::unordered_map<std::string, std::string> map;

    // decode
    int start = 0;
    while (start < json.size() - 1) {
        int mid = json.find(':', start);
        int end = json.find(',', mid);
        if (end == std::string::npos) end = json.size() - 1;   // at the end

        auto key = std::string(json.substr(start + 1, mid - start - 2));
        auto val = std::string(json.substr(mid + 2, end - mid - 3));

        map[key] = val;
        start = end + 1;
        std::cout << key << " " << val << "\n";   // debugging
    }
    return map;
}


Vec2 strToVec2(std::string str) {
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