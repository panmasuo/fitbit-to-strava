#pragma once

#include <filesystem>
#include <string>

#include "nlohmann/json.hpp"

[[nodiscard]] auto load(const std::filesystem::path& name) -> nlohmann::json;

[[nodiscard]] auto parse(const std::string& content) -> nlohmann::json;

[[nodiscard]] auto save(
    const std::filesystem::path& name,
    const std::string& content
) -> bool;
