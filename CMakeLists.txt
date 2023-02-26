cmake_minimum_required(VERSION 3.15)
project(libh265 VERSION 1.0)

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
set_target_properties(h265 PROPERTIES
                    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

target_include_directories(h265 PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

target_compile_definitions(h265 PRIVATE "H265_API_EXPORTS")
target_compile_definitions(h265 PRIVATE "_GNU_SOURCE")
target_compile_options(h265 PRIVATE "-fvisibility=hidden")
target_compile_options(h265 PRIVATE "-std=gnu99")

if(WIN32)
  target_link_libraries(h265 ws2_32)
endif()

find_package(json-c CONFIG REQUIRED)
target_link_libraries(h265 json-c::json-c)
target_link_libraries(h265 ulog)