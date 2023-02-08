#pragma once

#include <boost/log/trivial.hpp>  
#include <boost/log/utility/setup/console.hpp>
#include <boost/log/utility/setup/common_attributes.hpp>
#include <boost/date_time.hpp>
#include <boost/log/core.hpp>
#include <boost/log/expressions.hpp>
#include <boost/json.hpp>

#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace boost_log{

namespace logging = boost::log;
namespace json = boost::json;
namespace exp = logging::expressions;
namespace http = boost::beast::http;
namespace net = boost::asio;

const std::string PORT = "port";
const std::string ADDRESS = "address";
const std::string IP = "ip";
const std::string URL = "URI";
const std::string METHOD = "method";
const std::string CODE = "code";
const std::string CONTENT_TYPE = "content_type";
const std::string RESPONSE_TIME = "response_time";
const std::string EXCEPTION = "text";
const std::string WHERE = "where";

struct StartServer{
    explicit StartServer(int port, std::string address)
        :port_{port}, 
         address_{address}{}
         
    int port_;
    std::string address_;
};

struct FinishServer{
    FinishServer(int code)
        : code_{code}{}

    int code_;
};

struct ExceptionReciever{
    ExceptionReciever(int code, std::string_view where, std::string what)
        : code_{code},where_{where}, what_{what}{}

    int code_;
    std::string what_;
    std::string_view where_;
};

struct Request{
    Request(http::request<http::string_body>& req, boost::asio::ip::tcp::endpoint& ep)
        : url{req.target()}, method{req.method_string()}, ip{ep.address().to_string()}{}

    std::string url;
    std::string method;
    std::string ip;
};

template <typename Body, typename Fields>
struct Response {
    Response(http::response<Body, Fields>& res, boost::asio::ip::tcp::endpoint& ep, uint64_t time)
        : code{res.result_int()}, time_{time}, ip{ep.address().to_string()}, content_type_{res[http::field::content_type]}{}
    
    unsigned int code;
    std::string content_type_;
    std::string ip;
    uint64_t time_;
};

template <typename Body, typename Fields>
void tag_invoke(json::value_from_tag, json::value& val, const Response<Body, Fields>& res){
    val = {{IP, res.ip},
           {RESPONSE_TIME, res.time_},
           {CODE, res.code},
           {CONTENT_TYPE, res.content_type_}};
}

void tag_invoke(json::value_from_tag, json::value& val, const StartServer& data);
void tag_invoke(json::value_from_tag, json::value& val, const Request& req);
void tag_invoke(json::value_from_tag, json::value& val, const FinishServer& data);
void tag_invoke(json::value_from_tag, json::value& val, const ExceptionReciever& data);

} //namespace