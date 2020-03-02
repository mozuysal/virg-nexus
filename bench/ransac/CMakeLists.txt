# bench/ransac/CMakeLists.txt
add_custom_target(benchmark-ransac
  COMMAND ./run.sh | tee "out/$(git rev-parse HEAD).csv"
  DEPENDS nx-fit-homography
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Benchmark RANSAC")
