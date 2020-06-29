.. _create_histogram:

Create a histogram
==================

**Method 1** - Using the histogram constructor

Syntax::

    histogram<Type1, Type2, Type3,... TypeN> 

Type1 .. TypeN correspond to the axis type of the N axes in the histogram

Example: If we want a 3D histogram of Axis1 of type `int`, Axis2 of type `float` and Axis3 of type `std::string`
we would do it this way::

    histogram<int, float, std::string> h;

And done.


**Method 2** - Using make_histogram()

There is an alternative way to create the histogram directly from
a GIL image view.

Syntax::

    auto h = make_histogram(view(image));

where *image* could be a `gray8_image_t`/`rgb8_image_t` object read from source.





