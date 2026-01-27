#include <filesystem>
#include <fstream>
#include <print>

#include "app.hpp"

using namespace nlohmann;

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

