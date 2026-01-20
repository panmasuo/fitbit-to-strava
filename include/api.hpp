#pragma once

#include <tuple>
#include <string>

auto fitbit_create_authorization_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string;

auto fitbit_post_authentication_request(
    const std::string& client_id, const std::string& client_secret,
    const std::string& redirect_uri, const std::string& code
) -> std::tuple<int, std::string>;

