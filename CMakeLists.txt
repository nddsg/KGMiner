cmake_minimum_required(VERSION 3.2)
project(GBpedia)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_SOURCE_DIR}/bin")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

link_directories(/usr/lib /usr/local/lib)

set(SOURCE_FILES src/main.cpp)
add_executable(GBpedia ${SOURCE_FILES} src/opts.cpp src/opts.h src/rdf_parser.cpp src/rdf_parser.h)
target_link_libraries(GBpedia /usr/local/lib/librdf++.dylib libboost_program_options.a)
