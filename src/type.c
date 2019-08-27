/*
 * Copyright 2018 Bytes & Brains
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

#include <postgres.h>        // Datum, etc.
#include <fmgr.h>            // PG_FUNCTION_ARGS, etc.
#include <utils/geo_decls.h> // making native points
#include <access/hash.h>     // hash_any

#include <h3api.h> // Main H3 include
#include "extension.h"

Datum h3_string_to_h3(PG_FUNCTION_ARGS);
Datum h3_to_string(PG_FUNCTION_ARGS);


PG_FUNCTION_INFO_V1(h3index_in);
Datum h3index_in(PG_FUNCTION_ARGS)
{
    return h3_string_to_h3(fcinfo);
}

PG_FUNCTION_INFO_V1(h3index_out);
Datum h3index_out(PG_FUNCTION_ARGS)
{
    return h3_to_string(fcinfo);
}

PG_FUNCTION_INFO_V1(h3index_eq);
Datum h3index_eq(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a == *b);
}

PG_FUNCTION_INFO_V1(h3index_ne);
Datum h3index_ne(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a != *b);
}

PG_FUNCTION_INFO_V1(h3index_lt);
Datum h3index_lt(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a < *b);
}

PG_FUNCTION_INFO_V1(h3index_le);
Datum h3index_le(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a <= *b);
}

PG_FUNCTION_INFO_V1(h3index_gt);
Datum h3index_gt(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a > *b);
}

PG_FUNCTION_INFO_V1(h3index_ge);
Datum h3index_ge(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);
    PG_RETURN_BOOL(*a >= *b);
}

PG_FUNCTION_INFO_V1(h3index_cmp);
Datum h3index_cmp(PG_FUNCTION_ARGS)
{
    H3Index *a = PG_GETARG_H3_INDEX_P(0);
    H3Index *b = PG_GETARG_H3_INDEX_P(1);

    if (*a > *b)
    {
        PG_RETURN_INT32(1);
    }
    else if (*a == *b)
    {
        PG_RETURN_INT32(0);
    }
    else
    {
        PG_RETURN_INT32(-1);
    }
}

PG_FUNCTION_INFO_V1(h3index_hash);
Datum h3index_hash(PG_FUNCTION_ARGS)
{
    H3Index *index = PG_GETARG_H3_INDEX_P(0);

    uint32 hash = hash_any((unsigned char *) index, sizeof(H3Index));

    PG_RETURN_INT32(hash);
}