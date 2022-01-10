cmake_minimum_required(VERSION 3.5)

project(avionics-pc-tools-gui LANGUAGES CXX)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_VERBOSE_MAKEFILE ON)


set(CMAKE_INCLUDE_CURRENT_DIR ON)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

find_package(QT NAMES Qt5 COMPONENTS Widgets Charts REQUIRED)
find_package(Qt${QT_VERSION_MAJOR} COMPONENTS Widgets Charts REQUIRED)


include_directories(${CMAKE_SOURCE_DIR}/src-cpp)

add_executable(${PROJECT_NAME}
        src-cpp/main.cpp
        src-cpp/windows/MainWindow.cpp
        src-cpp/widgets/TerminalWidget.cpp
        src-cpp/widgets/SerialPortSelectorWidget.cpp
        src-cpp/widgets/OnBoardSensorsTestWidget.cpp
        src-cpp/widgets/DebuggingToolsWidget.cpp
        src-cpp/widgets/FlightEmulationWidget.cpp
        src-cpp/utilities/QDirFinder.cpp
        src-cpp/emulation/DataFeeder.cpp
        src-cpp/emulation/IRunnable.hpp
        src-cpp/emulation/EventDetector.cpp
        src-cpp/emulation/EmulationController.cpp
        src-cpp/dialogs/FlightDataFormatDialog.cpp
#        src/resources/resources.qrc
        src-cpp/utilities/ChartDialogUtils.cpp
        src-cpp/utilities/ChartDialogUtils.h
        src-cpp/utilities/common.h
        )

## Sources
add_subdirectory(src-cpp/serial)
add_subdirectory(src-cpp/widgets/graphics/cube)
target_link_libraries(${PROJECT_NAME} PUBLIC serial Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts cube)

