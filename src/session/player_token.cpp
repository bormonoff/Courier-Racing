#include "player_token.h"
#include <iostream>

namespace game_session{

std::string GenerateToken(){
    std::stringstream ss;
    std::string result;
    for (int i = 0; i < 32; i++)
    {
        size_t element = util::GetRandomNumber(1, 15);
        if(element > 9){
            if(element == 10){ss << 'a'; continue;}
            if(element == 11){ss << 'b'; continue;}
            if(element == 12){ss << 'c'; continue;}
            if(element == 13){ss << 'd'; continue;}
            if(element == 14){ss << 'e'; continue;}
            if(element == 15){ss << 'f'; continue;}
        }
        ss << element;
    }
    getline(ss, result);
    return result;
}

} // namespace