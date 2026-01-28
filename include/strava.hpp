#pragma once

#include <filesystem>
#include <string>

[[nodiscard]] auto strava_create_auth_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string;

[[nodiscard]] auto strava_post_authentication_request(
    const std::string& client_id, const std::string& client_secret,
    const std::string& redirect_uri, const std::string& code
) -> std::tuple<int, std::string>;

[[nodiscard]] auto strava_post_workout(
    const std::string& access_token, const std::filesystem::path& workout,
    const std::string& activity_type
) -> std::tuple<int, std::string>;

