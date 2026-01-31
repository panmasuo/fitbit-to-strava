#include "cpr/cpr.h"

#include "strava.hpp"

auto Strava::authorization_url(
    std::string_view redirect_uri
) const -> std::string
{
    auto session = cpr::Session{};

    session.SetUrl({"http://www.strava.com/oauth/authorize"});
    session.SetParameters({
        {"client_id", client_id},
        {"response_type", "code"},
        {"redirect_uri", redirect_uri.data()},
        {"approval_prompt", "force"},
        {"scope", "activity:write"}
    });

    return session.GetFullRequestUrl();
}

auto Strava::authorize(
    std::string_view redirect_uri, std::string_view code
) const -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.strava.com/oauth/token"});
    session.SetPayload({
        {"client_id", client_id},
        {"client_secret", client_secret},
        {"code", code.data()},
        {"grant_type", "authorization_code"}
    });

    const auto response = session.Post();

    return {response.status_code, response.text};
}

auto Strava::post_workout(
    std::string_view access_token, const std::filesystem::path& workout,
    std::string_view activity_type
) const -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.strava.com/api/v3/uploads"});
    session.SetMultipart({
        {"file", cpr::File{workout}},
        {"data_type", "tcx"},
        {"activity_type", activity_type.data()}
    });
    session.SetHeader({
        {"Authorization", "Bearer " + std::string{access_token}}
    });

    const auto response = session.Post();

    return {response.status_code, response.text};
}

