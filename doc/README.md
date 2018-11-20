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

_TODO_: Build for editing vs build for release, etc.

```console
(.venv)$ echo "using doxygen ;" > ~/user-config.jam
(.venv)$ b2 doc/
```