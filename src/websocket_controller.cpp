#include "header/websocket_controller.h"

using json = nlohmann::json;

NLOHMANN_DEFINE_TYPE_NON_INTRUSIVE(Settings, env, hostname, public_encryption_key, registration_key, encrypted_registration_key);

WebSocketController::WebSocketController(struct Settings *settings)
{
    this->_settings = settings;
    this->_client = new sio::client();

    this->initThread = std::thread(&WebSocketController::initializeConnection, this);
}

WebSocketController::~WebSocketController()
{
    if (this->initThread.joinable())
    {
        this->initThread.join();
    }
    delete this->_client;
}

void WebSocketController::initializeConnection()
{
    std::string uri = "http://" + this->_settings->hostname;
    std::cout << "Connecting to: " << uri << std::endl;

    this->_client->set_open_listener(std::bind(&WebSocketController::on_open, this));
    this->_client->set_close_listener(std::bind(&WebSocketController::on_close, this, std::placeholders::_1));
    this->_client->set_fail_listener(std::bind(&WebSocketController::on_fail, this));
    this->_client->connect(uri);
    this->_client->socket()->on("token_update", std::bind(&WebSocketController::on_token_update, this, std::placeholders::_1));

    while (this->_client->opened())
    {
        std::this_thread::sleep_for(std::chrono::seconds(1));
    }
}

void WebSocketController::on_open()
{
    std::cout << "Connected to server" << std::endl;

    json registration_key = {"registration_key", this->_settings->registration_key};
    json encrypted_registration_key = {"encrypted_registration_key", this->_settings->encrypted_registration_key};
    json data = {registration_key, encrypted_registration_key};

    this->_client->socket()->emit("device_join", data.dump());
}

void WebSocketController::on_token_update(sio::event &ev)
{
    this->_settings->jwt_token = ev.get_message()->get_string();
    std::cout << "Updated JWT token to: " << this->_settings->jwt_token << std::endl;
}

void WebSocketController::on_fail()
{
    std::cout << "Connection failed" << std::endl;
}

void WebSocketController::on_close(sio::client::close_reason const &reason)
{
    std::cout << "Disconnected from server" << std::endl;
}
