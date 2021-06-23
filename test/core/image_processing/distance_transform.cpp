//
// Copyright 2021 Harshit Pant <harshitpant83@gmail.com>
//
// Use, modification and distribution are subject to the Boost Software License,
// Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt)
//
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_processing/distance_transform.hpp>
#include <boost/core/lightweight_test.hpp>
#include <vector>

namespace gil = boost::gil;

template<typename T, typename ImgView>
void fill_image_view(std::vector<std::vector<T> >& values, ImgView image_view)
{
    for (std::ptrdiff_t y = 0; y < image_view.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < image_view.width(); ++x)
        {
            image_view(x, y)[0] = values[y][x];
        }
    }
}

template<typename ImgView1, typename ImgView2>
void check_for_negligible_varition(ImgView1 const& image_view1, ImgView2  const& image_view2)
{
    for (std::ptrdiff_t y = 0; y < image_view1.height(); ++y)
    {
        for (std::ptrdiff_t x = 0; x < image_view1.width(); ++x)
        {
            float variation = std::abs(image_view1(x, y)[0] - image_view2(x, y)[0]);
            BOOST_TEST_LT(variation, 0.001);
        }
    }
}

void test_manhattan_uint8_t_input_uint8_t_output_distance_from_off_pixels()
{

    gil::gray8_image_t img_in(7, 7);
    gil::gray8_image_t img_expected(7, 7);
    gil::gray8_image_t img_out(7, 7);

    // test values from wikipedia, in 8 bit binary form
    std::vector<std::vector<int>> val_in
    {
        {0,   0,   0,   0,   0,   0, 0},
        {0, 255, 255, 255, 255, 255, 0},
        {0, 255, 255, 255, 255, 255, 0},
        {0, 255, 255, 255, 255, 255, 0},
        {0, 255, 255, 255, 255, 255, 0},
        {0, 255, 255, 255, 255, 255, 0},
        {0,   0,   0,   0,   0,   0, 0}
    };

    std::vector<std::vector<int>> val_exp
    {
        {0, 0, 0, 0, 0, 0, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 1, 2, 2, 2, 1, 0},
        {0, 1, 2, 3, 2, 1, 0},
        {0, 1, 2, 2, 2, 1, 0},
        {0, 1, 1, 1, 1, 1, 0},
        {0, 0, 0, 0, 0, 0, 0}
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp, view(img_expected));

    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::manhattan, gil::mask_size::three);

    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));
}


void test_chessboard_uint16_t_input_uint8_t_output_distance_from_on_pixels()
{
    gil::gray16_image_t img_in(7, 7);
    gil::gray8_image_t img_expected(7, 7);
    gil::gray8_image_t img_out(7, 7);
    gil::gray32f_image_t img_expected_float32_t(7, 7);
    gil::gray32f_image_t img_out_float32_t(7, 7);

    std::vector<std::vector<int>> val_in
    {
        {0, 65535,  0,      0,  0,  0, 0},
        {0,     0,  0,      0,  0,  0, 0},
        {0,     0,  0,      0,  0,  0, 0},
        {0,     0,  0,      0,  0,  0, 0},
        {0,     0,  0,  65535,  0,  0, 0},
        {0,     0,  0,      0,  0,  0, 0},
        {0,     0,  0,      0,  0,  0, 0}
    };

    std::vector<std::vector<int>> val_exp_chessboard_three
    {
        {1, 0, 1, 2, 3, 4, 4},
        {1, 1, 1, 2, 3, 3, 3},
        {2, 2, 2, 2, 2, 2, 3},
        {3, 2, 1, 1, 1, 2, 3},
        {3, 2, 1, 0, 1, 2, 3},
        {3, 2, 1, 1, 1, 2, 3},
        {3, 2, 2, 2, 2, 2, 3}
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp_chessboard_three, view(img_expected));

    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::on_pixels,
                            gil::distance_type::chessboard, gil::mask_size::three);

    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

    std::vector<std::vector<int>> val_exp_manhattan_five
    {
        {1, 0, 1, 2, 3, 4, 5},
        {2, 1, 2, 3, 4, 5, 6},
        {3, 2, 3, 2, 3, 4, 5},
        {4, 3, 2, 1, 2, 3, 4},
        {3, 2, 1, 0, 1, 2, 3},
        {4, 3, 2, 1, 2, 3, 4},
        {5, 4, 3, 2, 3, 4, 5}
    };

    fill_image_view(val_exp_manhattan_five, view(img_expected));

    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::on_pixels,
                            gil::distance_type::manhattan, gil::mask_size::five);

    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

    std::vector<std::vector<float>> val_exp_euclidean_precise
    {
        {         1,         0,           1,         2,           3,         4,            5},
        {1.4142135f,         1,  1.4142135f, 2.236068f,  3.1622777f, 3.6055512f,  4.2426405f},
        { 2.236068f,         2,   2.236068f,         2,   2.236068f,  2.828427f,  3.6055512f},
        {3.1622777f, 2.236068f,  1.4142135f,         1,  1.4142135f,  2.236068f,  3.1622777f},
        {         3,         2,           1,         0,           1,          2,           3},
        {3.1622777f, 2.236068f,  1.4142135f,         1,  1.4142135f,  2.236068f,  3.1622777f},
        {3.6055512f, 2.828427f,   2.236068f,         2,   2.236068f,  2.828427f,  3.6055512f}
    };

    fill_image_view(val_exp_euclidean_precise, view(img_expected_float32_t));

    gil::distance_transform(view(img_in), view(img_out_float32_t), gil::distance_from::on_pixels,
                            gil::distance_type::precise_euclidean, gil::mask_size::not_applicable);

    BOOST_TEST(equal_pixels(const_view(img_out_float32_t), const_view(img_expected_float32_t)));
}

