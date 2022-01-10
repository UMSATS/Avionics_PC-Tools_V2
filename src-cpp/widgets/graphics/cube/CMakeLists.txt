cmake_minimum_required(VERSION 3.5)
project(cube LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)

set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 COMPONENTS Core Gui Widgets REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Core Gui Widgets REQUIRED)


add_library(
        quat_lib
        STATIC
        math/sensor_processing_lib.cpp
        math/vector_3d.cpp
        math/quaternion.cpp
        )

add_library(
        ${PROJECT_NAME}
        STATIC
        geometryengine.cpp
        rotationcube.cpp
        shaders.qrc
        textures.qrc
)

## Sources
target_link_libraries(${PROJECT_NAME} PUBLIC Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Core Qt${QT_VERSION_MAJOR}::Gui quat_lib)

