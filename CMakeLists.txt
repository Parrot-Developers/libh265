cmake_minimum_required(VERSION 3.15)
project(h265 VERSION 1.0)

set(LIB_SOURCES
    src/h265.c
    src/h265_bitstream.c
    src/h265_ctx.c
    src/h265_dump.c
    src/h265_reader.c
    src/h265_types.c
    src/h265_writer.c
)

add_library(${PROJECT_NAME} SHARED ${LIB_SOURCES})

# checks if set up rpath exists for install
if(COMMAND set_up_rpath)
    set_up_rpath()
else()
    message("Set up rpath not defined!")
endif()

option(BUILD_SHARED_LIBS "Build using shared libraries" ON)
set_target_properties(${PROJECT_NAME} PROPERTIES
                    POSITION_INDEPENDENT_CODE ${BUILD_SHARED_LIBS}
)

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
    target_link_libraries(${PROJECT_NAME} 
                    PRIVATE 
                      ws2_32
                      json-c::json-c
                      ulog

        )

    install(CODE "file(GLOB dll_files \"${CMAKE_CURRENT_BINARY_DIR}/*.dll\")")
    install(CODE "file(INSTALL DESTINATION \"\$\{CMAKE_INSTALL_PREFIX\}/bin\" TYPE SHARED_LIBRARY FILES \$\{dll_files\})")

else()
    target_link_libraries(${PROJECT_NAME} 
                    PRIVATE
                      json-c::json-c
                      ulog
                      )

    install(CODE "set(DEPENDENCY_PATHS \"\$\<TARGET_FILE_DIR:json-c::json-c\>\")")
    
    install(CODE [[
        file(GET_RUNTIME_DEPENDENCIES
            LIBRARIES $<TARGET_FILE:h265>
            RESOLVED_DEPENDENCIES_VAR _r_deps
            UNRESOLVED_DEPENDENCIES_VAR _u_deps
            DIRECTORIES ${DEPENDENCY_PATHS}
        )
        foreach(_file ${_r_deps})
            file(INSTALL
            DESTINATION "${CMAKE_INSTALL_PREFIX}/lib"
            TYPE SHARED_LIBRARY
            FOLLOW_SYMLINK_CHAIN
            FILES "${_file}"
            )
        endforeach()
        list(LENGTH _u_deps _u_length)
    ]])
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
