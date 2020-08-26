/*
 * Copyright 2020 Bytes & Brains
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

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "ALTER EXTENSION h3 UPDATE TO 'unreleased'" to load this file. \quit

CREATE OPERATOR <-> (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3_distance,
  COMMUTATOR = <->
);

-- Broken since 1.0.0 on update path
CREATE OR REPLACE FUNCTION h3_to_geometry(h3index) RETURNS geometry
  AS $$ SELECT ST_SetSRID(h3_to_geo($1)::geometry, 4326) $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;
CREATE OR REPLACE FUNCTION h3_to_geography(h3index) RETURNS geography
  AS $$ SELECT h3_to_geometry($1)::geography $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

DROP FUNCTION IF EXISTS h3_geo_to_h3(point, integer);
CREATE OR REPLACE FUNCTION h3_geo_to_h3(point, resolution integer, strict BOOLEAN default FALSE) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_geo_to_h3(point, integer, boolean) IS
'Indexes the location at the specified resolution';

DROP FUNCTION IF EXISTS h3_geo_to_h3(geometry, integer);
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geometry, resolution integer, strict BOOLEAN default FALSE) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::point, $2, $3); $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

DROP FUNCTION IF EXISTS h3_geo_to_h3(geography, integer);
CREATE OR REPLACE FUNCTION h3_geo_to_h3(geography, resolution integer, strict BOOLEAN default FALSE) RETURNS h3index
    AS $$ SELECT h3_geo_to_h3($1::geometry, $2, $3); $$ IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;