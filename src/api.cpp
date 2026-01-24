#include <chrono>
#include <format>

#include "cpr/cpr.h"

#include "api.hpp"

auto fitbit_create_authorization_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string
{
    return std::format("https://www.fitbit.com/oauth2/authorize?response_type=code&"
        "client_id={}&redirect_uri={}"
        "&scope=activity%20heartrate", client_id, redirect_uri
    );
}

auto fitbit_post_authentication_request(
    const std::string &client_id, const std::string &client_secret,
    const std::string &redirect_uri, const std::string &code
) -> std::tuple<int, std::string>
{
    const auto response = cpr::Post(
        cpr::Url{"https://api.fitbit.com/oauth2/token"},
        cpr::Payload{{"client_id", client_id}, {"grant_type", "authorization_code"},
                     {"code", code}, {"redirect_uri", redirect_uri}},
        cpr::Authentication{client_id, client_secret, cpr::AuthMode::BASIC}
    );

    return {response.status_code, response.text};
}

auto fitbit_get_activities(const std::string& access_token) -> std::tuple<int, std::string>
{
    const auto get_url = std::format(
        "https://api.fitbit.com/1/user/-/activities/list.json?beforeDate={:%Y-%m-%d}&limit=5",
        std::chrono::current_zone()->to_local(std::chrono::system_clock::now())
    );

    const auto response = cpr::Get(
        cpr::Url{get_url},
        cpr::Header{{"Authorization", "Bearer " + access_token}}
    );

    return {response.status_code, response.text};
}

auto fitbit_get_heartrate(
    const std::string& heartrate_link, const std::string& access_token
) -> std::tuple<int, std::string>
{
    const auto response = cpr::Get(
        cpr::Url{heartrate_link},
        cpr::Header{{"Authorization", "Bearer " + access_token}}
    );

    return {response.status_code, response.text};
}

