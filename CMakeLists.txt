cmake_minimum_required(VERSION 2.8.11)

project (ottest)

set(CMAKE_CXX_STANDARD 11)

option(USE_Z3 "Solve Sudoku problem using Z3 - an SMT Solver" OFF)

if (USE_Z3)
  list (APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}")
  include(ExternalProject)
  set (Z3_TAG "z3-4.13.0" CACHE STRING "Z3 git tag to use")
  set (Z3_REPO "https://github.com/Z3Prover/z3.git" CACHE STRING "Z3 repo")

  if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    set (Z3_DEBUG "-d")
  else()
    set (Z3_DEBUG "")
  endif()

  ExternalProject_Add(z3
    GIT_REPOSITORY  ${Z3_REPO}
    GIT_TAG ${Z3_TAG}
    BUILD_IN_SOURCE 1
    INSTALL_DIR ${CMAKE_BINARY_DIR}/run
    CONFIGURE_COMMAND env CC=${CMAKE_C_COMPILER} CXX=${CMAKE_CXX_COMPILER}
    ./configure -p <INSTALL_DIR> -b build --staticlib ${Z3_DEBUG}
    BUILD_COMMAND make -j3 -C build
    INSTALL_COMMAND make -C build install
    COMMAND ${CMAKE_COMMAND} -E touch ${CMAKE_CURRENT_LIST_FILE}
    LOG_CONFIGURE 1
    LOG_INSTALL 1
    LOG_BUILD 1)

  find_package(Z3 4.13.0)
  if (NOT Z3_FOUND)
    ExternalProject_Get_Property (z3 INSTALL_DIR)
    set(Z3_ROOT ${INSTALL_DIR} CACHE PATH "Forced location of Z3" FORCE)
    message(WARNING "No Z3 found. Run \n\tcmake --build . && cmake ${CMAKE_SOURCE_DIR}")
    return()
  else()
    set_target_properties(z3 PROPERTIES EXCLUDE_FROM_ALL ON)
    include_directories(${Z3_INCLUDE_DIR})
    message ("Found Z3 at ${Z3_EXECUTABLE}")
    # indicate that we want z3 binary to be included in the binary distribution
    install (PROGRAMS ${Z3_EXECUTABLE} DESTINATION bin)
  endif()


  if (EXISTS ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build)
    file(GLOB z3py
      ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build/*.py?
      ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build/*.py?
      ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build/*.so
      ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build/*.dylib
      ${CMAKE_CURRENT_BINARY_DIR}/z3-prefix/src/z3/build/*.dll
      )
    install(FILES ${z3py} DESTINATION lib/z3py)
  endif()
  add_executable(ottest_z3 sudoku.cpp)
  add_library(sudokuZ3 STATIC sudoku_z3.cpp)
  target_link_libraries(sudokuZ3 ${Z3_LIBRARY})
  target_compile_definitions(sudokuZ3 PRIVATE "USE_Z3")

  target_link_libraries(ottest_z3 ${Z3_LIBRARY} sudokuZ3)
  target_compile_definitions(ottest_z3 PRIVATE "USE_Z3")
  
else()
  add_executable(ottest sudoku.cpp)
  add_library(sudokuUvp STATIC sudoku_uvp.cpp)
  # set_target_properties(sudokuVp PROPERTIES IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR})
  target_link_libraries(ottest sudokuUvp)
endif()

