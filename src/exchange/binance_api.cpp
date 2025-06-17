// WebSocket message handler
#include "binance_api.hpp"
namespace exchange
{
    void on_message(Client *client, websocketpp::connection_hdl hdl, Client::message_ptr msg)
    {
        try
        {
            auto json = web::json::value::parse(msg->get_payload());
            double new_bid = std::stod(json[U("b")].as_string());
            double new_ask = std::stod(json[U("a")].as_string());

            {
                std::lock_guard<std::mutex> lock(order_book_mutex);
                best_bid = new_bid;
                best_ask = new_ask;
                double new_mid_price = (best_bid + best_ask) / 2.0;
                if (std::abs(new_mid_price - mid_price) > 1e-6)
                {
                    mid_price = new_mid_price;
                    log("Mid-price updated: " + std::to_string(mid_price));
                    update_orders(mid_price);
                }
            }
        }
        catch (const std::exception &e)
        {
            log("Error processing WebSocket message: " + std::string(e.what()));
        }
    }

    // WebSocket connection handlers
    void on_open(Client *client, websocketpp::connection_hdl hdl)
    {
        log("WebSocket connected");
    }

    void on_close(Client *client, websocketpp::connection_hdl hdl)
    {
        log("WebSocket disconnected, attempting reconnect...");
        running = false;
    }

    void on_fail(Client *client, websocketpp::connection_hdl hdl)
    {
        log("WebSocket connection failed");
    }
    // REST API request to place order
    void place_order(const std::string &side, double price, double quantity, std::string &order_id)
    {
        using namespace web::http;
        using namespace web::http::client;
        try
        {
            http_client client(U(REST_URL));
            std::stringstream query;
            query << "symbol=" << SYMBOL << "&side=" << side << "&type=LIMIT"
                  << "&quantity=" << quantity << "&price=" << price
                  << "&timeInForce=GTC&timestamp=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string signature = hmac_sha256(SECRET_KEY, query.str());
            query << "&signature=" << signature;

            http_request req(methods::POST);
            req.set_request_uri(U("/api/v3/order?" + query.str()));
            req.headers().add(U("X-MBX-APIKEY"), U(API_KEY));

            client.request(req).then([](http_response response)
                                     { return response.extract_json(); })
                .then([&order_id](web::json::value json)
                      {
            order_id = json[U("orderId")].as_string();
            log("Placed order: " + order_id); })
                .wait();
        }
        catch (const std::exception &e)
        {
            log("Error placing order: " + std::string(e.what()));
        }
    }

    // REST API request to cancel order
    void cancel_order(const std::string &order_id)
    {
        using namespace web::http;
        using namespace web::http::client;
        try
        {
            http_client client(U(REST_URL));
            std::stringstream query;
            query << "symbol=" << SYMBOL << "&orderId=" << order_id
                  << "&timestamp=" << std::chrono::duration_cast<std::chrono::milliseconds>(std::chrono::system_clock::now().time_since_epoch()).count();
            std::string signature = hmac_sha256(SECRET_KEY, query.str());
            query << "&signature=" << signature;

            http_request req(methods::DELETE);
            req.set_request_uri(U("/api/v3/order?" + query.str()));
            req.headers().add(U("X-MBX-APIKEY"), U(API_KEY));

            client.request(req).then([](http_response response)
                                     { return response.extract_json(); })
                .then([&order_id](web::json::value json)
                      { log("Canceled order: " + order_id); })
                .wait();
        }
        catch (const std::exception &e)
        {
            log("Error canceling order: " + std::string(e.what()));
        }
    }

}
