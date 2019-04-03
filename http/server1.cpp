//
// Created by ahorvat on 03.04.19..
//

#include "pistache/endpoint.h"

namespace http = Pistache::Http;
namespace net = Pistache;

class Handler : public http::Handler {

    HTTP_PROTOTYPE(Handler)

    void onRequest(const Pistache::Http::Request &request, Pistache::Http::ResponseWriter response) override {
        response.send(http::Code::Ok, "Hello, World!");
    }
};

int main(){
    net::Address addr{net::Ipv4::any(), net::Port{9080}};
    auto opts = http::Endpoint::options().threads(1);
    http::Endpoint server{addr};
    server.init(opts);
    server.setHandler(std::make_shared<Handler>());
    server.serve();

    return 0;
}