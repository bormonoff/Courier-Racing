#pragma once

#include <boost/json.hpp>
#include "../core/http_server.h"
#include "../model/model.h"
#include "../loggers/boost_log.h"
#include "../util/utils.h"

namespace http_handler{

namespace beast = boost::beast;
namespace http = beast::http;
namespace json = boost::json;
namespace net = boost::asio;
using namespace std::literals;

template<typename RequestHandler>
class RequestLoggerHandler{
public:
    explicit RequestLoggerHandler(RequestHandler&& request)
        : request_handler_{request}{};
    
    template <typename Body, typename Allocator, typename Send>
    void operator()(net::ip::tcp::endpoint& ep, http::request<Body, http::basic_fields<Allocator>>&& req, Send&& send){
        LogRequest(req, ep);
        auto start_time = util::CurrentTime();
        
        auto request_sender = [send = std::forward<Send>(send), ep, this, start_time](auto && response){
            uint64_t finish_time = util::CurrentTime();
            LogResponse(response, ep, finish_time-start_time);
            send(response);
        };

        request_handler_->operator()(std::forward<decltype(req)>(req), request_sender);
    }
    
private:
    template <typename Body, typename Allocator>
    void LogRequest(http::request<Body, http::basic_fields<Allocator>>& req, net::ip::tcp::endpoint& ep){
        BOOST_LOG_TRIVIAL(info) << boost_log::MakeResponse("request received", boost_log::Request(req, ep));
    }


    template <typename Body, typename Fields>
    void LogResponse(http::response<Body, Fields>& res, net::ip::tcp::endpoint ep, uint64_t time){
        BOOST_LOG_TRIVIAL(info) << boost_log::MakeResponse("response sent", boost_log::Response<Body, Fields>(res, ep, time));
    }

    RequestHandler& request_handler_;
};


} //namespace