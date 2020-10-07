set(EXT_SQL_FILE ${CMAKE_CURRENT_BINARY_DIR}/${EXT_NAME}--${PROJECT_VERSION}.sql)

set(INSTALL_SQL_FILES
        install/00-type.sql
        install/01-indexing.sql
        install/02-inspection.sql
        install/03-traversal.sql
        install/04-hierarchy.sql
        install/05-regions.sql
        install/06-uniedge.sql
        install/07-miscellaneous.sql
        install/10-operators.sql
        install/11-opclass_btree.sql
        install/12-opclass_hash.sql
        install/20-extension.sql
        install/30-deprecated.sql
        install/99-postgis.sql
)

set(UPDATE_SQL_FILES
        updates/h3--0.1.0.sql
        updates/h3--0.1.0--0.2.0.sql
        updates/h3--0.2.0--0.3.0.sql
        updates/h3--0.3.0--0.3.1.sql
        updates/h3--0.3.1--0.3.2.sql
        updates/h3--0.3.2--0.4.0.sql
        updates/h3--0.4.0--1.0.0.sql
        updates/h3--1.0.0--1.0.1.sql
        updates/h3--1.0.1--1.0.2.sql
        updates/h3--1.0.2--1.0.3.sql
        updates/h3--1.0.3--1.0.4.sql
        updates/h3--1.0.4--1.0.5.sql
        updates/h3--1.0.5--1.0.6.sql
        updates/h3--1.0.6--3.4.0.sql
        updates/h3--3.4.0--3.4.1.sql
        updates/h3--3.4.1--3.5.0.sql
        updates/h3--3.5.0--3.6.0.sql
        updates/h3--3.6.0--3.6.1.sql
        updates/h3--3.6.1--3.6.2.sql
        updates/h3--3.6.2--3.6.3.sql
        updates/h3--3.6.3--3.6.4.sql
        updates/h3--3.6.4--3.6.5.sql
        updates/h3--3.6.5--3.7.0.sql
        updates/h3--3.7.0--unreleased.sql
)

foreach(file ${INSTALL_SQL_FILES})
        file(READ ${file} CONTENTS)
        file(APPEND ${EXT_SQL_FILE} "${CONTENTS}")
endforeach()

install(
        FILES ${EXT_SQL_FILE} ${UPDATE_SQL_FILES}
        DESTINATION "${PostgreSQL_SHARE_DIR}/extension"
        COMPONENT extension
)
