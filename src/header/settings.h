#ifndef SETTINGS_H
#define SETTINGS_H

#include <string>

#define FILES_DIRECTORY "files/"
#define TEMP_DIRECTORY "temp/"
#define CONFIGURATION_FILE "configuration.json"

#define PROD_ENV "production"
#define DEV_ENV "development"

struct Settings
{
    std::string env;
    std::string hostname;
    std::string public_encryption_key;
    std::string registration_key;
    std::string encrypted_registration_key;

    std::string jwt_token;
};

#endif