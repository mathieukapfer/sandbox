cmake_minimum_required (VERSION 2.6)
project (Tuto)
add_executable (Tuto toto.cpp)

option(ARM "Enable ARM build" OFF)

if(ARM)
  message("ARM build requested")
endif()