.. _cumulative_histogram:

Making a cumulative histogram
=============================

Overview
--------

A cumulative histogram is exactly what it sounds like. The GIL histogram class has the option
to convert itself into its cumulative version. Since the container needs to first get an ordering
over the keys a key sorting takes place before calculating the cumulative histogram. 

Example:

    .. code-block:: cpp

        histogram<int, float> h;
        /*
        Fill histogram ...
        */
        auto h1 = cumulative_histogram(h);

Tip: *In case you need to store the cumulative histogram elsewhere, consider creating a copy of the histogram
and then call the function*.