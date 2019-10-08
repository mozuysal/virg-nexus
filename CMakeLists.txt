cmake_minimum_required(VERSION 2.8)

project(virg-nexus C CXX)

set(VERSION 0.1)

option(VIRG_NEXUS_PC_LINUX "Enable Linux Compilation"  ON)

set(virg_nexus_C_SOURCES
  src/nx_alloc.c
  src/nx_mem_block.c
  src/SFMT/SFMT.c
  src/nx_uniform_sampler.c
  src/nx_bit_ops.c
  src/nx_math.c
  src/nx_rotation_3d.c
  src/nx_svd.c
  src/nx_string.c
  src/nx_transform_2d.c
  src/nx_io.c
  src/nx_filesystem.c
  src/nx_timing.c
  src/nx_string_array.c
  src/nx_options.c
  src/nx_lexer.c
  src/nx_json_lexer.c
  src/nx_json_parser.c
  src/nx_json_tree.c
  src/nx_json_bundle.c
  src/nx_json_log.c
  src/nx_plotter.c
  src/nx_gc_cairo_imp.c
  src/nx_filter.c
  src/nx_image.c
  src/nx_image_io_pnm.c
  src/nx_image_io_jpeg.c
  src/nx_image_io_png.c
  src/nx_image_io.c
  src/nx_image_warp.c
  src/nx_affine_warp_processor.c
  src/nx_homography.c
  src/nx_epipolar.c
  src/nx_image_pyr.c
  src/nx_image_pyr_builder.c
  src/nx_colorspace.c
  src/nx_keypoint.c
  src/fast/fast_9.c
  src/fast/fast_nonmax.c
  src/nx_fast_detector.c
  src/nx_harris_detector.c
  src/nx_checkerboard_detector.c
  src/nx_keypoint_vector.c
  src/nx_brief_extractor.c
  src/nx_point_match_2d_stats.c
  src/nx_data_frame.c
  src/nx_csv_lexer.c
  src/nx_csv_parser.c
  src/nx_log_c_impl.c
  src/nx_pinhole.c
  src/nx_kitti_slam.c
)

set(virg_nexus_C_HEADERS
  include/virg/nexus/nx_config.h
  include/virg/nexus/nx_types.h
  include/virg/nexus/nx_assert.h
  include/virg/nexus/nx_log.h
  include/virg/nexus/nx_alloc.h
  include/virg/nexus/nx_mem_block.h
  include/virg/nexus/nx_uniform_sampler.h
  include/virg/nexus/nx_bit_ops.h
  include/virg/nexus/nx_math.h
  include/virg/nexus/nx_rotation_3d.h
  include/virg/nexus/nx_svd.h
  include/virg/nexus/nx_vec234.h
  include/virg/nexus/nx_mat234.h
  include/virg/nexus/nx_string.h
  include/virg/nexus/nx_vector_gen.h
  include/virg/nexus/nx_string_array.h
  include/virg/nexus/nx_transform_2d.h
  include/virg/nexus/nx_io.h
  include/virg/nexus/nx_filesystem.h
  include/virg/nexus/nx_timing.h
  include/virg/nexus/nx_options.h
  include/virg/nexus/nx_lexer.h
  include/virg/nexus/nx_json_lexer.h
  include/virg/nexus/nx_json_parser.h
  include/virg/nexus/nx_json_tree.h
  include/virg/nexus/nx_json_bundle.h
  include/virg/nexus/nx_json_log.h
  include/virg/nexus/nx_plotter.h
  include/virg/nexus/nx_gc.h
  include/virg/nexus/nx_filter.h
  include/virg/nexus/nx_image.h
  include/virg/nexus/nx_image_io.h
  include/virg/nexus/nx_image_io_params.h
  include/virg/nexus/nx_image_io_pnm.h
  include/virg/nexus/nx_image_io_jpeg.h
  include/virg/nexus/nx_image_io_png.h
  include/virg/nexus/nx_image_warp.h
  include/virg/nexus/nx_affine_warp_processor.h
  include/virg/nexus/nx_homography.h
  include/virg/nexus/nx_epipolar.h
  include/virg/nexus/nx_image_pyr.h
  include/virg/nexus/nx_image_pyr_builder.h
  include/virg/nexus/nx_colorspace.h
  include/virg/nexus/nx_keypoint.h
  include/virg/nexus/nx_keypoint_vector.h
  include/virg/nexus/nx_fast_detector.h
  include/virg/nexus/nx_harris_detector.h
  include/virg/nexus/nx_checkerboard_detector.h
  include/virg/nexus/nx_brief_extractor.h
  include/virg/nexus/nx_point_match_2d.h
  include/virg/nexus/nx_point_match_2d_stats.h
  include/virg/nexus/nx_data_frame.h
  include/virg/nexus/nx_csv_lexer.h
  include/virg/nexus/nx_csv_parser.h
  include/virg/nexus/nx_pinhole.h
  include/virg/nexus/nx_kitti_slam.h
)

