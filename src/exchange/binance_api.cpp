#include "binance_api.hpp"
#include "order/order_manager.hpp"

namespace {
bool running = false;
std::string api_key;
std::string api_secret;
const std::string api_base = "https://api.binance.com";
const std::string ws_base = "wss://stream.binance.com:9443/ws";
Client ws_client;
}

namespace exchange {

void log(const std::string& message) {
    std::cout << "[" << std::put_time(std::localtime(&std::time(nullptr)), "%Y-%m-%d %H:%M:%S") << "] " << message << std::endl;
}

void on_message(Client* client, websocketpp::connection_hdl hdl, Client::message_ptr msg) {
    try {
        auto json = web::json::value::parse(msg->get_payload());
        double new_bid = std::stod(json[U("b")].as_string());
        double new_ask = std::stod(json[U("a")].as_string());

        {
            std::lock_guard<std::mutex> lock(order::order_book_mutex);
            order::best_bid = new_bid;
            order::best_ask = new_ask;
            double new_mid_price = (new_bid + new_ask) / 2.0;
            if (std::abs(new_mid_price - order::mid_price) > 1e-6) {
                order::mid_price = new_mid_price;
                log("Mid-price updated: " + std::to_string(order::mid_price));
                order::update_orders(order::mid_price);
            }
        }
    } catch (const std::exception& e) {
        log("Error processing WebSocket message: " + std::string(e.what()));
    }
}

void on_open(Client* client, websocketpp::connection_hdl hdl) {
    log("WebSocket connected");
}

void on_close(Client* client, websocketpp::connection_hdl hdl) {
    log("WebSocket disconnected, attempting reconnect...");
    running = false;
}

void on_fail(Client* client, websocketpp::connection_hdl hdl) {
    log("WebSocket connection failed");
}

void place_order(const std::string& side, double price, double quantity, std::string& order_id) {
    web::http::client::http_client client(U(api_base));
    web::uri_builder builder(U("/api/v3/order"));
    
    builder.append_query(U("symbol"), U("BTCUSDT"));
    builder.append_query(U("side"), U(side));
    builder.append_query(U("type"), U("LIMIT"));
    builder.append_query(U("timeInForce"), U("GTC"));
    builder.append_query(U("quantity"), quantity);
    builder.append_query(U("price"), price);

    web::http::http_request request(web::http::methods::POST);
    request.set_request_uri(builder.to_uri());
    request.headers().add(U("X-MBX-APIKEY"), U(api_key));

    try {
        auto response = client.request(request).get();
        if (response.status_code() == web::http::status_codes::OK) {
            auto json = response.extract_json().get();
            order_id = json[U("orderId")].as_string();
            log("Order placed successfully: " + order_id);
        } else {
            log("Failed to place order: " + std::to_string(response.status_code()));
        }
    } catch (const std::exception& e) {
        log("Error placing order: " + std::string(e.what()));
    }
}
void cancel_order(const std::string& order_id) {
    web::http::client::http_client client(U(api_base));
    web::uri_builder builder(U("/api/v3/order"));
    
    builder.append_query(U("symbol"), U("BTCUSDT"));
    builder.append_query(U("orderId"), order_id);

    web::http::http_request request(web::http::methods::DEL);
    request.set_request_uri(builder.to_uri());
    request.headers().add(U("X-MBX-APIKEY"), U(api_key));

    try {
        auto response = client.request(request).get();
        if (response.status_code() == web::http::status_codes::OK) {
            log("Order cancelled successfully: " + order_id);
        } else {
            log("Failed to cancel order: " + std::to_string(response.status_code()));
        }
    } catch (const std::exception& e) {
        log("Error cancelling order: " + std::string(e.what()));
    }
}

void init_websocket() {
    ws_client.clear_access_channels(websocketpp::log::alevel::all);
    ws_client.clear_error_channels(websocketpp::log::elevel::all);
    
    ws_client.init_asio();
    ws_client.set_message_handler(bind(&on_message, &ws_client, ::_1, ::_2));
    ws_client.set_open_handler(bind(&on_open, &ws_client, ::_1));
    ws_client.set_close_handler(bind(&on_close, &ws_client, ::_1));
    ws_client.set_fail_handler(bind(&on_fail, &ws_client, ::_1));

    websocketpp::lib::error_code ec;
    std::string uri = ws_base + "/btcusdt@bookTicker";
    Client::connection_ptr con = ws_client.get_connection(uri, ec);

    if (ec) {
        log("Could not create connection: " + ec.message());
        return;
    }

    ws_client.connect(con);
    running = true;
    
    try {
        ws_client.run();
    } catch (const std::exception& e) {
        log("Error in WebSocket run loop: " + std::string(e.what()));
    }
}

}  // namespace exchange
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
