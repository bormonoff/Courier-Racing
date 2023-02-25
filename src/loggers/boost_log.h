#pragma once

#include "boost_response.h"

namespace boost_log{

template<class T>
json::object MakeResponse(const std::string& msg, T&& data) {
    json::object response;
    response["timestamp"] =  boost::posix_time::to_iso_extended_string(
        boost::posix_time::microsec_clock::local_time());
    response["data"] = json::value_from(data);
    response["message"] = msg;

    return response;
}

void LogFormatter(logging::record_view const& rec, logging::formatting_ostream& strm);
void InitBoostLog();
}  //namespace boost_log