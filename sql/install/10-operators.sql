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
-- B-tree Operators
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 0.1.0
CREATE OR REPLACE FUNCTION h3index_eq(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR = (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_eq,
  COMMUTATOR = '=',
  NEGATOR = '<>',
  RESTRICT = eqsel,
  JOIN = eqjoinsel,
  HASHES, MERGES
);

-- Availability: 0.1.0
CREATE OR REPLACE FUNCTION h3index_ne(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <> (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_ne,
  COMMUTATOR = '<>',
  NEGATOR = '=',
  RESTRICT = neqsel,
  JOIN = neqjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_lt(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR < (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_lt,
  COMMUTATOR = > ,
  NEGATOR = >= ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_le(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <= (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_le,
  COMMUTATOR = >= ,
  NEGATOR = > ,
  RESTRICT = scalarltsel,
  JOIN = scalarltjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_gt(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR > (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_gt,
  COMMUTATOR = < ,
  NEGATOR = <= ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_ge(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR >= (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3index_ge,
  COMMUTATOR = <= ,
  NEGATOR = < ,
  RESTRICT = scalargtsel,
  JOIN = scalargtjoinsel
);

-- ---------- ---------- ---------- ---------- ---------- ---------- ----------
-- R-tree Operators
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_overlaps(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR && (
	PROCEDURE = h3index_overlaps,
	LEFTARG = h3index, RIGHTARG = h3index,
	COMMUTATOR = &&,
    RESTRICT = contsel, JOIN = contjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_contains(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR @> (
    PROCEDURE = h3index_contains,
    LEFTARG = h3index, RIGHTARG = h3index,
    COMMUTATOR = <@,
    RESTRICT = contsel, JOIN = contjoinsel
);

-- Availability: 3.6.1
CREATE OR REPLACE FUNCTION h3index_contained_by(h3index, h3index) RETURNS boolean
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;
CREATE OPERATOR <@ (
    PROCEDURE = h3index_contained_by,
    LEFTARG = h3index, RIGHTARG = h3index,
    COMMUTATOR = @>,
    RESTRICT = contsel, JOIN = contjoinsel
);

-- Availability: 3.7.0
CREATE OPERATOR <-> (
  LEFTARG = h3index,
  RIGHTARG = h3index,
  PROCEDURE = h3_distance,
  COMMUTATOR = <->
);
