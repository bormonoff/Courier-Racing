#include "sdk.h"

#include <exception>
#include <thread>

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include "json/json_loader.h"

#include "core/http_server.h"
#include "commands/options.h"
#include "handlers/request_handler.h"
#include "loggers/boost_log.h"
#include "session/player_token.h"

namespace {

template <typename Fn>
void RunWorkers(unsigned max_threads, const Fn& fn) {
    max_threads = std::max(1u, max_threads);
    std::vector<std::jthread> workers;
    workers.reserve(max_threads - 1);
    while (--max_threads) {
        workers.emplace_back(fn);
    }
    fn();
}
}  // namespace

namespace net = boost::asio;
namespace beast = boost::beast;
namespace sys = boost::system;
namespace http = beast::http;

int main(int argc, char* argv[]) {
    try {
        auto args = commands::ParseCommandLine(argc, argv);

        boost_log::InitBoostLog();

        model::Game game;
        json_loot::LootTypes loot_types;
        json_loader::LoadGame(args.config, game, loot_types);
        
        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        const char* db_url = std::getenv("GAME_DB_URL");
        if (!db_url) {
            throw std::runtime_error("DB URL is not specified");
        }
        postgres::DataBase data_base{num_threads, db_url};

        auto handler = std::make_shared<http_handler::RequestHandler>(ioc, game, 
                                                                      args.root,
                                                                      args.store_data, 
                                                                      args.milliseconds, 
                                                                      args.random_spawn,
                                                                      args.save_interval,
                                                                      std::move(loot_types),
                                                                      data_base);
        http_handler::RequestLoggerHandler logger_handler{std::move(handler)};

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, 
                                  int signal_number) {
            if (!error) {
                ioc.stop();
            }
        });

        const std::string address = "0.0.0.0";
        constexpr unsigned short port = 8080;
        const net::ip::tcp::endpoint endpoint{net::ip::make_address(address), port};

        http_server::ServeHttp(ioc, endpoint, [&logger_handler](
            net::ip::tcp::endpoint& endpoint, 
            auto&& req, 
            auto&& send) {
            logger_handler(endpoint, std::forward<decltype(req)>(req), 
                           std::forward<decltype(send)>(send));
        });

        BOOST_LOG_TRIVIAL(info) << boost_log::MakeResponse("Server has started...", 
                                   boost_log::StartServer(port, address));

        RunWorkers(num_threads, [&ioc]{
            ioc.run();
        });    

        if (!args.store_data.empty()) {
            handler -> SaveState();
            BOOST_LOG_TRIVIAL(error) << boost_log::MakeResponse("Data has saved.", 
                                        boost_log::FinishServer(EXIT_SUCCESS));
        }
        BOOST_LOG_TRIVIAL(error) << boost_log::MakeResponse("Server has exited.", 
                                    boost_log::FinishServer(EXIT_SUCCESS));

    }
    catch (const std::exception& ex) {
        BOOST_LOG_TRIVIAL(error) << boost_log::MakeResponse("server exited",
                                    boost_log::ExceptionReciever(EXIT_FAILURE,
                                                                 "shutdown", 
                                                                 ex.what()));
        return EXIT_FAILURE;
    }
}
