#include "utils.h"

namespace util{

uint64_t CurrentTime(){
    using namespace std::chrono;
    return duration_cast<milliseconds>(system_clock::now().time_since_epoch()).count();
}

size_t GetRandomNumber(const int min, const int max){
    static std::random_device rand{};
    static std::mt19937_64 generator(rand());
    std::uniform_int_distribution<int> distribution{min, max};

    // std::cout<<sizeof(rand)<<" bytes - rand "<<std::endl;
    // std::cout<<sizeof(generator)<<" bytes - gen"<<std::endl;
    // std::cout<<sizeof(distribution)<<" bytes - distr"<<std::endl;

    return distribution(generator);
}

} // namespace 