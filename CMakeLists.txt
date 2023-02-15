cmake_minimum_required(VERSION 3.15)
project(libh265 VERSION 1.0)

add_library(h265_compiler_flags INTERFACE)
target_compile_features(h265_compiler_flags INTERFACE cxx_std_11)

set(LIB_SOURCES
    src/h265.c
    src/h265_bitstream.c
    src/h265_ctx.c
    src/h265_dump.c
    src/h265_reader.c
    src/h265_types.c
    src/h265_writer.c
)

add_library(h265 ${LIB_SOURCES})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

target_include_directories(h265 PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

target_compile_definitions(h265 PRIVATE "H265_API_EXPORTS")
target_compile_definitions(h265 PRIVATE "H_GNU_SOURCE")
target_compile_options(h265 PRIVATE "-fvisibility=hidden")

if(WIN32)
  target_link_libraries(h265 ws2_32)
endif()

find_package(json-c CONFIG REQUIRED)
target_link_libraries(h265 json-c::json-c json-c::json-c-static)
target_link_libraries(h265 ulog h265_compiler_flags)