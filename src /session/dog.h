#pragma once

#include <string>


namespace game_session{

class Dog{
public:
    Dog(std::string name)
        :name_{name}{
            id_ = count;
            count++;
    }
    
    const uint16_t GetID() const {return id_;}
    const std::string& GetName() const {return name_;}

private:
    std::string name_;
    uint16_t id_;
    static uint32_t count;
};

} // namespace