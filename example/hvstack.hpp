#include "boost/gil/image_view_factory.hpp"
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <stdexcept>
#include <vector>

namespace boost { namespace gil {
template <typename View>
void hstack(const std::vector<View>& views, const View& output_view)
{
    if (views.size() == 0)
    {
        throw std::invalid_argument("empty views vector is passed - cannot create stacked image");
    }

    auto dimensions = views.front().dimensions();
    for (const auto& view : views)
    {
        if (view.dimensions() != dimensions)
        {
            throw std::invalid_argument("one or many views are not of the same dimension");
        }
    }

    auto full_width = dimensions.x * views.size();
    if (output_view.width() != full_width || output_view.height() != dimensions.y)
    {
        throw std::invalid_argument("the destination view is not of the right dimensions");
    }

    for (std::size_t i = 0; i < views.size(); ++i)
    {
        auto subview = subimage_view(output_view, dimensions.x * i, 0, dimensions.x, dimensions.y);
        copy_pixels(views[i], subview);
    }
}

template <typename View>
image<typename View::value_type> hstack(const std::vector<View>& views)
{
    if (views.size() == 0)
    {
        throw std::invalid_argument("empty views vector is passed - cannot create stacked image");
    }

    auto dimensions = views.front().dimensions();
    image<typename View::value_type> result_image(dimensions.x * views.size(), dimensions.y);
    hstack(views, view(result_image));
    return result_image;
}

template <typename View>
void vstack(const std::vector<View>& views, const View& output_view)
{
    if (views.size() == 0)
    {
        throw std::invalid_argument("empty views vector is passed - cannot create stacked image");
    }

    auto dimensions = views.front().dimensions();
    for (const auto& view : views)
    {
        if (view.dimensions() != dimensions)
        {
            throw std::invalid_argument("one or many views are not of the same dimension");
        }
    }

    auto full_height = dimensions.y * views.size();
    if (output_view != full_height || output_view.width() != dimensions.x)
    {
        throw std::invalid_argument("the destination view is not of the right dimensions");
    }

    for (std::size_t i = 0; i < views.size(); ++i)
    {
        auto subview = subimage_view(output_view, 0, dimensions.y * i, dimensions.x, dimensions.y);
        copy_pixels(views[i], subview);
    }
}

template <typename View>
image<typename View::value_type> vstack(const std::vector<View>& views)
{
    if (views.size() == 0)
    {
        throw std::invalid_argument("empty views vector is passed - cannot create stacked image");
    }

    auto dimensions = views.front().dimensions();
    image<typename View::value_type> result_image(dimensions.x, dimensions.y * views.size());
    hstack(views, view(result_image));
    return result_image;
}
}} // namespace boost::gil
