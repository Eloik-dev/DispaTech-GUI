#ifndef REQUEST_CONTROLLER_H
#define REQUEST_CONTROLLER_H

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <curl/curl.h>
#include "settings.h"

using json = nlohmann::json;

class RequestController
{
private:
    Settings *_settings;
    static size_t WriteCallback(void *contents, size_t size, size_t nmemb, std::string *s);
    static size_t FileWriteCallback(void *ptr, size_t size, size_t nmemb, FILE *stream);

public:
    RequestController(Settings *settings);

    json get(const std::string &url);
    json post(const std::string &url, const json &data);

    bool getFile(const std::string &fileUrl, const std::string &destination);
};

#endif
