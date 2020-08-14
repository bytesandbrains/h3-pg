\pset tuples_only on
-- Variables for testing
\set resolution 10
\set hexagon '\'8a63a9a99047fff\''

SELECT h3_geo_to_h3(h3_to_geo(:hexagon)::geometry, :resolution) = '8a63a9a99047fff';

SELECT ST_NPoints( h3_to_geo_boundary_geometry(:hexagon)) = 7;
