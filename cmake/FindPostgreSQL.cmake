find_program(PostgreSQL_PG_CONFIG pg_config
  PATH
    /usr/local/pgsql/bin
    /opt/PostgreSQL/*/bin
    /Library/PostgreSQL/*/bin
    $ENV{ProgramFiles}/PostgreSQL/*/bin
    $ENV{SystemDrive}/PostgreSQL/*/bin
  DOC "Path to the pg_config executable"
)

function(get_pg_config var)
  execute_process(
    COMMAND ${PostgreSQL_PG_CONFIG} ${ARGN}
    OUTPUT_VARIABLE out
    OUTPUT_STRIP_TRAILING_WHITESPACE)
    message(STATUS "${var}: ${out}")
  set(${var} ${out} PARENT_SCOPE)
endfunction()

get_pg_config(PostgreSQL_VERSION_STRING     --version)
# linux: PostgreSQL 13.0 (Ubuntu 13.0-1.pgdg20.04+1)
# win32: PostgreSQL 13.0
get_pg_config(PostgreSQL_INCLUDE_DIR        --includedir)
# linux: /usr/include/postgresql
# win32: C:/PROGRA~1/POSTGR~1/13/include
get_pg_config(PostgreSQL_INCLUDE_SERVER_DIR --includedir-server)
# linux: /usr/include/postgresql/13/server
# win32: C:/PROGRA~1/POSTGR~1/13/include/server
get_pg_config(PostgreSQL_LIBRARY_DIR        --libdir)
# linux: /usr/lib/x86_64-linux-gnu
# win32: C:/PROGRA~1/POSTGR~1/13/lib
get_pg_config(PostgreSQL_PKG_LIBRARY_DIR    --pkglibdir)
# linux: /usr/lib/postgresql/13/lib
# win32: C:/PROGRA~1/POSTGR~1/13/lib
get_pg_config(PostgreSQL_SHARE_DIR          --sharedir)
# linux: /usr/share/postgresql/13
# win32: C:/PROGRA~1/POSTGR~1/13/share

# below not set on WIN32:
get_pg_config(PostgreSQL_CPP_FLAGS          --cppflags)
get_pg_config(PostgreSQL_C_FLAGS            --cflags)
get_pg_config(PostgreSQL_LD_FLAGS           --ldflags)
get_pg_config(PostgreSQL_LIBS               --libs)


# from timescaledb:
# Only Windows and FreeBSD need the base include/ dir instead of include/server/
set(PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_SERVER_DIR})
if(WIN32)
  list(APPEND PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_DIR})
  list(APPEND PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_SERVER_DIR}/port/win32)
  if(MSVC)
    list(APPEND PostgreSQL_INCLUDE_DIRS ${PostgreSQL_INCLUDE_SERVER_DIR}/port/win32_msvc)
  endif(MSVC)
endif(WIN32)
