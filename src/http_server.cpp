#include "http_server.h"

#include <boost/asio/dispatch.hpp>
#include <iostream> 
namespace http_server {

void ReportError(beast::error_code ec, std::string_view what) {
    std::cout << what <<": " << ec.message() << std::endl;
}

void SessionBase::Read() {
    using namespace std::literals;
    request_ = {};  
    stream_.expires_after(30s);  
 
    http::async_read(stream_, buffer_, request_,
    beast::bind_front_handler(&SessionBase::OnRead, GetSharedThis()));  
    }

void SessionBase::OnRead(beast::error_code ec, [[maybe_unused]] std::size_t bytes_read) {
    using namespace std::literals;

    if (ec == http::error::end_of_stream) {
        return Close(); 
    }
    if (ec) {
        return ReportError(ec, "read"sv); 
    }
    HandleRequest(std::move(request_));
    }  
    
void SessionBase::OnWrite(bool close, beast::error_code ec, [[maybe_unused]] std::size_t bytes_written) {
    if (ec) {
        return ReportError(ec, "write");
    }
    if (close) {        
        return Close();
    }
    Read();
    }

    void SessionBase::Close() {
        beast::error_code ec;
        stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
    }
        

void SessionBase::Run() {
    net::dispatch(stream_.get_executor(),
    beast::bind_front_handler(&SessionBase::Read, GetSharedThis()));
}

}  // namespace http_server
