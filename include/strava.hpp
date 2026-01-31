#pragma once

#include <filesystem>
#include <string>

struct Strava
{
    std::string client_id;
    std::string client_secret;

    [[nodiscard]] auto authorization_url(
        std::string_view redirect_uri
    ) const -> std::string;

    [[nodiscard]] auto authorize(
        std::string_view redirect_uri, std::string_view code
    ) const -> std::tuple<int, std::string>;

    [[nodiscard]] auto post_workout(
        std::string_view access_token, const std::filesystem::path& workout,
        std::string_view activity_type
    ) const -> std::tuple<int, std::string>;
};
