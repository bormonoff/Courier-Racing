cmake_minimum_required(VERSION 3.11)

project(game_server CXX)
set(CMAKE_CXX_STANDARD 20)

include(${CMAKE_BINARY_DIR}/conanbuildinfo.cmake)
conan_basic_setup(TARGETS)

find_package(Boost 1.78.0 REQUIRED)
if(Boost_FOUND)
  include_directories(${Boost_INCLUDE_DIRS})
endif()

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

set(CMAKE_BUILD_TYPE Debug)

add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

add_library(GameLib STATIC 
	src/model/model.h
	src/model/model.cpp
	src/model/loot_generator.h
	src/model/loot_generator.cpp
	src/model/collision_detector.h
	src/model/collision_detector.cpp
	src/util/utils.h
	src/util/utils.cpp
)

target_include_directories(GameLib PUBLIC src)

add_executable(game_server 
	src/main.cpp
	src/sdk.h
	src/core/http_server.cpp
	src/core/http_server.h
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/json/json_loot_types_storage.h
	src/handlers/request_handler.cpp
	src/handlers/request_handler.h
	src/handlers/api_handler.h
	src/handlers/api_handler.cpp
	src/handlers/logging_request_handler.h
	src/handlers/application.h
	src/handlers/application.cpp
	src/handlers/api_response_storage.h
	src/handlers/api_response_storage.cpp
	src/session/dog.h
	src/session/dog.cpp
	src/session/game_session.h
	src/session/game_session.cpp
	src/session/player.h
	src/session/player.cpp
	src/session/player_token.h
	src/session/player_token.cpp
	src/loggers/boost_log.cpp
	src/loggers/boost_log.h
	src/loggers/boost_response.h
	src/loggers/boost_response.cpp
	src/util/tagged.h
	src/util/boost_json.cpp
	src/util/literals_storage.h
	src/time/ticker.h
	src/time/ticker.cpp
	src/commands/options.h
	src/commands/options.cpp
)
add_executable(test_model 
	tests/model_tests.cpp
)

add_executable(test_loot_generator 
	tests/loot_generator_tests.cpp
)

add_executable(test_collision_detector
	tests/collision_detector_tests.cpp
)

target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost GameLib)
target_link_libraries(test_loot_generator PRIVATE CONAN_PKG::catch2 GameLib)
target_link_libraries(test_model PRIVATE CONAN_PKG::catch2 GameLib)
target_link_libraries(test_collision_detector PRIVATE CONAN_PKG::catch2 GameLib)

target_include_directories(game_server PRIVATE CONAN_PKG::boost	src)
target_include_directories(test_loot_generator PRIVATE src)
target_include_directories(test_model PRIVATE src)
target_include_directories(test_collision_detector PRIVATE src)


include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake) 

catch_discover_tests(test_model) 
catch_discover_tests(test_loot_generator) 
catch_discover_tests(test_collision_detector) 