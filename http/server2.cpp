//
// Created by ahorvat on 03.04.19..
//

#include <unordered_map>
#include <string>
#include <functional>
#include <optional>
#include <folly/ScopeGuard.h>
#include "pistache/endpoint.h"
#include "pistache/http.h"
#include "pistache/router.h"
#include "nlohmann/json.hpp"

class HttpServer {
public:
    typedef std::function<Pistache::Rest::Route::Result(const Pistache::Rest::Request,
                                                        Pistache::Http::ResponseWriter)> ResponseHandler;
private:
    struct TwoStringPariHash {
        size_t operator()(const std::pair<std::string, std::string> &p) const {
            auto h1 = std::hash<std::string>{}(p.first);
            auto h2 = std::hash<std::string>{}(p.second);

            return h1 ^ (h2 << 1);
        }
    };

    std::unordered_map<std::pair<std::string, std::string>, ResponseHandler, TwoStringPariHash> m_handlers;
    int m_port = 0;
    std::shared_ptr<Pistache::Http::Endpoint> m_endpoint;
    Pistache::Rest::Router m_router;

protected:
    void setupRoutes() &{
        using namespace Pistache::Rest;
        std::for_each(std::cbegin(m_handlers), std::cend(m_handlers), [this](auto &handler) mutable {
            auto &method = handler.first.first;
            auto &path = handler.first.second;
            auto &pistacheHandler = handler.second;

            if (method == "GET") {
                Pistache::Rest::Routes::Get(m_router, path, pistacheHandler);
            } else if (method == "POST") {
                Pistache::Rest::Routes::Post(m_router, path, pistacheHandler);
            } else if (method == "GET") {
                Pistache::Rest::Routes::Get(m_router, path, pistacheHandler);
            } else if (method == "DELETE") {
                Pistache::Rest::Routes::Delete(m_router, path, pistacheHandler);
            }
        });
    }

public:
    explicit HttpServer(uint16_t port) : m_port{port},
                                         m_endpoint{std::make_shared<Pistache::Http::Endpoint>(
                                                 Pistache::Address{Pistache::Ipv4::any(), port})} {

    }

    bool add_handler(const std::string &method, const std::string &path, const ResponseHandler handler) &{
        if (method == "POST" || method == "GET" || method == "PUT" || method == "DELETE") {
            auto const key = std::make_pair(method, path);
            if (m_handlers.find(key) == m_handlers.end()) {
                m_handlers[key] = handler;
                return true;
            } else {
                return false;
            }
        } else {
            return false;
        }
    }

    void init(int threads = 1) &{
        auto opts = Pistache::Http::Endpoint::options().threads(threads);

        m_endpoint->init(opts);
        setupRoutes();
    }

    void start() &{
        m_endpoint->setHandler(m_router.handler());
        m_endpoint->serve();
    }

    void stop() &{

    }


};

int main() {

    HttpServer server{9091};
    server.add_handler("GET", "/ping", [](const Pistache::Rest::Request request,
                                          Pistache::Http::ResponseWriter writer) -> Pistache::Rest::Route::Result {
        std::cout << request.query().as_str()
                  << ", body: " << request.body()
                  << ", :" << request.resource() << "\n";
        writer.send(Pistache::Http::Code::Ok, "Pong!");
    });
    server.add_handler("POST", "/json", [](const Pistache::Rest::Request request,
                                               Pistache::Http::ResponseWriter writer) -> Pistache::Rest::Route::Result {
        // https://github.com/nlohmann/json
        nlohmann::json j;

        j["version"] = 1;

        nlohmann::json author = {
                {"first_name", "Antun"},
                {"last_name",  "Horvat"}
        };

        j["author"] = author;


        try {
            auto json_request = nlohmann::json::parse(request.body());
            j["posted"] = json_request;
        } catch(...){
            j["status"] = "ERROR parsing";
        }



        writer.send(Pistache::Http::Code::Ok, j.dump());
    });

    server.init(2);
    server.start();

}