include(FetchContent)

set(CMAKE_POSITION_INDEPENDENT_CODE ON  CACHE INTERNAL "")
set(BUILD_SHARED_LIBS               OFF CACHE INTERNAL "")
set(BUILD_FILTERS                   OFF CACHE INTERNAL "")
set(BUILD_BENCHMARKS                OFF CACHE INTERNAL "")
set(BUILD_GENERATORS                OFF CACHE INTERNAL "")

FetchContent_Declare(h3
  GIT_REPOSITORY https://github.com/uber/h3.git
  GIT_TAG master
)
FetchContent_MakeAvailable(h3)

set(H3_INCLUDE_DIR ${h3_BINARY_DIR}/src/h3lib/include)
