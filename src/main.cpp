#include <cpr/status_codes.h>
#include <print>
#include <string>
#include <string_view>
#include <vector>

#include "app.hpp"
#include "fitbit.hpp"
#include "json.hpp"
#include "strava.hpp"

auto main(int argc, char **argv) -> int
{
    // TODO: create separate structure for config that will be loaded and parsed
    const auto config = load("client.json");
    if (config.empty()) {
        std::println("'client.json' config file is empty, exiting.");
        return {};
    }

    Fitbit fitbit{config.at("fitbit").at("id"), config.at("fitbit").at("secret")};

    auto tokens = load("token.json");
    std::tuple<int, std::string> fitbit_auth_response{};

    if (!tokens.empty() && tokens.contains("refresh_token")) {
        fitbit_auth_response = fitbit.refresh(
            tokens.at("refresh_token").get<std::string_view>());
    }

    if (cpr::status::HTTP_OK != std::get<int>(fitbit_auth_response)) {
        const auto fitbit_auth_url = fitbit.authorization_url(
            config.at("config").at("redirect_uri").get<std::string_view>());

        // ask user to click on the link and provide back the redirected url
        const auto fitbit_code = get_code_from_url(fitbit_auth_url, "code=", "#");
        fitbit_auth_response = fitbit.authorize(
            config.at("config").at("redirect_uri").get<std::string_view>(), fitbit_code);
    }

    tokens = parse(std::get<std::string>(fitbit_auth_response));
    if (tokens.empty() || tokens.contains("errors") || !save("token.json", tokens.dump(4))) {
        std::println("Parsing and saving new auth token failed, exiting.");
        return {};
    }

    const auto activities_response = fitbit.activities(
        tokens.at("access_token").get<std::string_view>());

    const auto activities_json = parse(std::get<std::string>(activities_response));

    auto workouts = std::vector<std::filesystem::path>{};
    for (const auto& activity : activities_json.at("activities")) {
        const auto heart_response = fitbit.heart_rate(
            activity.at("heartRateLink").get<std::string_view>(), tokens.at("access_token").get<std::string_view>());

        workouts.push_back(create_tcx(activity, parse(std::get<std::string>(heart_response))));
    }

    Strava strava{config.at("strava").at("id"), config.at("strava").at("secret")};

    const auto strava_auth_url = strava.authorization_url(
        config.at("config").at("redirect_uri").get<std::string_view>());

    const auto strava_code = get_code_from_url(strava_auth_url, "code=", "&");

    const auto& [strava_auth_status_code, strava_auth_text] = strava.authorize(
        config.at("config").at("redirect_uri").get<std::string_view>(), strava_code);

    const auto strava_auth_json = parse(strava_auth_text);
    const auto chosen_workout = ask_for_workout(workouts);

    const auto workout_response = strava.post_workout(
        strava_auth_json.at("access_token").get<std::string_view>(),
        workouts.at(chosen_workout),
        WorkoutName::convert_from(workouts.at(chosen_workout).string())
    );

    if (cpr::status::HTTP_CREATED == std::get<int>(workout_response)) {
        std::println("Workout {} created, thanks!", workouts.at(chosen_workout).string());
    }
    else {
        std::println("Workout {} not created due to error, exiting.", workouts.at(chosen_workout).string());
    }

    return {};
}
