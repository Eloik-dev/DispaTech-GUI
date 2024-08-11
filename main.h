#ifndef MAIN_H
#define MAIN_H

#include <iostream>
#include <filesystem>
#include <thread>
#include <chrono>
#include <string>
#include <boost/property_tree/ptree.hpp>
#include <boost/property_tree/ini_parser.hpp>
#include "src/header/application_controller.h"
#include "src/header/websocket_controller.h"
#include "src/header/rsa_controller.h"
#include "src/header/request_controller.h"
#include "src/header/configuration_controller.h"
#include "src/header/settings.h"
#include "src/header/http_codes.h"

#include <nlohmann/json.hpp>

using json = nlohmann::json;
using namespace std;
using namespace std::this_thread;
using namespace std::chrono;

#endif