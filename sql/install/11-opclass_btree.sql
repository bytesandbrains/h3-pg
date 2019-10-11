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
-- B-tree Operator Class (opclass_btree.c)
-- ---------- ---------- ---------- ---------- ---------- ---------- ----------

-- Availability: 0.1.0
CREATE OR REPLACE FUNCTION h3index_cmp(h3index, h3index) RETURNS integer
    AS 'h3' LANGUAGE C IMMUTABLE STRICT PARALLEL SAFE;

-- Availability: 0.1.0
CREATE OPERATOR CLASS btree_h3index_ops DEFAULT FOR TYPE h3index USING btree AS
    OPERATOR  1  <  ,
    OPERATOR  2  <= ,
    OPERATOR  3   = ,
    OPERATOR  4  >= ,
    OPERATOR  5  >  ,
    FUNCTION  1  h3index_cmp(h3index, h3index);
