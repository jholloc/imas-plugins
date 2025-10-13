#include "curl_wrapper.h"

#include <string>
#include <stdexcept>

namespace {

size_t write_callback(void* contents, const size_t size, const size_t count, std::string* output) {
    const size_t total_size = size * count;
    output->append(static_cast<char*>(contents), total_size);
    return total_size;
}

// Common options for both GET and POST requests
void set_common_options(CURL* handle_, std::string* response_data) {
    curl_easy_setopt(handle_, CURLOPT_WRITEFUNCTION, write_callback);
    curl_easy_setopt(handle_, CURLOPT_WRITEDATA, response_data); // pointer to string is passed to write_callback
}

// Handle the cURL response and check for errors
void handle_curl_response(const CURLcode response) {
    if (response != CURLE_OK) {
        throw std::runtime_error("curl_easy_perform() failed: " + std::string(curl_easy_strerror(response)) + "\n");
    }
}

// Handle exceptions and print error messages
void handle_error(const std::exception& e) {
    throw std::runtime_error("CURL error: " + std::string(e.what()) + "\n");
}

} // namespace

uda::plugins::imas::CurlWrapper::CurlWrapper() {
    curl_global_init(CURL_GLOBAL_DEFAULT);
    handle_ = curl_easy_init();
    if (!handle_) {
        throw std::runtime_error("Failed to initialize cURL.\n");
    }
}

uda::plugins::imas::CurlWrapper::~CurlWrapper() {
    if (handle_) {
        curl_easy_cleanup(handle_);
    }
    curl_global_cleanup();
}

[[nodiscard]] std::string uda::plugins::imas::CurlWrapper::perform_get_request(const std::string& url) const {
    std::string response;
    try {
        curl_easy_setopt(handle_, CURLOPT_URL, url.c_str());
        set_common_options(handle_, &response);
        const CURLcode curl_response = curl_easy_perform(handle_);
        handle_curl_response(curl_response);
    } catch (const std::exception& e) {
        handle_error(e);
    }
    return response;
}