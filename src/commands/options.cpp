#include <iostream>

#include "commands/options.h"

namespace commands {

Args ParseCommandLine(int argc, const char* const argv[]) {
    namespace po = boost::program_options;

    po::options_description desc{"All options"};
    Args args;
    desc.add_options()           
        ("help,h", "Show help")  
        ("config-file,c", po::value(&args.config)->value_name("file"),
         "set config file path")  
        ("www-root,w", po::value(&args.root)->value_name("dir"), 
         "set static files root")
        ("tick-period,t", po::value(&args.milliseconds)->value_name("milliseconds"),
         "set config file path")
        ("randomize-spawn-points,r",
         "spawn dogs at random positions");

    po::variables_map vm;
    po::store(po::parse_command_line(argc, argv, desc), vm);
    po::notify(vm);
    
    if (vm.contains("help")) {
        std::cout << desc;
        return args;
    }
    if (!vm.contains("config-file")) {
        throw std::runtime_error("Can't find root path. "
                                  "Use <-c> flag to specify config path");
    }
    if (!vm.contains("www-root")) {
        throw std::runtime_error("Can't find root path. "
                                  "Use <-w> flag to specify root path");
    }
    if (vm.contains("randomize-spawn-points")) {
        args.random_spawn = true;
    }
    return args;
}
} //namespace commands