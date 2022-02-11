#!/bin/bash

# FILL THESE WITH YOUR OWN SDKs PATHS and APP-ETHEREUM's ROOT
NANOS_SDK=$NANOS_SDK
NANOX_SDK=$NANOX_SDK
APP_ETHEREUM=$APP_ETHEREUM

# create elfs folder if it doesn't exist
mkdir -p elfs

# move to repo's root to build apps
cd ..

echo "*Building elfs for Nano S..."

echo "**Building app-plugin for Nano S..."
make clean BOLOS_SDK=$NANOS_SDK
make -j DEBUG=1 BOLOS_SDK=$NANOS_SDK
cp bin/app.elf "tests/elfs/plugin_nanos.elf"

echo "**Building app-ethereum for Nano S..."
cd $APP_ETHEREUM
make clean BOLOS_SDK=$NANOS_SDK
make -j DEBUG=1 BYPASS_SIGNATURES=1 BOLOS_SDK=$NANOS_SDK CHAIN=ethereum
cd -
cp "${APP_ETHEREUM}/bin/app.elf" "tests/elfs/ethereum_nanos.elf"


echo "*Building elfs for Nano X..."

echo "**Building plugin for Nano X..."
make clean BOLOS_SDK=$NANOX_SDK
make -j DEBUG=1 BOLOS_SDK=$NANOX_SDK
cp bin/app.elf "tests/elfs/plugin_nanox.elf"

echo "**Building app-ethereum for Nano X..."
cd $APP_ETHEREUM
make clean BOLOS_SDK=$NANOX_SDK
make -j DEBUG=1 BYPASS_SIGNATURES=1 BOLOS_SDK=$NANOX_SDK CHAIN=ethereum
cd -
cp "${APP_ETHEREUM}/bin/app.elf" "tests/elfs/ethereum_nanox.elf"

echo "done"
