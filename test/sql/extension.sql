\pset tuples_only on

--
-- TEST h3_get_extension_version
--

SELECT h3_get_extension_version() ~ '^[0-9]+\.[0-9]+\.[0-9]+$'
    OR h3_get_extension_version() = 'unreleased';