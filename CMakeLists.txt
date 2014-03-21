cmake_minimum_required(VERSION 2.8)

project(virg-nexus C CXX)

set(VERSION 1.0)

option(VIRG_NEXUS_ANDROID "Enable Android Compilation"  OFF)
option(VIRG_NEXUS_PC_LINUX "Enable Linux Compilation"  ON)
option(VIRG_NEXUS_PC_WIN32 "Enable Win32 Compilation"  OFF)

set(virg_nexus_SOURCES
  src/nx_alloc.c
  src/nx_mem_block.c
  src/SFMT/SFMT.c
  src/nx_uniform_sampler.c
  src/nx_math.c
  src/nx_spline.c
  src/nx_transform_2d.c
  src/nx_io.c
  src/nx_filter.c
  src/nx_image.c
  src/nx_image_warp.c
  src/nx_image_pyr.c
  src/nx_image_pyr_builder.c
  src/nx_float_image.c
  src/nx_colorspace.c
  src/fast/fast_9.c
  src/fast/fast_nonmax.c
  src/nx_fast_detector.c
  src/nx_brief_extractor.c
)

set(virg_nexus_HEADERS
  include/virg/nexus/nx_config.h
  include/virg/nexus/nx_types.h
  include/virg/nexus/nx_assert.h
  include/virg/nexus/nx_message.h
  include/virg/nexus/nx_alloc.h
  include/virg/nexus/nx_mem_block.h
  include/virg/nexus/nx_uniform_sampler.h
  include/virg/nexus/nx_math.h
  include/virg/nexus/nx_spline.h
  include/virg/nexus/nx_transform_2d.h
  include/virg/nexus/nx_io.h
  include/virg/nexus/nx_filter.h
  include/virg/nexus/nx_image.h
  include/virg/nexus/nx_image_warp.h
  include/virg/nexus/nx_image_pyr.h
  include/virg/nexus/nx_image_pyr_builder.h
  include/virg/nexus/nx_float_image.h
  include/virg/nexus/nx_colorspace.h
  include/virg/nexus/nx_keypoint.h
  include/virg/nexus/nx_fast_detector.h
  include/virg/nexus/nx_brief_extractor.h
)

if (NOT VIRG_NEXUS_ANDROID)
  set(virg_nexus_SOURCES ${virg_nexus_SOURCES} src/nx_message_c_impl.c)
endif()

add_library(virg-nexus STATIC ${virg_nexus_SOURCES})
set_target_properties(virg-nexus PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(virg-nexus-shared SHARED ${virg_nexus_SOURCES})
set_target_properties(virg-nexus-shared PROPERTIES OUTPUT_NAME virg-nexus)
set_target_properties(virg-nexus-shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} )
include_directories(include)

# --------------------------- configuration ----------------------------
set(CMAKE_INSTALL_PREFIX "$ENV{DEVEL_DIR}")

option(VIRG_NEXUS_USE_SIMD "Enable SIMD flags"  ON)

# Platform Specific Flags
# PC-Linux
if (VIRG_NEXUS_PC_LINUX)
  set(VIRG_NEXUS_FLAGS_PLATFORM "-DVIRG_NEXUS_PC -DVIRG_NEXUS_PC_LINUX")
  # SSE
  if (VIRG_NEXUS_USE_SIMD)
    set(VIRG_NEXUS_USE_SSE 1)
    set(VIRG_NEXUS_FLAGS_SIMD "-DVIRG_NEXUS_USE_SIMD -msse -mfpmath=sse -DVIRG_NEXUS_USE_SSE")
  endif (VIRG_NEXUS_USE_SIMD)
endif(VIRG_NEXUS_PC_LINUX)

# Android
if (VIRG_NEXUS_ANDROID)
  set(VIRG_NEXUS_FLAGS_PLATFORM "-DVIRG_NEXUS_ANDROID")
  # NEON
  if (VIRG_NEXUS_USE_SIMD)
    set(VIRG_NEXUS_USE_NEON 1)
    set(VIRG_NEXUS_FLAGS_SIMD "-DVIRG_NEXUS_USE_SIMD -DVIRG_NEXUS_USE_NEON")
  endif (VIRG_NEXUS_USE_SIMD)
