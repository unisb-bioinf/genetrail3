add_subdirectory(${GTEST_SRC_DIR} "gtest")

# This function allows to add a new Google Test based
# test case to CTest. It takes the following arguments:
# - TEST_NAME
#   The name of the test. A file called ${TEST_NAME}.cpp
#   must be present
#
# - SOURCE_FILES source_file1 [source_file2 ...]
#   Optionally a list of additional source files that
#   should be compiled for the test
#
# - LIBRARIES lib1 [lib2 ...]
#   Optionally a list of additional source files that
#   should be compiled for the test
function(add_gtest TEST_NAME)
    # Parse the additional arguments
    set(MULTI_ARGUMENTS LIBRARIES SOURCE_FILES)
    cmake_parse_arguments(MY_ARGS "" "" "${MULTI_ARGUMENTS}" ${ARGN})

    # Setup the executable
    add_executable(${TEST_NAME} "${TEST_NAME}.cpp" ${MY_ARGS_SOURCE_FILES})
    target_link_libraries(${TEST_NAME} gtest gtest_main test_driver ${MY_ARGS_LIBRARIES})

    target_include_directories(${TEST_NAME} PRIVATE
        ${EIGEN3_INCLUDE_DIR}
        ${RapidJSON_INCLUDE_DIR}
        ${Boost_INCLUDE_DIRS}
    )

    GT2_COMPILE_FLAGS(${TEST_NAME})

    # Add the test to CTest
    add_test(NAME ${TEST_NAME} WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}/test/libraries" COMMAND ${TEST_NAME})
endfunction()

function(create_test_config_file)
	set(TEST_DATA_PATH "${PROJECT_SOURCE_DIR}/data/")
	configure_file(${CMAKE_SOURCE_DIR}/test/libraries/cmake/config.h.in "${PROJECT_BINARY_DIR}/config.h")
	include_directories(${PROJECT_BINARY_DIR})
endfunction()

