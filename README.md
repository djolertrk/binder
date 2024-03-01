# Binder (fork)

**Binder** is a tool for automatic generation of Python bindings for C++11 projects using [Pybind11](https://github.com/pybind/pybind11) and [Clang LibTooling](http://clang.llvm.org/docs/LibTooling.html) libraries.  That is, Binder, takes a C++ project and compiles it into objects and functions that are all usable within Python.  Binder is different from prior tools in that it handles special features new in C++11.

[![Documentation Status](https://readthedocs.org/projects/cppbinder/badge/?version=latest)](http://cppbinder.readthedocs.org/en/latest/?badge=latest)
![](https://github.com/RosettaCommons/binder/workflows/build/badge.svg)

Reference documentation is provided at
[http://cppbinder.readthedocs.org/en/latest](http://cppbinder.readthedocs.org/en/latest).
A PDF version of the manual is available
[here](https://media.readthedocs.org/pdf/cppbinder/latest/cppbinder.pdf).

## How to build && run this fork?

### Clone

```
$ git clone --recursive https://github.com/djolertrk/binder
```

### Build process (on Linux, X86)

#### Build LLVM/Clang

```
$ mkdir build_llvm && cd build_llvm
$ cmake -G "Ninja" ../binder/llvm-project/llvm \
    -DCMAKE_BUILD_TYPE=Release \
    -DLLVM_TARGETS_TO_BUILD="X86" \
    -DLLVM_ENABLE_EH=1 \
    -DLLVM_ENABLE_ASSERTIONS=ON \
    -DLLVM_ENABLE_RTTI=ON \
    -DLLVM_ENABLE_PROJECTS="clang"
$ ninja -j3
$ cd ..
```

#### Build pybind11

```
$ mkdir build_pybind11 && cd build_pybind11
$ cmake ../binder/pybind11/
$ make -j4
```

#### Build binder

```
$ mkdir build && cd build
$ cmake -G "Ninja" ../binder -DCMAKE_MODULE_PATH="$PWD/../build_llvm/" -DCLANG_DIR="$PWD/../build_llvm/lib/cmake/clang" -Dpybind11_DIR="$PWD/../build_pybind11/" -DBINDER_ENABLE_TEST=OFF
$ ninja binder
```

### Run binder

An example:

```
$ bin/binder --v --trace --include-pybind11-stl --root-module simple_project --config simple_project.config --prefix output all_headers.hpp -- -DNDEBUG -I. -isystem ${path_to_binder_build}/build/lib/binder/include
```
