#ifndef CONFIGURATION_CONTROLLER_H
#define CONFIGURATION_CONTROLLER_H

#include <nlohmann/json.hpp>
#include <string>
#include <iostream>
#include <fstream>
#include <filesystem>
#include <curl/curl.h>
#include "request_controller.h"
#include "settings.h"
#include "http_codes.h"

using json = nlohmann::json;

class ConfigurationController
{
private:
    Settings *_settings;

public:
    ConfigurationController(Settings* settings);

    void updateLocalConfig();
    void updateFiles();
};

#endif
