/*
 * Copyright 2019 Bytes & Brains
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
#include "access/gist.h"	 // GiST

#include <h3api.h> // Main H3 include
#include "extension.h"

#define H3_ROOT_INDEX -1

static int
static int
gist_cmp(H3Index  * a, H3Index * b)
{
	int			aRes;
	int			bRes;

	uint64_t	cellMask = (1 << 45) - 1;	/* rightmost 45 bits*/
	uint64_t	aCell;
	uint64_t	bCell;
	uint64_t	mask;

    /* identity */
    if (*a == *b)
	{
		return 1;
	}

    /* no shared basecell */
    if (H3_GET_BASE_CELL(*a) != H3_GET_BASE_CELL(*b))
	{
		return 0;
	}

	aRes = H3_GET_RESOLUTION(*a);
	bRes = H3_GET_RESOLUTION(*b);
	aCell = *a & cellMask;
	bCell = *b & cellMask;

	/* a contains b */
	if (*a == H3_ROOT_INDEX || (aCell^bCell) >> (45 - 3 * aRes) == 0)
	{
		return 1;
	}

	/* a contained by b */
	if (*b == H3_ROOT_INDEX || (aCell^bCell) >> (45 - 3 * bRes) == 0)
	{
		return -1;
	}

	/* no overlap */
	return 0;
}

/**
 * GiST support
 */

static H3Index
common_ancestor(H3Index a, H3Index b)
{
	int			aRes;
	int			bRes;
	int			maxRes; 
	uint64_t	cellMask = (1 << 45) - 1;	/* rightmost 45 bits*/
	uint64_t	abCell;

	if (a == b)
	{
		return a;
	}

    /* do not even share the basecell */
    if (H3_GET_BASE_CELL(a) != H3_GET_BASE_CELL(b))
	{
		return H3_ROOT_INDEX;
	}

	/* intersections at res 1-15 */
	abCell = a & b & cellMask;

	/* basecell as the only common ancestor */
	if(abCell == 0)
	{
		return H3_SET_RESOLUTION(a | cellMask, 0);
	} 

	/* common ancestor at resolution > 0 */
	aRes = H3_GET_RESOLUTION(a);
	bRes = H3_GET_RESOLUTION(b);
	maxRes = (aRes < bRes) ? aRes : bRes;
	for (int i = maxRes; i > 0; i--)
	{
		if(abCell >> (i * 3) == 0)
		{
			mask = (1 << (i * 3)) - 1;
			return H3_SET_RESOLUTION(a | mask, i);
		}
	}

	return H3_ROOT_INDEX;
}


/**
 * The GiST Consistent method for H3 indexes
 * Should return false if for all data items x below entry,
 * the predicate x op query == false, where op is the oper
 * corresponding to strategy in the pg_amop table.
 */
PG_FUNCTION_INFO_V1(h3index_gist_consistent);
Datum
h3index_gist_consistent(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
	H3Index    *query = PG_GETARG_H3_INDEX_P(1);
	StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2);

	/* Oid subtype = PG_GETARG_OID(3); */
	bool	   *recheck = (bool *) PG_GETARG_POINTER(4);
	H3Index    *key = DatumGetH3IndexP(entry->key);

	/* When the result is true, a recheck flag must also be returned. */
	*recheck = true;

	switch (strategy)
	{
		case RTOverlapStrategyNumber:
			PG_RETURN_BOOL(gist_cmp(key, query) != 0);
		case RTContainsStrategyNumber:
			PG_RETURN_BOOL(gist_cmp(key, query) > 0);
		case RTContainedByStrategyNumber:
			if (GIST_LEAF(entry))
			{
				PG_RETURN_BOOL(gist_cmp(key, query) < 0);
			}
			/* internal nodes, just check if we overlap */
			PG_RETURN_BOOL(gist_cmp(key, query) != 0);
		default:
			ereport(ERROR, (
							errcode(ERRCODE_INTERNAL_ERROR),
						 errmsg("unrecognized StrategyNumber: %d", strategy))
				);
	}
}

/**
 * The GiST Union method for H3 indexes
 * returns the minimal H3 index that encloses all the entries in entryvec
 */
PG_FUNCTION_INFO_V1(h3index_gist_union);
Datum
h3index_gist_union(PG_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GISTENTRY  *entries = entryvec->vector;
	int			n = entryvec->n;
	H3Index    *out,
			   *tmp;

	out = palloc(sizeof(H3Index));
	tmp = DatumGetH3IndexP(entries[0].key);
	*out = *tmp;

	for (int i = 1; i < n; i++)
	{
		tmp = DatumGetH3IndexP(entries[i].key);
		*out = common_ancestor(*out, *tmp);
	}

	PG_RETURN_H3_INDEX_P(out);
}

/**
 * GiST Compress and Decompress methods for H3Indexes
 * do not do anything.
 */
PG_FUNCTION_INFO_V1(h3index_gist_compress);
Datum
h3index_gist_compress(PG_FUNCTION_ARGS)
{
	PG_RETURN_DATUM(PG_GETARG_DATUM(0));
}

