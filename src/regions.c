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

#include <postgres.h>            // Datum, etc.
#include <fmgr.h>                // PG_FUNCTION_ARGS, etc.
#include <funcapi.h>             // Definitions for functions which return sets
#include <access/htup_details.h> // Needed to return HeapTuple
#include <utils/array.h>         // using arrays
#include <utils/geo_decls.h>     // making native points
#include <utils/lsyscache.h>
#include <utils/memutils.h>
#include <catalog/pg_type.h>

#include <h3api.h> // Main H3 include
#include "extension.h"

/** @struct GeoPolygon
 *  @brief Simplified core of GeoJSON Polygon coordinates definition
 */
/*
typedef struct {
    double lat;  ///< latitude in radians
    double lon;  ///< longitude in radians
} GeoCoord;

typedef struct {
    int numVerts;
    GeoCoord *verts;
} Geofence;

typedef struct {
    Geofence geofence;  ///< exterior boundary of the polygon
    int numHoles;       ///< number of elements in the array pointed to by holes
    Geofence *holes;    ///< interior boundaries (holes) in the polygon
} GeoPolygon;
*/

/** @struct LinkedGeoCoord
 *  @brief A coordinate node in a linked geo structure, part of a linked list
 *
typedef struct LinkedGeoCoord LinkedGeoCoord;
struct LinkedGeoCoord
{
    GeoCoord vertex;
    LinkedGeoCoord *next;
};

** @struct LinkedGeoLoop
 *  @brief A loop node in a linked geo structure, part of a linked list
 *
typedef struct LinkedGeoLoop LinkedGeoLoop;
struct LinkedGeoLoop
{
    LinkedGeoCoord *first;
    LinkedGeoCoord *last;
    LinkedGeoLoop *next;
};

** @struct LinkedGeoPolygon
 *  @brief A polygon node in a linked geo structure, part of a linked list.
 *
typedef struct LinkedGeoPolygon LinkedGeoPolygon;
struct LinkedGeoPolygon
{
    LinkedGeoLoop *first;
    LinkedGeoLoop *last;
    LinkedGeoPolygon *next;
};
*/

static void polygonToGeofence(POLYGON *polygon, Geofence *geofence)
{
    geofence->numVerts = polygon->npts;
    geofence->verts = (GeoCoord *)palloc(geofence->numVerts * sizeof(GeoCoord));

    for (int i = 0; i < geofence->numVerts; i++)
    {
        geofence->verts[i].lon = degsToRads(polygon->p[i].x);
        geofence->verts[i].lat = degsToRads(polygon->p[i].y);
    }
}

static int linkedGeoLoopToNativePolygonSize(LinkedGeoLoop *linkedLoop)
{
    int count = 0;
    LinkedGeoCoord *linkedCoord = linkedLoop->first;
    while (linkedCoord != NULL)
    {
        count++;
        linkedCoord = linkedCoord->next;
    }
    return count;
}

static void linkedGeoLoopToNativePolygon(LinkedGeoLoop *linkedLoop, POLYGON *polygon)
{
    int count;
    LinkedGeoCoord *linkedCoord = linkedLoop->first;
    count = 0;
    while (linkedCoord != NULL)
    {
        (polygon->p[count]).x = radsToDegs(linkedCoord->vertex.lon);
        (polygon->p[count]).y = radsToDegs(linkedCoord->vertex.lat);
        linkedCoord = linkedCoord->next;
        count++;
    }
}

/**
 * void polyfill(const GeoPolygon* geoPolygon, int res, H3Index* out);
 */
PG_FUNCTION_INFO_V1(h3_polyfill);
Datum h3_polyfill(PG_FUNCTION_ARGS)
{
    if (SRF_IS_FIRSTCALL())
    {
        FuncCallContext *funcctx = SRF_FIRSTCALL_INIT();
        MemoryContext oldcontext =
            MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        int maxSize;
        H3Index *indices;
        ArrayType *holes;
        int nelems = 0;
        int resolution;
        GeoPolygon polygon;
        Datum value;
        bool isnull;

        // get function arguments
        POLYGON *exterior = PG_GETARG_POLYGON_P(0);
        if (!PG_ARGISNULL(1))
        {
            holes = PG_GETARG_ARRAYTYPE_P(1);
            nelems = ArrayGetNItems(ARR_NDIM(holes), ARR_DIMS(holes));
        }
        resolution = PG_GETARG_INT32(2);

        // build polygon
        polygonToGeofence(exterior, &(polygon.geofence));
        
        if (nelems)
        {
            int i = 0;
            ArrayIterator iterator = array_create_iterator(holes, 0, NULL);

            polygon.numHoles = nelems;
            polygon.holes = (Geofence *)palloc(polygon.numHoles * sizeof(Geofence));

            while( array_iterate(iterator, &value, &isnull) )
            {
                POLYGON *hole = DatumGetPolygonP(value);
                polygonToGeofence(hole, &(polygon.holes[i]));
                i++;
            }
        }
        else
        {
            polygon.numHoles = 0;
        }

        // produce hexagons into allocated memory
        maxSize = maxPolyfillSize(&polygon, resolution);
        indices = palloc0(maxSize * sizeof(H3Index));
        polyfill(&polygon, resolution, indices);

        funcctx->user_fctx = indices;
        funcctx->max_calls = maxSize;
        MemoryContextSwitchTo(oldcontext);
    }

    SRF_RETURN_H3_INDEXES_FROM_USER_FCTX();
}

