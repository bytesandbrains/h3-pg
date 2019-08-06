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

-- complain if script is sourced in psql, rather than via CREATE EXTENSION
\echo Use "ALTER EXTENSION h3 UPDATE TO '3.6.0'" to load this file. \quit

--- JUST REVERSE EVERYTHING FROM ALPHA

-- Hierarchical grid functions (hierarchy.c)
DROP FUNCTION IF EXISTS h3_to_center_child(h3index, resolution integer);

-- Miscellaneous H3 functions (miscellaneous.c)
DROP FUNCTION IF EXISTS h3_get_pentagon_indexes(resolution integer);

-- PostgreSQL operators
CREATE OR REPLACE FUNCTION h3_string_to_h3(cstring) RETURNS h3index
    AS 'h3', 'h3index_in' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OR REPLACE FUNCTION h3_to_string(h3index) RETURNS cstring
    AS 'h3', 'h3index_out' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- GiST operator class
DROP OPERATOR CLASS gist_h3index_ops USING gist;
DROP FUNCTION IF EXISTS h3index_gist_consistent(internal, h3index, smallint, oid, internal);
DROP FUNCTION IF EXISTS h3index_gist_union(internal, internal);
DROP FUNCTION IF EXISTS h3index_gist_compress(internal);
DROP FUNCTION IF EXISTS h3index_gist_decompress(internal);
DROP FUNCTION IF EXISTS h3index_gist_penalty(internal, internal, internal);
DROP FUNCTION IF EXISTS h3index_gist_picksplit(internal, internal);
DROP FUNCTION IF EXISTS h3index_gist_same(h3index, h3index, internal);
DROP FUNCTION IF EXISTS h3index_gist_distance(internal, h3index, smallint, oid, internal);


-- SP-GiST operator class
DROP OPERATOR CLASS spgist_h3index_ops USING spgist;
DROP FUNCTION IF EXISTS h3index_spgist_config(internal, internal);
DROP FUNCTION IF EXISTS h3index_spgist_choose(internal, internal);
DROP FUNCTION IF EXISTS h3index_spgist_picksplit(internal, internal);
DROP FUNCTION IF EXISTS h3index_spgist_inner_consistent(internal, internal);
DROP FUNCTION IF EXISTS h3index_spgist_leaf_consistent(internal, internal);

-- general
DROP OPERATOR <-> (h3index, h3index);
DROP OPERATOR && (h3index, h3index);
DROP OPERATOR @> (h3index, h3index);
DROP OPERATOR <@ (h3index, h3index);

DROP FUNCTION IF EXISTS h3index_overlap(h3index, h3index);
DROP FUNCTION IF EXISTS h3index_contains(h3index, h3index);
DROP FUNCTION IF EXISTS h3index_contained(h3index, h3index);
