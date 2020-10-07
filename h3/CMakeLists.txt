set(EXT_NAME h3)

add_subdirectory(sql)
add_subdirectory(src)

if(BUILD_TESTING)
  add_subdirectory(test)
endif()

set(EXT_CONTROL_FILE ${EXT_NAME}.control)
configure_file(${EXT_CONTROL_FILE} ${EXT_CONTROL_FILE})

install(
  FILES ${CMAKE_CURRENT_BINARY_DIR}/${EXT_CONTROL_FILE}
  DESTINATION "${PostgreSQL_SHARE_DIR}/extension"
  COMPONENT extension
)