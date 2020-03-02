# bench/ransac/CMakeLists.txt
execute_process(COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  OUTPUT_STRIP_TRAILING_WHITESPACE
  OUTPUT_VARIABLE git_code)

add_custom_target(benchmark-ransac
  COMMAND ./run.sh | tee "out/${git_code}.csv"
  DEPENDS nx-fit-homography
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Benchmark RANSAC")
