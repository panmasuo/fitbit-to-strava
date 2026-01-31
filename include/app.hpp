#pragma once

#include <filesystem>
#include <ranges>
#include <string_view>

#include "nlohmann/json.hpp"

struct WorkoutName
{
    [[nodiscard]] static auto convert_from(std::string_view fitbit_workout) -> std::string_view;

  private:
    constexpr static auto default_workout = std::string_view{"Workout"};

    constexpr static auto FITBIT = int(0);
    constexpr static auto STRAVA = int(1);

    constexpr static auto workout_map = std::array<std::tuple<std::string_view, std::string_view>, 2>{{
        {std::string_view{"Indoor climbing"}, std::string_view{"RockClimbing"}},
        {std::string_view{"Weightlifting"}, std::string_view{"WeightTraining"}}
    }};
};

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

[[nodiscard]] auto trimmer(
    std::string_view text, std::string_view from_keyword, std::string_view to_keyword
) -> std::string;

[[nodiscard]] auto get_code_from_url(
    std::string_view url, std::string_view from_keyword, std::string_view to_keyword
) -> std::string;

[[nodiscard]] auto create_tcx(
    const nlohmann::json& activity,
    const nlohmann::json& heartrate
) -> std::filesystem::path;

[[nodiscard]] auto ask_for_workout(const std::vector<std::filesystem::path>& workouts) -> int;