void test_euclidean_approx_and_manhattan_uint8_t_input_float32_t_output_distance_from_off_pixels()
{
    gil::gray8_image_t img_in(15, 15);
    gil::gray32f_image_t img_expected(15, 15);
    gil::gray32f_image_t img_out(15, 15);

    std::vector<std::vector<int>> val_in
    {
        {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        {255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255,   0,   0,   0,   0,   0, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255,   0,   0,   0,   0,   0, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255},
        {255,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0,   0, 255},
        {255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255}
    };

    std::vector<std::vector<float>> val_exp1
    {
        {1.4f, 1, 1, 1,    1, 1, 1, 1, 1, 1,    1, 1, 1, 1, 1.4f},
        {   1, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0,    1},
        {   1, 0, 1, 1,    1, 1, 1, 1, 1, 1,    1, 1, 1, 0,    1},
        {   1, 0, 1, 2,    2, 2, 2, 2, 2, 2,    2, 2, 1, 0,    1},
        {   1, 0, 1, 2, 1.4f, 1, 1, 1, 1, 1, 1.4f, 2, 1, 0,    1},
        {   1, 0, 1, 2,    1, 0, 0, 0, 0, 0,    1, 2, 1, 0,    1},
        {   1, 0, 1, 2,    1, 0, 1, 1, 1, 0,    1, 2, 1, 0,    1},
        {   1, 0, 1, 2,    1, 0, 1, 2, 1, 0,    1, 2, 1, 0,    1},
        {   1, 0, 1, 2,    1, 0, 1, 1, 1, 0,    1, 2, 1, 0,    1},
        {   1, 0, 1, 2,    1, 0, 0, 0, 0, 0,    1, 2, 1, 0,    1},
        {   1, 0, 1, 2, 1.4f, 1, 1, 1, 1, 1, 1.4f, 2, 1, 0,    1},
        {   1, 0, 1, 2,    2, 2, 2, 2, 2, 2,    2, 2, 1, 0,    1},
        {   1, 0, 1, 1,    1, 1, 1, 1, 1, 1,    1, 1, 1, 0,    1},
        {   1, 0, 0, 0,    0, 0, 0, 0, 0, 0,    0, 0, 0, 0,    1},
        {1.4f, 1, 1, 1,    1, 1, 1, 1, 1, 1,    1, 1, 1, 1, 1.4f}
    };

    std::vector<std::vector<float>> val_exp2
    {
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1},
        {1, 0, 1, 2, 2, 1, 1, 1, 1, 1, 2, 2, 1, 0, 1},
        {1, 0, 1, 2, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1},
        {1, 0, 1, 2, 1, 0, 1, 1, 1, 0, 1, 2, 1, 0, 1},
        {1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1, 2, 1, 0, 1},
        {1, 0, 1, 2, 1, 0, 1, 1, 1, 0, 1, 2, 1, 0, 1},
        {1, 0, 1, 2, 1, 0, 0, 0, 0, 0, 1, 2, 1, 0, 1},
        {1, 0, 1, 2, 2, 1, 1, 1, 1, 1, 2, 2, 1, 0, 1},
        {1, 0, 1, 2, 2, 2, 2, 2, 2, 2, 2, 2, 1, 0, 1},
        {1, 0, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 0, 1},
        {1, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 1},
        {2, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 1, 2}
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp1, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::euclidean_approximation, gil::mask_size::five);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

    fill_image_view(val_exp2, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::manhattan, gil::mask_size::five);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

}

