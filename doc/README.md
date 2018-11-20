# Boost.GIL Documentation

A simple guide about writing and building documentation for Boost.GIL.

## Prerequisites

- Python 3.4+
- [Install Sphinx](#install-sphinx)
- Install [Doxygen](http://www.doxygen.org)

## Installation

Create Python virtual environment:

```console
$ cd boost/libs/gil
$ python3 -m venv .venv
$ source ./.venv/bin/activate
```

Install Sphinx and Sphinx extensions:

```console
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
