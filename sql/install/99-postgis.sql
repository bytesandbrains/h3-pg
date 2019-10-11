/*
 * Copyright 2019 Bytes & Brains
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *     http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

-- ---------- ---------- ---------- ---------- ---------- ---------- ----------
-- PostGIS Functions
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 0.3.0
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geometry, resolution integer) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::point, $2); $$ LANGUAGE SQL;

-- Availability: 0.3.0
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geography, resolution integer) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::geometry, $2); $$ LANGUAGE SQL;

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geometry(h3index) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_to_geo($1)::geometry, 4326) $$ LANGUAGE SQL;

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geography(h3index) RETURNS geography
  AS $$ SELECT h3_to_geometry($1)::geography $$ LANGUAGE SQL;

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geo_boundary_geometry(h3index, extend BOOLEAN default FALSE) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_to_geo_boundary($1, $2)::geometry, 4326) $$ LANGUAGE SQL;

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geo_boundary_geography(h3index, extend BOOLEAN default FALSE) RETURNS geography
  AS $$ SELECT h3_to_geo_boundary_geometry($1, $2)::geography $$ LANGUAGE SQL;

-- Availability: 0.3.0
CREATE OR REPLACE FUNCTION h3_polyfill(multi geometry, resolution integer) RETURNS SETOF h3index
    AS $$ SELECT h3_polyfill(exterior, holes, resolution) FROM (
        SELECT 
            -- extract exterior ring of each polygon
            ST_MakePolygon(ST_ExteriorRing(poly))::polygon exterior,
            -- extract holes of each polygon
            (SELECT array_agg(hole)
                FROM (
                    SELECT ST_MakePolygon(ST_InteriorRingN(
                        poly,
                        generate_series(1, ST_NumInteriorRings(poly))
                    ))::polygon AS hole
                ) q_hole
            ) holes
        -- extract single polygons from multipolygon
        FROM (
            select (st_dump(multi)).geom as poly
        ) q_poly GROUP BY poly
    ) h3_polyfill; $$ LANGUAGE SQL IMMUTABLE STRICT;

-- Availability: 0.3.0
CREATE OR REPLACE FUNCTION h3_polyfill(multi geography, resolution integer) RETURNS SETOF h3index
AS $$ SELECT h3_polyfill($1::geometry, $2) $$ LANGUAGE SQL;

-- ---------- ---------- ---------- ---------- ---------- ---------- ----------
-- PostGIS Cast
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 0.3.0
CREATE CAST (h3index AS point) WITH FUNCTION h3_to_geo(h3index);

-- Availability: 0.3.0
CREATE CAST (h3index AS geometry) WITH FUNCTION h3_to_geometry(h3index);

-- Availability: 0.3.0
CREATE CAST (h3index AS geography) WITH FUNCTION h3_to_geography(h3index);
