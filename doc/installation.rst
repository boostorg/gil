Installation
============

The latest version of Boost.GIL can be downloaded from https://github.com/boostorg/gil.

The Boost.GIL is a header-only library. Meaning, it consists of header files only,
it does not require Boost to be built and it does not require any libraries
to link against.

.. note::

   The exception to dependencies-free rule of GIL is the I/O extension
   which requires client libraries implementing popular image formats
   like libpng, libjpeg, etc.

In order to use Boost.GIL, including ``boost/gil.hpp`` and telling your compiler
where to find Boost and GIL headers should be sufficient for most projects.

Compiling
---------

The Boost.GIL library source code should successfully compile with any
compiler with complete C++11 support.

.. note::

   We are planning to drop support for require C++14 support in Boost 1.76 or later,
   or selectively drop support for GCC 5 due to its issues with inheriting constructors,
   see `discussion for PR #526 <https://github.com/boostorg/gil/pull/526>`_.

For the actual list of currently tested compilers, check results of the library CI
builds linked from the `README.md <https://github.com/boostorg/gil/blob/develop/README.md>`_
or inspect the CI services configuration files in the `develop branch <https://github.com/boostorg/gil/tree/develop>`_
of the library repository.
