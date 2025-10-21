cmake_minimum_required(VERSION 3.16)
project(backtothefuture LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

option(BUILD_TESTS "Build tests" ON)

set(HEADERS
  include/pack.hpp
  include/unpack.hpp
  include/types.hpp
  include/scan.hpp
)

add_library(core
  src/pack.cpp
  src/unpack.cpp
  src/scan.cpp
)
target_sources(core
  PUBLIC
    FILE_SET HEADERS
      BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR}/include
      FILES ${HEADERS}
)

add_executable(backtothefuture src/main.cpp)
target_link_libraries(backtothefuture PRIVATE core)

if (BUILD_TESTS)
  enable_testing()
  include(FetchContent)
  
  FetchContent_Declare(
    googletest
    # URL https://github.com/google/googletest/archive/refs/tags/v1.14.0.zip
    GIT_REPOSITORY https://github.com/google/googletest.git
    GIT_TAG v1.14.0
  )
  if (MSVC)
    set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
  endif()

  FetchContent_MakeAvailable(googletest)

  add_subdirectory(test)
endif()
