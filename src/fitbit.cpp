#include <chrono>
#include <format>

#include "cpr/cpr.h"

#include "fitbit.hpp"

auto fitbit_create_authorization_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.fitbit.com/oauth2/authorize"});
    session.SetParameters({
        {"response_type", "code"},
        {"client_id", client_id},
        {"redirect_uri", redirect_uri}
    });

    return session.GetFullRequestUrl();
}

auto fitbit_post_authentication_request(
    const std::string &client_id, const std::string &client_secret,
    const std::string &redirect_uri, const std::string &code
) -> std::tuple<int, std::string>
{
    auto session = cpr::Session();

    session.SetUrl({"https://api.fitbit.com/oauth2/token"});
    session.SetPayload({
        {"client_id", client_id},
        {"grant_type", "authorization_code"},
        {"code", code},
        {"redirect_uri", redirect_uri}
    });
    session.SetAuth({client_id, client_secret, cpr::AuthMode::BASIC});

    const auto response = session.Post();

    return {response.status_code, response.text};
}

auto fitbit_get_activities(const std::string& access_token) -> std::tuple<int, std::string>
{
    using namespace std::chrono;

    auto session = cpr::Session{};
    const auto today = std::format("{:%Y-%m-%d}", current_zone()->to_local(system_clock::now()));

    session.SetUrl({"https://api.fitbit.com/1/user/-/activities/list.json"});
    session.SetParameters({
        {"beforeDate", today},
        {"limit", "5"}
    });
    session.SetHeader({
        {"Authorization", "Bearer " + access_token}
    });

    const auto response = session.Get();

    return {response.status_code, response.text};
}

auto fitbit_get_heartrate(
    const std::string& heartrate_link, const std::string& access_token
) -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({heartrate_link});
    session.SetHeader({
        {"Authorization", "Bearer " + access_token}
    });

    const auto response = session.Get();

    return {response.status_code, response.text};
}
