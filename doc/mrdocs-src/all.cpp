//
// Copyright (c) 2026 Samuel Debionne
//
// Distributed under the Boost Software License, Version 1.0. (See accompanying
// file LICENSE_1_0.txt or copy at http://www.boost.org/LICENSE_1_0.txt)
//
// Official repository: https://github.com/boostorg/gil
//
// Single translation unit used by MrDocs (see ../mrdocs.yml) to extract GIL's
// public API reference. Deliberately excludes extension/io/{jpeg,png,tiff,...}
// (the format-specific backends), since those require the corresponding
// external image libraries (libjpeg, libpng, libtiff, ...) just to parse.
//
#include <boost/gil.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/toolbox/toolbox.hpp>
#include <boost/gil/extension/numeric/affine.hpp>
#include <boost/gil/extension/histogram/std.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/io/base.hpp>
#include <boost/gil/io/bit_operations.hpp>
#include <boost/gil/io/conversion_policies.hpp>
#include <boost/gil/io/device.hpp>
#include <boost/gil/io/error.hpp>
#include <boost/gil/io/get_read_device.hpp>
#include <boost/gil/io/get_reader.hpp>
#include <boost/gil/io/get_write_device.hpp>
#include <boost/gil/io/get_writer.hpp>
#include <boost/gil/io/make_backend.hpp>
#include <boost/gil/io/make_dynamic_image_reader.hpp>
#include <boost/gil/io/make_dynamic_image_writer.hpp>
#include <boost/gil/io/make_reader.hpp>
#include <boost/gil/io/make_scanline_reader.hpp>
#include <boost/gil/io/make_writer.hpp>
#include <boost/gil/io/path_spec.hpp>
#include <boost/gil/io/read_and_convert_image.hpp>
#include <boost/gil/io/read_and_convert_view.hpp>
#include <boost/gil/io/reader_base.hpp>
#include <boost/gil/io/read_image.hpp>
#include <boost/gil/io/read_image_info.hpp>
#include <boost/gil/io/read_view.hpp>
#include <boost/gil/io/row_buffer_helper.hpp>
#include <boost/gil/io/scanline_read_iterator.hpp>
#include <boost/gil/io/typedefs.hpp>
#include <boost/gil/io/write_view.hpp>
