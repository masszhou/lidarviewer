TEMPLATE = lib
QT += core gui network

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11
DEFINES += MAKE_SHARED_LIB

# The following define makes your compiler emit warnings if you use
# any Qt feature that has been marked deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
DEFINES += QT_DEPRECATED_WARNINGS

# You can also make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    src/camera.cpp \
    src/camera_frustum.cpp \
    src/dummy_axis.cpp \
    src/dummy_box.cpp \
    src/dummy_boxes.cpp \
    src/dummy_canvas.cpp \
    src/dummy_car.cpp \
    src/dummy_floorgrid.cpp \
    src/octree.cpp \
    src/point_cloud.cpp \
    src/sector.cpp \
    src/selection_box.cpp \
    src/transform3d.cpp \
    src/widget_lidarvisualizer.cpp \
    src/window_viewer.cpp

INCLUDEPATH += $$PWD/include
HEADERS += \
    include/lidar/box3.h \
    include/lidar/camera.h \
    include/lidar/camera_frustum.h \
    include/lidar/comm_funcs.h \
    include/lidar/dummy_axis.h \
    include/lidar/dummy_box.h \
    include/lidar/dummy_boxes.h \
    include/lidar/dummy_canvas.h \
    include/lidar/dummy_car.h \
    include/lidar/dummy_floorgrid.h \
    include/lidar/octree.h \
    include/lidar/octree_node.h \
    include/lidar/palettes.h \
    include/lidar/point_cloud.h \
    include/lidar/sector.h \
    include/lidar/selection_box.h \
    include/lidar/transform3d.h \
    include/lidar/vertex.h \
    include/lidar/vertex_stl.h \
    include/lidar/widget_lidarvisualizer.h \
    include/lidar/window_viewer.h

RESOURCES += \
    resources/lidar_viewer_resources.qrc

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target