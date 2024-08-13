cmake_minimum_required(VERSION 2.8) 

project (PPL_TRACKING)

set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -ggdb -Wall -Wextra -std=gnu++0x")

set (CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/config)

find_package (Qt4 REQUIRED)
find_package (OpenMP REQUIRED)
find_package (OpenCV REQUIRED)
find_package (OpenCL REQUIRED)
find_package (OpenGL REQUIRED)
find_package (Webcam REQUIRED)

set (QT_USE_QTOPENGL TRUE)

include (${QT_USE_FILE})

add_definitions (${QT_DEFINITIONS})

if (OPENMP_FOUND)
  set (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
  set (CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
  set (CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")
endif()

set (CMAKE_S
  "src/main.cpp"
  "src/colorlist.cpp"
  )

set (CMAKE_H

  )

set (CMAKE_GUI_S
  "gui/opencamera.cpp"
  "gui/camerasettings.cpp"
  "gui/controls.cpp"
  "gui/patternselection.cpp"
  "gui/rgbhistogram.cpp"
  "gui/videoplayer.cpp"
  "gui/player.cpp"
  "gui/mainwindow.cpp"
  )

set (CMAKE_MOC
  "src/colorlist.h"
  "gui/opencamera.h"
  "gui/camerasettings.h"
  "gui/controls.h"
  "gui/patternselection.h"
  "gui/rgbhistogram.h"
  "gui/videoplayer.h"
  "gui/player.h"
  "gui/mainwindow.h"
  )

set (CMAKE_UI
  "gui/opencamera.ui"
  "gui/camerasettings.ui"
  "gui/controls.ui"
  "gui/patternselection.ui"
  "gui/player.ui"
  "gui/mainwindow.ui"
  )

set (CMAKE_RES
  "gui/icons.qrc"
  )

qt4_wrap_cpp (CMAKE_MOC_S ${CMAKE_MOC})
qt4_wrap_ui (CMAKE_UI_S ${CMAKE_UI})
qt4_add_resources (CMAKE_RES_S ${CMAKE_RES})

include_directories (
  ${CMAKE_BINARY_DIR}
  ${OPENCL_INCLUDE_DIR}
  ${OPENGL_INCLUDE_DIR}
  ${CMAKE_SOURCE_DIR}/src
  ${CMAKE_SOURCE_DIR}/gui
  ${CMAKE_SOURCE_DIR}/libs
  )

add_subdirectory (libs)
add_subdirectory (kernels)

add_executable (ppl_tracking
  ${CMAKE_S}
  ${CMAKE_H}
  ${CMAKE_GUI_S}
  ${CMAKE_MOC_S}
  ${CMAKE_UI_S}
  ${CMAKE_RES_S}
  )

target_link_libraries (ppl_tracking
  ${QT_LIBRARIES}
  ${OpenCV_LIBS}
  ${OPENCL_LIBRARIES}
  ${OPENGL_LIBRARY}
  ${Webcam_LIBRARY}
  COMMON_LIBRARY
  WEBCAM_WRAPPER_LIBRARY
  OPENCV_THREAD_LIBRARY
  CLASSIFIERS_LIBRARY
  FILTERS_LIBRARY
  )
