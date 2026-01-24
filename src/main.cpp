#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>

#include "api.hpp"
#include "json.hpp"

template<typename Strategy>
struct Trim : std::ranges::range_adaptor_closure<Trim<Strategy>>
{
    std::string_view keyword;
    Strategy subrange;

    constexpr explicit Trim(std::string_view keyword, Strategy callback) : keyword(keyword), subrange(callback) {}

    auto operator()(std::ranges::viewable_range auto&& range) const -> std::string_view
    {
        const auto text = std::string_view{range};
        const auto match = std::string_view{std::ranges::search(text, this->keyword)};

        if (match.empty()) {
            return {};
        }

        return this->subrange(text, match);
    }
};

auto trim_to = [](std::string_view text, std::string_view match) -> std::string_view
{
    return {match.begin(), text.end()};
};

auto trim_after = [](std::string_view text, std::string_view match) -> std::string_view
{
    return {text.begin(), match.begin()};
};

auto main(int argc, char **argv) -> int
{
    const auto client_id = std::string{"deadbeef"};
    const auto client_secret = std::string{"deadbeef"};
    const auto redirect_uri = std::string{"https://localhost:5000"};

    const auto authorization_url = fitbit_create_authorization_url(client_id, redirect_uri);

    std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", authorization_url);

    auto response_url = std::string{};
    std::cin >> response_url;

    // use keywords to trim the 'code' part of response URL
    const auto from_keyword = std::string_view{"code="};
    const auto to_keyword = std::string_view{"#"};

    const auto code = response_url | Trim{from_keyword, trim_to} | std::views::drop(from_keyword.size())
              | Trim{to_keyword, trim_after} | std::ranges::to<std::string>();

    const auto& [auth_status_code, auth_text] = fitbit_post_authentication_request(
        client_id, client_secret, redirect_uri, code);

    if (save(auth_text)) {
        std::println("great success {}", code);
    }

    return {};
}

