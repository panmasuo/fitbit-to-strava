#include <cpr/status_codes.h>
#include <print>
#include <stdexcept>
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

    // Fitbit stage - authorize, create tokens, get activities
    Fitbit fitbit{config.at("fitbit").at("id"), config.at("fitbit").at("secret")};

    auto fitbit_tokens = load("fitbit.json");
    std::tuple<int, std::string> fitbit_auth_response{};

    if (!fitbit_tokens.empty() && fitbit_tokens.contains("refresh_token")) {
        fitbit_auth_response = fitbit.refresh(
            fitbit_tokens.at("refresh_token").get<std::string_view>());
    }

    if (cpr::status::HTTP_OK != std::get<int>(fitbit_auth_response)) {
        const auto fitbit_auth_url = fitbit.authorization_url(
            config.at("config").at("redirect_uri").get<std::string_view>());

        // ask user to click on the link and provide back the redirected url
        const auto fitbit_code = get_code_from_url(fitbit_auth_url, "code=", "#");
        fitbit_auth_response = fitbit.authorize(
            config.at("config").at("redirect_uri").get<std::string_view>(), fitbit_code);
    }

    fitbit_tokens = parse(std::get<std::string>(fitbit_auth_response));
    if (fitbit_tokens.empty() || fitbit_tokens.contains("errors") || !save("fitbit.json", fitbit_tokens.dump(4))) {
        std::println("Parsing and saving new auth token failed, exiting.");
        return {};
    }

    const auto activities_response = fitbit.activities(
        fitbit_tokens.at("access_token").get<std::string_view>());

    const auto activities_json = parse(std::get<std::string>(activities_response));

    // create new TCX files and save them into directory and vector for user to choose later
    auto workouts = std::vector<std::filesystem::path>{};
    for (const auto& activity : activities_json.at("activities")) {
        const auto heart_response = fitbit.heart_rate(
            activity.at("heartRateLink").get<std::string_view>(), fitbit_tokens.at("access_token").get<std::string_view>());

        workouts.push_back(create_tcx(activity, parse(std::get<std::string>(heart_response))));
    }

    // Strava part - authorize, save tokens, choose the workout to post
    Strava strava{config.at("strava").at("id"), config.at("strava").at("secret")};

    auto strava_tokens = load("strava.json");
    std::tuple<int, std::string> strava_auth_response{};

    if (!strava_tokens.empty() && strava_tokens.contains("refresh_token")) {
        strava_auth_response = strava.refresh(
            strava_tokens.at("refresh_token").get<std::string_view>()
        );
    }

    if (cpr::status::HTTP_OK != std::get<int>(strava_auth_response)) {
        const auto strava_auth_url = strava.authorization_url(
            config.at("config").at("redirect_uri").get<std::string_view>());

        const auto strava_code = get_code_from_url(strava_auth_url, "code=", "&");
        strava_auth_response = strava.authorize(
            config.at("config").at("redirect_uri").get<std::string_view>(), strava_code);
    }

    strava_tokens = parse(std::get<std::string>(strava_auth_response));
    if (strava_tokens.empty() || strava_tokens.contains("errors") || !save("strava.json", strava_tokens.dump(4))) {
        std::println("Parsing and saving new auth token failed, exiting.");
        return {};
    }

    const auto chosen_workout = ask_for_workout(workouts);
    const auto workout_type = WorkoutName::convert_from(workouts.at(chosen_workout).string());
    std::println("Posting {} to Strava!", workout_type);

    const auto workout_response = strava.post_workout(
        strava_tokens.at("access_token").get<std::string_view>(),
        workouts.at(chosen_workout),
        workout_type
    );

    if (cpr::status::HTTP_CREATED == std::get<int>(workout_response)) {
        std::println("Workout {} created, thanks!", workouts.at(chosen_workout).string());
    }
    else {
        std::println("Workout {} not created due to error, exiting.", workouts.at(chosen_workout).string());
    }

    return {};
}
