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

add_executable(game_server
	src/main.cpp
	src/sdk.h
	src/core/http_server.cpp
	src/core/http_server.h
	src/model/model.h
	src/model/model.cpp
	src/json/json_loader.h
	src/json/json_loader.cpp
	src/handlers/request_handler.cpp
	src/handlers/request_handler.h
	src/handlers/api_handler.h
	src/handlers/api_handler.cpp
	src/handlers/logging_request_handler.h
	src/handlers/application.h
	src/handlers/application.cpp
	src/handlers/api_response_storage.h
	src/handlers/api_response_storage.cpp
	src/handlers/literals_storage.h
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
	src/util/utils.h
	src/util/utils.cpp
	src/util/tagged.h
	src/util/boost_json.cpp
	src/time/ticker.h
	src/time/ticker.cpp
	src/commands/options.h
	src/commands/options.cpp
)

target_link_libraries(game_server PRIVATE Threads::Threads CONAN_PKG::boost)
target_include_directories(game_server PRIVATE 
	CONAN_PKG::boost
	src
)
set(CMAKE_BUILD_TYPE Debug)

add_compile_definitions(BOOST_BEAST_USE_STD_STRING_VIEW)

