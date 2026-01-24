#include <iostream>
#include <print>
#include <string>
#include <string_view>

#include "api.hpp"
#include "app.hpp"
#include "json.hpp"

auto trim_to = [](std::string_view text, std::string_view match) -> std::string_view
{
    return {match.begin(), text.end()};
};

auto trim_after = [](std::string_view text, std::string_view match) -> std::string_view
{
    return {text.begin(), match.begin()};
};

auto main(int argc, char **argv) -> int
{
    const auto client_id = std::string{"deadbeef"};
    const auto client_secret = std::string{"deadbeef"};
    const auto redirect_uri = std::string{"https://localhost:5000"};

    const auto authorization_url = fitbit_create_authorization_url(client_id, redirect_uri);

    std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", authorization_url);

    auto response_url = std::string{};
    std::cin >> response_url;

    // use keywords to trim the 'code' part of response URL
    const auto from_keyword = std::string_view{"code="};
    const auto to_keyword = std::string_view{"#"};

    const auto code = response_url
                    | Trim{from_keyword, trim_to} | std::views::drop(from_keyword.size())
                    | Trim{to_keyword, trim_after} 
                    | std::ranges::to<std::string>();

    const auto& [auth_status_code, auth_text] = fitbit_post_authentication_request(
        client_id, client_secret, redirect_uri, code);

    const auto auth_json = parse(auth_text);
    const auto& [activities_status_code, activities_text] = fitbit_get_activities(
        auth_json.at("access_token"));

    const auto activities_json = parse(activities_text);

    for (const auto& activity : activities_json.at("activities")) {
        const auto& [heartrate_status_code, heartrate_text] = fitbit_get_heartrate(
            activity.at("heartRateLink").get<std::string>(), auth_json.at("access_token"));

        create_tcx(activity, parse(heartrate_text));
    }

    return {};
}

