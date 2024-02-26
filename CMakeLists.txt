# This file can be used to build `binder` using preinstalled LLVM/Clang
# from llvm-project git submodule.
# TODO: Handle out-of-tree llvm build.

cmake_minimum_required(VERSION 3.13.4)

project(binder CXX C)
set(VERSION 1.4.1)

option(STATIC "Statically compile Binder. See `documentation/install.rst` for more information." OFF)
option(BINDER_ENABLE_TEST      "Enables building of tests" ON)
option(BINDER_MOCK_TEST        "Mock running of binder on tests" OFF)
option(BINDER_USE_PYTHON_IN_TEST  "Try to import python modules during tests" ON)

set(CMAKE_VERBOSE_MAKEFILE ON)

set(BINDER_ROOT_SRC_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(BINDER_SOURCE_DIR ${BINDER_ROOT_SRC_DIR}/source)
set(LLVM_ROOT_DIR ${BINDER_SOURCE_DIR}/llvm-project)

include("GNUInstallDirs")

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake/Modules ${CMAKE_MODULE_PATH})

# Find clang.
if (NOT CLANG_DIR)
  message(FATAL_ERROR "CLANG_DIR must be specified in order to find AddClang")
endif()

find_package(Clang REQUIRED CONFIG PATHS ${CLANG_DIR} NO_DEFAULT_PATH)
message(STATUS "Using ClangConfig.cmake in: ${CLANG_DIR}")

message(STATUS  "binder: BINDER_USE_PYTHON_IN_TEST=${BINDER_USE_PYTHON_IN_TEST}")
message(STATUS  "binder: Using LLVM_DIR=${LLVM_DIR} to search for cmake configurations.")
message(STATUS  "binder: Using Clang_DIR=${CLANG_DIR} to search for cmake configurations.")
message(STATUS  "binder: Using CMAKE_MODULE_PATH=${CMAKE_MODULE_PATH} to search for cmake configurations.")

if (Clang_FOUND AND LLVM_FOUND AND NOT LLVMCONFIG )
  set(CMAKE_MODULE_PATH "${LLVM_CMAKE_DIR};${CMAKE_MODULE_PATH}")
  message(STATUS  "binder: cmake configurations llvm and clang were found. LLVM_CMAKE_DIR=${LLVM_CMAKE_DIR}")

  include(AddLLVM)
  include(LLVMConfig)

  message(STATUS  "binder: LLVM_LIBRARY_DIR=${LLVM_LIBRARY_DIR}")
  message(STATUS  "binder: LLVM_VERSION=${LLVM_VERSION}")

  # Copy clang built-in headers.
  file(COPY "${LLVM_BINARY_DIR}/lib/clang/${LLVM_VERSION_MAJOR}/include" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}/lib/binder")

  # Set up LLVM include paths. Required by gtest.
  include_directories(
    ${LLVM_INCLUDE_DIRS}
  )

  include_directories(
    ${LLVM_INCLUDE_DIRS}
    ${CLANG_INCLUDE_DIRS}
    ${BINDER_SOURCE_DIR}/include
    ${CMAKE_BINARY_DIR}/include
  )

  include_directories( ${CLANG_INCLUDE_DIRS} )
  message(STATUS  "binder: CLANG_INCLUDE_DIRS=${CLANG_INCLUDE_DIRS}")
