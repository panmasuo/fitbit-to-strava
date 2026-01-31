#include <filesystem>
#include <fstream>
#include <iostream>
#include <print>

#include "app.hpp"

using namespace nlohmann;

auto get_code_from_url(std::string_view url, std::string_view from_keyword, std::string_view to_keyword) -> std::string
{
    auto trim_to = [](std::string_view text, std::string_view match) -> std::string_view
    {
        std::println("{}", text);
        return {match.begin(), text.end()};
    };

    auto trim_after = [](std::string_view text, std::string_view match) -> std::string_view
    {
        std::println("{}", text);
        return {text.begin(), match.begin()};
    };

    std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", url);

    auto response_url = std::string{};
    std::cin >> response_url;

    return response_url | Trim{from_keyword, trim_to} | std::views::drop(from_keyword.size())
                        | Trim{to_keyword, trim_after} 
                        | std::ranges::to<std::string>();
}


auto create_tcx(const json& activity, const json& heartrate) -> std::filesystem::path 
{
    // TODO catch json errors
    const auto activity_name = activity.at("activityName").get<std::string>();
    const auto activity_log_id = activity.at("logId").get<long long>();
    const auto activity_start_time = activity.at("startTime").get<std::string>();
    const auto activity_duration = activity.at("duration").get<double>() / 1000.0;
    const auto activity_calories = activity.at("calories").get<int>();

    const auto file_name = std::filesystem::path(
        std::format("workouts/fitbit_{}_{}.tcx", activity_name, activity_start_time));

    auto file_stream = std::ofstream{file_name, std::ios::out | std::ios::trunc};

    if (!file_stream) {
        std::println("Cannot open file stream for {}", file_name.string());

        return {};
    }

/* NOTE: funny formatting */
    std::print(file_stream, R"(<?xml version="1.0" encoding="UTF-8"?>
<TrainingCenterDatabase xmlns="http://www.garmin.com/xmlschemas/TrainingCenterDatabase/v2">
  <Activities>
    <Activity Sport="Other">
      <Id>{}</Id>
      <Lap StartTime="{}">
        <TotalTimeSeconds>{:.1f}</TotalTimeSeconds>
        <Calories>{}</Calories>
        <Track>
)", activity_log_id, activity_start_time, activity_duration, activity_calories);

    const auto& date = std::string_view{activity_start_time}.substr(
        0, std::string_view{"yyyy-mm-ddT"}.length());

    for (const auto& point : heartrate.at("activities-heart-intraday").at("dataset")) {
        const auto& time = point.at("time").get<std::string>();
        const auto& value = point.at("value").get<int>();

/* NOTE: funny formatting */
        std::print(file_stream, R"(          <Trackpoint>
            <Time>{}{}</Time>
            <HeartRateBpm><Value>{}</Value></HeartRateBpm>
          </Trackpoint>
)", date, time, value);

    }

/* NOTE: funny formatting */
    std::print(file_stream, R"(        </Track>
      </Lap>
    </Activity>
  </Activities>
</TrainingCenterDatabase>
)");

    return file_name;
}

auto ask_for_workout(const std::vector<std::filesystem::path>& workouts) -> int
{
    for (const auto& [i, workout] : std::views::enumerate(workouts)) {
        std::println("{}.\t{}", i, workout.string());
    }

    int chosen_workout;
    std::print("Choose workout to upload to Strava:\n\t> ");
    std::cin >> chosen_workout;

    return chosen_workout;
}
