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

#include <postgres.h> // Datum, etc.
#include <fmgr.h>     // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>  // Definitions for functions which return sets

#include <h3api.h> // Main H3 include
#include "extension.h"

// Average hexagon area in square (kilo)meters at the given resolution
PG_FUNCTION_INFO_V1(h3_hex_area);
Datum h3_hex_area(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    int km = PG_GETARG_BOOL(1);
    double area;
    if (km) {
        area = hexAreaKm2(resolution);
    } else {
        area = hexAreaM2(resolution);
    }
    PG_RETURN_FLOAT8(area);
}


// Average hexagon edge length in (kilo)meters at the given resolution
PG_FUNCTION_INFO_V1(h3_edge_length);
Datum h3_edge_length(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    bool km = PG_GETARG_BOOL(1);
    double length;
    if (km) {
        length = edgeLengthKm(resolution);
    } else {
        length = edgeLengthM(resolution);
    }
    PG_RETURN_FLOAT8(length);
}


// Number of unique H3 indexes at the given resolution
PG_FUNCTION_INFO_V1(h3_num_hexagons);
Datum h3_num_hexagons(PG_FUNCTION_ARGS)
{
    int resolution = PG_GETARG_INT32(0);
    unsigned long long retVal = numHexagons(resolution);
    PG_RETURN_INT64(retVal);
}

// Provides all resolution 0 indexes
PG_FUNCTION_INFO_V1(h3_get_res_0_indexes);
Datum h3_get_res_0_indexes(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        int count = res0IndexCount();
        H3Index *indexes = palloc(sizeof(H3Index) * count);
        getRes0Indexes(indexes);

        funcctx->user_fctx = indexes;
        funcctx->max_calls = count;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}
