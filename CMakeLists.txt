cmake_minimum_required(VERSION 3.15)

set(VERSION_MAJOR 0)
set(VERSION_MINOR 1)
project(virg-nexus
  VERSION ${VERSION_MAJOR}.${VERSION_MINOR}
  LANGUAGES C CXX
  DESCRIPTION "VIRG-Nexus Library")

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

set(virg_nexus_C_SOURCES
  src/nx_alloc.c
  src/nx_mem_block.c
  src/SFMT/SFMT.c
  src/nx_uniform_sampler.c
  src/nx_gaussian_sampler.c
  src/nx_bit_ops.c
  src/nx_math.c
  src/nx_mat.c
  src/nx_statistics.c
  src/nx_graph.c
  src/nx_rotation_3d.c
  src/nx_quaternion.c
  src/nx_svd.c
  src/nx_linear_solvers.c
  src/nx_string.c
  src/nx_hash_sha256.c
  src/nx_transform_2d.c
  src/nx_io.c
  src/nx_filesystem.c
  src/nx_string_array.c
  src/nx_options.c
  src/nx_lexer.c
  src/nx_json_lexer.c
  src/nx_json_parser.c
  src/nx_json_tree.c
  src/nx_json_bundle.c
  src/nx_json_log.c
  src/nx_gnuplot.c
  src/nx_gc_cairo_imp.c
  src/nx_filter.c
  src/nx_image.c
  src/nx_image_io_pnm.c
  src/nx_image_io_jpeg.c
  src/nx_image_io_png.c
  src/nx_image_io.c
  src/nx_image_warp.c
  src/nx_affine_warp_processor.c
  src/nx_usac.c
  src/nx_usac_homography.c
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
  src/nx_sift_detector.c
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
  src/nx_temple_color_128.c
  src/nx_vgg_affine_dataset.c
)

