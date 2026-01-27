#include <iostream>
#include <print>
#include <string>
#include <string_view>

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

     const auto client_id = std::string{""};
     const auto client_secret = std::string{""};
     const auto redirect_uri = std::string{"https://localhost:5000"};

     const auto authorization_url = fitbit_create_authorization_url(client_id, redirect_uri);

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
         client_id, client_secret, redirect_uri, fitbit_code);

     const auto auth_json = parse(auth_text);
     const auto& [activities_status_code, activities_text] = fitbit_get_activities(
         auth_json.at("access_token"));

     const auto activities_json = parse(activities_text);


    std::filesystem::path last_workout;
     for (const auto& activity : activities_json.at("activities")) {
         const auto& [heartrate_status_code, heartrate_text] = fitbit_get_heartrate(
             activity.at("heartRateLink").get<std::string>(), auth_json.at("access_token"));

         last_workout = create_tcx(activity, parse(heartrate_text));
         break;
     }

    // strava part
    const auto strava_client_id = std::string{""};
    const auto strava_client_secret = std::string{""};
    const auto strava_auth_url = strava_create_auth_url(strava_client_id, redirect_uri);

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
        strava_client_id, strava_client_secret, redirect_uri, strava_code);

    const auto strava_auth_json = parse(strava_auth_text);
    std::println("{}", strava_auth_text);
    const auto reps = strava_post_workout(strava_auth_json.at("access_token"), last_workout, "rock_climbing");

    return {};
}
