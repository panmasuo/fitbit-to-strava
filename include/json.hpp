#pragma once

#include <string>

#include "nlohmann/json.hpp"

[[nodiscard]] auto parse(const std::string& content) -> nlohmann::json;

[[nodiscard]] auto save(const std::string& content) -> bool;

