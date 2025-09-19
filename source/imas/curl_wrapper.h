#pragma once

#include <curl/curl.h>
#include <string>

namespace uda {
namespace plugins {
namespace imas {

class CurlWrapper {
public:
    CurlWrapper();
    ~CurlWrapper();

    CurlWrapper(const CurlWrapper&) = delete;
    CurlWrapper(CurlWrapper&&) = delete;
    CurlWrapper& operator=(const CurlWrapper&) = delete;
    CurlWrapper& operator=(CurlWrapper&&) = delete;

    // Function to perform a GET request
    [[nodiscard]] std::string perform_get_request(const std::string& url) const;

private:
    CURL* handle_;
};

}
}
}