# Boost.GIL Documentation

A simple guide about writing and building documentation for Boost.GIL.

The documentation is written in AsciiDoc and built with [Antora](https://antora.org/).
The C++ API reference is generated from the header comments with
[MrDocs](https://github.com/cppalliance/mrdocs) (see `mrdocs.yml` and
`mrdocs-src/all.cpp`) and wired into the Antora site as the `reference:`
module, with `modules/ROOT/pages/reference.adoc` as a curated landing page
into it. MrDocs itself isn't a build dependency here -- it's downloaded
automatically by the `@cppalliance/antora-cpp-reference-extension` Antora
extension the first time the reference is built.

## Prerequisites

- [Node.js](https://nodejs.org/) 20+ (for Antora)
- A C++ compiler (for MrDocs to parse GIL's headers)

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

The site is generated at `doc/html/index.html`.
