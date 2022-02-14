#!/bin/bash

# FILL THESE WITH YOUR OWN SDKs PATHS and APP-ETHEREUM's ROOT
NANOS_SDK=$NANOS_SDK
NANOX_SDK=$NANOX_SDK
APP_ETHEREUM=$APP_ETHEREUM

set -e

build_plugin() {
    # arguments: <SDK letter>
    echo "** Building app-plugin for Nano $1..."
    local target="tests/elfs/plugin_nano${1,,}.elf"
    if [ $1 == "S" ];
    then
        local sdk=$NANOS_SDK
    elif [ $1 == "X" ];
    then
        local sdk=$NANOX_SDK
    else
        echo "Unknown SDK '$1'"
        exit 1
    fi
    cd ..
    make clean BOLOS_SDK=$sdk
    make -j DEBUG=1 BOLOS_SDK=$sdk
    cp bin/app.elf $target
    cd -
}

build_ethereum() {
    # arguments: <SDK letter>
    echo "** Building app-ethereum for Nano $1..."
    local target="tests/elfs/ethereum_nano${1,,}.elf"
    if [ $1 == "S" ];
    then
        local sdk=$NANOS_SDK
    elif [ $1 == "X" ];
    then
        local sdk=$NANOX_SDK
    else
        echo "Unknown SDK '$1'"
        exit 1
    fi
    cd $APP_ETHEREUM
    make clean BOLOS_SDK=$sdk
    make -j DEBUG=1 BYPASS_SIGNATURES=1 BOLOS_SDK=$sdk CHAIN=ethereum
    cp bin/app.elf $target
    cd -
}


main() {
    # create elfs folder if it doesn't exist
    mkdir -p elfs

    if [ $# -ne 0 ];
    then
        test -d $1 ||
            (echo "Provided argument '$1' is expected to be the app-ethereum repository path, but is not a directory" && exit 1);
        APP_ETHEREUM=$1
    fi

    echo "* Building elfs for Nano S..."
    build_plugin "S"
    build_ethereum "S"

    echo "* Building elfs for Nano X..."
    build_plugin "X"
    build_ethereum "X"

    echo "* Done"
}


main $@