void test_all_uint8_t_input_float32_t_ouptut_distance_from_off_pixels()
{
    gil::gray8_image_t img_in(15, 15);
    gil::gray32f_image_t img_expected(15, 15);
    gil::gray32f_image_t img_out(15, 15);

    std::vector<std::vector<int>> val_in
    {
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255,   0, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255},
        { 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255, 255,   0}
    };

    std::vector<std::vector<float>> val_exp_precise
    {
        {3.6055512f,  3.1622777f,          3, 3.1622777f, 3.6055512f, 4.2426405f,  4.1231055f, 3.1622777f,
          2.236068f,  1.4142135f,          1, 1.4142135f,  2.236068f, 3.1622777f, 4.1231055f},
        { 2.828427f,   2.236068f,          2,  2.236068f,  2.828427f, 3.6055512f,           4,
                  3,           2,          1,          0,          1,          2,           3,         4},
        { 2.236068f,  1.4142135f,          1, 1.4142135f,  2.236068f, 3.1622777f,  4.1231055f, 3.1622777f,
          2.236068f,  1.4142135f,          1, 1.4142135f,  2.236068f, 3.1622777f, 4.1231055f},
        {         2,           1,          0,          1,          2,          3,           4, 3.6055512f,
          2.828427f,   2.236068f,          2,  2.236068f,  2.828427f, 3.6055512f,  4.472136f},
        { 2.236068f,  1.4142135f,          1, 1.4142135f,  2.236068f, 3.1622777f,  4.1231055f, 4.2426405f,
         3.6055512f,  3.1622777f,          3, 3.1622777f, 3.6055512f, 4.2426405f,          5},
        { 2.828427f,   2.236068f,          2,  2.236068f,  2.828427f, 3.6055512f,   4.472136f,          5,
          4.472136f,  4.1231055f,          4, 4.1231055f,  4.472136f,          5,  5.656854f},
        {3.6055512f,  3.1622777f,          3, 3.1622777f, 3.6055512f, 4.2426405f,           5, 5.8309517f,
         5.3851647f,  5.0990195f,          5, 5.0990195f, 5.3851647f, 5.8309517f, 6.4031243f},
        { 4.472136f,  4.1231055f,          4, 4.1231055f,  4.472136f,          5,   5.656854f, 6.4031243f,
         6.3245554f,  6.0827622f,          6, 6.0827622f, 6.3245554f, 6.7082043f, 7.0000005f},
        {5.3851647f,  5.0990195f,          5, 5.0990195f, 5.3851647f, 5.8309517f,  6.4031243f, 7.0710683f,
         7.2801094f,  7.0710683f, 7.0000005f, 6.7082043f, 6.3245554f, 6.0827622f,          6},
        {6.3245554f,  6.0827622f,          6, 6.0827622f, 6.3245554f, 6.7082043f,  7.2111025f, 7.8102503f,
         7.8102503f,  7.0710683f, 6.4031243f, 5.8309517f, 5.3851647f, 5.0990195f,          5},
        {7.2801094f,  7.0710683f, 7.0000005f, 7.0710683f, 7.2801094f, 7.6157737f,   8.062258f,  8.062258f,
         7.2111025f,  6.4031243f,  5.656854f,          5,  4.472136f, 4.1231055f,          4},
        { 8.246211f,   8.062258f,          8,  8.062258f,  8.246211f, 8.5440035f,  8.5440035f, 7.6157737f,
         6.7082043f,  5.8309517f,          5, 4.2426405f, 3.6055512f, 3.1622777f,          3},
        { 9.219544f,   9.055386f,          9,  9.055386f,  9.219544f,  9.219544f,   8.246211f, 7.2801094f,
         6.3245554f,  5.3851647f,  4.472136f, 3.6055512f,  2.828427f,  2.236068f,          2},
        {10.198039f,  10.049875f,         10, 10.049875f, 10.049875f,  9.055386f,   8.062258f, 7.0710683f,
         6.0827622f,  5.0990195f, 4.1231055f, 3.1622777f,  2.236068f, 1.4142135f,          1},
        {11.180341f,  11.045361f,         11,         11,         10,          9,           8,          7,
                  6,           5,          4,          3,          2,          1,           0}
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp_precise, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::precise_euclidean, gil::mask_size::not_applicable);

    check_for_negligible_varition(view(img_out), view(img_expected));

    std::vector<std::vector<float>> val_exp_euclidean_three
    {
         { 3.6935883f,   3.2792969f,     2.8650055f,   3.2792969f,    3.6935883f,    4.1078796f,
           4.2342987f,   3.2792969f,      2.324295f,   1.3692932f,   0.95500183f,    1.3692932f,
            2.324295f,   3.2792969f,    4.2342987f},
         { 2.7385864f,    2.324295f,     1.9100037f,    2.324295f,    2.7385864f,    3.6935883f,
           3.8200073f,   2.8650055f,     1.9100037f,  0.95500183f,             0,   0.95500183f,
           1.9100037f,   2.8650055f,    3.8200073f},
         {  2.324295f,   1.3692932f,    0.95500183f,   1.3692932f,     2.324295f,    3.2792969f,
           4.2342987f,   3.2792969f,      2.324295f,   1.3692932f,   0.95500183f,    1.3692932f,
            2.324295f,   3.2792969f,    4.2342987f},
         { 1.9100037f,  0.95500183f,              0,  0.95500183f,    1.9100037f,    2.8650055f,
           3.8200073f,   3.6935883f,     2.7385864f,    2.324295f,    1.9100037f,     2.324295f,
           2.7385864f,   3.6935883f,      4.64859f},
         {  2.324295f,   1.3692932f,    0.95500183f,   1.3692932f,     2.324295f,    3.2792969f,
           4.2342987f,   4.1078796f,     3.6935883f,   3.2792969f,    2.8650055f,    3.2792969f,
           3.6935883f,   4.1078796f,    5.0628815f},
         { 2.7385864f,    2.324295f,     1.9100037f,    2.324295f,    2.7385864f,    3.6935883f,
             4.64859f,   5.0628815f,       4.64859f,   4.2342987f,    3.8200073f,    4.2342987f,
             4.64859f,   5.0628815f,     5.477173f},
         { 3.6935883f,   3.2792969f,     2.8650055f,   3.2792969f,    3.6935883f,    4.1078796f,
           5.0628815f,   6.0178833f,      5.603592f,   5.1893005f,     4.775009f,    5.1893005f,
            5.603592f,   6.0178833f,    6.4321747f},
         {   4.64859f,   4.2342987f,     3.8200073f,   4.2342987f,      4.64859f,    5.0628815f,
            5.477173f,   6.4321747f,     6.5585938f,   6.1443024f,     5.730011f,    6.1443024f,
           6.5585938f,    6.972885f,     6.685013f},
         {  5.603592f,   5.1893005f,      4.775009f,   5.1893005f,     5.603592f,    6.0178833f,
           6.4321747f,    6.846466f,     7.5135956f,    7.099304f,     6.685013f,     6.972885f,
           6.5585938f,   6.1443024f,     5.730011f},
         { 6.5585938f,   6.1443024f,      5.730011f,   6.1443024f,    6.5585938f,     6.972885f,
           7.3871765f,    7.801468f,      7.801468f,    6.846466f,    6.4321747f,    6.0178833f,
            5.603592f,   5.1893005f,     4.775009f},
         { 7.5135956f,    7.099304f,      6.685013f,    7.099304f,    7.5135956f,     7.927887f,
            8.342178f,    8.342178f,     7.3871765f,   6.4321747f,     5.477173f,    5.0628815f,
             4.64859f,   4.2342987f,    3.8200073f},
         {  8.468597f,    8.054306f,     7.6400146f,    8.054306f,     8.468597f,     8.882889f,
            8.882889f,    7.927887f,      6.972885f,   6.0178833f,    5.0628815f,    4.1078796f,
           3.6935883f,   3.2792969f,    2.8650055f},
         {  9.423599f,    9.009308f,     8.5950165f,    9.009308f,     9.423599f,     9.423599f,
            8.468597f,   7.5135956f,     6.5585938f,    5.603592f,      4.64859f,    3.6935883f,
           2.7385864f,    2.324295f,    1.9100037f},
         { 10.378601f,     9.96431f,      9.550018f,     9.96431f,      9.96431f,     9.009308f,
            8.054306f,    7.099304f,     6.1443024f,   5.1893005f,    4.2342987f,    3.2792969f,
            2.324295f,   1.3692932f,   0.95500183f},
         { 11.333603f,   10.919312f,      10.50502f,    10.50502f,     9.550018f,    8.5950165f,
           7.6400146f,    6.685013f,      5.730011f,    4.775009f,    3.8200073f,    2.8650055f,
           1.9100037f,  0.95500183f,            0.f} 
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp_euclidean_three, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::euclidean_approximation, gil::mask_size::three);

    check_for_negligible_varition(view(img_out), view(img_expected));

    std::vector<std::vector<float>> val_exp_euclidean_five
    {
        {   3.5969f,    3.1969f,           3,    3.1969f,    3.5969f,       4.2f,
            4.1969f,    3.1969f,     2.1969f,       1.4f,          1,       1.4f,
            2.1969f,    3.1969f,    4.1969f},
        {      2.8f,    2.1969f,           2,    2.1969f,       2.8f,    3.5969f,
                  4,          3,           2,          1,          0,          1,
                  2,          3,          4},
        {   2.1969f,       1.4f,           1,       1.4f,    2.1969f,    3.1969f,
            4.1969f,    3.1969f,     2.1969f,       1.4f,          1,       1.4f,
            2.1969f,    3.1969f,    4.1969f},
        {         2,          1,           0,          1,          2,          3,
                  4,    3.5969f,        2.8f,    2.1969f,          2,    2.1969f,
               2.8f,    3.5969f,    4.3938f},
        {   2.1969f,       1.4f,           1,       1.4f,    2.1969f,    3.1969f,
            4.1969f,       4.2f,     3.5969f,    3.1969f,          3,    3.1969f,
            3.5969f,       4.2f, 4.9968996f},
        {      2.8f,    2.1969f,           2,    2.1969f,       2.8f,    3.5969f,
            4.3938f, 4.9968996f,     4.3938f,    4.1969f,          4,    4.1969f,
            4.3938f, 4.9968996f,       5.6f},
        {   3.5969f,    3.1969f,           3,    3.1969f,    3.5969f,       4.2f,
         4.9968996f,    5.7938f,     5.3938f,    5.1969f,          5,    5.1969f,
            5.3938f,    5.7938f, 6.3968997f},
        {   4.3938f,    4.1969f,           4,    4.1969f,    4.3938f, 4.9968996f,
               5.6f, 6.3968997f,     6.3938f,    6.1969f,          6,    6.1969f,
            6.3938f, 6.5906997f,          7},
        {   5.3938f,    5.1969f,           5,    5.1969f,    5.3938f,    5.7938f,
         6.3968997f,          7,     7.3938f,    7.1969f,          7, 6.5906997f,
            6.3938f,    6.1969f,          6},
        {   6.3938f,    6.1969f,           6,    6.1969f,    6.3938f, 6.5906997f,
         7.1937995f,    7.7969f,     7.7969f,          7, 6.3968997f,    5.7938f,
            5.3938f,    5.1969f,          5},
        {   7.3938f,    7.1969f,           7,    7.1969f,    7.3938f, 7.5906997f,
            7.9907f,    7.9907f,  7.1937995f, 6.3968997f,       5.6f, 4.9968996f,
            4.3938f,    4.1969f,          4},
        {   8.3938f,  8.196899f,           8,  8.196899f,    8.3938f,    8.5907f,
            8.5907f, 7.5906997f,  6.5906997f,    5.7938f, 4.9968996f,       4.2f,
            3.5969f,    3.1969f,          3},
        {   9.3938f,  9.196899f,           9,  9.196899f,    9.3938f,    9.3938f,
            8.3938f,    7.3938f,     6.3938f,    5.3938f,    4.3938f,    3.5969f,
               2.8f,    2.1969f,          2},
        {10.393799f, 10.196899f,          10, 10.196899f, 10.196899f,  9.196899f,
          8.196899f,    7.1969f,     6.1969f,    5.1969f,    4.1969f,    3.1969f,
            2.1969f,       1.4f,          1},
        {11.393799f, 11.196899f,          11,         11,         10,          9,
                  8,          7,           6,          5,          4,          3,
                  2,          1,           0}
    };

    fill_image_view(val_in, view(img_in));
    fill_image_view(val_exp_euclidean_five, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::euclidean_approximation, gil::mask_size::five);

    check_for_negligible_varition(view(img_out), view(img_expected));


    std::vector<std::vector<float>> val_exp_manhatten_three
    {
        { 5,  4,  3,  4,  5,  6,  5,  4,  3,  2,  1,  2,  3,  4,  5},
        { 4,  3,  2,  3,  4,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4},
        { 3,  2,  1,  2,  3,  4,  5,  4,  3,  2,  1,  2,  3,  4,  5},
        { 2,  1,  0,  1,  2,  3,  4,  5,  4,  3,  2,  3,  4,  5,  6},
        { 3,  2,  1,  2,  3,  4,  5,  6,  5,  4,  3,  4,  5,  6,  7},
        { 4,  3,  2,  3,  4,  5,  6,  7,  6,  5,  4,  5,  6,  7,  8},
        { 5,  4,  3,  4,  5,  6,  7,  8,  7,  6,  5,  6,  7,  8,  8},
        { 6,  5,  4,  5,  6,  7,  8,  9,  8,  7,  6,  7,  8,  8,  7},
        { 7,  6,  5,  6,  7,  8,  9, 10,  9,  8,  7,  8,  8,  7,  6},
        { 8,  7,  6,  7,  8,  9, 10, 11, 10,  9,  8,  8,  7,  6,  5},
        { 9,  8,  7,  8,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4},
        {10,  9,  8,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3},
        {11, 10,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2},
        {12, 11, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1},
        {13, 12, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0}
    };

    fill_image_view(val_exp_manhatten_three, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::manhattan, gil::mask_size::three);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));



    std::vector<std::vector<float>> val_exp_manhatten_five
    {
        { 5,  4,  3,  4,  5,  6,  5,  4,  3,  2,  1,  2,  3,  4,  5},
        { 4,  3,  2,  3,  4,  5,  4,  3,  2,  1,  0,  1,  2,  3,  4},
        { 3,  2,  1,  2,  3,  4,  5,  4,  3,  2,  1,  2,  3,  4,  5},
        { 2,  1,  0,  1,  2,  3,  4,  5,  4,  3,  2,  3,  4,  5,  6},
        { 3,  2,  1,  2,  3,  4,  5,  6,  5,  4,  3,  4,  5,  6,  7},
        { 4,  3,  2,  3,  4,  5,  6,  7,  6,  5,  4,  5,  6,  7,  8},
        { 5,  4,  3,  4,  5,  6,  7,  8,  7,  6,  5,  6,  7,  8,  8},
        { 6,  5,  4,  5,  6,  7,  8,  9,  8,  7,  6,  7,  8,  8,  7},
        { 7,  6,  5,  6,  7,  8,  9, 10,  9,  8,  7,  8,  8,  7,  6},
        { 8,  7,  6,  7,  8,  9, 10, 11, 10,  9,  8,  8,  7,  6,  5},
        { 9,  8,  7,  8,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4},
        {10,  9,  8,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3},
        {11, 10,  9, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2},
        {12, 11, 10, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1},
        {13, 12, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0}
    };

    fill_image_view(val_exp_manhatten_five, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::manhattan, gil::mask_size::five);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));


    std::vector<std::vector<float>> val_exp_chessboard_three
    {
        { 3,  3,  3,  3,  3,  3,  4,  3,  2,  1,  1,  1,  2,  3,  4},
        { 2,  2,  2,  2,  2,  3,  4,  3,  2,  1,  0,  1,  2,  3,  4},
        { 2,  1,  1,  1,  2,  3,  4,  3,  2,  1,  1,  1,  2,  3,  4},
        { 2,  1,  0,  1,  2,  3,  4,  3,  2,  2,  2,  2,  2,  3,  4},
        { 2,  1,  1,  1,  2,  3,  4,  3,  3,  3,  3,  3,  3,  3,  4},
        { 2,  2,  2,  2,  2,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4},
        { 3,  3,  3,  3,  3,  3,  4,  5,  5,  5,  5,  5,  5,  5,  5},
        { 4,  4,  4,  4,  4,  4,  4,  5,  6,  6,  6,  6,  6,  6,  6},
        { 5,  5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6},
        { 6,  6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5},
        { 7,  7,  7,  7,  7,  7,  7,  7,  6,  5,  4,  4,  4,  4,  4},
        { 8,  8,  8,  8,  8,  8,  8,  7,  6,  5,  4,  3,  3,  3,  3},
        { 9,  9,  9,  9,  9,  9,  8,  7,  6,  5,  4,  3,  2,  2,  2},
        {10, 10, 10, 10, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  1},
        {11, 11, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0}
    };
    
    fill_image_view(val_exp_chessboard_three, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::chessboard, gil::mask_size::three);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

    std::vector<std::vector<float>> val_exp_chessboard_five
    {
        {3,   3,  3,  3,  3,  3,  4,  3,  2,  1,  1,  1,  2,  3,  4},
        {2,   2,  2,  2,  2,  3,  4,  3,  2,  1,  0,  1,  2,  3,  4},
        {2,   1,  1,  1,  2,  3,  4,  3,  2,  1,  1,  1,  2,  3,  4},
        {2,   1,  0,  1,  2,  3,  4,  3,  2,  2,  2,  2,  2,  3,  4},
        {2,   1,  1,  1,  2,  3,  4,  3,  3,  3,  3,  3,  3,  3,  4},
        {2,   2,  2,  2,  2,  3,  4,  4,  4,  4,  4,  4,  4,  4,  4},
        {3,   3,  3,  3,  3,  3,  4,  5,  5,  5,  5,  5,  5,  5,  5},
        {4,   4,  4,  4,  4,  4,  4,  5,  6,  6,  6,  6,  6,  6,  6},
        {5,   5,  5,  5,  5,  5,  5,  5,  6,  6,  6,  6,  6,  6,  6},
        {6,   6,  6,  6,  6,  6,  6,  6,  6,  5,  5,  5,  5,  5,  5},
        {7,   7,  7,  7,  7,  7,  7,  7,  6,  5,  4,  4,  4,  4,  4},
        {8,   8,  8,  8,  8,  8,  8,  7,  6,  5,  4,  3,  3,  3,  3},
        {9,   9,  9,  9,  9,  9,  8,  7,  6,  5,  4,  3,  2,  2,  2},
        {10, 10, 10, 10, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  1},
        {11, 11, 11, 11, 10,  9,  8,  7,  6,  5,  4,  3,  2,  1,  0}
    };
    
    fill_image_view(val_exp_chessboard_five, view(img_expected));
    gil::distance_transform(view(img_in), view(img_out), gil::distance_from::off_pixels,
                            gil::distance_type::chessboard, gil::mask_size::five);
    BOOST_TEST(equal_pixels(const_view(img_out), const_view(img_expected)));

}

int main()
{
    test_manhattan_uint8_t_input_uint8_t_output_distance_from_off_pixels();
    test_chessboard_uint16_t_input_uint8_t_output_distance_from_on_pixels();
    test_euclidean_approx_and_manhattan_uint8_t_input_float32_t_output_distance_from_off_pixels();
    test_all_uint8_t_input_float32_t_ouptut_distance_from_off_pixels();
    return ::boost::report_errors();
}
