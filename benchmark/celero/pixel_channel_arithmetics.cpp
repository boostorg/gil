#include <celero/Celero.h>

#include <boost/gil.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>

#include <iostream>
#include <vector>
#include <functional>
#include <stdexcept>

namespace gil = boost::gil;

constexpr std::size_t imageSize = 1024 * 1024;

class ChannelArithmeticsIntVectorFixture : public celero::TestFixture {
public:
    std::vector<std::uint16_t> img;
    ChannelArithmeticsIntVectorFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;
        return { celero::TestFixture::ExperimentValue(imageSize, 0) };
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
        img = std::vector<std::uint16_t>(imageSize, gil::gray16_pixel_t(0));
    }

    virtual void tearDown() override {
        if (!std::all_of(img.begin(), img.end(), [](auto p) { return p > 0; }))
            throw std::logic_error("doesn't calculate correctly");
    }
};

class ChannelArithmeticsPixelVectorFixture : public celero::TestFixture {
public:
    std::vector<gil::gray16_pixel_t> img;

    ChannelArithmeticsPixelVectorFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;
        return { celero::TestFixture::ExperimentValue(imageSize, 0) };
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
        img = std::vector<gil::gray16_pixel_t>(imageSize, gil::gray16_pixel_t(0));
    }

    virtual void tearDown() override {
        if (!std::all_of(img.begin(), img.end(), [](auto p) { return p > 0; }))
            throw std::logic_error("doesn't calculate correctly");
    }
};

class ChannelArithmeticsImageFixture : public celero::TestFixture {
public:
    gil::gray16_image_t img;
    ChannelArithmeticsImageFixture() {}

    virtual std::vector<celero::TestFixture::ExperimentValue> getExperimentValues() const override {
        std::vector<celero::TestFixture::ExperimentValue> problemSpace;
        return { celero::TestFixture::ExperimentValue(imageSize, 0) };
    }

    virtual void setUp(const celero::TestFixture::ExperimentValue& experimentValue) override {
        img = gil::gray16_image_t(1024, 1024, gil::gray16_pixel_t{ 0 });
        
    }

    virtual void tearDown() override {
        auto view = gil::view(img);
        if (!std::all_of(view.begin(), view.end(), [](auto p) { return p > 0; }))
            throw std::logic_error("doesn't calculate correctly");
    }
};

BASELINE_F(RawMemory, RawMemoryVector, ChannelArithmeticsIntVectorFixture, 2, 1'000) {
    std::cout << img.size() << '\n';
    for (auto&& p : img)
        p += 1;
}

BENCHMARK_F(GilImage, GilImageAlgorithm, ChannelArithmeticsImageFixture, 2, 1'000) {
    std::cout << img.height() * img.width() << '\n';
    celero::DoNotOptimizeAway(img);
    auto op = std::bind(
        gil::pixel_plus_t<gil::gray16_pixel_t, gil::gray16_pixel_t, gil::gray16_pixel_t>(),
        std::placeholders::_1, gil::gray16_pixel_t{ 1 });
    gil::transform_pixels(gil::const_view(img), gil::view(img), op);
}

BENCHMARK_F(GilPixelVector, PixelVectorReinterpret, ChannelArithmeticsPixelVectorFixture, 2, 1'000) {
    std::cout << img.size() << '\n';
    auto start = reinterpret_cast<std::uint16_t*>(img.data());
    auto end = start + img.size();
    for (; start != end; ++start)
        *start += 1;

}

CELERO_MAIN
