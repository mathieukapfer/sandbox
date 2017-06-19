cmake_minimum_required(VERSION 2.8.11)

project(testproject)

SET(CMAKE_SKIP_RPATH  true)

# Find includes in corresponding build directories
set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the Qt library
set(QTPATH /home/user/Projects/websocket_poc/webserver/externals/qt/x86/)
set(CMAKE_PREFIX_PATH ${QTPATH})
find_package(Qt5Core)

include_directories (${QTPATH}/include)
include_directories (${QTPATH}/mkspecs/linux-g++-32)
include_directories (${QTPATH}/include/QtCore)
link_directories (${QTPATH}/lib/)

# Tell CMake to create the helloworld executable
add_executable(helloworld Test.cpp Test.h log.h)
set_target_properties(helloworld PROPERTIES COMPILE_FLAGS "-m32 -pipe -O2 -std=gnu++0x -Wall -W -D_REENTRANT -fPIC -DQT_NO_DEBUG -DQT_CORE_LIB" )
set_target_properties(helloworld PROPERTIES LINK_FLAGS "-m32 " )

# Use the Widgets module from Qt 5.
target_link_libraries(helloworld Qt5::Core)
##target_link_libraries(helloworld libQt5Core.so)
