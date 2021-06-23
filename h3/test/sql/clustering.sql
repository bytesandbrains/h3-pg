\pset tuples_only on
\set string '\'801dfffffffffff\''
\set hexagon ':string::h3index'

CREATE TABLE h3_test_clustering (hex h3index PRIMARY KEY);
INSERT INTO h3_test_clustering (hex) SELECT * from h3_get_res_0_indexes();
CREATE INDEX h3_test_clustering_index ON h3_test_clustering USING btree (hex);
CLUSTER h3_test_clustering_index ON h3_test_clustering;
--
-- TEST clustering on B-tree
--
SELECT hex = :hexagon FROM (
    SELECT hex FROM h3_test_clustering WHERE hex = :hexagon
) q;
