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

add_library(GameLib STATIC 
	src/model/road.h
	src/model/building.h
	src/model/office.h
	src/model/lost_things.h
	src/model/lost_things.cpp
	src/model/roadmap.h
	src/model/roadmap.cpp
	src/model/map.h
	src/model/map.cpp
	src/model/game.h
	src/model/game.cpp
	src/model/loot_generator.h
	src/model/loot_generator.cpp
	src/model/collision_detector.h
	src/model/collision_detector.cpp
	src/util/utils.h
	src/util/utils.cpp
)

target_include_directories(GameLib PUBLIC src)

add_library(GameSessionLib STATIC 
	src/session/dog.h
	src/session/dog.cpp
	src/session/game_session.h
	src/session/game_session.cpp
	src/session/player.h
	src/session/player.cpp
	src/session/player_token.h
	src/session/player_token.cpp
)

target_include_directories(GameSessionLib PUBLIC src)


add_executable(game_server 
	src/main.cpp
	src/sdk.h
	src/core/http_server.cpp
	src/core/http_server.h
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/json/json_loot_types_storage.h
	src/serialization/item_serialization.h
	src/serialization/item_serialization.cpp
	src/serialization/dog_serialization.h
	src/serialization/dog_serialization.cpp
	src/serialization/session_serialization.h
	src/serialization/session_serialization.cpp
	src/handlers/request_handler.cpp
	src/handlers/request_handler.h
	src/handlers/api_handler.h
	src/handlers/api_handler.cpp
	src/handlers/log_request_handler.h
	src/handlers/application.h
	src/handlers/application.cpp
	src/handlers/api_response_storage.h
	src/handlers/api_response_storage.cpp
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

add_executable(test_dog
	tests/dog_tests.cpp
)

add_executable(test_player_token
	tests/player_token_tests.cpp
	tests/catch2_custom_types.h
)

add_executable(test_game_session
	tests/game_session_tests.cpp
	tests/catch2_custom_types.h
)

add_executable(test_serialization
	tests/state_serialization_tests.cpp
	src/serialization/item_serialization.h
	src/serialization/item_serialization.cpp
	src/serialization/dog_serialization.h
	src/serialization/dog_serialization.cpp
	src/serialization/session_serialization.h
	src/serialization/session_serialization.cpp
)

target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost GameLib GameSessionLib)
target_link_libraries(test_loot_generator PRIVATE CONAN_PKG::catch2 GameLib)
target_link_libraries(test_model PRIVATE CONAN_PKG::catch2 GameLib)
target_link_libraries(test_collision_detector PRIVATE CONAN_PKG::catch2 GameLib)
target_link_libraries(test_dog PRIVATE CONAN_PKG::catch2 GameLib GameSessionLib)
target_link_libraries(test_player_token PRIVATE CONAN_PKG::catch2 GameSessionLib GameLib)
target_link_libraries(test_game_session PRIVATE CONAN_PKG::catch2 GameSessionLib GameLib)
target_link_libraries(test_serialization PRIVATE CONAN_PKG::catch2 CONAN_PKG::boost GameSessionLib GameLib)

target_include_directories(game_server PRIVATE CONAN_PKG::boost	src)
target_include_directories(test_loot_generator PRIVATE src)
target_include_directories(test_model PRIVATE src)
target_include_directories(test_collision_detector PRIVATE src)
target_include_directories(test_dog PRIVATE src)
target_include_directories(test_player_token PRIVATE src)
target_include_directories(test_game_session PRIVATE src)
target_include_directories(test_serialization PRIVATE CONAN_PKG::boost src)



set(CMAKE_BUILD_TYPE Debug)

include(CTest)
include(${CONAN_BUILD_DIRS_CATCH2}/Catch.cmake) 

catch_discover_tests(test_model) 
catch_discover_tests(test_loot_generator) 
catch_discover_tests(test_collision_detector) 
catch_discover_tests(test_dog) 
catch_discover_tests(test_player_token) 
catch_discover_tests(test_game_session) 
catch_discover_tests(test_serialization) 


add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

