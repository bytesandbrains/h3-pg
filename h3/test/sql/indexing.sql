\pset tuples_only on

-- neighbouring indexes (one hexagon, one pentagon) at resolution 3
\set geo POINT(-144.52399108028, 49.7165031828995)
\set hexagon '\'831c02fffffffff\'::h3index'
\set pentagon '\'831c00fffffffff\'::h3index'
\set edgecross '\'8003fffffffffff\'::h3index'
\set resolution 3

--
-- TEST h3_to_geo and h3_geo_to_h3
--

-- convertion to geo works
SELECT h3_to_geo(:hexagon) ~= :geo;

-- convertion to h3 index works
SELECT h3_geo_to_h3(:geo, :resolution) = :hexagon;

-- h3_to_geo is inverse of h3_geo_to_h3
SELECT h3_to_geo(i) ~= :geo AND h3_get_resolution(i) = :resolution FROM (
    SELECT h3_geo_to_h3(:geo, :resolution) AS i
) AS q;
-- h3_geo_to_h3 is inverse of h3_to_geo
SELECT h3_geo_to_h3(g, r) = :hexagon FROM (
    SELECT h3_to_geo(:hexagon) AS g, h3_get_resolution(:hexagon) AS r
) AS q;
-- same for pentagon
SELECT h3_geo_to_h3(g, r) = :pentagon FROM (
    SELECT h3_to_geo(:pentagon) AS g, h3_get_resolution(:pentagon) AS r
) AS q;

--
-- TEST h3_to_geo_boundary
--

-- polyfill of geo boundary returns original index
SELECT h3_polyfill(h3_to_geo_boundary(:hexagon), null, :resolution) = :hexagon;

-- same for pentagon
SELECT h3_polyfill(h3_to_geo_boundary(:pentagon), null, :resolution) = :pentagon;

-- the boundary of an edgecrossing index is different with flag set to true
SELECT h3_to_geo_boundary(:hexagon) ~= h3_to_geo_boundary(:hexagon, true)
AND NOT h3_to_geo_boundary(:edgecross) ~= h3_to_geo_boundary(:edgecross, true);
