#include "header/websocket_controller.h"

using json = nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, env, hostname, public_encryption_key, registration_key, encrypted_registration_key);

WebSocketController::WebSocketController(struct Settings *settings)
{
    this->_settings = settings;
    this->_client = std::make_unique<sio::client>();

    this->initThread = std::thread(&WebSocketController::initializeConnection, this);
}

WebSocketController::~WebSocketController()
{
    if (initThread.joinable())
    {
        initThread.join();
    }
}

void WebSocketController::initializeConnection()
{
    std::string uri = "http://" + this->_settings->hostname;
    std::cout << "Connecting to: " << uri << std::endl;

    this->_client->set_open_listener(std::bind(&WebSocketController::on_open, this));
    this->_client->set_close_listener(std::bind(&WebSocketController::on_close, this, std::placeholders::_1));
    this->_client->set_fail_listener(std::bind(&WebSocketController::on_fail, this));
    this->_client->connect(uri);
    this->_client->socket("/device")->on("token_update", std::bind(&WebSocketController::on_token_update, this, std::placeholders::_1));
    this->_client->socket("/device")->on("config_update", std::bind(&WebSocketController::on_config_update, this, std::placeholders::_1));

    while (this->_client->opened())
    {
        sleep_for(seconds(1));
    }

    while (this->_settings->jwt_token.size() == 0)
    {
        std::cout << "Waiting for JWT token to be set by Websocket..." << std::endl;
        sleep_for(seconds(10));
    }

    ConfigurationController::updateFiles(this->_settings);
    ConfigurationController::updateLocalConfig(this->_settings);
}

void WebSocketController::on_open()
{
    std::cout << "Connected to server" << std::endl;

    json registration_key = {"registration_key", this->_settings->registration_key};
    json encrypted_registration_key = {"encrypted_registration_key", this->_settings->encrypted_registration_key};
    json data = {registration_key, encrypted_registration_key};

    this->_client->socket("/device")->emit("device_join", data.dump());
}

void WebSocketController::on_token_update(sio::event &ev)
{
    this->_settings->jwt_token = ev.get_message()->get_string();
    std::cout << "Updated JWT token to: " << this->_settings->jwt_token << std::endl;
}

void WebSocketController::on_config_update(sio::event &ev)
{
    ConfigurationController::updateLocalConfig(this->_settings);
    ConfigurationController::updateFiles(this->_settings);
    std::cout << "Updated configuation and files" << std::endl;

    this->_client->socket("/device")->emit("confirm_config_update");
}

void WebSocketController::on_fail()
{
    std::cout << "Connection failed" << std::endl;
}

void WebSocketController::on_close(sio::client::close_reason const &reason)
{
    std::cout << "Disconnected from server" << std::endl;
}