endif(VIRG_NEXUS_ANDROID)

set(VIRG_NEXUS_FLAGS_DEBUG          "-Wall -pedantic -g -rdynamic ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELEASE        "-Wall -pedantic -O2 -DNDEBUG ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELWITHDEBINFO "-Wall -pedantic -O2 -DNDEBUG -g -rdynamic ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")

set(CMAKE_C_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG} -std=c99")
set(CMAKE_C_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE} -std=c99")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO} -std=c99")

set(CMAKE_CXX_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG} -std=c++0x")
set(CMAKE_CXX_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE} -std=c++0x")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO} -std=c++0x")

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

configure_file(virg-nexus.pc.in ${CMAKE_SOURCE_DIR}/lib/pkgconfig/virg-nexus.pc @ONLY)

# -------------------------- install target ----------------------------
install(TARGETS virg-nexus ARCHIVE DESTINATION lib)
install(TARGETS virg-nexus-shared LIBRARY DESTINATION lib)
foreach(virg_nexus_header ${virg_nexus_HEADERS})
  get_filename_component(install_RELPATH ${virg_nexus_header} PATH)
  install(FILES ${virg_nexus_header} DESTINATION ${install_RELPATH})
endforeach(virg_nexus_header)
install(FILES lib/pkgconfig/virg-nexus.pc DESTINATION lib/pkgconfig)
install(DIRECTORY lib/python DESTINATION lib FILES_MATCHING PATTERN "*.py")

# --------------------------------- doc target -----------------------------------
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)


# --------------------------------- cblas/lapack ----------------------------------
option(VIRG_NEXUS_USE_TUNED_LAPACK "Use ATLAS generated cblas/lapack version" OFF)

set (LAPACK_LIBRARY_DIRS "")
set (LAPACK_LIBRARIES "-llapack -lcblas")
set (LAPACK_INCLUDE_DIR "")

if (VIRG_NEXUS_USE_TUNED_LAPACK)
  set (LAPACK_INCLUDE_DIR "")
  set (LAPACK_LIBRARY_DIRS "")
  set (LAPACK_LIBRARIES "-llapack -lcblas -latlas")
endif (VIRG_NEXUS_USE_TUNED_LAPACK)

include_directories(${LAPACK_INCLUDE_DIR})
link_directories(${LAPACK_LIBRARY_DIRS})

# --------------------------------- Goggle Test ----------------------------------
if (CMAKE_USE_PTHREADS_INIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_PTHREAD=1")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_PTHREAD=0")
endif(CMAKE_USE_PTHREADS_INIT)
set(GTEST_DIR ext/gtest-1.6.0)
set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR})
include_directories(${GTEST_INCLUDE_DIRECTORIES})
add_library(gtest ${GTEST_DIR}/src/gtest-all.cc)
set_target_properties(gtest PROPERTIES ARCHIVE_OUTPUT_DIRECTORY lib)

# ------------------------------- tests --------------------------------
set(test_SOURCES
  tests/tests_main.cc
  tests/tests_mem_block.cc
  tests/tests_filter.cc
  tests/tests_image.cc
  tests/tests_image_pyr.cc
  tests/tests_float_image.cc
  tests/tests_fast_detector.cc
  tests/tests_brief_extractor.cc
)

set(VIRG_NEXUS_TEST_DATA_PATH "${CMAKE_SOURCE_DIR}/tests/data")
configure_file(${CMAKE_SOURCE_DIR}/tests/test_data.hh.in ${CMAKE_SOURCE_DIR}/tests/test_data.hh @ONLY)
include_directories(${CMAKE_SOURCE_DIR}/tests)

add_executable(virg-nexus-tests ${test_SOURCES})
target_link_libraries(virg-nexus-tests virg-nexus gtest ${CMAKE_THREAD_LIBS_INIT} ${EXEC_LIBRARIES})
set_target_properties(virg-nexus-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

add_custom_target(unit-tests bin/virg-nexus-tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing" VERBATIM)

add_custom_target(unit-tests-valgrind valgrind --leak-check=full --track-origins=yes --show-reachable=yes bin/virg-nexus-tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing with Valgrind" VERBATIM)

