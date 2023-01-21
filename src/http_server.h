#pragma once
#include "sdk.h"
//
#include <iostream>
#include <boost/asio/ip/tcp.hpp>
#include <boost/asio/strand.hpp>
#include <boost/beast/core.hpp>
#include <boost/beast/http.hpp>

namespace http_server {

void ReportError(boost::system::error_code& error, std::string_view what);

namespace net = boost::asio;
using tcp = net::ip::tcp;
namespace beast = boost::beast;
namespace sys = boost::system;
namespace http = beast::http;

class SessionBase{
public:
    void Run();

protected:
    using HttpRequest = http::request<http::string_body>;

    explicit SessionBase(tcp::socket&& socket)
        : stream_{std::move(socket)}{
    }
    
    SessionBase(const SessionBase&) = delete;
    SessionBase& operator= (const SessionBase&) = delete;

    ~SessionBase()=default;

    virtual std::shared_ptr<SessionBase> GetSharedThis() = 0;
    virtual void HandleRequest(HttpRequest&& request) = 0;

    template<typename Body, typename Field>
    void Write(http::response<Body, Field>&& response){
        auto safe_response = std::make_shared<http::response<Body, Field>>(std::move(response));
        auto self = GetSharedThis();
        http::async_write(stream_, *safe_response,
            [self, safe_response](beast::error_code error, std::size_t bytes_written){
                self->OnWrite(safe_response->need_eof(), error, bytes_written);
            });
    }

private:                                                     
    void Read();

    void OnRead(beast::error_code error, std::size_t bytes_read);

    void OnWrite(bool close, beast::error_code error, std::size_t bytes_written);

    void Close();

    beast::tcp_stream stream_;
    beast::flat_buffer buffer_;
    HttpRequest request_;
};

template<typename RequestHandler>
class Session : public SessionBase, public std::enable_shared_from_this<Session<RequestHandler>> {
public:
    template<typename Handler>
    Session(tcp::socket&& socket, Handler&& handler)
        : SessionBase(std::move(socket))
        , request_handler_(std::forward<Handler>(handler)){
    }

private:
    void HandleRequest(HttpRequest&& request) override{
        request_handler_(std::move(request), [self = this->shared_from_this()](auto&& response) {
            self->Write(std::move(response));
        });
    }

    std::shared_ptr<SessionBase> GetSharedThis() override{
        return this->shared_from_this();
    }

    RequestHandler request_handler_;
};

template<typename RequestHandler>
class Listener : public std::enable_shared_from_this<Listener<RequestHandler>> {
public:
    template<typename Handler> 
    Listener(net::io_context& io,const tcp::endpoint& endpoint, Handler&& request_handler)
        :io_{io},
        acceptor_{net::make_strand(io)},
        request_handler_{std::forward<Handler>(request_handler)}{
        acceptor_.open(endpoint.protocol());
        acceptor_.set_option(net::socket_base::reuse_address(true));
        acceptor_.bind(endpoint);
        acceptor_.listen(net::socket_base::max_listen_connections);
    }

    void Run(){
        DoAccept();
    }

private:
    void DoAccept() {
        acceptor_.async_accept(
            net::make_strand(io_),
            beast::bind_front_handler(&Listener::OnAccept, this->shared_from_this()));
    }

    void OnAccept(sys::error_code error, tcp::socket socket){
        if(error){
            return ReportError(error, "accept");
        }

        AssyncRunSession(std::move(socket));

        DoAccept();
    }

    void AssyncRunSession(tcp::socket&& socket){
        std::make_shared<Session<RequestHandler>>(std::move(socket), request_handler_)->Run();
    }

    net::io_context& io_;
    tcp::acceptor acceptor_;
    RequestHandler request_handler_;
};

template <typename RequestHandler>
void ServeHttp(net::io_context& ioc, const tcp::endpoint& endpoint, RequestHandler&& handler) {
    
    using MyListener = Listener<std::decay_t<RequestHandler>>;
    std::make_shared<MyListener>(ioc, endpoint, std::forward<RequestHandler>(handler))->Run();
}

}  // namespace http_server