else()
  message(STATUS  "binder: llvm or clang cmake configurations were not found.")
  message(STATUS  "binder: CLang_FOUND=${Clang_FOUND}")
  message(STATUS  "binder: LLVM_FOUND=${LLVM_FOUND}")
  message(STATUS  "binder: Trying to look for llvm-config.")

  set(LLVM_DIR ${LLVM_DIR_ORIG})
  set(Clang_DIR ${Clang_DIR_ORIG})
  set(CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH_ORIG}")

  message(STATUS  "binder: Restored original LLVM_DIR=${LLVM_DIR}")
  message(STATUS  "binder: Restored original Clang_DIR=${Clang_DIR}")
  if (LLVMCONFIG)
    message(STATUS  "binder: The explicit location of  llvm-config is set to LLVMCONFIG=${LLVMCONFIG}")
    if (EXISTS ${LLVMCONFIG})
      set (LLVMCONFIGEXE ${LLVMCONFIG})
    else()
      message(FATAL_ERROR  "binder: File ${LLVMCONFIG} does not exist")
    endif()
  else()
    message(STATUS  "binder: Searching for llvm-config in ${LLVM_DIR}/bin $ENV{PATH} /usr/bin/ /usr/local/bin")
    find_program(LLVMCONFIGEXE NAMES llvm-config HINTS  ${LLVM_DIR}/bin $ENV{PATH} /usr/bin/ /usr/local/bin)
  endif()

  if (LLVMCONFIGEXE)
    message(STATUS  "binder: llvm-config found in ${LLVMCONFIGEXE}")
    execute_process (COMMAND ${LLVMCONFIGEXE}  --version     OUTPUT_VARIABLE  LLVMVERSION OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND ${LLVMCONFIGEXE}  --libdir      OUTPUT_VARIABLE  LLVMLIBDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND ${LLVMCONFIGEXE}  --includedir  OUTPUT_VARIABLE  LLVMINCLUDEDIR OUTPUT_STRIP_TRAILING_WHITESPACE)
    execute_process (COMMAND ${LLVMCONFIGEXE}  --cxxflags    OUTPUT_VARIABLE  LLVMCXXFLAGS OUTPUT_STRIP_TRAILING_WHITESPACE)

    set(LLVM_COMPILE_FLAGS ${LLVMCXXFLAGS})
    include_directories( ${LLVMINCLUDEDIR})
    include_directories( ${CLANG_INCLUDE_DIRS} )

    link_directories(${LLVMLIBDIR} )
    string(REPLACE "." ";" VERSION_LIST ${LLVMVERSION})

    list(GET VERSION_LIST 0 LLVM_VERSION_MAJOR)
    list(GET VERSION_LIST 1 LLVM_VERSION_MINOR)
    list(GET VERSION_LIST 2 LLVM_VERSION_PATCH)

    message(STATUS  "binder: llvm version from ${LLVMCONFIGEXE} is ${LLVMVERSION}")
    message(STATUS  "binder: llvm library dir from ${LLVMCONFIGEXE} is ${LLVMLIBDIR}")
    message(STATUS  "binder: llvm includedir dir from ${LLVMCONFIGEXE} is ${LLVMINCLUDEDIR}")
  else()
    message(FATAL_ERROR  "binder: llvm-config not found")
  endif()
endif()

message(STATUS  "binder: LLVM_VERSION_MAJOR=${LLVM_VERSION_MAJOR}")
message(STATUS  "binder: LLVM_VERSION_MINOR=${LLVM_VERSION_MINOR}")
message(STATUS  "binder: LLVM_VERSION_PATCH=${LLVM_VERSION_PATCH}")

macro(add_clang_executable name)
  add_executable( ${name} ${ARGN} )
    message(STATUS "binder: We have cmake functions to update LLVM_COMPILE_FLAGS")
    if (LLVM_COMPILER_IS_GCC_COMPATIBLE)
      list(APPEND LLVM_COMPILE_FLAGS "-fno-rtti")
    elseif (MSVC)
      list(APPEND LLVM_COMPILE_FLAGS "/GR-")
    endif ()
  set_target_properties(${name} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")
endmacro(add_clang_executable)

if( ${LLVM_VERSION_MAJOR} GREATER_EQUAL 16 )
  # LLVM 16 introduces the use of C++ features from C++17.
  set(CMAKE_CXX_STANDARD 17)
else()
  set(CMAKE_CXX_STANDARD 14)
endif()

include_directories(source)
add_subdirectory(source)

if (BINDER_ENABLE_TEST)
 if(${CMAKE_VERSION} VERSION_LESS "3.0.0")
   message("You are running cmake version ${CMAKE_VERSION}.")
   message("The testing suite will be disabled as it requires cmake 3.0.0 or higher.")
 else()
   ENABLE_TESTING()
   message(STATUS "binder: The tests enabled.")
   add_subdirectory(test)
 endif()
endif()
