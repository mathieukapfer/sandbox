cmake_minimum_required(VERSION 2.8.11)

project(testproject)

# change option for QT
# force 32bit exe on 64bit host
set(FORCE_32BITS "-m32")

#set(COMMON_CXX_FLAGS "-fmessage-length=0 -pipe -Wall -Wextra -Weffc++ -Werror -pedantic -std=c++98")
set(COMMON_CXX_FLAGS "-fmessage-length=0 -pipe -Wall -Wextra -Werror -pedantic -std=c++11 -fPIC -DQT_NO_VERSION_TAGGING ${FORCE_32BITS}")

# Find includes in corresponding build directories
set(CMAKE_PREFIX_PATH /home/user/Projects/websocket_poc/webserver/externals/qt/x86)
#set(CMAKE_INCLUDE_CURRENT_DIR ON)

# Instruct CMake to run moc automatically when needed.
set(CMAKE_AUTOMOC ON)

# Find the QtWidgets library
find_package(Qt5Core)

# Tell CMake to create the helloworld executable
add_executable(helloworld main.cpp)

# Use the Widgets module from Qt 5.
target_link_libraries(helloworld Qt5::Core)
