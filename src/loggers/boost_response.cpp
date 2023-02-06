#include "boost_response.h"

namespace boost_log{

void tag_invoke(json::value_from_tag, json::value& val, const StartServer& data){
    val = {{PORT, data.port_},
           {ADDRESS, data.address_}};
}

void tag_invoke(json::value_from_tag, json::value& val, const Request& req){
    val = {{IP, req.ip},
           {URL, req.url},
           {METHOD, req.method}};
}

void tag_invoke(json::value_from_tag, json::value& val, const FinishServer& data){
    val = {{CODE, data.code_}};
}

void tag_invoke(json::value_from_tag, json::value& val, const ExceptionReciever& data){
    val = {{CODE, data.code_},
           {EXCEPTION, data.what_},
           {WHERE, data.where_}};
}

} //namespace
