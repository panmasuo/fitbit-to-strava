#pragma once

#include <filesystem>
#include <ranges>
#include <string_view>

#include "nlohmann/json.hpp"

template<typename Strategy>
struct Trim : std::ranges::range_adaptor_closure<Trim<Strategy>>
{
    std::string_view keyword;
    Strategy subrange;

    constexpr explicit Trim(std::string_view keyword, Strategy callback) : keyword(keyword), subrange(callback) {}

    auto operator()(std::ranges::viewable_range auto&& range) const -> std::string_view
    {
        const auto text = std::string_view{range};
        const auto match = std::string_view{std::ranges::search(text, this->keyword)};

        if (match.empty()) {
            return {};
        }

        return this->subrange(text, match);
    }
};

[[nodiscard]] auto create_tcx(
    const nlohmann::json& activity,
    const nlohmann::json& heartrate
) -> std::filesystem::path;

