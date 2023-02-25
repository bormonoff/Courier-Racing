#pragma once

#include <boost/program_options.hpp>

namespace commands {

struct Args {
    std::string config;
    std::string root;
    size_t milliseconds{0};
    bool random_spawn{0};
}; 

Args ParseCommandLine(int argc, const char* const argv[]);
} //namespace commands