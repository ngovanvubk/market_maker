#pragma once
#include "include/include.hpp"
using Client = websocketpp::client<websocketpp::config::asio_client>;

namespace exchange
{
    void on_message(Client *client, websocketpp::connection_hdl hdl, Client::message_ptr msg);
    void on_open(Client *client, websocketpp::connection_hdl hdl);
    void on_close(Client *client, websocketpp::connection_hdl hdl);
    void on_fail(Client *client, websocketpp::connection_hdl hdl);
    void place_order(const std::string &side, double price, double quantity, std::string &order_id);
    void cancel_order(const std::string &order_id);
}