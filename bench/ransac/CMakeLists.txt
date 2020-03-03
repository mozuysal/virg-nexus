# bench/ransac/CMakeLists.txt
add_custom_target(benchmark-ransac
  COMMAND ./run.sh
  DEPENDS nx-fit-homography
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Benchmark RANSAC")
