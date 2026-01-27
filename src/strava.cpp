#include <cpr/payload.h>
#include <format>

#include "cpr/cpr.h"

#include "strava.hpp"

auto strava_create_auth_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string
{
    return std::format("http://www.strava.com/oauth/authorize?client_id={}"
        "&response_type=code&redirect_uri={}&approval_prompt=force&scope=activity:write",
        client_id, redirect_uri
    );
}

[[nodiscard]] auto strava_post_authentication_request(
    const std::string& client_id, const std::string& client_secret,
    const std::string& redirect_uri, const std::string& code
) -> std::tuple<int, std::string>
{
    const auto request = cpr::Post(
        cpr::Url{"https://www.strava.com/oauth/token"},
        cpr::Payload{
            {"client_id", client_id},
            {"client_secret", client_secret},
            {"code", code},
            {"grant_type", "authorization_code"}
        }
    );

    return {request.status_code, request.text};
}

auto strava_post_workout(
    const std::string& access_token, const std::filesystem::path& workout,
    const std::string& activity_type
) -> bool
{
    const auto url = cpr::Url{"https://www.strava.com/api/v3/uploads"};

    const auto data = cpr::Multipart{
        {"file", cpr::File{workout}},
        {"data_type", "tcx"},
        {"activity_type", activity_type} 
    };

    const auto response = cpr::Post(
        url, cpr::Header{{"Authorization", "Bearer " + access_token}}, data);

    return response.status_code == 201;
}

