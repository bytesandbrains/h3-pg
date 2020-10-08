include(FetchH3)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(SYSTEM ${PostgreSQL_INCLUDE_DIRS})
message(STATUS "INCLUDING ${PostgreSQL_INCLUDE_DIRS}")

if(UNIX)
  set(CMAKE_C_STANDARD 11)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} -L${PostgreSQL_LIBRARY_DIR}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} -L${PostgreSQL_LIBRARY_DIR}")
  set(CMAKE_C_FLAGS "${PostgreSQL_C_FLAGS} ${CMAKE_C_FLAGS}")
  set(CMAKE_CPP_FLAGS "${CMAKE_CPP_FLAGS} ${PostgreSQL_CPP_FLAGS}")
endif()

if(WIN32)
  string(APPEND CMAKE_MODULE_LINKER_FLAGS " ${PostgreSQL_PKG_LIBRARY_DIR}/postgres.lib")
  string(APPEND CMAKE_MODULE_LINKER_FLAGS " ws2_32.lib Version.lib")
  set(CMAKE_C_FLAGS "-D_CRT_SECURE_NO_WARNINGS")

  message(CMAKE_MODULE_LINKER_FLAGS)
endif (WIN32)

if(PG_LDFLAGS)
  set(CMAKE_SHARED_LINKER_FLAGS "${CMAKE_SHARED_LINKER_FLAGS} ${PG_LDFLAGS}")
  set(CMAKE_MODULE_LINKER_FLAGS "${CMAKE_MODULE_LINKER_FLAGS} ${PG_LDFLAGS}")
endif()

configure_file(include/extension.in.h include/extension.h)

add_library(${PROJECT_NAME} MODULE
  include/extension.h
  lib/extension.c
  lib/hierarchy.c
  lib/indexing.c
  lib/inspection.c
  lib/miscellaneous.c
  lib/opclass_btree.c
  lib/opclass_hash.c
  lib/operators.c
  lib/regions.c
  lib/traversal.c
  lib/type.c
  lib/uniedges.c
)

target_link_libraries(${PROJECT_NAME} h3)

set_target_properties(${PROJECT_NAME} PROPERTIES
  OUTPUT_NAME ${EXT_NAME}
  PREFIX ""
)

install(
  TARGETS ${PROJECT_NAME}
  DESTINATION ${PostgreSQL_PKG_LIBRARY_DIR}
  COMPONENT extension
)
