/*
 * Copyright 2018-2020 Bytes & Brains
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *	   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#include <postgres.h>		 // Datum, etc.
#include <fmgr.h>			 // PG_FUNCTION_ARGS, etc.
#include <utils/geo_decls.h> // making native points

#include <h3api.h> // Main H3 include
#include "extension.h"
#include <math.h>

PGDLLEXPORT Datum h3_geo_to_h3(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_to_geo(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_to_geo_boundary(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(h3_geo_to_h3);
PG_FUNCTION_INFO_V1(h3_to_geo);
PG_FUNCTION_INFO_V1(h3_to_geo_boundary);

/* Indexes the location at the specified resolution */
Datum
h3_geo_to_h3(PG_FUNCTION_ARGS)
{
	Point	   *geo = PG_GETARG_POINT_P(0);
	int			resolution = PG_GETARG_INT32(1);

	H3Index		idx;
	GeoCoord	location;

	if (h3_guc_strict)
	{
		ASSERT_EXTERNAL(geo->x >= -180 && geo->x <= 180, "Longitude must be between -180 and 180 degrees inclusive, but got %f.", geo->x);
		ASSERT_EXTERNAL(geo->y >= -90 && geo->y <= 90, "Latitude must be between -90 and 90 degrees inclusive, but got %f.", geo->y);
	}

	location.lon = degsToRads(geo->x);
	location.lat = degsToRads(geo->y);

	idx = geoToH3(&location, resolution);
	ASSERT_EXTERNAL(idx, "Indexing failed at specified resolution.");

	PG_FREE_IF_COPY(geo, 0);
	PG_RETURN_H3INDEX(idx);
}

/* Finds the centroid of the index */
Datum
h3_to_geo(PG_FUNCTION_ARGS)
{
	H3Index		idx = PG_GETARG_H3INDEX(0);

	Point	   *geo = palloc(sizeof(Point));
	GeoCoord	center;

	h3ToGeo(idx, &center);

	geo->x = radsToDegs(center.lon);
	geo->y = radsToDegs(center.lat);

	PG_RETURN_POINT_P(geo);
}

/* Finds the boundary of the index */
Datum
h3_to_geo_boundary(PG_FUNCTION_ARGS)
{
	H3Index		idx = PG_GETARG_H3INDEX(0);
	bool		extend = PG_GETARG_BOOL(1);

	double		delta,
				firstLon,
				lon,
				lat;

	int			size;
	POLYGON    *polygon;
	GeoBoundary boundary;

	h3ToGeoBoundary(idx, &boundary);

	size = offsetof(POLYGON, p) +sizeof(polygon->p[0]) * boundary.numVerts;
	polygon = (POLYGON *) palloc(size);
	SET_VARSIZE(polygon, size);
	polygon->npts = boundary.numVerts;

	firstLon = boundary.verts[0].lon;
	if (firstLon < 0)
	{
		delta = -2 * M_PI;
	}
	else
	{
		delta = +2 * M_PI;
	}

	for (int v = 0; v < boundary.numVerts; v++)
	{
		lon = boundary.verts[v].lon;
		lat = boundary.verts[v].lat;

		/* check if different sign */
		if (extend && fabs(lon - firstLon) > M_PI)
			lon = lon + delta;

		polygon->p[v].x = radsToDegs(lon);
		polygon->p[v].y = radsToDegs(lat);
	}

	PG_RETURN_POLYGON_P(polygon);
}
