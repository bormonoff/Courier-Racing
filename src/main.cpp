#include "sdk.h"

#include <boost/asio/io_context.hpp>
#include <boost/asio/signal_set.hpp>
#include <thread>

#include "json_loader.h"
#include "request_handler.h"
#include "http_server.h"

using namespace std::literals;
namespace net = boost::asio;
namespace sys = boost::system;

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
namespace beast = boost::beast;
namespace sys = boost::system;
namespace http = beast::http;

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: game_server <game-config-json>" << std::endl;
        return EXIT_FAILURE;
    }
    try{
        model::Game game = json_loader::LoadGame(argv[1]);

        http_handler::RequestHandler handler{game};

        const unsigned num_threads = std::thread::hardware_concurrency();
        net::io_context ioc(num_threads);

        net::signal_set signals(ioc, SIGINT, SIGTERM);
        signals.async_wait([&ioc](const boost::system::error_code& error, int signal_number){
            if(!error){
                std::cout << "Signal " << signal_number << " received" << std::endl;
                ioc.stop();
            }
        });

        const auto address = net::ip::make_address("0.0.0.0");
        constexpr unsigned short port = 8080;
        
        http_server::ServeHttp(ioc, {address, port}, [&handler](auto&& req, auto&& send) {
            handler(std::forward<decltype(req)>(req), std::forward<decltype(send)>(send));
        });

        std::cout << "Server has started..."sv << std::endl;

        RunWorkers(num_threads, [&ioc]{
            ioc.run();
        });    
    }
    catch (const std::exception& ex) {
        std::cerr << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}
