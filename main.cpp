#include "main.h"

struct Settings *generateSettings()
{
    struct Settings *settings = new Settings;

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("settings.ini", pt);

    settings->env = pt.get<string>("Default.ENV");
    settings->public_encryption_key = pt.get<string>("Security.PUBLIC_KEY");
    settings->registration_key = pt.get<string>("Security.REGISTRATION_KEY");

    if (settings->env == DEV_ENV)
    {
        settings->hostname = pt.get<string>("Default.HOSTNAME_DEV");
    }
    else
    {
        settings->hostname = pt.get<string>("Default.HOSTNAME_PROD");
    }

    RSAController *rsa_controller = new RSAController();
    rsa_controller->encryptString(settings->registration_key, settings->public_encryption_key, settings->encrypted_registration_key);

    return settings;
}

int main(int argc, char **argv)
{
    struct Settings *settings = generateSettings();

    WebSocketController *websocket_controller = new WebSocketController(settings);

    while (settings->jwt_token.size() == 0)
    {
        cout << "Waiting for JWT token to be set by Websocket..." << endl;
        sleep_for(seconds(1));
    }

    FileManager *fileManager = new FileManager();
    ConfigurationController *configurationController = new ConfigurationController(settings);

    Controller *controller = new Controller(fileManager);
    controller->initializeApplication(argc, argv);

    return 1;
}
