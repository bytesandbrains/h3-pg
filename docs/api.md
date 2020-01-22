# API Reference

- [Indexing](#indexing-functions)
- [Inspection](#index-inspection-functions)
- [Traversal](#grid-traversal-functions)
- [Hierarchy](#hierarchical-grid-functions)
- [Regions](#region-functions)
- [Unidirectional Edges](#unidirectional-edge-functions)
- [Miscellaneous](#miscellaneous-h3-functions)
- [PostGIS](#postgis-integration)

## Indexing functions

### h3_geo_to_h3(`point`, resolution `int`) ⇒ `h3index`

Converts native PostgreSQL point to hex at given resolution.

```
SELECT h3_geo_to_h3(POINT('64.7498111652365,89.5695822308866'), 8);
  h3_geo_to_h3
-----------------
 880326b88dfffff
(1 row)
```

### h3_to_geo(`h3index`) ⇒ `point`

Finds the centroid of this hex in native PostgreSQL point type.

```
SELECT h3_to_geo('880326b88dfffff');
            h3_to_geo
-------------------------------------
 (64.7498111652365,89.5695822308866)
(1 row)
```

### h3_to_geo_boundary(`h3index`) ⇒ `polygon`

Find the boundary of this hex, in native PostgreSQL polygon type.

```
SELECT h3_to_geo_boundary(:hexagon);
                              h3_to_geo_boundary
-----------------------------------------------------------------------------
 ((89.5656359347422,64.3352882950961),...,(89.570369702947,64.104106930976))
(1 row)
```

## Index inspection functions

### h3_get_resolution(`h3index`) ⇒ `integer`

Returns the resolution of this hex.

```
SELECT h3_get_resolution(:hexagon), h3_get_resolution(:pentagon);
 h3_get_resolution | h3_get_resolution
-------------------+-------------------
                 8 |                 3
(1 row)
```

### h3_get_base_cell(`h3index`) ⇒ `integer`

Returns the base cell number of the given hex.

```
SELECT h3_get_base_cell(:hexagon), h3_get_base_cell(h3_to_parent(:hexagon));
 h3_get_base_cell | h3_get_base_cell
------------------+------------------
                2 |                2
(1 row)
```

### h3_string_to_h3(`cstring`) ⇒ `h3index`

Converts the string representation to H3Index representation. Not very useful, since the internal representation can already be displayed as text for output, and read as text from input.

```
SELECT h3_string_to_h3('880326b88dfffff);
 h3_string_to_h3
-----------------
 880326b88dfffff
(1 row)
```

### h3_to_string(`h3index`) ⇒ `cstring`

Converts the H3Index representation of the index to the string representation. Not very useful, since the internal representation can already be displayed as text for output, and read as text from input.

```
SELECT h3_to_string('880326b88dfffff);
 h3_to_string
-----------------
 880326b88dfffff
(1 row)
```

### h3_is_valid(`h3index`) ⇒ `boolean`

Returns whether this is a valid hex.

```
SELECT h3_is_valid(:hexagon), h3_is_valid(:pentagon), h3_is_valid(:invalid);
 h3_is_valid | h3_is_valid | h3_is_valid
-------------+-------------+-------------
 t           | t           | f
(1 row)

```

### h3_is_res_class_iii(`h3index`) ⇒ `boolean`

Returns true if the resolution of the given index has a class-III rotation,
returns false if it has a class-II.

```
SELECT h3_is_res_class_iii(:hexagon), h3_is_res_class_iii(h3_to_parent(:hexagon));
 h3_is_res_class_iii | h3_is_res_class_iii
---------------------+---------------------
 f                   | t
(1 row)
```

### h3_is_pentagon(`h3index`) ⇒ `boolean`

Returns whether this represents a pentagonal cell.

```
SELECT h3_is_pentagon(:hexagon), h3_is_pentagon(:pentagon);
 h3_is_pentagon | h3_is_pentagon
----------------+----------------
 f              | t
(1 row)
```

### h3_get_faces(`h3index`) ⇒ `integer[]`

Find all icosahedron faces intersected by a given H3 index.

```
SELECT h3_get_faces('851c0047fffffff');
 h3_get_faces
--------------
 {11,6}
(1 row)
```

## Grid traversal functions

### h3_k_ring(`h3index`, k `integer default 1`) ⇒ `setof`(`h3index`)

Returns all hexes within `k` (default 1) distance of the origin `hex`, including itself.

k-ring 0 is defined as the origin index, k-ring 1 is defined as k-ring 0 and all neighboring indices, and so on.

Output is provided in no particular order.

```
SELECT h3_k_ring('880326b88dfffff');
   h3_k_ring
-----------------
 880326b88dfffff
 880326b8ebfffff
 880326b8e3fffff
 880326b885fffff
 880326b881fffff
 880326b889fffff
 880326b8c7fffff
(7 rows)
```

### h3_k_ring_distances(`h3index`, k `integer default 1`) ⇒ `setof`(index `h3index`, distance `integer`)

Finds the set of all hexes within `k` (default 1) distance of the origin `hex` and their respective distances, including itself.

Output rows are provided in no particular order.

```
SELECT * FROM h3_k_ring_distances(:hexagon);
      index      | distance
-----------------+----------
 880326b88dfffff |        0
 880326b8ebfffff |        1
 880326b8e3fffff |        1
 880326b885fffff |        1
 880326b881fffff |        1
 880326b889fffff |        1
 880326b8c7fffff |        1
(7 rows)
```

### h3_hex_range(`h3index`, k `integer default 1`) ⇒ `setof`(`h3index`)

Returns all hexes within `k` (default 1) distance of the origin `hex`, including itself.

Output is sorted in increasing distance from origin.
Will report an error if it encounters a pentagon, in this case use k_ring.

```
SELECT h3_hex_range(:hexagon);
    h3_hex_range
-----------------
 880326b88dfffff
 880326b8ebfffff
 880326b8e3fffff
 880326b885fffff
 880326b881fffff
 880326b889fffff
 880326b8c7fffff
(7 rows)
```

### h3_hex_range_distances(`h3index`, k `integer default 1`) ⇒ `setof`(index` h3index`, distance `integer`)

Finds the set of all hexes within `k` (default 1) distance of the origin `hex` and their respective distances, including itself.

Output is sorted in increasing distance from origin.
Will report an error if it encounters a pentagon, in this case use h3_k_ring.

```
SELECT * FROM h3_hex_range_distances(:hexagon);
      index      | distance
-----------------+----------
 880326b88dfffff |        0
 880326b8ebfffff |        1
 880326b8e3fffff |        1
 880326b885fffff |        1
 880326b881fffff |        1
 880326b889fffff |        1
 880326b8c7fffff |        1
(7 rows)
```

### h3_hex_ranges(`h3index[]`, k `integer default 1`) ⇒ `setof`(`h3index`)

Returns all hexes within `k` (default 1) distance of every hex in the given array, including themselves.

Output is sorted in first by increasing order of elements in the array, secondly by distance of the particular element.
Will report an error if it encounters a pentagon.

```
SELECT h3_hex_range(:hexagon), h3_hex_range('880326b8ebfffff'), h3_hex_ranges('{880326b88dfffff,880326b8ebfffff}'::h3index[]);
    h3_hex_range    |    h3_hex_range    |   h3_hex_ranges
-----------------+-----------------+-----------------
 880326b88dfffff | 880326b8ebfffff | 880326b88dfffff
 880326b8ebfffff | 880326b8e9fffff | 880326b8ebfffff
 880326b8e3fffff | 880326b8e1fffff | 880326b8e3fffff
 880326b885fffff | 880326b8e3fffff | 880326b885fffff
 880326b881fffff | 880326b88dfffff | 880326b881fffff
 880326b889fffff | 880326b8c7fffff | 880326b889fffff
 880326b8c7fffff | 880326b8c5fffff | 880326b8c7fffff
                 |                 | 880326b8ebfffff
                 |                 | 880326b8e9fffff
                 |                 | 880326b8e1fffff
                 |                 | 880326b8e3fffff
                 |                 | 880326b88dfffff
                 |                 | 880326b8c7fffff
                 |                 | 880326b8c5fffff
(14 rows)
```

### h3_hex_ring(h3index[, k]) ⇒ `setof`(`h3index`)

Returns the hollow ring of hexes with `k` (default 1) distance of the origin `hex`.

Will report an error if it encounters a pentagon, in this case use h3_k_ring.

```
SELECT h3_hex_ring(:hexagon, 2);
    h3_hex_ring
-----------------
 880326b8c1fffff
 880326b8c5fffff
 880326b8e9fffff
 880326b8e1fffff
 880326b8e7fffff
 880326b8a9fffff
 880326b8abfffff
 880326b887fffff
 880326b883fffff
 880326b88bfffff
 880326b8d5fffff
 880326b8c3fffff
(12 rows)
```

### h3_distance(h3index, h3index) ⇒ `integer`

Determines the distance in grid cells between the two given indices.

```
SELECT h3_distance('880326b881fffff', '880326b885fffff');
 h3_distance
-------------
           1
(1 row)
```

EXPERIMENTALS

## Hierarchical grid functions

### h3_to_parent(h3index[, parentRes]) ⇒ `h3index`

Returns the parent (coarser) hex containing this `hex` at given `parentRes` (if no resolution is given, parent at current resolution minus one is found).

```
SELECT h3_to_parent('880326b88dfffff', 5);
     h3_to_parent
-----------------
 850326bbfffffff
(1 row)
```

### h3_to_children(h3index[, childRes]) ⇒ `setof`(`h3index`)

Returns all hexes contained by `hex` at given `childRes` (if no resolution is given, children at current resolution plus one is found).

May cause problems with too large memory allocations. Please see `h3_to_children_slow`.

```
SELECT h3_to_children('880326b88dfffff', 9);
    h3_to_children
-----------------
 890326b88c3ffff
 890326b88c7ffff
 890326b88cbffff
 890326b88cfffff
 890326b88d3ffff
 890326b88d7ffff
 890326b88dbffff
(7 rows)
```

### h3_to_children_slow(h3index[, childRes]) ⇒ `setof`(`h3index`)

This functions does the same as `h3_to_children_slow` but allocates smaller chunks of memory at the cost speed.

### h3_compact(h3index[]) ⇒ `setof`(`h3index`)

Returns the compacted version of the input array. I.e. if all children of an hex is included in the array, these will be compacted into the parent hex.

```
SELECT h3_compact(array_cat(ARRAY(SELECT h3_to_children('880326b88dfffff')), ARRAY(SELECT h3_to_children('880326b88bfffff'))));
   h3_compact
-----------------
 880326b88bfffff
 880326b88dfffff
(2 rows)
```

### h3_uncompact(`h3index[]` resolution `integer default -1`) ⇒ `setof`(`h3index`)

Uncompacts the given hex array at the given resolution. If no resolution it will be chosen to be the highest resolution of the given indexes + 1.

```
SELECT h3_uncompact(array_cat(ARRAY(SELECT h3_to_parent('880326b88dfffff')), '{880326b88bfffff}'::h3index[]));
  h3_uncompact
-----------------
 890326b8803ffff
 ...
 890326b88bbffff
(56 rows)
```

## Region functions

### h3_polyfill(exterior `polygon`, holes `polygon[]`, resolution `integer`) ⇒ `setof`(`h3index`)

Polyfill takes a given exterior native postgres polygon and an array of interior holes (also native polygons), along with a resolutions and fills it with hexagons that are contained.

```
SELECT h3_polyfill(exterior, holes, 1) FROM
(
 SELECT *  FROM h3_set_to_linked_geo(ARRAY(SELECT h3_to_children('8059fffffffffff, 1)))
) q;
   h3_polyfill
-----------------
 8158fffffffffff
 8159bffffffffff
 8158bffffffffff
 81597ffffffffff
 81587ffffffffff
 81593ffffffffff
 81583ffffffffff
(7 rows)
```

### h3_set_to_linked_geo(`h3index[]`) ⇒ `setof`(exterior `polygon`, holes `polygon[]`)

Create records of exteriors and holes.

```
SELECT h3_polyfill(exterior, holes, 1) FROM
(
 SELECT *  FROM h3_set_to_linked_geo(ARRAY(SELECT h3_to_children('8059fffffffffff, 1)))
) q;
   h3_polyfill
-----------------
 8158fffffffffff
 ...
 81583ffffffffff
(7 rows)
```

## Unidirectional edge functions

Unidirectional edges are a form of H3Indexes that denote a unidirectional edge between two neighbouring indexes.

### h3_indexes_are_neighbors(`h3index`, `h3index`) ⇒ `boolean`

Determines whether or not the two indexes are neighbors. Returns true if they are and false if they aren't

```
SELECT h3_indexes_are_neighbors(:hexagon, '880326b8ebfffff'), h3_indexes_are_neighbors('880326b881fffff', '880326b8ebfffff');
 h3_indexes_are_neighbors | h3_indexes_are_neighbors
--------------------------+--------------------------
 t                        | f
(1 row)
```

### h3_get_h3_unidirectional_edge(origin `h3index`, destination `h3index`) ⇒ `h3index`

Returns the edge from origin to destination

Will error if the two indexes are not neighbouring

```
SELECT(h3_get_h3_unidirectional_edge(:hexagon, :neighbour));
 h3_get_h3_unidirectional_edge
----------------------------
 1180326b885fffff
(1 row)
```

### h3_unidirectional_edge_is_valid(`h3index`) ⇒ `boolean`

Returns true if the given hex is a valid edge.

```
SELECT h3_unidirectional_edge_is_valid(h3_get_h3_unidirectional_edge(:hexagon, :neighbour));
 h3_unidirectional_edge_is_valid
---------------------------------
 t
(1 row)
```

### h3_get_origin_h3_index_from_unidirectional_edge(`h3index`) ⇒ `h3index`

Returns the origin index from the given edge

```
SELECT h3_get_origin_h3_index_from_unidirectional_edge(h3_get_h3_unidirectional_edge(:hexagon, :neighbour)), :hexagon;
 h3_get_origin_h3_index_from_unidirectional_edge |    ?column?
----------------------------------------------+-----------------
 880326b885fffff                              | 880326b885fffff
(1 row)
```

### h3_get_destination_h3_index_from_unidirectional_edge(`h3index`) ⇒ `h3index`

Returns the destination index from the given edge

```
SELECT h3_get_destination_h3_index_from_unidirectional_edge(h3_get_h3_unidirectional_edge(:hexagon, :neighbour)), :neighbour;
 h3_get_destination_h3_index_from_unidirectional_edge |    ?column?
---------------------------------------------------+-----------------
 880326b887fffff                                   | 880326b887fffff
(1 row)
```

### h3_get_h3_indexes_from_unidirectional_edge(`h3index`) ⇒ (origin `h3index`, destination `h3index`)

Returns both the origin and destination indexes from the given edge

```
SELECT * FROM h3_get_h3_indexes_from_unidirectional_edge(h3_get_h3_unidirectional_edge(:hexagon, :neighbour));
     origin      |   destination
-----------------+-----------------
 880326b885fffff | 880326b887fffff
(1 row)
```

### h3_get_h3_unidirectional_edges_from_hexagon(`h3index`) ⇒ `setof`(`h3index`)

Returns the set of all valid unidirectional edges with the given origin

```
SELECT h3_get_h3_unidirectional_edges_from_hexagon(:hexagon);
 h3_get_h3_unidirectional_edges_from_hexagon
------------------------------------------
 1180326b885fffff
 1280326b885fffff
 1380326b885fffff
 1480326b885fffff
 1580326b885fffff
 1680326b885fffff
(6 rows)
```

### h3_get_h3_unidirectional_edge_boundary(`h3index`) ⇒ `polygon`

Find the boundary of this edge, in native PostgreSQL polygon type.

```
SELECT h3_get_h3_unidirectional_edge_boundary(h3_get_h3_unidirectional_edge(:hexagon, :neighbour));
                     h3_get_h3_unidirectional_edge_boundary
---------------------------------------------------------------------------
 ((89.5830164946548,64.7146398954916),(89.5790678021742,64.2872231517217))
(1 row)
```

## Miscellaneous H3 functions

### h3_hex_area(resolution `integer`, km `boolean` default false) ⇒ `float`

Returns the area of an hex in square (kilo)meters at the given resolution.

```
SELECT h3_hex_area(10, true);
 h3_hex_area
--------------
    0.0150475
(1 row)

SELECT h3_hex_area(10);
 h3_hex_area
-------------
     15047.5
(1 row)
```

### h3_edge_length(resolution `integer`, km `boolean` default false) ⇒ `float`

Returns the length of the edges of an hex in (kilo)meters at the given resolution.

```
SELECT h3_edge_length(10, true);
 h3_edge_length
----------------
    0.065907807
(1 row)

SELECT h3_edge_length(10);
 h3_edge_length
---------------
   65.90780749
(1 row)
```

### h3_num_hexagons(resolution `integer`) ⇒ `bigint`

Returns the number of unique indexes at the given resolution

```
SELECT h3_num_hexagons(15);
  h3_num_hexagons
-----------------
 569707381193162
(1 row)
```

### h3_get_res_0_indexes() ⇒ `setof`(`h3index`)

Returns all 122 basecells.

```
SELECT h3_get_res_0_indexes();
  h3_get_res_0_indexes
-----------------
 ...
(122 rows)
```

## PostGIS integration

We provide some simple wrappers for casting to PostGIS types.

### h3_geo_to_h3(geometry / geography, resolution) ⇒ `h3index`

The `h3_geo_to_h3` function has been overloaded to support both PostGIS `geometry` and `geography`.

### h3_to_geometry(`h3index`) ⇒ `geometry`

Finds the centroid of this hex as PostGIS geometry type.

```
SELECT h3_to_geometry('8a63a9a99047fff');
                  h3_to_geometry
----------------------------------------------------
 0101000020E61000008BE4AED877D54B40C46F27D42B2F2940
(1 row)
```

### h3_to_geography(`h3index`) ⇒ `geography`

Same as above, but returns `geography` type.

### h3_to_geo_boundary_geometry(`h3index`) ⇒ `geometry`

Find the boundary of this hex, as PostGIS type.

```
SELECT boundary_geometry('8a63a9a99047fff');
    boundary_geometry
-------------------------
 0103000020...FB70D54B40
(1 row)
```

### h3_to_geo_boundary_geography(`h3index`) ⇒ `geography`

Same as above, but returns `geography` type.

## Functions without bindings

Some functions does not have bindings:

* `degsToRads`/`radsToDegs`: Use postgres built-ins `RADIANS()` and `DEGREES()` instead.
* Memory handling functions: `maxKringSize`, `h3LineSize`, `maxH3ToChildrenSize`, `maxUncompactSize`, `maxPolyfillSize`, `destroyLinkedPolygon`. Memory is handled by the extensnion.
