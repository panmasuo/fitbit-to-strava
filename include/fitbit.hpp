#pragma once

#include <tuple>
#include <string>

struct Fitbit
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
        std::string_view redirect_uri,
        std::string_view code
    ) const -> std::tuple<int, std::string>;

    /**
     * Refresh the tokens using already provided `refresh_token`. User don't
     * have to authorize everytime if refresh is done.
     */
    [[nodiscard]] auto refresh(
        std::string_view refresh_token
    ) const -> std::tuple<int, std::string>;

    /**
     * Return list of acitivites previously saved on the Fitbit account.
     * Each activity returns a link to a heart rate time series.
     */
    [[nodiscard]] auto activities(
        std::string_view access_token
    ) const -> std::tuple<int, std::string>;

    /**
     * Recive the heart rate time series used for creating the TCX workout file.
     */
    [[nodiscard]] auto heart_rate(
        std::string_view heartrate_link, std::string_view access_token
    ) const -> std::tuple<int, std::string>;
};