set(virg_nexus_C_HEADERS
  include/virg/nexus/nx_config.h
  include/virg/nexus/nx_types.h
  include/virg/nexus/nx_assert.h
  include/virg/nexus/nx_log.h
  include/virg/nexus/nx_alloc.h
  include/virg/nexus/nx_mem_block.h
  include/virg/nexus/nx_uniform_sampler.h
  include/virg/nexus/nx_gaussian_sampler.h
  include/virg/nexus/nx_bit_ops.h
  include/virg/nexus/nx_math.h
  include/virg/nexus/nx_statistics.h
  include/virg/nexus/nx_graph.h
  include/virg/nexus/nx_rotation_3d.h
  include/virg/nexus/nx_quaternion.h
  include/virg/nexus/nx_svd.h
  include/virg/nexus/nx_linear_solvers.h
  include/virg/nexus/nx_vec.h
  include/virg/nexus/nx_vec234.h
  include/virg/nexus/nx_mat.h
  include/virg/nexus/nx_mat234.h
  include/virg/nexus/nx_string.h
  include/virg/nexus/nx_hash_sha256.h
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
  include/virg/nexus/nx_gnuplot.h
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
  include/virg/nexus/nx_usac.h
  include/virg/nexus/nx_usac_homography.h
  include/virg/nexus/nx_homography.h
  include/virg/nexus/nx_epipolar.h
  include/virg/nexus/nx_image_pyr.h
  include/virg/nexus/nx_image_pyr_builder.h
  include/virg/nexus/nx_colorspace.h
  include/virg/nexus/nx_keypoint.h
  include/virg/nexus/nx_keypoint_vector.h
  include/virg/nexus/nx_fast_detector.h
  include/virg/nexus/nx_harris_detector.h
  include/virg/nexus/nx_sift_detector.h
  include/virg/nexus/nx_checkerboard_detector.h
  include/virg/nexus/nx_brief_extractor.h
  include/virg/nexus/nx_point_match_2d.h
  include/virg/nexus/nx_point_match_2d_stats.h
  include/virg/nexus/nx_data_frame.h
  include/virg/nexus/nx_csv_lexer.h
  include/virg/nexus/nx_csv_parser.h
  include/virg/nexus/nx_pinhole.h
  include/virg/nexus/nx_kitti_slam.h
  include/virg/nexus/nx_temple_color_128.h
  include/virg/nexus/nx_vgg_affine_dataset.h
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

set(virg_nexus_OPENGL_C_HEADERS
  include/virg/nexus/GL/nx_gl.h
  include/virg/nexus/GL/nx_gl_shader.h
  include/virg/nexus/GL/nx_gl_program.h
  include/virg/nexus/GL/nx_gl_buffer.h
  include/virg/nexus/GL/nx_gl_projection.h
)

set(virg_nexus_OPENGL_C_SOURCES
  src/GL/nx_gl.c
  src/GL/nx_gl_shader.c
  src/GL/nx_gl_program.c
  src/GL/nx_gl_buffer.c
  src/GL/nx_gl_projection.c
)

set(virg_nexus_HEADERS ${virg_nexus_C_HEADERS})
set(virg_nexus_SOURCES ${virg_nexus_C_SOURCES})

option(VIRG_NEXUS_CXX_API "Compile and install C++ API"  ON)
if (VIRG_NEXUS_CXX_API)
   list(APPEND virg_nexus_HEADERS ${virg_nexus_CXX_HEADERS})
   list(APPEND virg_nexus_SOURCES ${virg_nexus_CXX_SOURCES})
endif (VIRG_NEXUS_CXX_API)

option(VIRG_NEXUS_OPENGL_C_HELPERS "Compile and install OpenGL C Helpers"  ON)
if (VIRG_NEXUS_OPENGL_C_HELPERS)
   list(APPEND virg_nexus_HEADERS ${virg_nexus_OPENGL_C_HEADERS})
   list(APPEND virg_nexus_SOURCES ${virg_nexus_OPENGL_C_SOURCES})
endif (VIRG_NEXUS_OPENGL_C_HELPERS)

add_library(virg-nexus STATIC ${virg_nexus_SOURCES})
set_target_properties(virg-nexus PROPERTIES ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_library(virg-nexus-shared SHARED ${virg_nexus_SOURCES})
target_link_libraries(virg-nexus-shared -lm -llapack)
set_target_properties(virg-nexus-shared PROPERTIES OUTPUT_NAME virg-nexus)
set_target_properties(virg-nexus-shared PROPERTIES LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

# -------------------------------- Dependencies ----------------------------------
set(EXEC_LIBRARIES -lm)

# --------------------------- configuration ----------------------------
set(CMAKE_INSTALL_PREFIX "$ENV{DEVEL_DIR}")
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} ${CMAKE_SOURCE_DIR} )
include_directories(include)

option(VIRG_NEXUS_USE_SIMD "Enable SIMD flags"  ON)
# SSE
if (VIRG_NEXUS_USE_SIMD)
  set(NX_HAVE_SIMD 1)
  set(NX_SIMD_AVX2 1)
  set(VIRG_NEXUS_SIMD_ALIGNMENT 64)
  set(VIRG_NEXUS_FLAGS_SIMD "-mavx2")
else()
  set(VIRG_NEXUS_SIMD_ALIGNMENT 8)
endif (VIRG_NEXUS_USE_SIMD)

set(VIRG_NEXUS_FLAGS_DEBUG          "-Wextra -Wall -g -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELEASE        "-Wextra -Wall -O3 -DNDEBUG -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_SIMD}")
set(VIRG_NEXUS_FLAGS_RELWITHDEBINFO "-Wextra -Wall -O3 -DNDEBUG -g -fno-omit-frame-pointer -DJSMN_PARENT_LINKS=1 ${VIRG_NEXUS_FLAGS_SIMD}")

set(CMAKE_C_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG}")
set(CMAKE_C_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE}")
set(CMAKE_C_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO}")
set(CMAKE_CXX_FLAGS_DEBUG          "${VIRG_NEXUS_FLAGS_DEBUG}")
set(CMAKE_CXX_FLAGS_RELEASE        "${VIRG_NEXUS_FLAGS_RELEASE}")
set(CMAKE_CXX_FLAGS_RELWITHDEBINFO "${VIRG_NEXUS_FLAGS_RELWITHDEBINFO}")

# set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
set(CMAKE_BUILD_TYPE RelWithDebInfo)

configure_file(${CMAKE_CURRENT_SOURCE_DIR}/include/virg/nexus/nx_config.h.in ${CMAKE_CURRENT_SOURCE_DIR}/include/virg/nexus/nx_config.h @ONLY)

# -------------------------- install target ----------------------------
install(TARGETS virg-nexus ARCHIVE DESTINATION lib)
install(TARGETS virg-nexus-shared LIBRARY DESTINATION lib)
foreach(virg_nexus_header ${virg_nexus_HEADERS})
  get_filename_component(install_RELPATH ${virg_nexus_header} PATH)
  install(FILES ${virg_nexus_header} DESTINATION ${install_RELPATH})
endforeach(virg_nexus_header)
install(FILES lib/pkgconfig/virg-nexus.pc DESTINATION lib/pkgconfig)

# --------------------------------- edit target ----------------------------------
add_custom_target(edit emacsclient -n ${virg_nexus_SOURCES} ${virg_nexus_HEADERS} CMakeLists.txt
  WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
  COMMENT "Open source and header files using Emacs server")

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

list(APPEND EXEC_LIBRARIES ${PNG_LIBRARIES} ${JPEG_LIBRARIES})
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

list(APPEND EXEC_LIBRARIES ${LAPACK_LIBRARIES})
# ------------------------------------ Cairo -------------------------------------
include(FindPkgConfig)

pkg_check_modules(CAIRO REQUIRED cairo)
include_directories(${CAIRO_INCLUDE_DIRS})
link_directories(${CAIRO_LIBRARY_DIRS})

list(APPEND EXEC_LIBRARIES ${CAIRO_LDFLAGS})
# ----------------------------------- OpenGL -------------------------------------
if(VIRG_NEXUS_OPENGL_C_HELPERS)
  find_package(OpenGL REQUIRED)

  include_directories(${OPENGL_INCLUDE_DIR})

  list(APPEND EXEC_LIBRARIES ${OPENGL_gl_LIBRARY})
endif(VIRG_NEXUS_OPENGL_C_HELPERS)
# -------------------------------- Dependencies ----------------------------------
foreach(LIB ${EXEC_LIBRARIES})
  set(MODULE_LIBS "${MODULE_LIBS} ${LIB}")
endforeach()
configure_file(virg-nexus.pc.in ${CMAKE_SOURCE_DIR}/lib/pkgconfig/virg-nexus.pc @ONLY)

# ---------------------------- external dependencies -----------------------------
add_subdirectory(ext)

# ------------------------------------ tests -------------------------------------
add_subdirectory(tests)

# ------------------------------------ tools -------------------------------------
add_subdirectory(tools)

# --------------------------------- benchmarks -----------------------------------
add_subdirectory(bench)
