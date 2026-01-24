#include <filesystem>
#include <fstream>
#include <print>

#include "json.hpp"

using namespace nlohmann;

auto parse(const std::string& content) -> json
{
    return json::parse(content);
}

auto save(const std::string& content) -> bool
{
    const auto file_name = std::filesystem::path("token.json");
    auto file_stream = std::ofstream{file_name, std::ios::out | std::ios::trunc};

    if (!file_stream) {
        std::println("Cannot open file stream for {}", file_name.string());

        return false;
    }

    std::println(file_stream, "{}", parse(content).dump(4));

    return true;
}

