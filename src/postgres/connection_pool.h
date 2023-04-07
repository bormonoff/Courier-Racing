// This class provides thread safety access to Postgres

#include <condition_variable>
#include <memory>

#include <pqxx/pqxx>

namespace postgres {

class ConnectionPool {
    using PoolType = ConnectionPool;
    using ConnectionPtr = std::shared_ptr<pqxx::connection>;

public:
    class ConnectionWrapper {
    public:
        ConnectionWrapper(std::shared_ptr<pqxx::connection>&& conn, PoolType& pool) noexcept
            : conn_{std::move(conn)}
            , pool_{&pool} {
        }

        ConnectionWrapper(const ConnectionWrapper&) = delete;
        ConnectionWrapper& operator=(const ConnectionWrapper&) = delete;

        ConnectionWrapper(ConnectionWrapper&&) = default;
        ConnectionWrapper& operator=(ConnectionWrapper&&) = default;

        pqxx::connection& operator*() const& noexcept {
            return *conn_;
        }
        pqxx::connection& operator*() const&& = delete;

        pqxx::connection* operator->() const& noexcept {
            return conn_.get();
        }

        ~ConnectionWrapper() {
            if (conn_) {
                pool_->ReturnConnection(std::move(conn_));
            }
        }

    private:
        std::shared_ptr<pqxx::connection> conn_;
        PoolType* pool_;
    };

    // ConnectionFactory is a functional object returning std::shared_ptr<pqxx::connection>
    template <typename ConnectionFactory>
    ConnectionPool(size_t capacity, ConnectionFactory&& connection_factory) {
        pool_.reserve(capacity);
        for (size_t i = 0; i < capacity; ++i) {
            pool_.emplace_back(connection_factory());
        }
    }

    ConnectionWrapper GetConnection() {
        std::unique_lock lock{mutex_};
        // Block currnent thread and wait until cond_var get's a empty thread signal 
        cond_var_.wait(lock, [this] {
            return used_connections_ < pool_.size();
        });
        return {std::move(pool_[used_connections_++]), *this};
    }

private:
    void ReturnConnection(ConnectionPtr&& conn) {
        // Move connection back in threads pool
        {
            std::lock_guard lock{mutex_};
            assert(used_connections_ != 0);
            pool_[--used_connections_] = std::move(conn);
        }
        // Notify waiting thead that it can takes connection
        cond_var_.notify_one();
    }

    std::mutex mutex_;
    std::condition_variable cond_var_;
    std::vector<ConnectionPtr> pool_;
    size_t used_connections_ = 0;
}; 

} // namespace postgres