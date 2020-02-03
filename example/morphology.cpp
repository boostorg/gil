#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/image_processing/morphology.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil.hpp>
#include <boost/multi_array.hpp>

#define SE_SIZE 3

using namespace boost::gil;
using namespace std;

int main()
{
    gray8_image_t img;
    read_image("test_adaptive.png", img, png_tag{});
    gray8_image_t img_dilate(img.dimensions());
    gray8_image_t img_erode(img.dimensions());

    int se_arr[SE_SIZE][SE_SIZE] = {{0,1,0},{1,1,1},{0,1,0}};

    typedef boost::multi_array<int, 2> se_type;
    se_type se{boost::extents[SE_SIZE][SE_SIZE]};
    for(int i=0; i < SE_SIZE; ++i)
    {
        for(int j=0; j < SE_SIZE; ++j)
        {
            se[i][j] = se_arr[i][j];
        }
    }

    dilate(view(img), view(img_dilate), se);
    erode(view(img), view(img_erode), se);

    write_view("dilated_image.png", view(img_dilate), png_tag{});
    write_view("eroded_image.png", view(img_erode), png_tag{});
}
