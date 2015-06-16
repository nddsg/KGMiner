cmake_minimum_required(VERSION 2.8)
project(GBpedia)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11 -O2")

include_directories(/usr/include /usr/local/include ./deps/include)
link_directories(/usr/lib /usr/local/lib ./deps/lib)

set(SOURCE_FILES src/parser/main.cpp)

add_executable(gbparser src/parser/main.cpp src/parser/opts.cpp src/parser/opts.h src/parser/rdf_parser.cpp src/parser/rdf_parser.h)
add_executable(gbserver src/gbserver/main.cpp src/gbserver/opts.h src/gbserver/opts.cpp src/gbserver/node_loader.h src/gbserver/edge_loader.h src/gbserver/type_loader.h src/gbserver/socket_server.h src/gbserver/graph.h src/gbserver/edge_list.cpp src/gbserver/edge_list.h)
add_executable(gbclient src/gbclient/main.cpp src/gbclient/socket_client.h)


IF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")
   # Mac OS X specific code
    target_link_libraries(gbparser librdf++.dylib libboost_program_options.a boost_system boost_filesystem)
    target_link_libraries(gbserver libboost_program_options.a boost_system boost_filesystem boost_thread-mt)
    target_link_libraries(gbclient libboost_program_options.a boost_system boost_filesystem boost_thread-mt)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Darwin")

IF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")
    # Linux specific code
    target_link_libraries(gbparser librdf++.so libboost_program_options.a boost_system boost_filesystem)
    target_link_libraries(gbserver libboost_program_options.a boost_system boost_filesystem boost_thread)
    target_link_libraries(gbclient libboost_program_options.a boost_system boost_filesystem boost_thread)
ENDIF(${CMAKE_SYSTEM_NAME} MATCHES "Linux")