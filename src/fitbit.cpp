#include <chrono>
#include <format>
#include <print>

#include "cpr/session.h"

#include "fitbit.hpp"

auto Fitbit::authorization_url(std::string_view redirect_uri) const -> std::string
{
    auto session = cpr::Session{};

    session.SetUrl({"https://www.fitbit.com/oauth2/authorize"});
    session.SetParameters({
        {"response_type", "code"},
        {"client_id", this->client_id},
        {"redirect_uri", redirect_uri.data()},
        {"scope", "activity heartrate"}
    });

    return session.GetFullRequestUrl();
}

auto Fitbit::authorize(
    std::string_view redirect_uri,
    std::string_view code
) const -> std::tuple<int, std::string>
{
    auto session = cpr::Session();

    session.SetUrl({"https://api.fitbit.com/oauth2/token"});
    session.SetPayload({
        {"client_id", this->client_id},
        {"grant_type", "authorization_code"},
        {"code", code.data()},
        {"redirect_uri", redirect_uri.data()}
    });
    session.SetAuth({client_id, client_secret, cpr::AuthMode::BASIC});

    const auto response = session.Post();

    return {response.status_code, response.text};
}

auto Fitbit::refresh(std::string_view refresh_token) const -> std::tuple<int, std::string>
{
    auto session = cpr::Session();

    session.SetUrl({"https://api.fitbit.com/oauth2/token"});
    session.SetPayload({
        {"grant_type", "refresh_token"},
        {"refresh_token", refresh_token.data()},
    });
    session.SetAuth({this->client_id, this->client_secret, cpr::AuthMode::BASIC});

    const auto response = session.Post();

    return {response.status_code, response.text};
}

auto Fitbit::activities(std::string_view access_token) const -> std::tuple<int, std::string>
{
    using namespace std::chrono;

    auto session = cpr::Session{};
    const auto tomorrow = std::format("{:%Y-%m-%d}", current_zone()->to_local(system_clock::now() + days{1}));

    session.SetUrl({"https://api.fitbit.com/1/user/-/activities/list.json"});
    session.SetParameters({
        {"beforeDate", tomorrow},
        {"limit", "5"}
    });
    session.SetHeader({
        {"Authorization", "Bearer " + std::string{access_token}}
    });

    std::println("{}", session.GetFullRequestUrl());
    const auto response = session.Get();

    return {response.status_code, response.text};
}

auto Fitbit::heart_rate(
    std::string_view heartrate_link, std::string_view access_token
) const -> std::tuple<int, std::string>
{
    auto session = cpr::Session{};

    session.SetUrl({heartrate_link});
    session.SetHeader({
        {"Authorization", "Bearer " + std::string{access_token}}
    });

    const auto response = session.Get();

    return {response.status_code, response.text};
}