/**
 * void polyfill(const GeoPolygon* geoPolygon, int res, H3Index* out);
 *
 *
 * https://stackoverflow.com/questions/51127189/how-to-return-array-into-array-with-custom-type-in-postgres-c-function
 */
PG_FUNCTION_INFO_V1(h3_set_to_multi_polygon);
Datum h3_set_to_multi_polygon(PG_FUNCTION_ARGS)
{
    FuncCallContext *funcctx;
    TupleDesc tuple_desc;

    LinkedGeoPolygon *linkedPolygon;
    LinkedGeoLoop *linkedLoop;

    if (SRF_IS_FIRSTCALL())
    {
        MemoryContext oldcontext;
        ArrayType *array;
        int numHexes;
        H3Index *h3Set;
        H3Index *idx;

        funcctx = SRF_FIRSTCALL_INIT();
        oldcontext = MemoryContextSwitchTo(funcctx->multi_call_memory_ctx);

        ENSURE_TYPEFUNC_COMPOSITE(get_call_result_type(fcinfo, NULL, &tuple_desc));

        // get function arguments
        array = PG_GETARG_ARRAYTYPE_P(0);

        numHexes = ArrayGetNItems(ARR_NDIM(array), ARR_DIMS(array));
        h3Set = palloc(sizeof(H3Index) * numHexes);
        idx = (H3Index *)ARR_DATA_PTR(array);

        for (int i = 0; i < numHexes; i++)
        {
            h3Set[i] = fetch_att(idx, true, sizeof(H3Index));
            idx++;
        }

        // produce hexagons into allocated memory
        linkedPolygon = palloc(sizeof(LinkedGeoPolygon));
        h3SetToLinkedGeo(h3Set, numHexes, linkedPolygon);

        funcctx->user_fctx = linkedPolygon;
        funcctx->tuple_desc = BlessTupleDesc(tuple_desc);
        MemoryContextSwitchTo(oldcontext);
    }

    funcctx = SRF_PERCALL_SETUP();
    linkedPolygon = (LinkedGeoPolygon *)funcctx->user_fctx;

    if (linkedPolygon)
    {
        HeapTuple tuple;
        Datum result;
        int count;
        int size;
        POLYGON *polygon;

        Datum *elems;
        Datum values[2];
        bool nulls[2];

        int16 typlen;
        bool typbyval;
        char typalign;
        ArrayType *retarr;

        tuple_desc = funcctx->tuple_desc;

        linkedLoop = linkedPolygon->first;
        count = linkedGeoLoopToNativePolygonSize(linkedLoop);
        size = offsetof(POLYGON, p) + sizeof(polygon->p[0]) * count;
        polygon = palloc0(size);
        SET_VARSIZE(polygon, size);
        polygon->npts = count;
        linkedGeoLoopToNativePolygon(linkedLoop, polygon);

        values[0] = PolygonPGetDatum(polygon);
        nulls[0] = false;

        // construct array
        count = 0;
        linkedLoop = linkedPolygon->first->next;
        while (linkedLoop != NULL)
        {
            count++;
            linkedLoop = linkedLoop->next;
        }
        elems = (Datum *)palloc(count * sizeof(Datum));
        if (count)
        {
            linkedLoop = linkedPolygon->first->next;
            for (int i = 0; i < count; i++)
            {
                int subcount = linkedGeoLoopToNativePolygonSize(linkedLoop);
                POLYGON *polygon;
                int size = offsetof(POLYGON, p) + sizeof(polygon->p[0]) * subcount;
                polygon = palloc0(size);
                SET_VARSIZE(polygon, size);
                polygon->npts = subcount;
                linkedGeoLoopToNativePolygon(linkedLoop, polygon);
                elems[i] = PolygonPGetDatum(polygon);
                linkedLoop = linkedLoop->next;
            }
        }

        get_typlenbyvalalign(POLYGONOID, &typlen, &typbyval, &typalign);
        retarr =
            construct_array(elems, count, POLYGONOID, typlen, typbyval, typalign);
        values[1] = PointerGetDatum(retarr);
        nulls[1] = false;

        tuple = heap_form_tuple(tuple_desc, values, nulls);
        result = HeapTupleGetDatum(tuple);

        funcctx->user_fctx = linkedPolygon->next;
        SRF_RETURN_NEXT(funcctx, result);
    }
    else
    {
        destroyLinkedPolygon(linkedPolygon);
        SRF_RETURN_DONE(funcctx);
    }
}
