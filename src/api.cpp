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

