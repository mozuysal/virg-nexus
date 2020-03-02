# tests/CMakeLists.txt
set(test_SOURCES
  tests_main.cc
  tests_mem_block.cc
  tests_bit_ops.cc
  tests_string.cc
  tests_string_array.cc
  tests_options.cc
  tests_graph.cc
  tests_filter.cc
  tests_svd.cc
  tests_statistics.cc
  tests_sha256.cc
  tests_rotation.cc
  tests_quaternion.cc
  tests_homography.cc
  tests_epipolar.cc
  tests_pinhole.cc
  tests_image.cc
  tests_image_pyr.cc
  tests_fast_detector.cc
  tests_brief_extractor.cc
  tests_data_frame.cc
  tests_lexer.cc
  tests_json_lexer.cc
  tests_csv_lexer.cc
  tests_csv_parser.cc
  tests_gnuplot.cc
  tests_gc.cc
)

if(VIRG_NEXUS_CXX_API)
  list(APPEND test_SOURCES
    tests_options_cxx.cc)
endif(VIRG_NEXUS_CXX_API)

set(VIRG_NEXUS_TEST_DATA_PATH "${CMAKE_CURRENT_SOURCE_DIR}/data")
configure_file(${CMAKE_CURRENT_SOURCE_DIR}/test_data.hh.in ${CMAKE_CURRENT_SOURCE_DIR}/test_data.hh @ONLY)

add_executable(virg-nexus-tests)
target_sources(virg-nexus-tests PRIVATE ${test_SOURCES})
target_include_directories(virg-nexus-tests PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(virg-nexus-tests virg-nexus gtest ${CMAKE_THREAD_LIBS_INIT} ${EXEC_LIBRARIES})
set_target_properties(virg-nexus-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_custom_target(unit-tests
  COMMAND ./virg-nexus-tests --gtest_death_test_style=threadsafe `printenv NX_TEST_FLAGS`
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  DEPENDS virg-nexus-tests
  COMMENT "Unit Testing")

set(valgrind_flags --leak-check=full --track-origins=yes --show-reachable=yes --suppressions=${CMAKE_SOURCE_DIR}/valgrind_nexus.supp)
set(valgrind_nexus_test_flags --valgrind --gtest_death_test_style=threadsafe --gtest_filter=-*DEATH `printenv NX_TEST_FLAGS`)
add_custom_target(unit-tests-valgrind
  COMMAND valgrind ${valgrind_flags} ./virg-nexus-tests ${valgrind_nexus_test_flags}
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/bin
  DEPENDS virg-nexus-tests
  COMMENT "Unit Testing with Valgrind")
