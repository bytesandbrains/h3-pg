\pset tuples_only on
-- Variables for testing
\set resolution 10
\set hexagon '\'8a63a9a99047fff\''
\set meter ST_SetSRID(ST_Point(6196902.235389061,1413172.0833316022), 3857)
\set degree ST_SetSRID(ST_Point(55.6677199224442,12.592131261648213), 4326)

SELECT h3_geo_to_h3(:meter, :resolution) = '8a63a9a99047fff';
SELECT h3_geo_to_h3(:degree, :resolution) = '8a63a9a99047fff';

SELECT h3_geo_to_h3(h3_to_geometry(:hexagon), :resolution) = '8a63a9a99047fff';

SELECT ST_NPoints( h3_to_geo_boundary_geometry(:hexagon)) = 7;

-- test h3_geo_to_h3 throws for srid-less geometry
CREATE FUNCTION h3_test_postgis_nounit() RETURNS boolean LANGUAGE PLPGSQL
    AS $$
        BEGIN
            PERFORM h3_geo_to_h3(ST_Point(6196902, 1413172), 1);
            RETURN false;
        EXCEPTION WHEN OTHERS THEN
            RETURN true;
        END;
    $$;

SELECT h3_test_postgis_nounit();