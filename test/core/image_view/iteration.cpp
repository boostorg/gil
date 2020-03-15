// the tests were adapted from contribution of evansds
// https://github.com/evansds
// link to original: https://github.com/boostorg/gil/issues/432#issue-561546949

#include <boost/gil/typedefs.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <boost/core/lightweight_test.hpp>

#include <vector>

using pixel_type = boost::gil::rgb8_pixel_t;

void test_col_end()
{
    std::vector<pixel_type> v(50);
    auto view = boost::gil::interleaved_view(10, 5, v.data(), 10 * sizeof(pixel_type));
    view.col_end(0);  // asserts
}

void test_row_end()
{
    std::vector<pixel_type> v(50);
    auto view = boost::gil::interleaved_view(10, 5, v.data(), 10 * sizeof(pixel_type));
    auto it = view.row_end(0);  // asserts
}

int main()
{
    test_col_end();
    test_row_end();
    return boost::report_errors();
}
