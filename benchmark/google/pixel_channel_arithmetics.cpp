#include <algorithm>
#include <iostream>
#include <vector>

#include <benchmark/benchmark.h>

#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

using namespace boost::gil;

static void std_algo(benchmark::State& state)
{
    std::vector<uint16_t> img(1024 * 1024, 0);

    for (auto _ : state)
        // The code to benchmark
        for (auto&& p : img)
            p += 1;

    if (!std::all_of(img.begin(), img.end(), [&state](auto p) { return p == state.iterations(); }))
        state.SkipWithError("std_algo wrong result");
}
BENCHMARK(std_algo);

static void gil_algo_stdfunction(benchmark::State& state)
{
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });

    //Pixel operation
    auto op = std::bind(
        gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>(),
        _1, gray16_pixel_t{ 1 });

    for (auto _ : state)
        // The code to benchmark
        gil::transform_pixels(gil::const_view(img), gil::view(img), op);

    if (!std::all_of(gil::view(img).begin(), gil::view(img).end(), [&state](auto p) { return p == state.iterations(); }))
        state.SkipWithError("gil_algo_stdfunction wrong result");
}
BENCHMARK(gil_algo_stdfunction);

static void gil_algo_lambda(benchmark::State& state)
{
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>()(value, second_arg);
    };

    for (auto _ : state)
        // The code to benchmark
        gil::transform_pixels(gil::const_view(img), gil::view(img), op2);

    if (!std::all_of(gil::view(img).begin(), gil::view(img).end(), [&state](auto p) { return p == state.iterations(); }))
        state.SkipWithError("gil_algo_lambda wrong result");
}
BENCHMARK(gil_algo_lambda);

static void gil_algo_lambda_plus(benchmark::State& state)
{
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return value + second_arg;
    };

    for (auto _ : state)
        // The code to benchmark
        gil::transform_pixels(gil::const_view(img), gil::view(img), op2);

    if (!std::all_of(gil::view(img).begin(), gil::view(img).end(), [&state](auto p) { return p == state.iterations(); }))
        state.SkipWithError("gil_algo_lambda_plus wrong result");
}
BENCHMARK(gil_algo_lambda_plus);

static void gil_algo_lambda_copy(benchmark::State& state)
{
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });
    gil::gray16_image_t img2(1024, 1024, gray16_pixel_t{ 0 });

    auto input_view = gil::const_view(img);
    auto output_view = gil::view(img2);
    auto plus = gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>{};

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>()(value, second_arg);
    };

    for (auto _ : state)
        // The code to benchmark
        gil::transform_pixels(input_view, output_view, op2);

    if (!std::all_of(output_view.begin(), output_view.end(), [&state](auto p) { return p == 1; }))
        state.SkipWithError("gil_algo_lambda_copy wrong result");
}
BENCHMARK(gil_algo_lambda_copy);

static void gil_algo_rangefor(benchmark::State& state) {
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });
    gil::gray16_image_t img2(1024, 1024, gray16_pixel_t{ 0 });

    auto input_view = gil::const_view(img);
    auto output_view = gil::view(img2);
    auto plus = gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>{};

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>()(value, second_arg);
    };

    for (auto _ : state) {
        auto d_first = output_view.begin();
        for (auto pixel : input_view) {
            *d_first++ = op2(pixel);
        }
    }

    if (!std::all_of(output_view.begin(), output_view.end(), [&state](auto p) { return p == 1; }))
        state.SkipWithError("gil_algo_rangefor wrong result");
}
BENCHMARK(gil_algo_rangefor);

static void gil_algo_rangefor_plus(benchmark::State& state) {
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });
    gil::gray16_image_t img2(1024, 1024, gray16_pixel_t{ 0 });

    auto input_view = gil::const_view(img);
    auto output_view = gil::view(img2);
    auto plus = gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>{};

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>()(value, second_arg);
    };

    for (auto _ : state) {
        auto d_first = output_view.begin();
        for (const auto& pixel : input_view) {
            *d_first++ = pixel + 1;
        }
    }

    if (!std::all_of(output_view.begin(), output_view.end(), [&state](auto p) { return p == 1; }))
        state.SkipWithError("gil_algo_rangefor_plus wrong result");
}
BENCHMARK(gil_algo_rangefor_plus);

static void gil_algo_copy_mem(benchmark::State& state) {
    using namespace std::placeholders;
    namespace gil = boost::gil;

    gil::gray16_image_t img(1024, 1024, gray16_pixel_t{ 0 });
    gil::gray16_image_t img2(1024, 1024, gray16_pixel_t{ 0 });

    auto input_view = gil::const_view(img);
    auto output_view = gil::view(img2);

    //Pixel operation
    auto op2 = [second_arg = gray16_pixel_t{ 1 }](gray16_pixel_t value) {
        return gil::pixel_plus_t<gray16_pixel_t, gray16_pixel_t, gray16_pixel_t>()(value, second_arg);
    };

    for (auto _ : state) {
        auto start = std::addressof(input_view(0, 0));
        auto end = std::addressof(input_view(input_view.width() - 1, input_view.height() - 1)) + 1;
        auto d_start = std::addressof(output_view(0, 0));
        while (start != end) {
            *d_start++ = *start++ + 1;
        }
    }

    if (!std::all_of(output_view.begin(), output_view.end(), [&state](auto p) { return p == 1; }))
        state.SkipWithError("gil_algo_copy_mem wrong result");
}
BENCHMARK(gil_algo_copy_mem);

static void gil_algo_pixel_vector(benchmark::State& state) {
    using namespace std::placeholders;
    namespace gil = boost::gil;
    std::vector<gil::gray16_pixel_t> img(1024 * 1024, gil::gray16_pixel_t(0));

    for (auto _ : state) {
        for (auto& pixel : img)
            pixel.at(std::integral_constant<int, 0>{}) = pixel.at(std::integral_constant<int, 0>{}) + 1;
    }

    if (!std::all_of(img.begin(), img.end(), [&state](auto p) {return p == state.iterations(); }))
        state.SkipWithError("gil_algo_pixel_vector wrong result");
}
BENCHMARK(gil_algo_pixel_vector);

static void gil_algo_pixel_vector_reinterpret(benchmark::State& state) {
    using namespace std::placeholders;
    namespace gil = boost::gil;
    std::vector<gil::gray16_pixel_t> img(1024 * 1024, gil::gray16_pixel_t(0));

    for (auto _ : state) {
        auto start = reinterpret_cast<std::uint16_t*>(img.data());
        auto end = start + img.size();
        for (; start != end; ++start)
            *start += 1;
    }

    if (!std::all_of(img.begin(), img.end(), [&state](auto p) {return p == state.iterations(); }))
        state.SkipWithError("gil_algo_pixel_vector wrong result");
}
BENCHMARK(gil_algo_pixel_vector_reinterpret);

BENCHMARK_MAIN();
