\pset tuples_only on
\set string '\'801dfffffffffff\''
\set asbigint 576988517884755967
\set hexagon ':string::h3index'
\set pentagon '\'844c001ffffffff\'::h3index'

--
-- TEST operators
--
SELECT :hexagon = :hexagon;
SELECT NOT :hexagon = :pentagon;
SELECT NOT :hexagon <> :hexagon;
SELECT :hexagon <> :pentagon;

--
-- TEST bigint casting
--
SELECT :asbigint = :hexagon::bigint;

SELECT :hexagon = :asbigint::h3index;
