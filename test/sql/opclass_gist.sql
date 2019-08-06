\pset tuples_only on
\set hexagon '\'831c02fffffffff\'::h3index'

CREATE TABLE h3_test_gist (hex h3index);
CREATE INDEX GIST_IDX ON h3_test_gist USING gist(hex);
INSERT INTO h3_test_gist (hex) SELECT h3_to_parent(:hexagon);
INSERT INTO h3_test_gist (hex) SELECT h3_to_children(:hexagon);
INSERT INTO h3_test_gist (hex) SELECT h3_to_center_child(:hexagon, 15);

--
-- TEST GiST
--
SELECT COUNT(*) = 1 FROM h3_test_gist WHERE hex @> :hexagon;
SELECT COUNT(*) = 8 FROM h3_test_gist WHERE hex <@ :hexagon;
