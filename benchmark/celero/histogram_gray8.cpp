//
// Copyright 2019 Mateusz Loskot <mateusz at loskot dot net>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
// Benchmark of various pixel iteration and access methods for a contiguous images
// composed of homogeneous pixels with single 8-bit integer channel (grayscale).
//
#include <boost/gil.hpp>
#include <celero/Celero.h>
#include <algorithm>
#include <cassert>
#include <cstdint>
#include <cmath>
#include <array>
#include <random>
#include <vector>
namespace gil = boost::gil;

class RandomImageGray8Fixture : public celero::TestFixture
{
public:
    RandomImageGray8Fixture() : gen_(rd_()), uid_(0, 255), histogram_{0}
    {
    }

    std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const final
    {
        // Generate sequence of square image sizes
        int const initial_power = static_cast<int>(std::ceil(std::log(initial_image_size_) / std::log(2)));

        std::vector<celero::TestFixture::ExperimentValue> problem_space;
        problem_space.resize(this->number_of_images_);
        for (int i = 0; i < this->number_of_images_; i++)
        {
            problem_space[i] = { int64_t(pow(2, initial_power + i)) };
        }
        return problem_space;
    }

    void setUp(const celero::TestFixture::ExperimentValue& experimentValue) final
    {
        auto const size = static_cast<std::ptrdiff_t>(experimentValue.Value);
        this->image_.recreate(size, size);
        auto view = gil::view(this->image_);
        for (auto it = view.begin(); it != view.end(); ++it)
            *it = static_cast<std::uint8_t>(uid_(gen_));
    }

    void clear_histogram()
    {
        this->histogram_.fill(0);
    }

    void assert_histogram()
    {
#ifndef NDEBUG
        std::size_t const total = std::accumulate(this->histogram_.cbegin(), this->histogram_.cend(), 0U);
        assert(total == gil::view(this->image_).size());
#endif
    }

    std::random_device rd_;
    std::mt19937 gen_;
    std::uniform_int_distribution<int> uid_;

    std::array<std::uint8_t, 256> histogram_;
    gil::gray8_image_t image_;
#ifdef NDEBUG
    int const initial_image_size_{64};
    int const number_of_images_{6};
#else
    int const initial_image_size_{2};
    int const number_of_images_{1};
#endif
};

#ifdef NDEBUG
constexpr int samples = 30; // Celero default minimum
constexpr int iterations = 100;
#else
constexpr int samples = 1;
constexpr int iterations = 1;
#endif

BASELINE_F(HistogramGray8, 1d_raw_pointer, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::view(this->image_);
    std::uint8_t* p = gil::interleaved_view_get_raw_data(view);
    for (std::size_t i = 0, size = view.size(); i < size; i++)
        ++h[p[i]];

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 1d_index_operator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::view(this->image_);
    std::ptrdiff_t const pixel_count = view.width() * view.height();
    for (std::ptrdiff_t i = 0; i < pixel_count; i++)
        ++h[view[i]];

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 1d_index_at, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::view(this->image_);
    std::ptrdiff_t const pixel_count = view.width() * view.height();
    for (std::ptrdiff_t i = 0; i < pixel_count; i++)
        ++h[*view.at(i)];

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 1d_iterator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    for (auto it = view.begin(), end = view.end(); it != end; ++it)
        ++h[*it];

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 1d_reverse_iterator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    for (auto it = view.rbegin(), end = view.rend(); it != end; ++it)
        ++h[*it];

    this->assert_histogram();
}


BENCHMARK_F(HistogramGray8, 1d_for_each_pixel, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    assert(view.is_1d_traversable());

    gil::for_each_pixel(view, [&h](gil::gray8c_pixel_t const& pixel) {
        ++h[pixel];
    });

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 2d_call_operator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    auto const view_h = view.height();
    auto const view_w = view.width();
    for (std::ptrdiff_t y = 0; y < view_h; y++)
    {
        for (std::ptrdiff_t x = 0; x < view_w; x++)
        {
            // The binary call operator() computes the location of the pixel in a 2D grid,
            // which involves addition and multiplication, thus it costs performance.
            ++h[view(x, y)];
        }
    }

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 2d_xy_locator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    auto loc = view.xy_at(0, 0);
    auto const view_h = view.height();
    auto const view_w = view.width();
    for (std::ptrdiff_t y = 0; y < view_h; y++)
    {
        for (std::ptrdiff_t x = 0; x < view_w; x++)
        {
            ++h[loc(0, 0)];
            ++loc.x();
        }
        // carriage return, equivalent of: loc.x() -= view.width(); ++loc.y();
        loc += gil::point_t{-view.width(), 1};
    }

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 2d_x_iterator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    auto const view_h = view.height();
    for (std::ptrdiff_t y = 0; y < view_h; y++)
    {
        // The two iterator types are raw C pointers, so the dereferencing
        // and arithmetic is a fast pointer indexing operator.
        gil::gray8c_view_t::x_iterator it = view.row_begin(y);
        gil::gray8c_view_t::x_iterator end = view.row_end(y);
        while (it != end)
            ++h[*it++];
    }

    this->assert_histogram();
}

BENCHMARK_F(HistogramGray8, 2d_y_iterator, RandomImageGray8Fixture, samples, iterations)
{
    this->clear_histogram();
    auto& h = this->histogram_;

    auto view = gil::const_view(this->image_);
    auto const view_w = view.width();
    for (std::ptrdiff_t x = 0; x < view_w; x++)
    {
        gil::gray8c_view_t::y_iterator it = view.col_begin(x);
        gil::gray8c_view_t::y_iterator end = view.col_end(x);
        while (it != end)
            ++h[*it++];
    }

    this->assert_histogram();
}

CELERO_MAIN
