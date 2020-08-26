/*
 * Copyright 2018-2019 Bytes & Brains
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
-- Indexing Functions (indexing.c)
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_geo_to_h3(point, resolution integer, strict BOOLEAN default FALSE) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_geo_to_h3(point, resolution integer, boolean) IS
'Indexes the location at the specified resolution';

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geo(h3index) RETURNS point
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_to_geo(h3index) IS
'Finds the centroid of the index';

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_to_geo_boundary(h3index, extend_at_meridian BOOLEAN default FALSE) RETURNS polygon
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_to_geo_boundary(h3index, boolean) IS
'Finds the boundary of the index, second argument extends coordinates when crossing 180th meridian to help visualization';