# Boost.GIL Documentation

A simple guide about writing and building documentation for Boost.GIL.

## Prerequisites

- Python 3.4+
- [Sphinx](http://www.sphinx-doc.org/) 1.7.9
- [Doxygen](http://www.doxygen.org)

## Installation

Prepare Python virtual environment with Sphinx and Exhale:

```console
$ cd boost/libs/gil
$ python3 -m venv .venv
$ source ./.venv/bin/activate
(.venv)$ pip install -r doc/requirements.txt
```

## Build

Run Sphinx toolchain directly:

```console
(.venv)$ cd doc
(.venv)$ make html
```

Alternatively, build via `b2`:

```console
(.venv)$ b2 doc/
```
