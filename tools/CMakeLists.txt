# tools/CMakeLists.txt
set(TOOL_DIR ${CMAKE_CURRENT_SOURCE_DIR})
set(TOOL_MAN_DIR ${TOOL_DIR}/man)
set(TOOL_MAN_INSTALL_DIR ${CMAKE_INSTALL_PREFIX}/share/man)

set(NEXUS_TOOLS
  nx-harris-detector
  nx-sift-benchmark
  nx-stitch
  nx-fit-homography
)

add_executable(nx-harris-detector)
target_sources(nx-harris-detector PRIVATE nx_harris_detector_main.c)
install(FILES ${TOOL_MAN_DIR}/nx-harris-detector.1 DESTINATION ${TOOL_MAN_INSTALL_DIR}/man1)

add_executable(nx-sift-benchmark)
target_sources(nx-sift-benchmark PRIVATE nx_sift_benchmark_main.c)
  
add_executable(nx-stitch)
target_sources(nx-stitch PRIVATE nx_stitch_main.c nx_panorama_builder.c)

add_executable(nx-fit-homography)
target_sources(nx-fit-homography PRIVATE nx_fit_homography_main.c)

if(VIRG_NEXUS_CXX_API)
  list(APPEND NEXUS_TOOLS vg-stereo-matcher)
  
  add_executable(vg-stereo-matcher)
  target_sources(vg-stereo-matcher PRIVATE vg_stereo_matcher_main.cc)
  install(FILES ${TOOL_MAN_DIR}/vg-stereo-matcher.1 DESTINATION ${TOOL_MAN_INSTALL_DIR}/man1)
endif(VIRG_NEXUS_CXX_API)

foreach(NEXUS_TOOL ${NEXUS_TOOLS})
  target_link_libraries(${NEXUS_TOOL} virg-nexus ${EXEC_LIBRARIES})
  set_target_properties(${NEXUS_TOOL} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ../bin)
endforeach(NEXUS_TOOL)

install(TARGETS ${NEXUS_TOOLS} RUNTIME DESTINATION bin)
