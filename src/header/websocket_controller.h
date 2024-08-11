#ifndef WEBSOCKET_CONTROLLER_H
#define WEBSOCKET_CONTROLLER_H

#include <sio_client.h>
#include <iostream>
#include <string>
#include <thread>
#include <nlohmann/json.hpp>
#include "settings.h"

class WebSocketController
{
private:
    struct Settings *_settings;
    sio::client *_client;

    void initializeConnection();

    void on_open();
    void on_token_update(sio::event &ev);
    void on_fail();
    void on_close(sio::client::close_reason const &reason);

    std::thread initThread;

public:
    WebSocketController(struct Settings *settings);
    ~WebSocketController();
};

#endif