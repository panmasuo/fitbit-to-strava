#pragma once

#include <tuple>
#include <string>

[[nodiscard]] auto fitbit_create_authorization_url(
    const std::string& client_id, const std::string& redirect_uri
) -> std::string;

[[nodiscard]] auto fitbit_post_authentication_request(
    const std::string& client_id, const std::string& client_secret,
    const std::string& redirect_uri, const std::string& code
) -> std::tuple<int, std::string>;

[[nodiscard]] auto fitbit_get_activities(
    const std::string& access_token
) -> std::tuple<int, std::string>;

[[nodiscard]] auto fitbit_get_heartrate(
    const std::string& heartrate_link, const std::string& access_token
) -> std::tuple<int, std::string>;
