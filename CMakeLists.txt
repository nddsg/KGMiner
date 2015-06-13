cmake_minimum_required(VERSION 3.2)
project(GBpedia)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories(/usr/lib /usr/local/lib)

set(SOURCE_FILES src/parser/main.cpp)

add_executable(gbparser src/parser/main.cpp src/parser/opts.cpp src/parser/opts.h src/parser/rdf_parser.cpp src/parser/rdf_parser.h)
target_link_libraries(gbparser /usr/local/lib/librdf++.dylib libboost_program_options.a boost_system boost_filesystem)

add_executable(gbserver src/gbserver/main.cpp src/gbserver/opts.h src/gbserver/opts.cpp src/gbserver/node_loader.h src/gbserver/edge_loader.h src/gbserver/type_loader.h src/gbserver/socket_server.h)
target_link_libraries(gbserver libboost_program_options.a boost_system boost_filesystem boost_thread-mt)

add_executable(gbclient src/gbclient/main.cpp src/gbclient/socket_client.h)
target_link_libraries(gbclient libboost_program_options.a boost_system boost_filesystem boost_thread-mt)
