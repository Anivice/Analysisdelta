cmake_minimum_required(VERSION 3.20)

# --- initial setup --- #
set(PROJECT_NAME "Analysisdelta")
set(PROJECT_VERSION "0.0.1")

project(${PROJECT_NAME})
set(CMAKE_CXX_STANDARD 20)

if ("${CMAKE_BUILD_TYPE}" STREQUAL "Debug")
    add_compile_definitions(CMAKE_BUILD_DEBUG=1)
    enable_testing()
endif()

# --- target build --- #

set(MODULE_LIST)

# <1>: error handler:
add_library(error_handler STATIC
    src/debug/debug.cpp src/debug/error.cpp
    src/include/debug.h src/include/error_handler.h)
target_include_directories(error_handler PUBLIC src/include)
list(APPEND MODULE_LIST "error_handler")

#add_executable(mount.htmpfs
#        src/include/fuse_ops.h)
#target_include_directories(mount.htmpfs PUBLIC src/include)
#target_link_libraries(mount.htmpfs PUBLIC ${EXTERNAL_LIBRARIES} ${PROJECT_NAME})
#target_link_libraries(mount.htmpfs PUBLIC ${LIBFUSE_LIBRARIES})
#target_include_directories(mount.htmpfs PUBLIC ${LIBFUSE_INCLUDE_DIRS})
#target_compile_options(mount.htmpfs PUBLIC ${LIBFUSE_CFLAGS_OTHER})

#function(add_single_file EXEC_NAME PATH_PREFIX)
#    add_executable(${EXEC_NAME} ${PATH_PREFIX}/${EXEC_NAME}.cpp)
#    target_include_directories(${EXEC_NAME} PUBLIC src/include)
#    target_link_libraries(${EXEC_NAME} PUBLIC ${EXTERNAL_LIBRARIES} ${PROJECT_NAME})
#
#    message("Single exec `${EXEC_NAME}` enabled")
#endfunction()

# --- unit tests --- #
function(_add_test TEST DESCRIPTION)
    set(TEST_NAME "UT_${TEST}")

    add_executable(${TEST_NAME} "tests/${TEST}_test.cpp")
    target_link_libraries(${TEST_NAME} PUBLIC ${MODULE_LIST})
    target_include_directories(${TEST_NAME} PUBLIC src/include)

    add_test(NAME ${TEST_NAME}
            COMMAND ${TEST_NAME}
            WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")

    message("Unit test `${TEST_NAME}`\t(${DESCRIPTION}) enabled")
endfunction()

_add_test(debug "Debug test")
