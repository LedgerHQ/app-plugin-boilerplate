# Fuzzing plugins

## Fuzzing

Fuzzing allows us to test how a program behaves when provided with invalid, unexpected, or random data as input.

In the case of `app-plugin-boilerplate` we want to test the code that receives and parses data from an application. Looking at `fuzz_plugin.c` we can see that is tries to simulate the execution of a plugin, by initializing the contract, providing a number of random parameters and finalizing it. It also provides random information lookup results requested by the plugin and finally displays the information to stdout.

`fuzz_plugin.c`, implements `int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)`, which provides an array of random bytes that can be used to simulate the data exchanged between an app and the plugin. If the application crashes, or a [sanitizer](https://github.com/google/sanitizers) detects any kind of access violation, the fuzzing process is stopped, a report regarding the vulnerability is shown, and the input that triggered the bug is written to disk under the name `crash-*`. The vulnerable input file created can be passed as an argument to the fuzzer to triage the issue.


## Compilation

In `fuzzing` folder

```
cmake -DBOLOS_SDK=/path/to/sdk -DCMAKE_C_COMPILER=/usr/bin/clang -Bbuild -H.
```

then

```
make -C build
```

## Run

```
./build/plugin_fuzzer
```