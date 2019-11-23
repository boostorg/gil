# Release Notes

All notable changes to [Boost.GIL](https://github.com/boostorg/gil/) project will be documented in this file.
The format is based on [Keep a Changelog](https://keepachangelog.com/en/1.0.0/).

## [Unreleased]

## [1.72.0] - 2019-12-11

### Added
- GSoC 2019: Lanczos resampling for image down scaling ([PR #309](https://github.com/boostorg/gil/pull/309)).
- GSoC 2019: Methods for binary thresholding, inverted binary thresholding and truncation thresholding ([PR #313](https://github.com/boostorg/gil/pull/313)).
- GSoC 2019: Otsu thresholding method ([PR #314](https://github.com/boostorg/gil/pull/314)).
- GSoC 2019: Adaptive thresholding using mean or gaussian-weighted sum of the neighbourhood area ([PR #315](https://github.com/boostorg/gil/pull/315)).
- GSoC 2019: Harris response calculation (corner detector without non-maximum filtering) ([PR #350](https://github.com/boostorg/gil/pull/350)).
- GSoC 2019: Hessian corner detector ([PR #364](https://github.com/boostorg/gil/pull/364)).
- GSoC 2019: Types for defining 2D kernel, `kernel_2d` and `kernel_2d_fixed`, in Numeric extension ([PR #361](https://github.com/boostorg/gil/pull/361)).
- GSoC 2019: Implementation of 2D convolution as new function `convolve_2d` ([PR #367](https://github.com/boostorg/gil/pull/367)).
- GSoC 2019: Box filtering using the average filter ([PR #383](https://github.com/boostorg/gil/pull/383)).
- GSoC 2019: Blur function based on normalized mean filter ([PR #383](https://github.com/boostorg/gil/pull/383)).
- GSoC 2019: Sobel and Scharr operators ([PR #392](https://github.com/boostorg/gil/pull/392)).
- GSoC 2019: Median filter to remove noise from image ([PR #393](https://github.com/boostorg/gil/pull/393)).
- Continued adding new test cases and significantly improved overall test coverage.
- Documented purpose of `cached_location_t` ([PR #287](https://github.com/boostorg/gil/pull/287)).
- Function `convolve_1d` in Numeric extension for convenient use of `convolve_rows` and `convolve_cols` ([PR #347](https://github.com/boostorg/gil/pull/347) and [PR #367](https://github.com/boostorg/gil/pull/367)).
- Function `extend_boundary` in Numeric extension to perform image boundary extension ([PR #386](https://github.com/boostorg/gil/pull/386)).
- Project release notes maintained in Markdown file `RELEASES.md` ([PR #404](https://github.com/boostorg/gil/pull/404)).

### Changed
- Move all tests, core features and extensions, inside `test/` directory ([PR #302](https://github.com/boostorg/gil/pull/302)).

### Removed
- Replace Boost.MPL with Boost.MP11 ([PR #274](https://github.com/boostorg/gil/pull/274)).
- Removed use of Boost.TypeTraits ([PR #274](https://github.com/boostorg/gil/pull/274)).
- Dropped support for GCC <= 4.8 ([PR #296](https://github.com/boostorg/gil/pull/296)).
- Remove `include/boost/gil/version.hpp` file as unused ([PR #403](https://github.com/boostorg/gil/pull/403)).

### Fixed
- Undetermined value of default-initialized channel and pixel objects ([PR #273](https://github.com/boostorg/gil/pull/273)).
- Undefined behaviour due to `std::is_trivially_default_constructible` specializations ([PR #284](https://github.com/boostorg/gil/pull/284)).
- Crash when reading PNG files with an invalid header ([PR #385](https://github.com/boostorg/gil/pull/385)).
- Applied the [Rule of Three](https://en.wikipedia.org/wiki/Rule_of_three_(C%2B%2B_programming)) for numerous types.
- Removed uses of deprecated implicit definition of defaulted copy assignment operator or copy constructor.

## [1.68.0] - 2018-08-09

### Added
- The library now requires a C++11-compliant compiler.
- Added Toolbox extension following the [review and acceptance into Boost](https://lists.boost.org/boost-announce/2011/01/0281.php).

### Changed
- The I/O extensions have been entirely rewritten as I/O v2, [reviewed and accepted into Boost](https://lists.boost.org/boost-announce/2011/01/0281.php).
- Documentation has been reformatted and updated.

### Removed
- The existing I/O v1 extension has been replaced with I/O v2.

## [1.53.0] - 2013-02-04

### Fixed
- Fixed self-assignment warnings (Trac [#4919](https://svn.boost.org/trac10/ticket/4919)).

## [1.35.0] - 2008-03-29

### Added
- First release of Generic Image Library developed by Lubomir Bourdev and Hailin Jin following the [review and acceptance into Boost](https://lists.boost.org/Archives/boost/2006/11/112896.php).
