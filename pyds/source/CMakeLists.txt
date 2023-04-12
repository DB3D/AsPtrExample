# define our project name
project(AsPtrExample)

# define the include directories our project may need
include_directories(
  ${PYTHON_INCLUDE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_SOURCE_DIR}/source/blender/makesdna
  ${CMAKE_SOURCE_DIR}/source/blender/makesrna
  ${CMAKE_SOURCE_DIR}/source/blender/blenkernel
  ${CMAKE_SOURCE_DIR}/source/blender/blenlib
  ${CMAKE_SOURCE_DIR}/source/blender/imbuf
  ${CMAKE_SOURCE_DIR}/source/blender/depsgraph
  ${CMAKE_SOURCE_DIR}/source/blender/windowmanager
  ${CMAKE_SOURCE_DIR}/intern/guardedalloc
)

# definitions needed for buildinfo's..
add_definitions(-DBUILD_DATE)
add_definitions(-DWITH_BUILDINFO_HEADER)

# define our main cpp file to compile
set(SRC
  main.cpp
  buildinfo.c
)

add_library(makepyd_AsPtrExample SHARED ${SRC}) # needed to build a .pyd
set_target_properties(makepyd_AsPtrExample PROPERTIES PREFIX "") # needed to build a .pyd
set_target_properties(makepyd_AsPtrExample PROPERTIES OUTPUT_NAME "readmem") # define output file name

# link libraries our project may need, may vary depending on the module we include
target_link_libraries(
  makepyd_AsPtrExample
  ${PYTHON_LIBRARIES}
  bf_python
  bf_collada
  bf_blenlib
  bf_dna
  bf_imbuf
  bf_editor_io
  bf_editor_scene
  bf_editor_object
  bf_editor_mesh
  bf_blenloader
  bf_blenkernel
  dbghelp
  Ws2_32
  Comctl32
  imm32
  Shcore
  vfw32
  version
  Dwmapi
)

# define destination folder, will be located in "buildRelease/bin/release/bakedpyd_AsPtrExample/"
install(TARGETS makepyd_AsPtrExample DESTINATION ${CMAKE_INSTALL_PREFIX}/bakedpyd_AsPtrExample)