include(FetchH3)

include_directories(${CMAKE_CURRENT_BINARY_DIR}/include)
include_directories(SYSTEM ${PostgreSQL_INCLUDE_SERVER_DIR})

if(WIN32)
  string(APPEND CMAKE_MODULE_LINKER_FLAGS " ${PostgreSQL_LIBRARY_DIR}/postgres.lib")

  include_directories(SYSTEM ${PostgreSQL_INCLUDE_DIR})

  string(APPEND CMAKE_MODULE_LINKER_FLAGS " ws2_32.lib Version.lib")
  set(CMAKE_C_FLAGS "-D_CRT_SECURE_NO_WARNINGS")
  include_directories(SYSTEM ${PG_INCLUDE_SERVER_DIR}/port/win32)

  if(MSVC)
    message(STATUS "is MSVC")
    include_directories(SYSTEM ${PG_INCLUDE_SERVER_DIR}/port/win32_msvc)
  endif (MSVC)
endif (WIN32)

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
