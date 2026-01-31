#pragma once

#include <tuple>
#include <string>

struct Fitbit
{
    std::string client_id;
    std::string client_secret;

    [[nodiscard]] auto authorization_url(
        std::string_view redirect_uri
    ) const -> std::string;

    [[nodiscard]] auto authorize(
        std::string_view redirect_uri,
        std::string_view code
    ) const -> std::tuple<int, std::string>;

    [[nodiscard]] auto refresh(
        std::string_view refresh_token
    ) const -> std::tuple<int, std::string>;

    [[nodiscard]] auto activities(
        std::string_view access_token
    ) const -> std::tuple<int, std::string>;

    [[nodiscard]] auto heart_rate(
        std::string_view heartrate_link, std::string_view access_token
    ) const -> std::tuple<int, std::string>;
};
