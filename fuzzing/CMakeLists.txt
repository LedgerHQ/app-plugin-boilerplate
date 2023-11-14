cmake_minimum_required(VERSION 3.10)

if(${CMAKE_VERSION} VERSION_LESS 3.10)
    cmake_policy(VERSION ${CMAKE_MAJOR_VERSION}.${CMAKE_MINOR_VERSION})
endif()

# project information
project(Fuzzer
        VERSION 1.0
        DESCRIPTION "Contract parser of Boilerplate plugin app"
        LANGUAGES C)

# guard against bad build-type strings
if (NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Release")
endif()

if (NOT CMAKE_C_COMPILER_ID MATCHES "Clang")
    message(FATAL_ERROR "Fuzzer needs to be built with Clang")
endif()

if (NOT DEFINED BOLOS_SDK)
    message(FATAL_ERROR "BOLOS_SDK environment variable not found.")
endif()

# guard against in-source builds
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR "In-source builds not allowed. Please make a new directory (called a build directory) and run CMake from there. You may need to remove CMakeCache.txt. ")
endif()

# specify C standard
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED True)

# compatible with ClusterFuzzLite
if (NOT DEFINED ENV{LIB_FUZZING_ENGINE})
    set(COMPILATION_FLAGS_ "-fsanitize=fuzzer,address,undefined,signed-integer-overflow")
else()    
    set(COMPILATION_FLAGS_ "$ENV{LIB_FUZZING_ENGINE} $ENV{CXXFLAGS}")
endif()    
string(REPLACE " " ";" COMPILATION_FLAGS ${COMPILATION_FLAGS_})

add_compile_options(-Wall -Wextra -g -pedantic)
# Just to limit compilation warnings of the plugin sources
add_compile_options(-Wno-implicit-function-declaration)
# Flag depending on the Build Type
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ggdb2 -O3")

set(ANGLE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(ETH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ethereum-plugin-sdk")

add_compile_definitions(
        HAVE_SHA3
        HAVE_HASH
        HAVE_ECC
        IO_HID_EP_LENGTH=64
        APPNAME="PluginBoilerplate"
)

add_library(angle
        ${ANGLE_DIR}/handle_finalize.c
        ${ANGLE_DIR}/handle_init_contract.c
        ${ANGLE_DIR}/handle_provide_parameter.c
        ${ANGLE_DIR}/handle_provide_token.c
        ${ANGLE_DIR}/handle_query_contract_id.c
        ${ANGLE_DIR}/handle_query_contract_ui.c
        ${ANGLE_DIR}/plugin.c
        ${ETH_DIR}/include/utils.c
        ${ETH_DIR}/include/eth_internals.c
)

add_executable(fuzz fuzz.c mocks.c)

target_link_libraries(fuzz PUBLIC angle)
target_compile_options(fuzz PUBLIC ${COMPILATION_FLAGS})
target_link_options(fuzz PUBLIC ${COMPILATION_FLAGS})

include_directories(".")
include_directories(${ANGLE_DIR})
include_directories(${ETH_DIR}/include)
include_directories(${BOLOS_SDK}/lib_cxng/include)
include_directories(${BOLOS_SDK}/lib_cxng/src)
include_directories(${BOLOS_SDK}/include)
