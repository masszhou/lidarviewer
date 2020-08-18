cmake_minimum_required(VERSION 3.10)
# version 3.4 is required as other do not work with C++14 and clang
# version 3.10 is required due to CMP0071

project(qt_viewer VERSION 1.0
        DESCRIPTION "autonomous driving project"
        LANGUAGES CXX)

add_definitions(-std=c++14)
set(CXX_FLAGS "-Wall -g")
set(CMAKE_CXX_FLAGS, "${CXX_FLAGS}")

# ---------------------------------------------------------------------
# Qt Setup
# ---------------------------------------------------------------------
# ---IMPORTANT---
# For find_package for QT to be successful, CMake must find the Qt installation in one of the following ways:
# Set your CMAKE_PREFIX_PATH environment variable to the Qt 5 installation prefix. This is the recommended way.
# cmake -DCMAKE_PREFIX_PATH=/opt/Qt/5.14.1/gcc_64/lib/cmake ..

# Test for supported Qt version
find_program(QMAKE_EXECUTABLE NAMES qmake HINTS ${QTDIR} ENV QTDIR PATH_SUFFIXES bin)
execute_process(COMMAND ${QMAKE_EXECUTABLE} -query QT_VERSION OUTPUT_VARIABLE QT_VERSION)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)
# As moc files are generated in the binary dir, tell CMake
# to always look for includes there:
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Find the QtWidgets library
find_package(Qt5 COMPONENTS
             Core REQUIRED
             Widgets REQUIRED
             Gui REQUIRED
             Network REQUIRED
             OpenGL REQUIRED)

qt5_add_resources(RESOURCES resources/lidar_viewer_resources.qrc)


# ---------------------------------------------------------------------
# Lidar Viewer Setup
# ---------------------------------------------------------------------
set(LIDARVIEWER_SOURCE_FILES
        src/camera.cpp
        src/camera_frustum.cpp
        src/dummy_axis.cpp
        src/dummy_box.cpp
        src/dummy_boxes.cpp
        src/dummy_canvas.cpp
        src/dummy_car.cpp
        src/dummy_floorgrid.cpp
        src/octree.cpp
        src/point_cloud.cpp
        src/sector.cpp
        src/selection_box.cpp
        src/transform3d.cpp
        src/widget_lidarvisualizer.cpp
)
# Due to error "undefined reference to vtable" for qt+cmake
# "automoc" can not parse header files, must explicit add into "add_executable"
# more read "Qt 5 cmake fails with undefined reference to vtable on hello world with inc & src as subdirs"
# https://stackoverflow.com/questions/19761767/qt-5-cmake-fails-with-undefined-reference-to-vtable-on-hello-world-with-inc-sr
set(LIDARVIEWER_HEADER_FILES
        include/qt_viewer/box3.h
        include/qt_viewer/camera.h
        include/qt_viewer/camera_frustum.h
        include/qt_viewer/comm_funcs.h
        include/qt_viewer/dummy_axis.h
        include/qt_viewer/dummy_box.h
        include/qt_viewer/dummy_boxes.h
        include/qt_viewer/dummy_canvas.h
        include/qt_viewer/dummy_car.h
        include/qt_viewer/dummy_floorgrid.h
        include/qt_viewer/octree.h
        include/qt_viewer/octree_node.h
        include/qt_viewer/palettes.h
        include/qt_viewer/point_cloud.h
        include/qt_viewer/sector.h
        include/qt_viewer/selection_box.h
        include/qt_viewer/transform3d.h
        include/qt_viewer/vertex.h
        include/qt_viewer/vertex_stl.h
        include/qt_viewer/widget_lidarvisualizer.h
)

add_executable(lidarviewer_exec
        src/viewer.cpp
        ${LIDARVIEWER_SOURCE_FILES}
        ${LIDARVIEWER_HEADER_FILES}
        ${RESOURCES})
target_link_libraries(lidarviewer_exec
        PUBLIC
        Qt5::Widgets
        Qt5::Gui
        Qt5::Core
        Qt5::OpenGL
        Qt5::Network)
target_include_directories(lidarviewer_exec
        PRIVATE
            include/qt_viewer
        )
set_target_properties(lidarviewer_exec
        PROPERTIES
        CXX_STANDARD 14
        POSITION_INDEPENDENT_CODE ON
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
        )


