# Changelog
All notable changes to this project will be documented in this file.

The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## Versioning
The H3 core library adheres to [Semantic Versioning](http://semver.org/).
H3-pg has a `major.minor.patch` version scheme. The major and minor version
numbers of H3-pg are the major and minor version of the bound core library,
respectively. The patch version is incremented independently of the core
library.

Because H3-pg is versioned in lockstep with the H3 core library, please
avoid adding features or APIs which do not map onto the
[H3 core API](https://uber.github.io/h3/#/documentation/api-reference/).

## [Unreleased]
### Added
- Casting to and from `bigint`.
- Make target for `pgindent` (used for formatting c our code)

## [3.5.0] - 2019-08-01
### Added
- `h3_get_faces` function to find icosahedron faces for an index.
- `h3_hex_area` replacing `h3_hex_area_m2` and `h3_hex_area_km2`.
- `h3_edge_length` replacing `h3_edge_length_m` and `h3_edge_length_km`.
- Check bindings against `binding-functions` exported by H3.
### Changed
- Bump `h3` to `3.5.0`.
### Fixed
- Fix build for PostgreSQL 12 (#4, thanks @Komzpa)
### Removed
- Removed H3 core library version check, since we know which version we are linking.
- Removed `h3_hex_area_m2`, `h3_hex_area_km2`, `h3_edge_length_m` and `h3_edge_length_km` in favor of `h3_hex_area` and `h3_edge_length`.
- Removed `hex_range`, `hex_ranges` and `hex_range_distances` to align with `binding-functions`.

## [3.4.1] - 2019-06-14
### Added
- Added more docker test utilities.
### Fixed
- Fix `abs` warning.

## [3.4.0] - 2019-06-13
### Changed
- Use same versioning scheme as python and java bindings, which is to lock major and minor to h3 core, and incrementing patch independently.
### Fixed
- Remove explicit type specifier :PATH from install prefix.
### Removed
- Removed degree/radian conversion helpers, recommend postgres built-in RADIANS/DEGREES instead.

## [1.0.6] - 2019-06-03
### Changed
- Bump H3 to 3.4.4

## [1.0.5] - 2019-02-15
### Fixed
- Regression: Remember to add all update files on install

## [1.0.4] - 2019-02-15
### Fixed
- Fix polyfill for polygon with multiple holes.

## [1.0.3] - 2019-01-27
### Fixed
- Fix updatepath makefile target.

## [1.0.2] - 2019-01-27
### Removed
- Removed git tag check in `distribute` makefile target, since it causes error on pgxn install.

## [1.0.1] - 2019-01-27
### Fixed
- Removed usage of `FALSE` instead of 0 in conditional in `ASSERT` macro.

## [1.0.0] - 2019-01-27
### Added
- Added `h3_get_extension_version()`.
- Added test ensuring upgrade path has same result as fresh install.
- Added hash operator class, now `WHERE IN` works.
- Makefile now builds and links `libh3.a` locally instead of using system installation.
### Changed
- Renamed `h3_basecells` to `h3_get_res_0_indexes`.
- Renamed all functions with double `h3_h3_` prefix to use single `h3_` prefix.

## [0.4.0] - 2019-01-12
### Added
- Added `h3_line` (binding for h3Line)
### Fixed
- Fixed `h3_h3_to_children_slow`

## [0.3.2] - 2019-01-08
### Fixed
- Fixed B-Tree operator class, now indexing works.

## [0.3.1] - 2018-12-17
### Changed
- Added flag `extend` such that polygons returned by `h3_h3_to_geo_boundary` are not wrapped at 180th meridian.

## 0.3.0 - 2018-12-11
### Added
- First public release.

[Unreleased]: https://github.com/bytesandbrains/h3-pg/compare/v3.5.0...HEAD
[3.5.0]: https://github.com/bytesandbrains/h3-pg/compare/v3.4.1...v3.5.0
[3.4.1]: https://github.com/bytesandbrains/h3-pg/compare/v3.4.0...v3.4.1
[3.4.0]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.6...v3.4.0
[1.0.6]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.5...v1.0.6
[1.0.5]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.4...v1.0.5
[1.0.4]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.3...v1.0.4
[1.0.3]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.2...v1.0.3
[1.0.2]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.1...v1.0.2
[1.0.1]: https://github.com/bytesandbrains/h3-pg/compare/v1.0.0...v1.0.1
[1.0.0]: https://github.com/bytesandbrains/h3-pg/compare/v0.4.0...v1.0.0
[0.4.0]: https://github.com/bytesandbrains/h3-pg/compare/v0.3.2...v0.4.0
[0.3.2]: https://github.com/bytesandbrains/h3-pg/compare/v0.3.1...v0.3.2
[0.3.1]: https://github.com/bytesandbrains/h3-pg/compare/v0.3.0...v0.3.1
