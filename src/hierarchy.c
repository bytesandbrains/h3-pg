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

#include <postgres.h>       // Datum, etc.
#include <fmgr.h>           // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>        // Definitions for functions which return sets
#include <utils/array.h>    // Arrays
#include <utils/memutils.h> // MaxAllocSize

#include <h3api.h> // Main H3 include
#include "extension.h"

// Returns the parent (coarser) index containing given index
PG_FUNCTION_INFO_V1(h3_to_parent);
Datum h3_to_parent(PG_FUNCTION_ARGS)
{
    H3Index *parent;

    // get function arguments
    H3Index *origin = PG_GETARG_H3_INDEX_P(0);
    int parentRes = PG_GETARG_INT32(1);
    int childRes = h3GetResolution(*origin);
    if (parentRes == -1)
    { // resolution parameter not set
        parentRes = childRes - 1;
    }
    ASSERT(
        parentRes <= childRes,
        ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
        "Requested parent resolution %d is finer than input index resolution %d",
        parentRes, childRes
    );

    // get parent
    parent = palloc(sizeof(H3Index));
    *parent = h3ToParent(*origin, parentRes);
    ASSERT_EXTERNAL(*parent, "Could not generate parent");

    PG_RETURN_H3_INDEX_P(parent);
}

// Returns children indexes at given resolution (or next resolution if none given)
PG_FUNCTION_INFO_V1(h3_to_children);
Datum h3_to_children(PG_FUNCTION_ARGS)
{
    // stuff done only on the first call of the function
    if (SRF_IS_FIRSTCALL())
    {
        int maxSize;
        int size;
        H3Index *children;

        // create a function context for cross-call persistence
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();

        // switch to memory context appropriate for multiple function calls
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        // BEGIN One-time setup code

        // ensure valid resolution target
        H3Index *origin = PG_GETARG_H3_INDEX_P(0);
        int resolution = PG_GETARG_INT32(1);
        if (resolution == -1)
        { // resolution parameter not set
            resolution = h3GetResolution(*origin) + 1;
        }
        ASSERT(
            resolution <= MAX_H3_RES,
            ERRCODE_NUMERIC_VALUE_OUT_OF_RANGE,
            "Maximum resolution exceeded"
        );

        maxSize = maxH3ToChildrenSize(*origin, resolution);
        size = maxSize * sizeof(H3Index);
        ASSERT(
            AllocSizeIsValid(size),
            ERRCODE_OUT_OF_MEMORY,
            "Cannot allocate necessary amount memory, try using h3_to_children_slow()"
        );

        children = palloc(size);
        h3ToChildren(*origin, resolution, children);
        ASSERT_EXTERNAL(*children, "Could not generate children");

        funcctx->user_fctx = children;
        funcctx->max_calls = maxSize;

        // END One-time setup code

        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

PG_FUNCTION_INFO_V1(h3_compact);
Datum h3_compact(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        int result;
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);

        int arrayLength = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        H3Index *h3set = palloc(sizeof(H3Index) * arrayLength);
        H3Index *idx = (H3Index *)ARR_DATA_PTR(array);

        int maxSize = arrayLength;
        H3Index *compactedSet = palloc0(maxSize * sizeof(H3Index));

        // Extract data from array into h3set, and wipe compactedSet memory
        for (int i = 0; i < arrayLength; i++)
        {
            h3set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        result = compact(h3set, compactedSet, arrayLength);
        ASSERT_EXTERNAL(result == 0, "Could not compact input array");

        funcctx->user_fctx = compactedSet;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

PG_FUNCTION_INFO_V1(h3_uncompact);
Datum h3_uncompact(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        int result;
        int maxSize;
        H3Index *uncompactedSet;

        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        ArrayType *array = PG_GETARG_ARRAYTYPE_P(0);
        int resolution = PG_GETARG_INT32(1);

        int arrayLength = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        H3Index *h3set = palloc(sizeof(H3Index) * arrayLength);
        H3Index *idx = (H3Index *)ARR_DATA_PTR(array);

        // Extract data from array into h3set, and wipe compactedSet memory
        for (int i = 0; i < arrayLength; i++)
        {
            h3set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        if (resolution == -1)
        { // resolution parameter not set
            int highRes = 0;
            // Find highest resolution in the given set
            for (int i = 0; i < arrayLength; i++)
            {
                int curRes = h3GetResolution(h3set[i]);
                if (curRes > highRes)
                {
                    highRes = curRes;
                }
            }
            // If the highest resolution is the maximun allowed, uncompact to that
            // Else uncompact one step further than the highest resolution
            resolution = (highRes == 15 ? highRes : highRes + 1);
        }

        maxSize = maxUncompactSize(h3set, arrayLength, resolution);
        uncompactedSet = palloc0(maxSize * sizeof(H3Index));

        result = uncompact(h3set, arrayLength, uncompactedSet, maxSize, resolution);
        ASSERT_EXTERNAL(result == 0,
            "Could not uncompact input array. This may be caused by choosing a lower resolution than some of the indexes"
        );

        funcctx->user_fctx = uncompactedSet;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}