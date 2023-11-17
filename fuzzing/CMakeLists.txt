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
    set(COMPILATION_FLAGS_ "$ENV{LIB_FUZZING_ENGINE} $ENV{CFLAGS}")
endif()    
string(REPLACE " " ";" COMPILATION_FLAGS ${COMPILATION_FLAGS_})

add_compile_options(-Wall -Wextra -g -pedantic)
# Just to limit compilation warnings of the plugin sources
add_compile_options(-Wno-implicit-function-declaration)
# Flag depending on the Build Type
set(CMAKE_C_FLAGS_DEBUG "${CMAKE_C_FLAGS_DEBUG} -O0")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -ggdb2 -O3")

set(SRC_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../src")
set(ETH_DIR "${CMAKE_CURRENT_SOURCE_DIR}/../ethereum-plugin-sdk")

add_compile_definitions(
    APPNAME="PluginBoilerplate"
)

add_compile_definitions(
    IO_HID_EP_LENGTH=64
    HAVE_ECC
    HAVE_ECC_WEIERSTRASS
    HAVE_SECP_CURVES
    HAVE_ECC_TWISTED_EDWARDS
    HAVE_ED_CURVES
    HAVE_ECDSA
    HAVE_EDDSA
    HAVE_HASH
    HAVE_BLAKE2
    HAVE_SHA256
    HAVE_SHA3
    HAVE_SHA512
)

include_directories(
    ${BOLOS_SDK}/include
    ${BOLOS_SDK}/lib_cxng/include
    ${BOLOS_SDK}/lib_cxng/src
    ${BOLOS_SDK}/target/nanox/include
    ${ETH_DIR}/include
    ${SRC_DIR}
)

add_executable(fuzz
    fuzz_plugin.c
    mocks.c

    ${SRC_DIR}/plugin.c
    ${SRC_DIR}/handle_init_contract.c
    ${SRC_DIR}/handle_provide_parameter.c
    ${SRC_DIR}/handle_finalize.c
    ${SRC_DIR}/handle_provide_token.c
    ${SRC_DIR}/handle_query_contract_ui.c
    ${SRC_DIR}/handle_query_contract_id.c

    # Ethereum SDK
    ${ETH_DIR}/include/eth_internals.c
    ${ETH_DIR}/include/utils.c

    # cxng
    ${BOLOS_SDK}/lib_cxng/src/cx_hash.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha256.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha512.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha3.c
    ${BOLOS_SDK}/lib_cxng/src/cx_blake2b.c
    ${BOLOS_SDK}/lib_cxng/src/cx_utils.c
    ${BOLOS_SDK}/lib_cxng/src/cx_ram.c
)

target_compile_options(fuzz PUBLIC ${COMPILATION_FLAGS})
target_link_options(fuzz PUBLIC ${COMPILATION_FLAGS})