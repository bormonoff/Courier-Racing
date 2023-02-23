#pragma once

#include "../core/http_server.h"
#include <chrono>
namespace time_control{

namespace net = boost::asio;
namespace sys = boost::system;


class Ticker : public std::enable_shared_from_this<Ticker> {
public:
    using Strand = net::strand<net::io_context::executor_type>;
    using Handler = std::function<void(size_t)>;

    Ticker(Strand strand, std::chrono::milliseconds period, Handler handler):
        strand_{strand},
        period_{period},
        handler_{handler}{   
    }

    void Start() {
        last_tick_ = std::chrono::steady_clock::now();
        ScheduleTick();
    }

private:

    void ScheduleTick() {
        timer_.expires_after(period_);
        timer_.async_wait(net::bind_executor(strand_, [self = shared_from_this()](sys::error_code ec) {
            self->OnTick(ec);
        }));
    }
    
    void OnTick(sys::error_code ec) {
        std::chrono::time_point<std::chrono::steady_clock> current_tick = std::chrono::steady_clock::now();
        std::chrono::milliseconds duration = std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick_);
        handler_(duration.count());
        last_tick_ = current_tick;
        ScheduleTick();
    }

    Strand strand_;
    net::steady_timer timer_{strand_};
    std::chrono::milliseconds period_;
    Handler handler_;
    std::chrono::time_point<std::chrono::steady_clock> last_tick_;
}; 

} //namespace