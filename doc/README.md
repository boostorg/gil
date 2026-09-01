# Boost.GIL Documentation

A simple guide about writing and building documentation for Boost.GIL.

The documentation is written in AsciiDoc and built with [Antora](https://antora.org/).
The C++ API reference is still generated with [Doxygen](https://www.doxygen.org)
(see `doxyfile`) and stitched into the Antora site by
`modules/ROOT/pages/reference.adoc` — this library is expected to move to a
[MrDocs](https://github.com/cppalliance/mrdocs)-generated, AsciiDoc-native
reference once an upstream MrDocs crash on GIL's header patterns is fixed;
see `mrdocs.yml` for details.

## Prerequisites

- [Node.js](https://nodejs.org/) 20+ (for Antora)
- [Doxygen](https://www.doxygen.org)

Both are available through the repository's `pixi.toml` under the `docs`
environment; from the repository root:

```console
$ pixi run -e docs docs-build
```

## Build manually

```console
$ cd libs/gil/doc
$ bash build_antora.sh
```

The site is generated at `doc/build/site/index.html`.
