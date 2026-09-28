#!/bin/bash -eu

# build fuzzers

pushd fuzzing
cmake -DBOLOS_SDK=../BOLOS_SDK -Bbuild -H.
make -C build
mv ./build/fuzz "${OUT}"
zip -j "${OUT}/fuzz_seed_corpus.zip" seeds/*.bin
popd
