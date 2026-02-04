#pragma once

#include <filesystem>
#include <ranges>
#include <string_view>

#include "nlohmann/json.hpp"

/**
 * Structure used for mapping workouts types from Fitbit API to Strava API.
 */
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

/**
 * Structure used for trimming strings.
 */
template<typename Strategy>
struct Trim : std::ranges::range_adaptor_closure<Trim<Strategy>>
{
    /* To which, or from which keyword the trimming must be performed. */
    std::string_view keyword;

    /* Callable function that will create subrange from given text and matched text. */
    Strategy subrange;

    constexpr explicit Trim(std::string_view keyword, Strategy callback)
        : keyword(keyword), subrange(callback) {}

    /* Create two subranges, one unchanged text and second starting on given keyword. */
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

/** 
 * Trim out and return the insides of the string, from provided keyword (excluded) 
 * to provided keyword (excluded).
 */
[[nodiscard]] auto trimmer(
    std::string_view text, std::string_view from_keyword, std::string_view to_keyword
) -> std::string;

/**
 * Prompt the user to access the URL and paste back returning URL.
 */
[[nodiscard]] auto get_code_from_url(
    std::string_view url, std::string_view from_keyword, std::string_view to_keyword
) -> std::string;

/**
 * Creates new TCX file using Fitbit activity with comibnation of provided heartrate time series.
 */
[[nodiscard]] auto create_tcx(
    const nlohmann::json& activity,
    const nlohmann::json& heartrate
) -> std::filesystem::path;

/**
 * Prompt the user about choosing the activity to post.
 */
[[nodiscard]] auto ask_for_workout(const std::vector<std::filesystem::path>& workouts) -> int;
