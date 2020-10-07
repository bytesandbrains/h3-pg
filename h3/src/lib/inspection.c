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
#include <utils/array.h>	 // Arrays
#include <utils/lsyscache.h> // get_typlenbyvalalign
#include <catalog/pg_type.h>

#include <h3api.h> // Main H3 include
#include "extension.h"

PGDLLEXPORT Datum h3_get_resolution(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_get_base_cell(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_is_valid(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_is_res_class_iii(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_is_pentagon(PG_FUNCTION_ARGS);
PGDLLEXPORT Datum h3_get_faces(PG_FUNCTION_ARGS);

PG_FUNCTION_INFO_V1(h3_get_resolution);
PG_FUNCTION_INFO_V1(h3_get_base_cell);
PG_FUNCTION_INFO_V1(h3_is_valid);
PG_FUNCTION_INFO_V1(h3_is_res_class_iii);
PG_FUNCTION_INFO_V1(h3_is_pentagon);
PG_FUNCTION_INFO_V1(h3_get_faces);

/* Returns the resolution of the index */
Datum
h3_get_resolution(PG_FUNCTION_ARGS)
{
	H3Index		hex = PG_GETARG_H3INDEX(0);
	int			resolution = h3GetResolution(hex);

	PG_RETURN_INT32(resolution);
}

/* Returns the base cell number of the index */
Datum
h3_get_base_cell(PG_FUNCTION_ARGS)
{
	H3Index		hex = PG_GETARG_H3INDEX(0);
	int			base_cell_number = h3GetBaseCell(hex);

	PG_RETURN_INT32(base_cell_number);
}

/* Returns true if this is a valid H3 index */
Datum
h3_is_valid(PG_FUNCTION_ARGS)
{
	H3Index		hex = PG_GETARG_H3INDEX(0);
	bool		isValid = h3IsValid(hex);

	PG_RETURN_BOOL(isValid);
}

/* Returns true if this index has a resolution with Class III orientation */
Datum
h3_is_res_class_iii(PG_FUNCTION_ARGS)
{
	H3Index		hex = PG_GETARG_H3INDEX(0);
	bool		isResClassIII = h3IsResClassIII(hex);

	PG_RETURN_BOOL(isResClassIII);
}

/* Returns true if this hex represents a pentagonal cell */
Datum
h3_is_pentagon(PG_FUNCTION_ARGS)
{
	H3Index		hex = PG_GETARG_H3INDEX(0);
	bool		isPentagon = h3IsPentagon(hex);

	PG_RETURN_BOOL(isPentagon);
}

/* Find all icosahedron faces intersected by a given H3 index */
Datum
h3_get_faces(PG_FUNCTION_ARGS)
{
	Oid			elmtype = INT4OID;
	int16		elmlen;
	bool		elmbyval;
	char		elmalign;

	H3Index		hex = PG_GETARG_H3INDEX(0);
	int			maxFaces = maxFaceCount(hex);

	ArrayType  *result;

	int			nelems = 0;

	/* get the faces */
	int		   *faces = palloc(maxFaces * sizeof(int));
	Datum	   *elements = palloc(maxFaces * sizeof(Datum));

	h3GetFaces(hex, faces);

	for (int i = 0; i < maxFaces; i++)
	{
		int			face = faces[i];

		/* add any valid face to result array */
		if (face > -1)
			elements[nelems++] = Int32GetDatum(face);
	}

	/* build the array */
	get_typlenbyvalalign(elmtype, &elmlen, &elmbyval, &elmalign);
	result = construct_array(elements, nelems, elmtype, elmlen, elmbyval, elmalign);
	PG_RETURN_ARRAYTYPE_P(result);
}