set(virg_nexus_CXX_SOURCES
  src/vg_options.cc
  src/vg_image.cc
  src/vg_image_pyr.cc
  src/vg_harris_detector.cc
  src/vg_brief_extractor.cc
  src/vg_descriptor_map.cc
  src/vg_point_correspondence_2d.cc
  src/vg_homography.cc
  src/vg_fundamental_matrix.cc
  src/vg_graphics_context.cc
  src/vg_image_annotator.cc
)

set(virg_nexus_CXX_HEADERS
  include/virg/nexus/vg_options.hpp
  include/virg/nexus/vg_image.hpp
  include/virg/nexus/vg_image_pyr.hpp
  include/virg/nexus/vg_harris_detector.hpp
  include/virg/nexus/vg_brief_extractor.hpp
  include/virg/nexus/vg_descriptor_map.hpp
  include/virg/nexus/vg_point_correspondence_2d.hpp
  include/virg/nexus/vg_homography.hpp
  include/virg/nexus/vg_fundamental_matrix.hpp
  include/virg/nexus/vg_color.hpp
  include/virg/nexus/vg_color_map.hpp
  include/virg/nexus/vg_graphics_context.hpp
  include/virg/nexus/vg_image_annotator.hpp
)

add_library(virg-nexus STATIC ${virg_nexus_C_SOURCES} ${virg_nexus_CXX_SOURCES})
set_target_properties(virg-nexus PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(virg-nexus-shared SHARED ${virg_nexus_C_SOURCES} ${virg_nexus_CXX_SOURCES})
target_link_libraries(virg-nexus-shared -lm -llapack)
set_target_properties(virg-nexus-shared PROPERTIES OUTPUT_NAME virg-nexus)
set_target_properties(virg-nexus-shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)


# --------------------------- configuration ----------------------------
set(CMAKE_INSTALL_PREFIX "$ENV{DEVEL_DIR}")
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} )
include_directories(include)

option(VIRG_NEXUS_USE_SIMD "Enable SIMD flags"  ON)

# Platform Specific Flags
# PC-Linux
if (VIRG_NEXUS_PC_LINUX)
  set(VIRG_NEXUS_FLAGS_PLATFORM "-DVIRG_NEXUS_PC -DVIRG_NEXUS_PC_LINUX")
  # SSE
  if (VIRG_NEXUS_USE_SIMD)
    set(VIRG_NEXUS_USE_SSE 1)
    set(VIRG_NEXUS_FLAGS_SIMD "-DVIRG_NEXUS_USE_SIMD -msse -msse2 -mfpmath=sse -DVIRG_NEXUS_USE_SSE -DHAVE_SSE2")
  endif (VIRG_NEXUS_USE_SIMD)
endif(VIRG_NEXUS_PC_LINUX)

set(VIRG_NEXUS_FLAGS_DEBUG          "-Wall -g -rdynamic -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELEASE        "-Wall -O2 -DNDEBUG -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELWITHDEBINFO "-Wall -O2 -DNDEBUG -g -rdynamic -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_PLATFORM} ${VIRG_NEXUS_FLAGS_SIMD}")

set(CMAKE_C_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG} -std=gnu99")
set(CMAKE_C_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE} -std=gnu99")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO} -std=gnu99")

set(CMAKE_CXX_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG} -std=c++11")
set(CMAKE_CXX_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE} -std=c++11")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO} -std=c++11")

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

# -------------------------- install target ----------------------------
install(TARGETS virg-nexus ARCHIVE DESTINATION lib)
install(TARGETS virg-nexus-shared LIBRARY DESTINATION lib)
foreach(virg_nexus_header ${virg_nexus_C_HEADERS} ${virg_nexus_CXX_HEADERS})
  get_filename_component(install_RELPATH ${virg_nexus_header} PATH)
  install(FILES ${virg_nexus_header} DESTINATION ${install_RELPATH})
endforeach(virg_nexus_header)
install(FILES lib/pkgconfig/virg-nexus.pc DESTINATION lib/pkgconfig)

# --------------------------------- doc target -----------------------------------
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  SET(DOXYGEN_INPUT ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile)
  SET(DOXYGEN_OUTPUT ${CMAKE_CURRENT_SOURCE_DIR}/doc/html/index.html)

  add_custom_command(
    OUTPUT ${DOXYGEN_OUTPUT}
    COMMAND ${CMAKE_COMMAND} -E echo_append "Generating API documentation with Doxygen"
    COMMAND ${DOXYGEN_EXECUTABLE} ${DOXYGEN_INPUT}
    COMMAND ${CMAKE_COMMAND} -E echo "Done."
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    DEPENDS ${DOXYGEN_INPUT} virg-nexus
    COMMENT "Generating API documentation with Doxygen" VERBATIM)

  add_custom_target(doc ALL DEPENDS ${DOXYGEN_OUTPUT})
endif(DOXYGEN_FOUND)
# ---------------------------------- jpeg/png ------------------------------------
include(FindPNG)
include(FindJPEG)

