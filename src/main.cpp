#include <iostream>
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
    auto trim_to = [](std::string_view text, std::string_view match) -> std::string_view
    {
        return {match.begin(), text.end()};
    };

    auto trim_after = [](std::string_view text, std::string_view match) -> std::string_view
    {
        return {text.begin(), match.begin()};
    };

    const auto config = load("client.json");
    if (config.empty()) {
        std::println("'client.json' config file is empty.");
        return {};
    }

    auto tokens = load("token.json");
    if (!tokens.contains("access_token")) {
        const auto authorization_url = fitbit_create_authorization_url(
            config.at("fitbit").at("id"),
            config.at("config").at("redirect_uri"));

        std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", authorization_url);

        auto response_url = std::string{};
        std::cin >> response_url;

        // use keywords to trim the 'code' part of response URL
        const auto from_keyword = std::string_view{"code="};
        const auto to_keyword = std::string_view{"#"};

        const auto fitbit_code = response_url
                            | Trim{from_keyword, trim_to} | std::views::drop(from_keyword.size())
                            | Trim{to_keyword, trim_after} 
                            | std::ranges::to<std::string>();

        const auto& [auth_status_code, auth_text] = fitbit_post_authentication_request(
            config.at("fitbit").at("id"), config.at("fitbit").at("secret"),
            config.at("config").at("redirect_uri"), fitbit_code);

        tokens = parse(auth_text);
        if (!save("token.json", tokens.dump(4))) {
            return {};
        }
    }
    else {
        const auto& [refresh_status_code, refresh_text] = fitbit_post_refresh_token(
            config.at("fitbit").at("id"), config.at("fitbit").at("secret"),
            tokens.at("refresh_token"));

        if (refresh_status_code != 200) {
            // delete token
            return {};
        }

        tokens = parse(refresh_text);
        if (!save("token.json", tokens.dump(4))) {
            return {};
        }
    }

    const auto& [activities_status_code, activities_text] = fitbit_get_activities(
        tokens.at("access_token"));

    const auto activities_json = parse(activities_text);

    auto workouts = std::vector<std::filesystem::path>{};
    for (const auto& activity : activities_json.at("activities")) {
        const auto& [heartrate_status_code, heartrate_text] = fitbit_get_heartrate(
            activity.at("heartRateLink").get<std::string>(), tokens.at("access_token"));

        workouts.push_back(create_tcx(activity, parse(heartrate_text)));
    }

    // strava part
    const auto strava_auth_url = strava_create_auth_url(
        config.at("strava").at("id"), config.at("config").at("redirect_uri"));

    std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", strava_auth_url);

    // use keywords to trim the 'code' part of response URL
    const auto strava_from_keyword = std::string_view{"code="};
    const auto strava_to_keyword = std::string_view{"&"};

    auto strava_response_url = std::string{};
    std::cin >> strava_response_url;

    const auto strava_code = strava_response_url
                    | Trim{strava_from_keyword, trim_to} | std::views::drop(strava_from_keyword.size())
                    | Trim{strava_to_keyword, trim_after} 
                    | std::ranges::to<std::string>();

    const auto& [strava_auth_status_code, strava_auth_text] = strava_post_authentication_request(
        config.at("strava").at("id"), config.at("strava").at("secret"),
        config.at("config").at("redirect_uri"), strava_code);

    const auto strava_auth_json = parse(strava_auth_text);

    for (const auto& [i, workout] : std::views::enumerate(workouts)) {
        std::println("{}.\t{}", i, workout.string());
    }

    std::print("Choose workout to upload to Strava:\n\t> ");
    
    int workout_index;
    std::cin >> workout_index;

    const auto reps = strava_post_workout(strava_auth_json.at("access_token"), workouts.at(workout_index), "WeightTraining");

    return {};
}
