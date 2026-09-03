#!/bin/bash
set -e # Exit with Non Zero exit Code

mkdir temp-doc
cd temp-doc

git init


git remote add upstream "https://github.com/boostorg/gil.git"

git fetch upstream
git switch gh-pages


if [ "${GITHUB_REF##*/}" = develop ]; then
    # Only updates develop directory and keeps others intact
    rm -rf develop
    mkdir -p develop/doc
    cp ../index.html develop/
    cp -a ../doc/build/site/. develop/doc/
else
    # main branch
    rm -f index.html
    rm -rf html gil
    cp -a ../doc/build/site/. .
fi

# Remove version control
rm -rf .git
