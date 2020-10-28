# 2020_OCT - ONSC Edit

# ledger
Simple doubly-entry accounting program

## Features

The `ledger_cli` program can process transactions and do basic
manipulation of the ledger books it generates. Note that this program
_does not claim to be aware of any accounting standards!_ The user is
responsible for ensuring that transactions comply with such standards
if they apply.

## Build

This project uses the CMake build system, which one can obtain at
[https://cmake.org/download/](https://cmake.org/download/).

After cloning the repository and initializing submodules,
```bash
git clone https://github.com/codylico/ledger.git
git submodule update --init --recursive
```

the user should make and enter a `build` directory,
```bash
mkdir build
cd build
```

then run CMake from that build directory.
```bash
cmake path/to/source/of/ledger
```

CMake should generate an IDE project or set of build files.
If using a Makefile-based build configuration, then run make.
```bash
make
```

The `ledger_cli` program should be built by default.

### Notes

Because of a change in submodules, users may need to synchronize and update the
submodule configuration of any of their active clones of this repository.
```bash
git submodule sync
git submodule update
```

## License

The source code is available under the MIT license.
