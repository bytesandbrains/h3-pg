cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable (CMake 3.14)

project(h3-pg VERSION 3.7.0
              DESCRIPTION "Bindings for H3"
              LANGUAGES C)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

set(default_build_type "Release")
if(NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
  message(STATUS "Setting build type to '${default_build_type}' as none was specified.")
  set(CMAKE_BUILD_TYPE "${default_build_type}" CACHE
      STRING "Choose the type of build." FORCE)
  # Set the possible values of build type for cmake-gui
  set_property(CACHE CMAKE_BUILD_TYPE PROPERTY STRINGS
    "Debug" "Release" "MinSizeRel" "RelWithDebInfo")
endif()

# enable testing and set BUILD_TESTING if no parent project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  message(STATUS "Testing enabled")
  if(NOT WIN32)
    include(CTest)
  endif()
endif()

find_package(PostgreSQL REQUIRED)

add_subdirectory(h3)