include_directories(${PNG_INCLUDE_DIR})
include_directories(${JPEG_INCLUDE_DIR})

link_directories(${PNG_LIBRARY_DIR})
link_directories(${JPEG_LIBRARY_DIR})

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

# ------------------------------------ Cairo -------------------------------------
include(FindPkgConfig)

pkg_check_modules(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})
# -------------------------------- Dependencies ----------------------------------
set(EXEC_LIBRARIES ${PNG_LIBRARIES} ${JPEG_LIBRARIES} ${LAPACK_LIBRARIES} ${CAIRO_LDFLAGS} -lm)
foreach(LIB ${EXEC_LIBRARIES})
  set(MODULE_LIBS "${MODULE_LIBS} ${LIB}")
endforeach()
configure_file(virg-nexus.pc.in ${CMAKE_SOURCE_DIR}/lib/pkgconfig/virg-nexus.pc @ONLY)

# --------------------------------- Goggle Test ----------------------------------
if (CMAKE_USE_PTHREADS_INIT)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_PTHREAD=1")
else()
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -DGTEST_HAS_PTHREAD=0")
endif(CMAKE_USE_PTHREADS_INIT)
set(GTEST_DIR ext/googletest/googletest)
set(GTEST_INCLUDE_DIRECTORIES ${GTEST_DIR}/include ${GTEST_DIR})
include_directories(${GTEST_INCLUDE_DIRECTORIES})
add_library(gtest ${GTEST_DIR}/src/gtest-all.cc)
set_target_properties(gtest PROPERTIES ARCHIVE_OUTPUT_DIRECTORY lib)

# ------------------------------- tests --------------------------------
set(test_SOURCES
  tests/tests_main.cc
  tests/tests_mem_block.cc
  tests/tests_bit_ops.cc
  tests/tests_string.cc
  tests/tests_string_array.cc
  tests/tests_options.cc
  tests/tests_filter.cc
  tests/tests_svd.cc
  tests/tests_rotation.cc
  tests/tests_homography.cc
  tests/tests_epipolar.cc
  tests/tests_pinhole.cc
  tests/tests_image.cc
  tests/tests_image_pyr.cc
  tests/tests_fast_detector.cc
  tests/tests_brief_extractor.cc
  tests/tests_data_frame.cc
  tests/tests_lexer.cc
  tests/tests_json_lexer.cc
  tests/tests_csv_lexer.cc
  tests/tests_csv_parser.cc
  tests/tests_gc.cc
)

set(VIRG_NEXUS_TEST_DATA_PATH "${CMAKE_SOURCE_DIR}/tests/data")
configure_file(${CMAKE_SOURCE_DIR}/tests/test_data.hh.in ${CMAKE_SOURCE_DIR}/tests/test_data.hh @ONLY)
include_directories(${CMAKE_SOURCE_DIR}/tests)

add_executable(virg-nexus-tests ${test_SOURCES})
target_link_libraries(virg-nexus-tests virg-nexus gtest ${CMAKE_THREAD_LIBS_INIT} ${EXEC_LIBRARIES})
set_target_properties(virg-nexus-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

add_custom_target(unit-tests bin/virg-nexus-tests --gtest_death_test_style=threadsafe `printenv NX_TEST_FLAGS`
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing")

set(valgrind-flags --leak-check=full --track-origins=yes --show-reachable=yes --suppressions=${CMAKE_SOURCE_DIR}/valgrind_nexus.supp)
add_custom_target(unit-tests-valgrind valgrind ${valgrind-flags} bin/virg-nexus-tests --valgrind --gtest_death_test_style=threadsafe --gtest_filter=-*DEATH `printenv NX_TEST_FLAGS`
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing with Valgrind")

# ------------------------------- tools --------------------------------
set(TOOL_DIR ${CMAKE_SOURCE_DIR}/tools)
set(TOOL_INCLUDE_DIR ${TOOL_DIR}/include)
set(TOOL_MAN_DIR ${TOOL_DIR}/man)
set(TOOL_MAN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/man)

include_directories(${TOOL_INCLUDE_DIR})

set(NEXUS_TOOLS
  nx-harris-detector
  vg-stereo-matcher
)

add_executable(nx-harris-detector tools/nx_harris_detector_main.c)
install(FILES ${TOOL_MAN_DIR}/nx-harris-detector.1 DESTINATION ${TOOL_MAN_INSTALL_DIR}/man1)

add_executable(vg-stereo-matcher tools/vg_stereo_matcher_main.cc)
install(FILES ${TOOL_MAN_DIR}/vg-stereo-matcher.1 DESTINATION ${TOOL_MAN_INSTALL_DIR}/man1)

foreach(NEXUS_TOOL ${NEXUS_TOOLS})
  target_link_libraries(${NEXUS_TOOL} virg-nexus ${EXEC_LIBRARIES})
  set_target_properties(${NEXUS_TOOL} PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)
endforeach(NEXUS_TOOL)

install(TARGETS ${NEXUS_TOOLS} RUNTIME DESTINATION bin)
