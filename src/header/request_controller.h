#ifndef REQUEST_CONTROLLER_H
#define REQUEST_CONTROLLER_H

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <vector>
#include <magic.h>
#include <curl/curl.h>
#include "settings.h"

using json = nlohmann::json;

class RequestController
{
private:
    Settings *_settings;
    static std::size_t WriteCallback(void *contents, std::size_t size, std::size_t nmemb, std::string *s);
    static std::size_t FileWriteCallback(void *ptr, std::size_t size, std::size_t nmemb, FILE *stream);
    static std::size_t HeaderCallback(char *buffer, size_t size, size_t nitems, void *userdata);

public:
    RequestController(Settings *settings);

    json get(const std::string &url);
    json post(const std::string &url, const json &data);

    bool getFile(const std::string &fileUrl, const std::string &destination);
};

#endif
