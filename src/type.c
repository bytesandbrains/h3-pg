/*
 * Copyright 2018-2019 Bytes & Brains
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
#include <access/hash.h>	 // hash_any

#include <h3api.h> // Main H3 include
#include "extension.h"

/* conversion */
PG_FUNCTION_INFO_V1(h3index_in);
PG_FUNCTION_INFO_V1(h3index_out);
PG_FUNCTION_INFO_V1(h3index_to_bigint);
PG_FUNCTION_INFO_V1(bigint_to_h3index);

/* b-tree */
PG_FUNCTION_INFO_V1(h3index_eq);
PG_FUNCTION_INFO_V1(h3index_ne);
PG_FUNCTION_INFO_V1(h3index_lt);
PG_FUNCTION_INFO_V1(h3index_le);
PG_FUNCTION_INFO_V1(h3index_gt);
PG_FUNCTION_INFO_V1(h3index_ge);

/* r-tree */
PG_FUNCTION_INFO_V1(h3index_overlaps);
PG_FUNCTION_INFO_V1(h3index_contains);
PG_FUNCTION_INFO_V1(h3index_contained_by);

/* static helpers */
static int
containment(H3Index * a, H3Index * b)
{
	int			aRes = h3GetResolution(*a);
	int			bRes = h3GetResolution(*b);
	H3Index		aParent = h3ToParent(*a, bRes);
	H3Index		bParent = h3ToParent(*b, aRes);

	/* a contains b */
	if (*a == bParent)
		return 1;

	/* a contained by b */
	if (*b == aParent)
		return -1;

	/* no overlap */
	return 0;
}

/* textual input/output functions */
Datum
h3index_in(PG_FUNCTION_ARGS)
{
	char	   *str = PG_GETARG_CSTRING(0);
	H3Index    *hex = palloc(sizeof(H3Index));

	*hex = stringToH3(str);

	PG_RETURN_H3_INDEX_P(hex);
}

Datum
h3index_out(PG_FUNCTION_ARGS)
{
	H3Index    *hex = PG_GETARG_H3_INDEX_P(0);
	char	   *str = palloc(17 * sizeof(char));

	h3ToString(*hex, str, 17);

	PG_RETURN_CSTRING(str);
}

/* bigint conversion functions */
Datum
h3index_to_bigint(PG_FUNCTION_ARGS)
{
	H3Index    *h3index = PG_GETARG_H3_INDEX_P(0);

	PG_RETURN_INT64(*h3index);
}

Datum
bigint_to_h3index(PG_FUNCTION_ARGS)
{
	int64		bigint = PG_GETARG_INT64(0);
	H3Index    *h3index = palloc(sizeof(H3Index));

	*h3index = bigint;
	PG_RETURN_H3_INDEX_P(h3index);
}

/* b-tree operators */
Datum
h3index_eq(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a == *b);
}

Datum
h3index_ne(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a != *b);
}

Datum
h3index_lt(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a < *b);
}

Datum
h3index_le(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a <= *b);
}

Datum
h3index_gt(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a > *b);
}

Datum
h3index_ge(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(*a >= *b);
}

/* r-tree operators */
Datum
h3index_overlaps(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(containment(a, b) != 0);
}

Datum
h3index_contains(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(containment(a, b) > 0);
}

Datum
h3index_contained_by(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);

	PG_RETURN_BOOL(containment(a, b) < 0);
}
