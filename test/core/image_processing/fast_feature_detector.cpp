//References- Following papers of Dr.Edward Rosten
/*1.Fusing points and lines for high performance tracking.
2.Machine learning for high-speed corner detection.
3.Faster and better: A machine learning approach to corner detection*/

#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/fast_feature_detector.hpp>
#include <boost/assert.hpp>
#include <boost/core/lightweight_test.hpp>
#include <iostream>
std::uint8_t null_matrix[] = {0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
                              0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0};
//testing an image without a feature point
void test1()
{
    boost::gil::gray8_view_t image1 = boost::gil::interleaved_view(
        5, 8, reinterpret_cast<boost::gil::gray8_pixel_t*>(null_matrix), 5);
    std::vector<boost::gil::point_t> keypoints;
    std::vector<int> scores;
    boost::gil::fast(image1, keypoints, scores, 20);
    std::vector<boost::gil::point_t> expected_keypoints;
    BOOST_ASSERT_MSG(
        expected_keypoints.size() == keypoints.size(), "dimensions do not match for keypoints");
}
//testing color image
void test2()
{
    boost::gil::rgb8_image_t input_color_image;
    boost::gil::read_image("box.jpg", input_color_image, boost::gil::jpeg_tag{});
    std::vector<boost::gil::point_t> keypoints;
    std::vector<int> scores;
    boost::gil::fast(boost::gil::view(input_color_image), keypoints, scores, 20);
    std::vector<boost::gil::point_t> expected_keypoints{
        boost::gil::point_t(218, 19),  boost::gil::point_t(44, 56),   boost::gil::point_t(280, 62),
        boost::gil::point_t(302, 77),  boost::gil::point_t(321, 93),  boost::gil::point_t(323, 93),
        boost::gil::point_t(329, 96),  boost::gil::point_t(45, 105),  boost::gil::point_t(101, 110),
        boost::gil::point_t(55, 118),  boost::gil::point_t(140, 141), boost::gil::point_t(326, 141),
        boost::gil::point_t(138, 143), boost::gil::point_t(314, 151), boost::gil::point_t(120, 179),
        boost::gil::point_t(130, 186), boost::gil::point_t(128, 187), boost::gil::point_t(132, 191),
        boost::gil::point_t(137, 195), boost::gil::point_t(139, 195), boost::gil::point_t(143, 197),
        boost::gil::point_t(59, 219),  boost::gil::point_t(63, 223),  boost::gil::point_t(70, 231),
        boost::gil::point_t(75, 237),  boost::gil::point_t(81, 244),  boost::gil::point_t(303, 261),
        boost::gil::point_t(107, 273), boost::gil::point_t(266, 273), boost::gil::point_t(260, 275),
        boost::gil::point_t(245, 280), boost::gil::point_t(115, 283), boost::gil::point_t(234, 284),
        boost::gil::point_t(231, 285), boost::gil::point_t(216, 289), boost::gil::point_t(125, 293),
        boost::gil::point_t(205, 294), boost::gil::point_t(132, 297), boost::gil::point_t(187, 299),
        boost::gil::point_t(171, 304), boost::gil::point_t(142, 313)};

    BOOST_ASSERT_MSG(
        expected_keypoints.size() == keypoints.size(), "dimensions do not match for keypoints");
    BOOST_ASSERT_MSG(expected_keypoints == keypoints, "keypoints do not match");
}
//testing grayscale image
void test3()
{
    boost::gil::rgb8_image_t input_color_image;
    boost::gil::read_image("box.jpg", input_color_image, boost::gil::jpeg_tag{});
    std::vector<boost::gil::point_t> keypoints;
    std::vector<int> scores;
    std::vector<boost::gil::point_t> expected_keypoints{
        boost::gil::point_t(218, 19),  boost::gil::point_t(44, 56),   boost::gil::point_t(280, 62),
        boost::gil::point_t(302, 77),  boost::gil::point_t(321, 93),  boost::gil::point_t(323, 93),
        boost::gil::point_t(329, 96),  boost::gil::point_t(45, 105),  boost::gil::point_t(101, 110),
        boost::gil::point_t(55, 118),  boost::gil::point_t(140, 141), boost::gil::point_t(326, 141),
        boost::gil::point_t(138, 143), boost::gil::point_t(314, 151), boost::gil::point_t(120, 179),
        boost::gil::point_t(130, 186), boost::gil::point_t(128, 187), boost::gil::point_t(132, 191),
        boost::gil::point_t(137, 195), boost::gil::point_t(139, 195), boost::gil::point_t(143, 197),
        boost::gil::point_t(59, 219),  boost::gil::point_t(63, 223),  boost::gil::point_t(70, 231),
        boost::gil::point_t(75, 237),  boost::gil::point_t(81, 244),  boost::gil::point_t(303, 261),
        boost::gil::point_t(107, 273), boost::gil::point_t(266, 273), boost::gil::point_t(260, 275),
        boost::gil::point_t(245, 280), boost::gil::point_t(115, 283), boost::gil::point_t(234, 284),
        boost::gil::point_t(231, 285), boost::gil::point_t(216, 289), boost::gil::point_t(125, 293),
        boost::gil::point_t(205, 294), boost::gil::point_t(132, 297), boost::gil::point_t(187, 299),
        boost::gil::point_t(171, 304), boost::gil::point_t(142, 313)};
    boost::gil::fast(boost::gil::view(input_color_image), keypoints, scores, 10);

    BOOST_ASSERT_MSG(
        expected_keypoints.size() == keypoints.size(), "dimensions do not match for keypoints");
    BOOST_ASSERT_MSG(expected_keypoints == keypoints, "keypoints do not match");
}
int main()
{
    test1();
    test2();
    test3();
    return boost::report_errors();
}
