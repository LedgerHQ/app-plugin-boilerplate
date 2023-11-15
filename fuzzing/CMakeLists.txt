cmake_minimum_required(VERSION 3.12)

project(PluginFuzzer LANGUAGES C)


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

# compatible with ClusterFuzzLite
if (NOT DEFINED ENV{LIB_FUZZING_ENGINE})
	set(COMPILATION_FLAGS -ggdb2 -fsanitize=fuzzer,address)
else()
	set(COMPILATION_FLAGS_ "$ENV{LIB_FUZZING_ENGINE} $ENV{CFLAGS}")
    string(REPLACE " " ";" COMPILATION_FLAGS ${COMPILATION_FLAGS_})
endif()


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

include_directories(.
    ${BOLOS_SDK}/include
    ${BOLOS_SDK}/lib_cxng/include
    ${BOLOS_SDK}/lib_cxng/src
    ${BOLOS_SDK}/target/nanox/include
)

# Fuzzer target
add_executable(plugin_fuzzer
    fuzz_plugin.c

    ../src/plugin.c
    ../src/handle_init_contract.c
    ../src/handle_provide_parameter.c
    ../src/handle_finalize.c
    ../src/handle_provide_token.c
    ../src/handle_query_contract_ui.c
    ../src/handle_query_contract_id.c

    # Ethereum SDK
    ../ethereum-plugin-sdk/include/eth_internals.c
    ../ethereum-plugin-sdk/include/utils.c

    # cxng
    ${BOLOS_SDK}/lib_cxng/src/cx_hash.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha256.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha512.c
    ${BOLOS_SDK}/lib_cxng/src/cx_sha3.c
    ${BOLOS_SDK}/lib_cxng/src/cx_blake2b.c
    ${BOLOS_SDK}/lib_cxng/src/cx_utils.c
    ${BOLOS_SDK}/lib_cxng/src/cx_ram.c
)

target_include_directories(plugin_fuzzer PRIVATE
    ../src
    ../ethereum-plugin-sdk/include
)

target_link_libraries(plugin_fuzzer PUBLIC libbsd.a)

target_compile_options(plugin_fuzzer PUBLIC ${COMPILATION_FLAGS})
target_link_options(plugin_fuzzer PUBLIC ${COMPILATION_FLAGS})