cmake_minimum_required(VERSION 3.15)
project(h265 VERSION 1.0)

set_up_rpath()

set(LIB_SOURCES
    src/h265.c
    src/h265_bitstream.c
    src/h265_ctx.c
    src/h265_dump.c
    src/h265_reader.c
    src/h265_types.c
    src/h265_writer.c
)

add_library(${PROJECT_NAME} ${LIB_SOURCES})

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)

target_include_directories(${PROJECT_NAME} PUBLIC
                            $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
                            $<INSTALL_INTERFACE:include>
)

target_compile_definitions(${PROJECT_NAME} PRIVATE "H265_API_EXPORTS")
target_compile_definitions(${PROJECT_NAME} PRIVATE "_GNU_SOURCE")
target_compile_options(${PROJECT_NAME} PRIVATE "-fvisibility=hidden")
target_compile_options(${PROJECT_NAME} PRIVATE "-std=gnu99")

find_package(json-c CONFIG REQUIRED)
if(WIN32)
    target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32
                      json-c::json-c 
                      json-c::json-c-static
                      ulog

        )

else()
target_link_libraries(${PROJECT_NAME} 
                    PRIVATE
                      json-c::json-c 
                      json-c::json-c-static
                      ulog
                      )
endif()


set(${PROJECT_NAME}-headers
    ${PROJECT_SOURCE_DIR}/include/${PROJECT_NAME}/
)

install(TARGETS ${PROJECT_NAME}
    EXPORT ${PROJECT_NAME}-targets
    ARCHIVE DESTINATION lib
    LIBRARY DESTINATION lib
    RUNTIME DESTINATION bin
)

# to get all header files correctly
install(
    DIRECTORY ${${PROJECT_NAME}-headers} DESTINATION include/${PROJECT_NAME}
)
