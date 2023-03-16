#pragma once

#include <chrono>
#include <mutex>

#include "core/http_server.h"

namespace time_control {

namespace net = boost::asio;
namespace sys = boost::system;

class Ticker : public std::enable_shared_from_this<Ticker> {
public:
    using Strand = net::strand<net::io_context::executor_type>;
    using Handler = std::function<void(size_t)>;

    Ticker(Strand strand, std::chrono::milliseconds period, Handler handler)
        : strand_{strand},
          period_{period},
          handler_{handler} {}

    void Start();

private:
    void ScheduleTick();    
    void OnTick(sys::error_code ec);

    std::mutex mutex;
    Strand strand_;
    net::steady_timer timer_{strand_};
    std::chrono::milliseconds period_;
    Handler handler_;
    std::chrono::time_point<std::chrono::steady_clock> last_tick_;
}; 
}  //namespace time_control
