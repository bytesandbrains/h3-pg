cmake_minimum_required(VERSION 3.14) # FetchContent_MakeAvailable (CMake 3.14)

project(h3-pg VERSION 3.7.0
              DESCRIPTION "Bindings for H3"
              LANGUAGES C)


list(APPEND CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake")

# enable testing and set BUILD_TESTING if no parent project
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
  message(STATUS "Testing enabled")
  if(NOT WIN32)
    message(STATUS "@@@Inlcuded CTEST")
    include(CTest)
  endif()
endif()

find_package(PostgreSQL REQUIRED)

add_subdirectory(h3)
