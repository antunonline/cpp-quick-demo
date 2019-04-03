//
// Created by ahorvat on 03.04.19..
//

#include <unordered_map>
#include <string>
#include <functional>
#include <optional>
#include <folly/ScopeGuard.h>
#include "pistache/endpoint.h"

using FunctionPtr = std::optional<std::function<void(const Pistache::Http::Request,
                                                     Pistache::Http::ResponseWriter &response)>>;

class RequestHandler : public Pistache::Http::Handler {

    std::unordered_map<Pistache::Http::Method,
            std::unordered_map<std::string, FunctionPtr>> m_handlers;

public:
    void onRequest(const Pistache::Http::Request &request, Pistache::Http::ResponseWriter response) override {
        bool responseSatisfied = false;
        auto guard = folly::makeGuard([&responseSatisfied, &response](){
            if(responseSatisfied==false){
                response.send(Pistache::Http::Code::Not_Found);
            }
        });

        auto method_handlers = m_handlers.find(request.method());
        if (method_handlers != m_handlers.end()) {

            auto path_handler = (*method_handlers).second.find("OK");
            if(path_handler != (*method_handlers).second.end()){
                FunctionPtr & ptr = (*path_handler).second;
                if(ptr){
                    ptr->operator()(request, response);
                    responseSatisfied = true;
                }

            }
        }
    }
};

class HttpServer {
    int m_port = 0;

public:
    explicit HttpServer(int port) : m_port{port} {

    }


};

int main() {

}