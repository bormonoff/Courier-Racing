#include "time/ticker.h"

namespace time_control {
    
void Ticker::Start() {
    last_tick_ = std::chrono::steady_clock::now();
    ScheduleTick();
}

void Ticker::ScheduleTick() {
    timer_.expires_after(period_);
    timer_.async_wait(net::bind_executor(strand_, 
                      [self = shared_from_this()](sys::error_code ec) {
        self->OnTick(ec);
    }));
}

void Ticker::OnTick(sys::error_code ec) {
    std::chrono::time_point<std::chrono::steady_clock> current_tick = 
        std::chrono::steady_clock::now();
    std::chrono::milliseconds duration = 
        std::chrono::duration_cast<std::chrono::milliseconds>(current_tick - last_tick_);
    
    // TODO Asio Strand doesn't work correctly. 
    // After Postgre dive deeper into boost asio
    mutex.lock();
    handler_(duration.count());
    mutex.unlock();
    last_tick_ = current_tick;
    ScheduleTick();
}
}  //namespace time_control
