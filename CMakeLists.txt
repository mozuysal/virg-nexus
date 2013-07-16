cmake_minimum_required(VERSION 2.8)

project(virg-nexus C CXX)

set(VERSION 1.0)

option(VIRG_NEXUS_ANDROID "Enable Android Compilation"  OFF)
option(VIRG_NEXUS_PC_LINUX "Enable Linux Compilation"  ON)
option(VIRG_NEXUS_PC_WIN32 "Enable Win32 Compilation"  OFF)

set(virg_nexus_SOURCES
  src/nx_alloc.c
  src/nx_mem_block.c
  src/nx_math.c
  src/nx_io.c
  src/nx_filter.c
  src/nx_image.c
  src/nx_colorspace.c
)

set(virg_nexus_HEADERS
  include/virg/nexus/nx_config.h
  include/virg/nexus/nx_types.h
  include/virg/nexus/nx_assert.h
  include/virg/nexus/nx_message.h
  include/virg/nexus/nx_alloc.h
  include/virg/nexus/nx_mem_block.h
  include/virg/nexus/nx_math.h
  include/virg/nexus/nx_io.h
  include/virg/nexus/nx_image.h
  include/virg/nexus/nx_colorspace.h
)

if (NOT VIRG_NEXUS_ANDROID)
  set(virg_nexus_SOURCES ${virg_nexus_SOURCES} src/nx_message_c_impl.c)
endif() 


add_library(virg-nexus STATIC ${virg_nexus_SOURCES})
set_target_properties(virg-nexus PROPERTIES ARCHIVE_OUTPUT_DIRECTORY lib)

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

set(CMAKE_BUILD_TYPE Debug)
# set(CMAKE_BUILD_TYPE Release)
# set(CMAKE_BUILD_TYPE RelWithDebInfo)

configure_file(virg-nexus.pc.in ${CMAKE_SOURCE_DIR}/virg-nexus.pc @ONLY)

# -------------------------- install target ----------------------------
install(TARGETS virg-nexus ARCHIVE DESTINATION lib)
foreach(virg_nexus_header ${virg_nexus_HEADERS})
  get_filename_component(install_RELPATH ${virg_nexus_header} PATH)
  install(FILES ${virg_nexus_header} DESTINATION ${install_RELPATH})
endforeach(virg_nexus_header)
install(FILES virg-nexus.pc DESTINATION lib/pkgconfig)

# -------------------------- ebrowse target ----------------------------
add_custom_target(browse DEPENDS ${marl_HEADERS} ${marl_SOURCES})
add_custom_command(COMMAND ebrowse
  ARGS ${marl_HEADERS} ${marl_SOURCES} -o BROWSE
  WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
  TARGET browse)

# --------------------------------- doc target -----------------------------------
find_package(Doxygen)
if (DOXYGEN_FOUND)
  configure_file(${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile @ONLY)
  add_custom_target(doc ALL
    ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/Doxyfile
    WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
    COMMENT "Generating API documentation with Doxygen" VERBATIM)
endif(DOXYGEN_FOUND)

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
)

add_executable(virg-nexus-tests ${test_SOURCES})
target_link_libraries(virg-nexus-tests virg-nexus gtest ${CMAKE_THREAD_LIBS_INIT} ${EXEC_LIBRARIES})
set_target_properties(virg-nexus-tests PROPERTIES RUNTIME_OUTPUT_DIRECTORY bin)

add_custom_target(unit-tests bin/virg-nexus-tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing" VERBATIM)

add_custom_target(unit-tests-valgrind valgrind --leak-check=full --show-reachable=yes bin/virg-nexus-tests
  WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
  DEPENDS virg-nexus virg-nexus-tests
  COMMENT "Unit Testing with Valgrind" VERBATIM)

