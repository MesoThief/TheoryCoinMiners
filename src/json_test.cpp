#include <fstream>
#include <iostream>
#include <nlohmann/json.hpp>

using json = nlohmann::json;

int main() {
    // JSON 파일 읽기
    std::ifstream f("test_data/example.json");
    json data = json::parse(f);
    std::cout << data.dump(4) << std::endl;

    // JSON 파일 쓰기
    json j;
    j["k"] = 2;
    j["matches"]["1"] = 11;
    j["matches"]["2"] = 22;
    j["matches"]["3"] = 33;
    j["pattern"] = "hello";

    // 이런 방법도 있음
    // json j = {
    //     {"k", 2},
    //     {"matches", {
    //         {"1", 11},
    //         {"2", 22},
    //         {"3", 33}
    //     }},
    //     {"pattern", "hello"}
    // };

    std::ofstream outFile("out.json");
    outFile << j.dump(4);
}
