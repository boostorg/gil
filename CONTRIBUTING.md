![logo](https://raw.githubusercontent.com/boostorg/gil/develop/doc/_static/gil.png)

# Contributing to Boost.GIL

Boost.GIL is a member of [Boost](https://www.boost.org) libraries.

If you wish to contribute a new feature or a bug fix,
please follow the workflow explained in this document.

## Prerequisites

- Experience with `git` command line basics.
- Familiarity with build toolset and development environment of your choice.
- Although this document tries to present all commands with necessary options,
  it may be a good idea to skim through the
  [Boost Getting Started](https://www.boost.org/more/getting_started/index.html)
  chapters, especially if you are going to use
  [Boost.Build](https://www.boost.org/build/) for the first time.

## Getting Started

First, you need learn some minimal basics of the
[modular Boost](https://svn.boost.org/trac/boost/wiki/ModularBoost)
super-project workflow.

The following steps are based on the official Boost
[Getting Started](https://github.com/boostorg/boost/wiki/Getting-Started).

**NOTE:** For brevity, commands below use notation for POSIX-like operating
systems and you may need to tweak them for Windows systems.

### 1. Clone Boost super-project

The preparation involves the following steps:

1. Download the Boost super-project and switch the local repository to
   `develop` branch.
2. Run `bootstrap` to build `b2` driver program for Boost.Build engine.

```shell
git clone --recursive https://github.com/boostorg/boost.git
cd boost
git checkout develop
./bootstrap.sh
```

3. Optionally, create full content of `/boost` virtual directory with
   all Boost headers linked from the individual modular Boost libraries.
   If you skip this step, executing `b2` to run tests will automatically
   create the directory with all headers required by Boost.GIL and tests.

```
./b2 headers
```

### 2. Checkout Boost.GIL development branch

1. Go to the Boost.GIL library submodule.
2. Checkout the `develop` branch.

```shell
cd libs/gil
git checkout develop
git branch -vv
git pull origin develop
```

### 3. Run tests

Try running some Boost.GIL tests to check your environment is properly configured.

* Boost.GIL core tests only

```shell
cd libs/gil
../../b2 test
```

* All Boost.GIL tests, including tests of extensions:

```shell
cd libs/gil
../../b2
```

The [b2 invocation](https://www.boost.org/build/doc/html/bbv2/overview/invocation.html)
explains available options like `toolset`, `variant` and others.

### 4. Fork Boost.GIL repository on GitHub

Follow [Forking Projects](https://guides.github.com/activities/forking/) guide
to get personal copy of [boostorg/gil](https://github.com/boostorg/gil)
repository from where you will be able to submit new contributions as
[pull requests](https://help.github.com/articles/about-pull-requests/).

Add your fork as git remote to the Boost.GIL submodule:

```shell
cd libs/gil
git remote add username https://github.com/username/gil.git
```

### 5. Submit a pull request

All Boost.GIL contributions should be developed inside a topic branch created by
branching off the `develop` branch of [boostorg/gil](https://github.com/boostorg/gil).

**NOTE:** The branching workflow model
[Boost recommends](https://svn.boost.org/trac10/wiki/StartModWorkflow)
is called Git Flow.

For example:

```shell
cd libs/gil
git checkout develop
git checkout -b feature/foo
```

Now, you are set to to develop a new feature for Boost.GIL,
then [git add](https://git-scm.com/docs/git-add) and
[git commit](https://git-scm.com/docs/git-commit) your changes.

Once it's finished, you can submit it as pull request for review:

```shell
cd libs/gil
git checkout feature/foo
git push username feature/foo
```

Finally, sign in to your GitHub account and
[create a pull request](https://help.github.com/articles/creating-a-pull-request/).

Your pull request will be automatically built and tests will run on Travis CI
and AppVeyor (see [README](README.md) for builds status). Please, keep an eye
on those CI builds and correct any problems detected in your contribution
by updating your pull request.

### 6. Update your pull request

In simplest (and recommended) case , your the pull request you submitted earlier
has *a single commit*, so you can simply update the existing commit with any
modifications required to fix failing CI builds or requested by reviewers.

First, it is a good idea to synchronize your topic branch with the latest
changes in the upstream `develop` branch:

```shell
cd libs/gil
git checkout develop
git pull origin develop
git checkout feature/foo
git rebase develop
```

Next, make your edits.

Finally, `git commit --amend` the *single-commit* in your topic branch and
update the pull request:

```shell
cd libs/gil
git checkout feature/foo
git add -A
git commit --amend
git push --force username feature/foo
```

**WARNING:** Ensure your pull request has a single commit, otherwise the
force push can corrupt your pull request.

If you wish to update pull request adding a new commit, then create new
commit and issue regular push:

```shell
git commit -m "Fix variable name"
git push username feature/foo
```

## Development environment

### Building with Boost.Build

TODO

### Building with CMake

TODO
