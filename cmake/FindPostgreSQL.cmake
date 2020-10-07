find_program(PostgreSQL_PG_CONFIG pg_config
  PATH
    /usr/local/pgsql/bin
    /opt/PostgreSQL/*/bin
    /Library/PostgreSQL/*/bin
    $ENV{ProgramFiles}/PostgreSQL/*/bin
    $ENV{SystemDrive}/PostgreSQL/*/bin
  DOC "Path to the pg_config executable"
)

exec_program(${PostgreSQL_PG_CONFIG} ARGS --includedir        OUTPUT_VARIABLE PostgreSQL_INCLUDE_DIR)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --includedir-server OUTPUT_VARIABLE PostgreSQL_INCLUDE_SERVER_DIR)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --libdir            OUTPUT_VARIABLE PostgreSQL_LIBRARY_DIR)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --pkglibdir         OUTPUT_VARIABLE PostgreSQL_PKG_LIBRARY_DIR)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --sharedir          OUTPUT_VARIABLE PostgreSQL_SHARE_DIR)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --cppflags          OUTPUT_VARIABLE PostgreSQL_CPP_FLAGS)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --cflags            OUTPUT_VARIABLE PostgreSQL_C_FLAGS)
exec_program(${PostgreSQL_PG_CONFIG} ARGS --version           OUTPUT_VARIABLE PostgreSQL_VERSION_STRING)

set(PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIR} ${PostgreSQL_INCLUDE_SERVER_DIR})
if(WIN32)
  list(APPEND PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_SERVER_DIR}/port/win32)
  if(MSVC)
    list(APPEND PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_SERVER_DIR}/port/win32_msvc)
  endif(MSVC)
endif(WIN32)
