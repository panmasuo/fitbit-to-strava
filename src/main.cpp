#include <iostream>
#include <print>
#include <string>

#include "api.hpp"
#include "json.hpp"

auto main(int argc, char **argv) -> int
{
    const auto client_id = std::string{"deadbeef"};
    const auto client_secret = std::string{"deadbeef"};
    const auto redirect_uri = std::string{"https://localhost:5000"};

    const auto authorization_url = fitbit_create_authorization_url(client_id, redirect_uri);

    std::print("Open this link:\n\t> {}\nPaste response URL:\n\t> ", authorization_url);

    auto response_url = std::string{};
    std::cin >> response_url;

    size_t start = response_url.find("code=") + 5;
    size_t end = response_url.find("#", start);
    std::string code = response_url.substr(start, end - start);

    const auto& [auth_status_code, auth_text] = fitbit_post_authentication_request(
        client_id, client_secret, redirect_uri, code);

    if (auto success = save(auth_text)) {
        std::println("great success");
    }

    return {};
}

