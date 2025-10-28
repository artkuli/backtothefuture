set(TEST_SOURCES

)

add_executable(tests ${TEST_SOURCES}
  hasher_test.cpp
  writer_test.cpp
  reader_test.cpp
  scan_test.cpp
  test_utils.hpp
  test_utils.cpp
)
target_link_libraries(tests
  PRIVATE
    core
    GTest::gtest_main
)

# Ostrzeżenia (opcjonalnie)
if (MSVC)
  target_compile_options(tests PRIVATE /W4)
else()
  target_compile_options(tests PRIVATE -Wall -Wextra -Wpedantic)
endif()

include(GoogleTest)
gtest_discover_tests(tests)
