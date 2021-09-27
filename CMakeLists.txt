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


include_directories(${CMAKE_SOURCE_DIR}/src)

add_executable(${PROJECT_NAME}
        src/main.cpp
        src/windows/MainWindow.cpp
        src/widgets/TerminalWidget.cpp
        src/widgets/SerialPortSelectorWidget.cpp
        src/widgets/OnBoardSensorsTestWidget.cpp
        src/widgets/DebuggingToolsWidget.cpp
        src/widgets/FlightEmulationWidget.cpp
        src/utilities/QDirFinder.cpp
        src/emulation/DataFeeder.cpp
        src/emulation/IRunnable.hpp
        src/emulation/EventDetector.cpp
        src/emulation/EmulationController.cpp
        src/dialogs/FlightDataFormatDialog.cpp
#        src/resources/resources.qrc
        src/utilities/ChartDialogUtils.cpp
        src/utilities/ChartDialogUtils.h
        src/utilities/common.h
        )

## Sources
add_subdirectory(src/serial)
add_subdirectory(src/widgets/graphics/cube)
target_link_libraries(${PROJECT_NAME} PUBLIC serial Qt${QT_VERSION_MAJOR}::Widgets Qt${QT_VERSION_MAJOR}::Charts cube)

