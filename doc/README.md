# Boost.GIL Documentation

A simple guide about writing and building documentation for Boost.GIL.

## Prerequisites

- Python 3.6+
- [Install Sphinx](#install-sphinx) 1.7.9+
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

```console
$ echo "using doxygen ;" > ~/user-config.jam
$ cd boost/libs/gil/doc
$ b2
```
