#include "boost_response.h"

namespace boost_log{

void LogFormatter(logging::record_view const& rec, 
                  logging::formatting_ostream& strm) {
    strm << rec[exp::smessage];
}

void InitBoostLog() {
    logging::add_common_attributes();
    logging::add_console_log(
        std::clog,
        boost::log::keywords::format = &LogFormatter
    );   
}
}  //namespace boost_log