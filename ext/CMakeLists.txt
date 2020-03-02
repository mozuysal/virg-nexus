# ext/CMakeLists.txt
# --------------------------------- Goggle Test ----------------------------------
set(GTEST_DIR googletest/googletest)

add_library(gtest)
target_sources(gtest PRIVATE ${GTEST_DIR}/src/gtest-all.cc)
target_include_directories(gtest PRIVATE ${GTEST_DIR} PUBLIC ${GTEST_DIR}/include)
if (CMAKE_USE_PTHREADS_INIT)
  target_compile_definitions(gtest PUBLIC "-DGTEST_HAS_PTHREAD=1")
else()
  target_compile_definitions(gtest PUBLIC "-DGTEST_HAS_PTHREAD=0")
endif(CMAKE_USE_PTHREADS_INIT)

add_library(virg::gtest ALIAS gtest)
