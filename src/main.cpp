#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <thread>

#include "json/json_loader.h"
#include "handlers/request_handler.h"
#include "core/http_server.h"
#include "loggers/boost_log.h"
#include <exception>
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
using tcp = net::ip::tcp;
using namespace std::literals;
namespace beast = boost::beast;
namespace sys = boost::system;
namespace http = beast::http;

int main(int argc, char* argv[]) {
    if (argc != 3) {
        std::cerr << "Usage: game_server <game-config-json>" << std::endl;
        return EXIT_FAILURE;
    }
    try{
        boost_log::InitBoostLog();

        model::Game game = json_loader::LoadGame(argv[1]);

        http_handler::RequestHandler handler{game, argv[2]};
        http_handler::RequestLoggerHandler logger_handler{handler};

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number){
            if(!error){
                BOOST_LOG_TRIVIAL(error) << boost_log::MakeResponse("server exited", boost_log::FinishServer(EXIT_SUCCESS));
                ioc.stop();
            }
        });

        const std::string address = "0.0.0.0";
        constexpr unsigned short port = 8080;
        const tcp::endpoint endpoint{net::ip::make_address(address), port};

        http_server::ServeHttp(ioc, endpoint, [&logger_handler](tcp::endpoint& endpoint, auto&& req, auto&& send) {
            logger_handler(endpoint, std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });

        BOOST_LOG_TRIVIAL(info) << boost_log::MakeResponse("Server has started...", boost_log::StartServer(port, address));

        RunWorkers(num_threads, [&ioc]{
            ioc.run();
        });    

    }
    catch (const std::exception& ex) {
        std::cout<<"ojdoijd"<<std::endl ;
        BOOST_LOG_TRIVIAL(error) << boost_log::MakeResponse("server exited", boost_log::ExceptionReciever(EXIT_FAILURE,"shutdown", ex.what()));
        return EXIT_FAILURE;
    }
}
