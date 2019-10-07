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
\echo Use "ALTER EXTENSION h3 UPDATE TO '3.5.1'" to load this file. \quit

CREATE OR REPLACE FUNCTION h3index_to_bigint(h3index) RETURNS bigint
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OR REPLACE FUNCTION bigint_to_h3index(bigint) RETURNS h3index
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- type casts
CREATE CAST (h3index AS bigint) WITH FUNCTION h3index_to_bigint(h3index);
CREATE CAST (bigint AS h3index) WITH FUNCTION bigint_to_h3index(bigint);