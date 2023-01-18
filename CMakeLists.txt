cmake_minimum_required(VERSION 3.11)

project(HelloAsync CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup()

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

add_executable(courier_race 
  src/main.cpp 
  src/http_server.cpp 
  src/http_server.h 
  src/sdk.h
  src/tagged.h
  src/model.h
  src/model.cpp
)

target_link_libraries(courier_race PRIVATE Threads::Threads)
