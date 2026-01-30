#include <cpr/status_codes.h>
#include <format>

#include "cpr/session.h"

#include "strava.hpp"

auto strava_create_auth_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string
{
    auto session = cpr::Session{};

    session.SetUrl({"http://www.strava.com/oauth/authorize"});
    session.SetParameters({
        {"client_id", client_id},
        {"response_type", "code"},
        {"redirect_uri", redirect_uri},
        {"approval_prompt", "force"},
        {"scope", "activity:write"}
    });

    return session.GetFullRequestUrl();
}

[[nodiscard]] auto strava_post_authentication_request(
    const std::string& client_id, const std::string& client_secret,
    const std::string& redirect_uri, const std::string& code
) -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.strava.com/oauth/token"});
    session.SetPayload({
        {"client_id", client_id},
        {"client_secret", client_secret},
        {"code", code},
        {"grant_type", "authorization_code"}
    });

    const auto response = session.Post();

    return {response.status_code, response.text};
}

auto strava_post_workout(
    const std::string& access_token, const std::filesystem::path& workout,
    const std::string& activity_type
) -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.strava.com/api/v3/uploads"});
    session.SetMultipart({
        {"file", cpr::File{workout}},
        {"data_type", "tcx"},
        {"activity_type", activity_type}
    });
    session.SetHeader({
        {"Authorization", "Bearer " + access_token}
    });

    const auto response = session.Post();

    return {response.status_code, response.text};
}

