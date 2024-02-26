find_package(Python3)
if(Python3_FOUND)
  message(STATUS "Found python at ${Python3_EXECUTABLE}")
endif()

set(LLVM_LIT_OUTPUT_DIR ${CMAKE_BINARY_DIR}/llvm/bin)

configure_lit_site_cfg(
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.site.cfg.py.in
  ${CMAKE_CURRENT_BINARY_DIR}/lit.site.cfg.py
  MAIN_CONFIG
  ${CMAKE_CURRENT_SOURCE_DIR}/lit.cfg.py
)

if (NOT pybind11_DIR)
  find_package(pybind11 CONFIG REQUIRED)
else()
  # TODO: This is a workaround.
  file(COPY "${pybind11_DIR}/CMakeFiles/Export/share/cmake/pybind11/pybind11Targets.cmake" DESTINATION "${pybind11_DIR}/pybind11Targets.cmake")
  find_package(pybind11 CONFIG REQUIRED PATHS ${pybind11_DIR})
endif()

message(STATUS "Found pybind11 v${pybind11_VERSION}: ${pybind11_INCLUDE_DIRS} and ${pybind11_INCLUDE_DIR}")

set(BINDER_TEST_DEPENDS
    binder
    FileCheck 
    count
    not
)

add_lit_testsuite(check-binder "Running binder tests"
  ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS ${BINDER_TEST_DEPENDS}
)
set_target_properties(check-binder PROPERTIES FOLDER "Tests")
