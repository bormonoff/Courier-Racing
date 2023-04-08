#include "postgres/connection_pool.h"

namespace postgres {

ConnectionPool::ConnectionWrapper ConnectionPool::GetConnection() {
    std::unique_lock lock{mutex_};
    // Block currnent thread and wait until cond_var get's a empty thread signal 
    cond_var_.wait(lock, [this] {
        return used_connections_ < pool_.size();
    });
    return {std::move(pool_[used_connections_++]), *this};
}

void ConnectionPool::ReturnConnection(ConnectionPtr&& conn) {
    // Move connection back in threads pool
    {
        std::lock_guard lock{mutex_};
        assert(used_connections_ != 0);
        pool_[--used_connections_] = std::move(conn);
    }
    // Notify waiting thead that it can takes connection
    cond_var_.notify_one();
}
}