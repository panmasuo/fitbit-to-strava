#include <filesystem>
#include <fstream>
#include <print>

#include "nlohmann/json.hpp"

#include "json.hpp"

using namespace nlohmann;

auto save(const std::string& content) -> bool
{
    const auto file_name = std::filesystem::path("token.json");
    auto file_stream = std::ofstream{file_name, std::ios::out | std::ios::trunc};

    if (!file_stream) {
        std::println("Cannot open file stream for {}", file_name.string());

        return false;
    }

    const auto parsed = nlohmann::json::parse(content);
    std::println(file_stream, "{}", parsed.dump(4));

    return true;
}

