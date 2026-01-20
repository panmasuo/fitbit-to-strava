#include <iostream>
#include <print>
#include <ranges>
#include <string>
#include <string_view>

#include "api.hpp"
#include "json.hpp"

/** 
 * @brief Custom viewable range adaptor for trimming provided string and keyword.
 *
 * 'trim_after' can be used as well with different subrange creation function.
 */
struct trim_to : std::ranges::range_adaptor_closure<trim_to>
{
    std::string_view keyword;

    constexpr explicit trim_to(std::string_view keyword) : keyword(keyword) {}

    /**
     * @brief Look for provided keyword in the viewable range string. Call virtual
     * 'create_subrange' method for returning desired subrange.
     */
    auto operator()(std::ranges::viewable_range auto&& range) const -> std::string_view
    {
        const auto text = std::string_view{range};
        const auto match = std::string_view{std::ranges::search(text, this->keyword)};
        if (match.empty()) {
            return {};
        }

        return create_subrange(match.begin(), text.begin(), text.end());
    }

    virtual auto create_subrange(std::string_view::const_iterator m_begin,
                                 [[maybe_unused]] std::string_view::const_iterator r_begin,
                                 std::string_view::const_iterator r_end
    ) const -> std::string_view
    {
        return {m_begin, r_end};
    }
};

/**
 * @brief Same structure like 'trim_to' class, but different subrange creation.
 */
struct trim_after : trim_to
{
    constexpr explicit trim_after(std::string_view keyword) : trim_to(keyword) {}

    auto create_subrange(std::string_view::const_iterator m_begin,
                         std::string_view::const_iterator r_begin,
                         [[maybe_unused]] std::string_view::const_iterator r_end
    ) const -> std::string_view override
    {
        return {r_begin, m_begin};
    }
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

    const auto code = response_url | trim_to{from_keyword} | std::views::drop(from_keyword.size())
              | trim_after{to_keyword} | std::ranges::to<std::string>();

    const auto& [auth_status_code, auth_text] = fitbit_post_authentication_request(
        client_id, client_secret, redirect_uri, code);

    if (save(auth_text)) {
        std::println("great success {}", code);
    }

    return {};
}

