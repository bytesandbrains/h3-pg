\pset tuples_only on

\set hexagon '\'880326b885fffff\'::h3index'
\set neighbor '\'880326b887fffff\'::h3index'
\set pentagon '\'831c00fffffffff\'::h3index'
\set edge '\'1180326b885fffff\'::h3index'

--
-- TEST h3_are_neighbor_cells
--

SELECT h3_are_neighbor_cells(:hexagon, :neighbor);
SELECT NOT h3_are_neighbor_cells(:hexagon, :hexagon);

--
-- TEST h3_cells_to_directed_edge
--

SELECT h3_cells_to_directed_edge(:hexagon, :neighbor) = :edge;

--
-- TEST h3_is_valid_directed_edge
--

SELECT h3_is_valid_directed_edge(:edge);
SELECT NOT h3_is_valid_directed_edge(:hexagon);

--
-- TEST h3_get_directed_edge_origin and
--      h3_get_directed_edge_destination
--

SELECT h3_get_directed_edge_origin(:edge) = :hexagon
AND h3_get_directed_edge_destination(:edge) = :neighbor;

--
-- TEST h3_directed_edge_to_cells
--

SELECT h3_directed_edge_to_cells(:edge) = (:hexagon, :neighbor);

--
-- TEST h3_origin_to_directed_edges
--

SELECT array_length(array_agg(edge), 1) = 6 FROM (
	SELECT h3_origin_to_directed_edges(:hexagon) edge
) q;
SELECT array_length(array_agg(edge), 1) = 5 expected FROM (
	SELECT h3_origin_to_directed_edges(:pentagon) edge
) q;

--
-- TEST h3_directed_edge_to_boundary
--

SELECT h3_directed_edge_to_boundary(:edge)
	~= polygon '((89.5830164946548,64.7146398954916),(89.5790678021742,64.2872231517217))'