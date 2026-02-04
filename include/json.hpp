#pragma once

#include <filesystem>
#include <string>

#include "nlohmann/json.hpp"

/**
 * Loads provided file and parses it into JSON.
 */
[[nodiscard]] auto load(const std::filesystem::path& name) -> nlohmann::json;

/**
 * Parses string input into JSON.
 */
[[nodiscard]] auto parse(const std::string& content) -> nlohmann::json;

/**
 * Saves provided content into provided path.
 */
[[nodiscard]] auto save(
    const std::filesystem::path& name,
    const std::string& content
) -> bool;
