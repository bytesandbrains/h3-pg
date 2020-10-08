find_program(PG_REGRESS pg_regress
  HINTS
    "${PostgreSQL_PKG_LIBRARY_DIR}/pgxs/src/test/regress/"
  REQUIRED)

set(TESTS
  extension
  hierarchy
  indexing
  inspection
  miscellaneous
  opclass_btree
  opclass_hash
  #postgis
  #regions
  traversal
  type
  uniedges
)

add_custom_target(installcheck
  COMMAND ${CMAKE_COMMAND} -E env
  ${PG_REGRESS}
  --temp-instance=${CMAKE_BINARY_DIR}/tmp
  --user=user_regress
  #--load-extension postgis
  --load-extension h3
  --inputdir=${PROJECT_SOURCE_DIR}/h3/test
  --outputdir=${CMAKE_BINARY_DIR}
  ${TESTS}
  USES_TERMINAL)

add_test(NAME regress
  COMMAND
  ${PG_REGRESS}
  --temp-instance=${CMAKE_BINARY_DIR}/tmp
  --user=user_regress
  #--load-extension postgis
  --load-extension h3
  --inputdir=${PROJECT_SOURCE_DIR}/h3/test
  --outputdir=${CMAKE_BINARY_DIR}
  ${TESTS})