//
// Copyright 2019 Olzhas Zhumabek <anonymous.from.applecity@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/numeric.hpp>
#include <boost/gil/image_processing/hessian.hpp>
#include <boost/gil/extension/numeric/matrix.hpp>

#include <boost/core/lightweight_test.hpp>

namespace gil = boost::gil;

template <typename View>
bool are_equal(View expected, View actual) {
    if (expected.dimensions() != actual.dimensions())
        return false;

    for (long int y = 0; y < expected.height(); ++y)
    {
        for (long int x = 0; x < expected.width(); ++x)
        {
            if (expected(x, y) != actual(x, y))
            {
                return false;
            }
        }
    }

    return true;
}

void test_blank_image()
{
    const gil::point_t dimensions(20, 20);
    gil::matrix_storage dx(dimensions);
    gil::matrix_storage dy(dimensions);

    gil::matrix_storage m11(dimensions);
    gil::matrix_storage m12_21(dimensions);
    gil::matrix_storage m22(dimensions);
    gil::matrix_storage expected(dimensions);
    gil::compute_hessian_entries(
        gil::view(dx),
        gil::view(dy),
        gil::view(m11),
        gil::view(m12_21),
        gil::view(m22)
    );
    BOOST_TEST(are_equal(gil::view(expected), gil::view(m11)));
    BOOST_TEST(are_equal(gil::view(expected), gil::view(m12_21)));
    BOOST_TEST(are_equal(gil::view(expected), gil::view(m22)));

    gil::matrix_storage hessian_response(dimensions);
    auto unnormalized_mean = gil::generate_unnormalized_mean(5);
    gil::compute_hessian_responses(
        gil::view(m11),
        gil::view(m12_21),
        gil::view(m22),
        unnormalized_mean,
        gil::view(hessian_response)
    );
    BOOST_TEST(are_equal(gil::view(expected), gil::view(hessian_response)));
}

int main(int argc, char* argv[])
{
    test_blank_image();
    return boost::report_errors();
}
