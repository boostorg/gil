ToolBox extension
=================

:authors:
   Christian Henning

.. comment
    [library Boost.GIL.ToolBox
    [quickbook 1.5]
    [id toolbox]
    [dirname gil/extension/toolbox]
    [purpose An mixed bag of some small extension for boost::gil]
    [category Image Processing]
    [authors [Henning, Christian]]
    [copyright 2012 Christian Henning]
    [source-mode c++]
    [license
		Distributed under the Boost Software License, Version 1.0.
		(See accompanying file LICENSE_1_0.txt or copy at
		[@http://www.boost.org/LICENSE_1_0.txt])
    ]
    ]

.. _Reference: reference.html

Overview
--------

ToolBox provides some helpful extension for boost::gil that are too small to be an extension themselves. There are:

* color spaces: cmyka, gray_alpha, hsl, hsv, lab, xyz
* metafunctions: channel_type_to_index, is_bit_aligned, is_similar, is_homogeneous, get_num_bits, channel_type, get_pixel_type, pixel_bit_size
* image types: indexed_image
* color converters: gray -> rgba

This extension will hopefully be added on by the community.

This extension is header-only. To use the toolbox just include the boost/gil/extension/toolbox.hpp.

Folder Structure
----------------

The toolbox tries to structure the small boost::gil extension into various folders. There is a folder for color_spaces, metafunctions, image_types, and color converters.

Acknowledgements
----------------

Thanks to all the people who have reviewed this library and made suggestions for improvements.

Reference
---------

The Reference_ section.

