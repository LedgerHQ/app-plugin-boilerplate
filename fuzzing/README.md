# Fuzzing on transaction parser

Fuzzing tests how a program behaves on invalid, unexpected, or random input. Here it targets
the `ERC8009` plugin's calldata-handling code.

The fuzzer implements `int LLVMFuzzerTestOneInput(const uint8_t *data, size_t size)`, which is
handed random bytes to simulate a serialized transaction. If the process crashes, or a
[sanitizer](https://github.com/google/sanitizers) detects an access violation, fuzzing stops, a
report is shown, and the triggering input is written to disk as `crash-*` — pass that file to
the fuzzer to triage the issue.

## Harness (`fuzz_plugin.c`)

One input is one transaction. The harness drives the plugin through the
app-ethereum call sequence: `init_contract` → `provide_parameter` (one call per
32-byte calldata word) → `finalize` → `provide_token` (when `tokenLookup1/2` is
set) → `query_contract_id` → `query_contract_ui` (every screen). It stops at
the first non-OK result, as the app does (`FALLBACK` from `finalize` included),
unless the input sets `FLAG_CONTINUE`.

What the app hands to the plugin, and how the harness produces it:

- buffers of the real app sizes: `query_contract_id` name 43 / version 131,
  `query_contract_ui` title 43 / msg 100; every buffer (selector, word,
  context, `txContent`, signer, lookup items, strings) is its own heap block,
  so ASan catches any access past its end;
- `parameterOffset = 4 + 32k` for word `k` (one word may be moved by
  `seq_delta`), `parameter_size` random 1..32 per word (the plugin must ignore
  it), a partial last word zero-padded, `dataSize` = `4 + len(args)` plus a
  signed delta (misaligned, short, > 2^32 …);
- `txContent`: destination from the four allowlisted Sepolia addresses or
  random, `destinationLength` raw; `value` of any length byte; the typed
  `chainID` length byte goes up to 255 while `chainID.value` stays 32 bytes;
  legacy `v`: `vLength` up to 255 over the 8-byte `v` (app-ethereum caps it
  at 8, the fuzzer does not);
- `finalize.address` (signer) random or equal to the target of a stored guard;
- lookup answers (`item1/item2`) only when requested: address = the requested
  token or random, ticker bytes with or without a NUL within the 51-byte
  buffer, decimals 0..255;
- the plugin context is pre-filled with garbage before `init_contract`.

stdout: `name: …` and `version: …` right after an OK `QUERY_CONTRACT_ID`, then
one `<title>: <msg>` line per OK `QUERY_CONTRACT_UI`; nothing else. Fuzz with
`-close_fd_mask=1`.

### Input format

Fixed 226-byte header, multi-byte integers little-endian, then the calldata
after its selector. Bytes 0..3 of every input are the calldata selector.

| Offset | Size | Field | Meaning |
|---:|---:|---|---|
| 0 | 4 | `selector` | calldata bytes 0..3 (passed to `init_contract`) |
| 4 | 1 | `flags` | bit 0 `FLAG_CONTINUE`: keep calling the next handlers after a non-OK result (never after a failed init); bit 1 `FLAG_BAD_VERSION`: `interfaceVersion = LATEST - 1`; bit 2 `FLAG_SMALL_CTX`: `pluginContextLength = sizeof(context_t) - 1` |
| 5 | 1 | `dest_sel` | bit 7 set: destination = `dest`; else allowlist `[dest_sel & 3]` (BalanceProxy, ApproveRouter, PermitRouter, SafeRouter) |
| 6 | 1 | `dest_len` | `txContent.destinationLength` (normally 20; 0 = contract creation) |
| 7 | 20 | `dest` | random destination |
| 27 | 1 | `value_len` | `txContent.value.length` (0..255) |
| 28 | 32 | `value` | `txContent.value.value`, big-endian in the first `value_len` bytes |
| 60 | 1 | `chain_len` | `txContent.chainID.length` (0..255; 0 = no typed chain id) |
| 61 | 32 | `chain` | `txContent.chainID.value`, big-endian in the first `chain_len` bytes |
| 93 | 1 | `v_len` | `txContent.vLength` (0..255; 0 = not a legacy tx) |
| 94 | 8 | `v` | `txContent.v` |
| 102 | 1 | `signer_sel` | bit 7 set: signer = target of stored guard `signer_sel & 7`; else `signer` |
| 103 | 20 | `signer` | `finalize.address` |
| 123 | 8 | `ds_delta` | int64: `dataSize = 4 + len(args) + ds_delta` (mod 2^64) |
| 131 | 4 | `chunk_seed` | u32 seed of the `parameter_size` PRNG |
| 135 | 2 | `seq_word` | u16: word whose `parameterOffset` gets `seq_delta` |
| 137 | 4 | `seq_delta` | int32 added to that `parameterOffset` (0 = sequential) |
| 141 | 1 | `lookup_flags` | bit 0/1: `item1`/`item2` provided; bit 2/3: its address is `item.addr` instead of the requested token |
| 142 | 42 | `item1` | `addr[20]`, `ticker_len`, `ticker[20]` (repeated up to `ticker_len` bytes; NUL-terminated only when `ticker_len < 51`), `decimals` |
| 184 | 42 | `item2` | same as `item1` |
| 226 | … | `args` | calldata bytes 4.. |

### Seeds

`fuzzing/seeds/` is a fixed, tracked corpus derived from `tests/erc8009_crafter.py`:

- `ok_<selector>_<n>.bin`: valid calls for each of the 14 selectors
  (canonical typed tx, canonical legacy tx, random calls with lookup answers);
  each one reaches `QUERY_CONTRACT_ID` (`fuzzing/build/fuzz <seed>` prints
  `name: ERC-8009`).
- `bad_<class>_<n>.bin`: the negative mutations of the crafter, one per
  (class, contract family), plus a few `FLAG_CONTINUE` copies and
  length-byte extremes (`chain_len_*`, `v_len_*`, `dest_len_19`).

`fuzzing/corpus/` (what libFuzzer adds while fuzzing) is not tracked.

## Manual usage based on Ledger container

### Preparation

Before being able to use the fuzzing tests, the environment must be prepared with all submodules.
To install them, use the following command in the repository root directory:

```shell
git submodule update --init
```

The fuzzer can run from the docker `ledger-app-builder-legacy`. You can download it from the `ghcr.io` docker repository:

```shell
sudo docker pull ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

You can then enter this development environment by executing the following command from the repository root directory:

```shell
sudo docker run --rm -ti --user "$(id -u):$(id -g)" -v "$(realpath .):/app" ghcr.io/ledgerhq/ledger-app-builder/ledger-app-builder-legacy:latest
```

### Compilation

Once in the container, go into the `fuzzing` folder to compile the fuzzer:

```shell
cd fuzzing

# cmake initialization
cmake -DBOLOS_SDK=/opt/nanox-secure-sdk -DCMAKE_C_COMPILER=/usr/bin/clang -Bbuild -H.

# Fuzzer compilation
make -C build
```

### Run

```shell
./build/fuzz
```

## macOS host build

The fuzz harness can also be built directly on a macOS host with ASan + UBSan
(no-recover), without a container. **Use Apple Clang, not Homebrew LLVM**:
on macOS 26, Homebrew LLVM 19's dynamic ASan runtime deadlocks during dyld's
process-startup initializers (a self-recursive lock in
`__asan::AsanInitFromRtl`, reproducible even with a trivial no-op fuzz
target — the process never reaches `main()`). Apple Clang's ASan/UBSan
runtime does not have this problem on this OS version, but Apple Clang has
no built-in `-fsanitize=fuzzer` runtime, so it is linked separately against
the static libFuzzer runtime that ships with Homebrew LLVM
(`libclang_rt.fuzzer_osx.a`, which supplies `main()` plus the
`__sanitizer_cov_*` callbacks the SanitizerCoverage instrumentation calls
into):

```shell
brew install llvm   # only needed for its libclang_rt.fuzzer_osx.a
```

`CMakeLists.txt` detects the compiler at configure time
(`CMAKE_C_COMPILER_ID STREQUAL "AppleClang"`) and switches automatically:
- Apple Clang → ASan/UBSan + manual SanitizerCoverage flags, linked against
  `LIBFUZZER_LIB` (a CMake cache variable, default
  `/opt/homebrew/opt/llvm/lib/clang/19/lib/darwin/libclang_rt.fuzzer_osx.a`;
  override with `-DLIBFUZZER_LIB=...` if your Homebrew LLVM version differs;
  configure fails with a clear message if the file is missing) plus
  `libc++`.
- Any other Clang on macOS (e.g. Homebrew LLVM used directly as `CC`) → the
  same `-fsanitize=fuzzer,address,undefined` combined build as before, but
  configure prints a `CMake Warning` about the ASan deadlock above; the
  build still proceeds so the hang is directly observable.
- Linux/ClusterFuzzLite (`LIB_FUZZING_ENGINE` set) is untouched either way.

### Preparation

Get a copy of the target's secure SDK (e.g. from a running Ledger dev
container) onto the host filesystem, since CMake/Clang need a local
directory to include from and glob sources out of:

```shell
docker cp <container>:/opt/<sdk-dir> /path/to/a/copy/of/the/Stax/SDK
```

### Compilation

```shell
CC="$(xcrun -f clang)" cmake -S fuzzing -B fuzzing/build \
    -DBOLOS_SDK=<path to a copy of the Stax SDK> -DPLUGIN_SRC_DIR="$PWD/src"

cmake --build fuzzing/build
```

`-DPLUGIN_SRC_DIR` is spelled out because it is a cache variable: a build
directory configured once against another tree keeps it otherwise.

`PLUGIN_SRC_DIR` is a CMake cache variable that defaults to
`fuzzing/../src` (the real plugin sources). It can be overridden to point
the source glob elsewhere, e.g. at a reference/boilerplate snapshot for a
pure toolchain check:

```shell
CC="$(xcrun -f clang)" cmake -S fuzzing -B fuzzing/build \
    -DBOLOS_SDK=<path to a copy of the Stax SDK> \
    -DPLUGIN_SRC_DIR=/path/to/boilerplate-src

cmake --build fuzzing/build
```

Because sources are collected with `file(GLOB_RECURSE)` at configure time,
re-run the `cmake -S ...` configure step whenever `PLUGIN_SRC_DIR` changes
or files are added/removed, before rebuilding.

### Run

```shell
mkdir -p fuzzing/corpus /tmp/fuzz_artifacts
UBSAN_OPTIONS=halt_on_error=1:print_stacktrace=1 \
    fuzzing/build/fuzz -max_total_time=300 -close_fd_mask=1 \
    -artifact_prefix=/tmp/fuzz_artifacts/ fuzzing/corpus/ fuzzing/seeds/

# replay one input (prints the name / version / screen lines)
fuzzing/build/fuzz fuzzing/seeds/ok_2f5a71c1_0.bin
```

### macOS-specific notes

- `_FORTIFY_SOURCE` is on by default on macOS and turns `strlcpy`/`strlcat`
  into `__builtin___*_chk` macros, which breaks `mocks.c`'s own
  `strlcpy`/`strlcat` definitions. The build disables it
  (`-U_FORTIFY_SOURCE -D_FORTIFY_SOURCE=0`) on Apple platforms only.
- `explicit_bzero` does not exist on macOS (no declaration, no symbol in
  libSystem) but is used by the SDK sources. On Apple platforms the build
  force-includes `fuzzing/macos_shim.h`, which provides a
  `static inline explicit_bzero`.
- Both of the above, and the Apple-Clang-vs-Homebrew-LLVM compiler switch,
  are guarded by `if (APPLE)` in `CMakeLists.txt` and do not affect the
  Linux/ClusterFuzzLite build.

## Full usage based on `clusterfuzzlite` container

Same context as CI, using the `clusterfuzzlite` environment directly.
`LIB_FUZZING_ENGINE`/`CFLAGS` come from ClusterFuzzLite (engine and
sanitizers); `CMakeLists.txt` only adds the device ABI (`-fshort-enums
-funsigned-char`), and `.clusterfuzzlite/build.sh` ships `fuzzing/seeds/` as
`$OUT/fuzz_seed_corpus.zip`.

More info: <https://google.github.io/clusterfuzzlite/>

### Preparation

Build the container, then run it to fuzz.

> **Note**: the container holds a copy of the sources (not cloned), so re-run `docker build` after each code change.

```shell
# Prepare directory tree
mkdir fuzzing/{corpus,out}
# Container generation
docker build -t app-plugin-erc8009 --file .clusterfuzzlite/Dockerfile .
```

### Compilation

```shell
docker run --rm --privileged -e FUZZING_LANGUAGE=c -v "$(realpath .)/fuzzing/out:/out" -ti app-plugin-erc8009
```

### Run

```shell
docker run --rm --privileged -e FUZZING_ENGINE=libfuzzer -e RUN_FUZZER_MODE=interactive -v "$(realpath .)/fuzzing/corpus:/tmp/fuzz_corpus" -v "$(realpath .)/fuzzing/out:/out" -ti gcr.io/oss-fuzz-base/base-runner run_fuzzer fuzz
```
