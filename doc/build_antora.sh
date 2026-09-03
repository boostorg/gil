#!/bin/bash
#
# Copyright (c) 2026 Samuel Debionne
#
# Distributed under the Boost Software License, Version 1.0. (See accompanying
# file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
#
# Official repository: https://github.com/boostorg/gil
#

set -e

SCRIPT_DIR=$( cd -- "$( dirname -- "${BASH_SOURCE[0]}" )" &> /dev/null && pwd )
cd "$SCRIPT_DIR"

if [ -z "${PLAYBOOK:-}" ]; then
  PLAYBOOK="local-playbook.yml"
fi

# Required by the @cppalliance/antora-cpp-reference-extension (see
# local-playbook.yml): a full Boost superproject checkout, so GIL's headers
# resolve cross-library includes (boost/mp11.hpp, boost/stl_interfaces/...).
# `system-env` on that extension's dependency entry means it reuses this
# instead of cloning its own -- CI's setup-boost action already produces one
# at ../../.. from here (boost-root/libs/gil/doc), same as this fallback.
export BOOST_SRC_DIR="${BOOST_SRC_DIR:-$(realpath "$SCRIPT_DIR/../../..")}"
echo "Using BOOST_SRC_DIR=$BOOST_SRC_DIR"

# Pin the @cppalliance/antora-cpp-reference-extension to a local MrDocs
# instead of it downloading the latest GitHub release, which as of this
# writing still segfaults on GIL's headers (upstream fix not released yet).
# When running under `pixi run -e docs`, CONDA_PREFIX/bin/mrdocs is the
# package from the sdebionne/label/esrf-bcu channel (see pixi.toml), which
# repackages the fixed dev build. Falls through to auto-download otherwise.
if [ -z "${MRDOCS_ROOT:-}" ] && [ -n "${CONDA_PREFIX:-}" ] && [ -x "$CONDA_PREFIX/bin/mrdocs" ]; then
  export MRDOCS_ROOT="$CONDA_PREFIX"
fi
if [ -n "${MRDOCS_ROOT:-}" ]; then
  echo "Using MRDOCS_ROOT=$MRDOCS_ROOT"
fi

echo "Generating the MrDocs compilation database..."
# See mrdocs.yml / mrdocs-src/all.cpp: a single translation unit is enough
# for MrDocs to extract the whole public API, and is far faster than trying
# to point it at GIL's real CMakeLists.txt (which needs libjpeg/libpng/
# libtiff just to configure, for the extension/io backends this deliberately
# excludes -- see mrdocs-src/all.cpp for why).
# The file must be literally named compile_commands.json -- MrDocs rejects
# any other basename even when given as a full, valid, existing path.
cat > compile_commands.json <<EOF
[
  {
    "directory": "$SCRIPT_DIR/mrdocs-src",
    "command": "c++ -std=c++17 -I$SCRIPT_DIR/../include -I$BOOST_SRC_DIR -c all.cpp",
    "file": "all.cpp"
  }
]
EOF

echo "Installing npm dependencies..."
npm ci

echo "Building documentation with Antora..."
PATH="$(pwd)/node_modules/.bin:${PATH}"
export PATH
npx antora --clean --fetch "$PLAYBOOK" --stacktrace --log-level all

echo "Done"
