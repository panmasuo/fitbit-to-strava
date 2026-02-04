#pragma once

#include <filesystem>
#include <string>

struct Strava
{
    std::string client_id;
    std::string client_secret;

    /**
     * Returns URL that needs to be accesses by the User to recive
     * access and refresh tokens. After accessing provided URL, User
     * has to accept the request and save the returning URL.
     */
    [[nodiscard]] auto authorization_url(
        std::string_view redirect_uri
    ) const -> std::string;

    /**
     * Use `code` returned from authorization stage to retrive
     * access and refresh tokens.
     */
    [[nodiscard]] auto authorize(
        std::string_view redirect_uri, std::string_view code
    ) const -> std::tuple<int, std::string>;

    /**
     * Refresh the tokens using already provided `refresh_token`. User don't
     * have to authorize everytime if refresh is done.
     */
    [[nodiscard]] auto refresh(
        std::string_view refresh_token
    ) const -> std::tuple<int, std::string>;

    /**
     * Post the TCX workout file to Strava.
     */
    [[nodiscard]] auto post_workout(
        std::string_view access_token, const std::filesystem::path& workout,
        std::string_view activity_type
    ) const -> std::tuple<int, std::string>;
};
