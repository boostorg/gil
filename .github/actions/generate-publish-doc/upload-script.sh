#!/bin/bash
set -e # Exit with Non Zero exit Code

# Save some useful information
SHA=`git rev-parse --verify HEAD`

mkdir temp-doc
cd temp-doc
git init

git config user.name "Github Action Upload Docs"
git config user.email "Github Action Upload Docs"

git remote add upstream "https://$1@github.com/boostorg/gil.git"

git fetch upstream
git switch gh-pages


if [ "${GITHUB_REF##*/}" = develop ]; then
    rm -r develop
    mkdir -p develop/doc
    cp ../index.html develop/
    cp ../doc/index.html develop/doc
    cp -a ../doc/html develop/doc/
else
    rm index.html
    rm -r html
    cp ../doc/index.html .
    cp -r ../doc/html .
fi


git add .

# Commit the new version.   
git commit -m "Deploy to GitHub Pages: ${SHA}"

# Now that we're all set up, we can push.
git push -q upstream HEAD:gh-pages
