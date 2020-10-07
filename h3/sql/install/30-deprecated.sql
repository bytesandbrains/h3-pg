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

--@ deprecated: 3.7.0
CREATE OR REPLACE FUNCTION h3_hex_area(resolution integer, km boolean) RETURNS float
    AS $$ SELECT h3_hex_area($1, CASE WHEN $2 THEN 'km' ELSE 'm' END) $$
    IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;

--@ deprecated: 3.7.0
CREATE OR REPLACE FUNCTION h3_edge_length(resolution integer, km boolean) RETURNS float
    AS $$ SELECT h3_edge_length($1, CASE WHEN $2 THEN 'km' ELSE 'm' END) $$
    IMMUTABLE STRICT PARALLEL SAFE LANGUAGE SQL;
