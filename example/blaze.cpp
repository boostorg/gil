#include <blaze/Blaze.h>
#include <boost/gil/extension/blaze/blaze.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/typedefs.hpp>

namespace gil = boost::gil;

int main()
{
    using element_type = blaze::StaticVector<std::uint8_t, 3>;
    element_type v({0, 0, 255});
    blaze::DynamicMatrix<element_type> matrix(16, 16, v);

    auto image = gil::from_matrix<gil::rgb8_image_t>(matrix);
    gil::write_view("output.png", gil::view(image), gil::png_tag{});
}
