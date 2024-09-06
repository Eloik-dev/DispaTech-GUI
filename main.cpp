#include "main.h"

std::unique_ptr<Settings> generateSettings()
{
    std::unique_ptr<Settings> settings = std::make_unique<Settings>();

    boost::property_tree::ptree pt;
    boost::property_tree::ini_parser::read_ini("settings.ini", pt);

    settings->env = pt.get<std::string>("Default.ENV");
    settings->public_encryption_key = pt.get<std::string>("Security.PUBLIC_KEY");
    settings->registration_key = pt.get<std::string>("Security.REGISTRATION_KEY");

    if (settings->env == DEV_ENV)
    {
        settings->hostname = pt.get<std::string>("Default.HOSTNAME_DEV");
    }
    else
    {
        settings->hostname = pt.get<std::string>("Default.HOSTNAME_PROD");
    }

    std::unique_ptr<RSAController> rsa_controller = std::make_unique<RSAController>();
    rsa_controller->encryptString(settings->registration_key, settings->public_encryption_key, settings->encrypted_registration_key);

    return settings;
}

int main(int argc, char **argv)
{
    std::unique_ptr<Settings> settings = generateSettings();
    std::unique_ptr<WebSocketController> websocket_controller = std::make_unique<WebSocketController>(settings.get());

    std::unique_ptr<FileManager> fileManager = std::make_unique<FileManager>();
    std::unique_ptr<Controller> controller = std::make_unique<Controller>(fileManager.get());
    return controller->initializeApplication(argc, argv);
}
