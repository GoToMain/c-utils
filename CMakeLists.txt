#
#  Copyright (c) 2020-2026 Siddharth Chandrasekaran <sidcha@osdp.dev>
#
#  SPDX-License-Identifier: Apache-2.0
#

cmake_minimum_required(VERSION 3.14 FATAL_ERROR)
project(utils VERSION 0.1.0 LANGUAGES C)

# libutils.a
set(LIB_UTILS utils)
file(GLOB LIB_UTILS_SRC ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
file(GLOB LIB_UTILS_INC ${CMAKE_CURRENT_SOURCE_DIR}/include/utils/*.h)
add_library(${LIB_UTILS} STATIC ${LIB_UTILS_SRC} ${LIB_UTILS_INC})
target_include_directories(${LIB_UTILS} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${LIB_UTILS} PROPERTIES PUBLIC_HEADER "${LIB_UTILS_INC}")

# check target
set(TEST_BIN test-utils)
file(GLOB TEST_SRC
	${CMAKE_CURRENT_SOURCE_DIR}/tests/*.c
	${CMAKE_CURRENT_SOURCE_DIR}/tests/*.h
)
add_executable(${TEST_BIN} ${TEST_SRC})
target_link_libraries(${TEST_BIN} ${LIB_UTILS} pthread)
target_include_directories(${TEST_BIN} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
set_target_properties(${TEST_BIN} PROPERTIES EXCLUDE_FROM_ALL TRUE)

function(download_file URL FILE_PATH)
	message("Downloading ${FILE_PATH}..")
	file(DOWNLOAD ${URL} ${FILE_PATH} STATUS DOWNLOAD_RESULT)
	list(GET DOWNLOAD_RESULT 0 DOWNLOAD_RESULT_CODE)
	if(NOT DOWNLOAD_RESULT_CODE EQUAL 0)
		message(FATAL_ERROR "Failed downloading ${URL}! Error: ${DOWNLOAD_RESULT}.")
	endif()
endfunction()

add_custom_target(check_utils
	COMMAND $<TARGET_FILE:${TEST_BIN}> -i"${CMAKE_CURRENT_SOURCE_DIR}/tests/input"
	DEPENDS ${TEST_BIN}
	USES_TERMINAL
)
