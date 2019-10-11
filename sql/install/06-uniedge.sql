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
-- Unidirectional Edge Functions (uniedges.c)
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_indexes_are_neighbors(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_indexes_are_neighbors(h3index, h3index) IS
'Returns true if the given indices are neighbors';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_h3_unidirectional_edge(origin h3index, destination h3index) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_h3_unidirectional_edge(origin h3index, destination h3index) IS
'Returns a unidirectional edge H3 index based on the provided origin and destination.';

-- Availability: 1.0.0
CREATE OR REPLACE FUNCTION h3_unidirectional_edge_is_valid(edge h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_unidirectional_edge_is_valid(edge h3index) IS
'Returns true if the given edge is valid.';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_origin_h3_index_from_unidirectional_edge(edge h3index) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_origin_h3_index_from_unidirectional_edge(edge h3index) IS
'Returns the origin index from the given edge.';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_destination_h3_index_from_unidirectional_edge(edge h3index) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_destination_h3_index_from_unidirectional_edge(edge h3index) IS
'Returns the destination index from the given edge.';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_h3_indexes_from_unidirectional_edge(edge h3index, OUT origin h3index, OUT destination h3index) RETURNS record
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_h3_indexes_from_unidirectional_edge(edge h3index) IS
'Returns the pair of indices from the given edge.';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_h3_unidirectional_edges_from_hexagon(h3index) RETURNS SETOF h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_h3_unidirectional_edges_from_hexagon(h3index) IS
'Returns all unidirectional edges with the given index as origin';

-- Availability: 0.2.0
CREATE OR REPLACE FUNCTION h3_get_h3_unidirectional_edge_boundary(edge h3index) RETURNS polygon
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
    COMMENT ON FUNCTION h3_get_h3_unidirectional_edge_boundary(edge h3index) IS
'Provides the coordinates defining the unidirectional edge.';
