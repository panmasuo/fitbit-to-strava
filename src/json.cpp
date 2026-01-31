#include <filesystem>
#include <fstream>
#include <print>

#include "json.hpp"

using namespace nlohmann;

auto load(const std::filesystem::path& name) -> json
{
    auto file_stream = std::ifstream{name, std::ios::in};

    if (!file_stream) {
        std::println("Cannot open file stream for {}", name.string());

        return {};
    }

    std::string buffer{std::istreambuf_iterator<char>(file_stream),
                       std::istreambuf_iterator<char>()};

    std::println("Loaded {}.", name.string());
    return parse(buffer);
}

auto parse(const std::string& content) -> json
{
    return json::parse(content);
}

auto save(
    const std::filesystem::path& name,
    const std::string& content
) -> bool
{
    auto file_stream = std::ofstream{name};

    if (!file_stream) {
        std::println("Cannot open file stream for {:s}", name.string());

        return false;
    }

    std::println(file_stream, "{}", parse(content).dump(4));
    std::println("Saved {}.", name.string());

    return true;
}