PG_FUNCTION_INFO_V1(h3index_gist_decompress);
Datum
h3index_gist_decompress(PG_FUNCTION_ARGS)
{
	PG_RETURN_POINTER(PG_GETARG_POINTER(0));
}

/*
** The GiST Penalty method for H3 indexes
** We use change resolution as our penalty metric
*/
PG_FUNCTION_INFO_V1(h3index_gist_penalty);
Datum
h3index_gist_penalty(PG_FUNCTION_ARGS)
{
	GISTENTRY  *origentry = (GISTENTRY *) PG_GETARG_POINTER(0);
	GISTENTRY  *newentry = (GISTENTRY *) PG_GETARG_POINTER(1);
	float	   *penalty = (float *) PG_GETARG_POINTER(2);

	H3Index    *orig = DatumGetH3IndexP(origentry->key);
	H3Index    *new = DatumGetH3IndexP(newentry->key);

	H3Index		ancestor = common_ancestor(*orig, *new);

	*penalty = (float) H3_GET_RESOLUTION(*orig) - H3_GET_RESOLUTION(ancestor);

	PG_RETURN_POINTER(penalty);
}

/**
 * The GiST PickSplit method for H3 indexes
 */
PG_FUNCTION_INFO_V1(h3index_gist_picksplit);
Datum
h3index_gist_picksplit(PG_FUNCTION_ARGS)
{
	GistEntryVector *entryvec = (GistEntryVector *) PG_GETARG_POINTER(0);
	GIST_SPLITVEC *v = (GIST_SPLITVEC *) PG_GETARG_POINTER(1);

	OffsetNumber maxoff = entryvec->n - 1;
	GISTENTRY  *ent = entryvec->vector;
	int			i,
				nbytes;
	OffsetNumber *left,
			   *right;
	H3Index    *tmp_union,
			   *unionL,
			   *unionR;
	GISTENTRY **raw_entryvec;

	nbytes = (maxoff + 1) * sizeof(OffsetNumber);

	v->spl_left = (OffsetNumber *) palloc(nbytes);
	left = v->spl_left;
	v->spl_nleft = 0;

	v->spl_right = (OffsetNumber *) palloc(nbytes);
	right = v->spl_right;
	v->spl_nright = 0;

	unionL = NULL;
	unionR = NULL;

	/* Initialize the raw entry vector. */
	raw_entryvec = (GISTENTRY **) malloc(entryvec->n * sizeof(void *));
	for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
		raw_entryvec[i] = &(ent[i]);

	for (i = FirstOffsetNumber; i <= maxoff; i = OffsetNumberNext(i))
	{
		int			real_index = raw_entryvec[i] - ent;

		tmp_union = DatumGetH3IndexP(ent[real_index].key);
		Assert(tmp_union != NULL);

		/*
		 * Choose where to put the index entries and update unionL and unionR
		 * accordingly. Append the entries to either v_spl_left or
		 * v_spl_right, and care about the counters.
		 */

		if (v->spl_nleft < v->spl_nright)
		{
			if (unionL == NULL)
				unionL = tmp_union;
			else
				*unionL = common_ancestor(*unionL, *tmp_union);

			*left = real_index;
			++left;
			++(v->spl_nleft);
		}
		else
		{
			if (unionR == NULL)
				unionR = tmp_union;
			else
				*unionR = common_ancestor(*unionR, *tmp_union);

			*right = real_index;
			++right;
			++(v->spl_nright);
		}
	}

	v->spl_ldatum = PointerGetDatum(unionL);
	v->spl_rdatum = PointerGetDatum(unionR);
	PG_RETURN_POINTER(v);
}

PG_FUNCTION_INFO_V1(h3index_gist_same);
Datum
h3index_gist_same(PG_FUNCTION_ARGS)
{
	H3Index    *a = PG_GETARG_H3_INDEX_P(0);
	H3Index    *b = PG_GETARG_H3_INDEX_P(1);
	bool	   *result = (bool *) PG_GETARG_POINTER(2);

	*result = *a == *b;
	PG_RETURN_POINTER(result);
}

PG_FUNCTION_INFO_V1(h3index_gist_distance);
Datum
h3index_gist_distance(PG_FUNCTION_ARGS)
{
	GISTENTRY  *entry = (GISTENTRY *) PG_GETARG_POINTER(0);
	H3Index    *query = PG_GETARG_H3_INDEX_P(1);

	/* StrategyNumber strategy = (StrategyNumber) PG_GETARG_UINT16(2); */
	/* Oid			subtype = PG_GETARG_OID(3); */
	/* bool    *recheck = (bool *) PG_GETARG_POINTER(4); */

	H3Index    *key = DatumGetH3IndexP(entry->key);

	/*
	 * int			 aRes = H3_GET_RESOLUTION(*query); int		 bRes =
	 * H3_GET_RESOLUTION(*key); H3Index	  aParent = h3ToCenterChild(*query,
	 * bRes); H3Index	  bParent = h3ToCenterChild(*key, aRes);
	 */

	int			distance = h3Distance(*query, *key);

	DEBUG(" dist %i", distance);
	PG_RETURN_INT32(distance);
}
