// All headers included in one translation unit
#include <boost/gil/concepts/basic.hpp>
#include <boost/gil/concepts/channel.hpp>
#include <boost/gil/concepts/color.hpp>
#include <boost/gil/concepts/color_base.hpp>
#include <boost/gil/concepts/concept_check.hpp>
#include <boost/gil/concepts/detail/type_traits.hpp>
#include <boost/gil/concepts/detail/utility.hpp>
#include <boost/gil/concepts/dynamic_step.hpp>
#include <boost/gil/concepts/fwd.hpp>
#include <boost/gil/concepts/image.hpp>
#include <boost/gil/concepts/image_view.hpp>
#include <boost/gil/concepts/pixel.hpp>
#include <boost/gil/concepts/pixel_based.hpp>
#include <boost/gil/concepts/pixel_dereference.hpp>
#include <boost/gil/concepts/pixel_iterator.hpp>
#include <boost/gil/concepts/pixel_locator.hpp>
#include <boost/gil/concepts/point.hpp>
#include <boost/gil/algorithm.hpp>
#include <boost/gil/bit_aligned_pixel_iterator.hpp>
#include <boost/gil/bit_aligned_pixel_reference.hpp>
#include <boost/gil/channel.hpp>
#include <boost/gil/channel_algorithm.hpp>
#include <boost/gil/cmyk.hpp>
#include <boost/gil/color_base.hpp>
#include <boost/gil/color_base_algorithm.hpp>
#include <boost/gil/color_convert.hpp>
#include <boost/gil/concepts.hpp>
#include <boost/gil/deprecated.hpp>
#include <boost/gil/device_n.hpp>
#include <boost/gil/dynamic_step.hpp>
#include <boost/gil/gray.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/gil/iterator_from_2d.hpp>
#include <boost/gil/locator.hpp>
#include <boost/gil/metafunctions.hpp>
#include <boost/gil/packed_pixel.hpp>
#include <boost/gil/pixel.hpp>
#include <boost/gil/pixel_iterator.hpp>
#include <boost/gil/pixel_iterator_adaptor.hpp>
#include <boost/gil/planar_pixel_iterator.hpp>
#include <boost/gil/planar_pixel_reference.hpp>
#include <boost/gil/point.hpp>
#include <boost/gil/position_iterator.hpp>
#include <boost/gil/premultiply.hpp>
#include <boost/gil/promote_integral.hpp>
#include <boost/gil/rgb.hpp>
#include <boost/gil/rgba.hpp>
#include <boost/gil/step_iterator.hpp>
#include <boost/gil/typedefs.hpp>
#include <boost/gil/utilities.hpp>
#include <boost/gil/virtual_locator.hpp>
#include <boost/gil/extension/dynamic_image/algorithm.hpp>
#include <boost/gil/extension/dynamic_image/any_image.hpp>
#include <boost/gil/extension/dynamic_image/any_image_view.hpp>
#include <boost/gil/extension/dynamic_image/apply_operation.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_at_c.hpp>
#include <boost/gil/extension/dynamic_image/dynamic_image_all.hpp>
#include <boost/gil/extension/dynamic_image/image_view_factory.hpp>
#include <boost/gil/extension/numeric/affine.hpp>
#include <boost/gil/extension/numeric/algorithm.hpp>
#include <boost/gil/extension/numeric/channel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/convolve.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <boost/gil/extension/numeric/pixel_numeric_operations.hpp>
#include <boost/gil/extension/numeric/resample.hpp>
#include <boost/gil/extension/numeric/sampler.hpp>
#include <boost/gil/extension/toolbox/color_converters.hpp>
#include <boost/gil/extension/toolbox/color_converters/gray_to_rgba.hpp>
#include <boost/gil/extension/toolbox/color_converters/rgb_to_luminance.hpp>
#include <boost/gil/extension/toolbox/color_spaces.hpp>
#include <boost/gil/extension/toolbox/color_spaces/cmyka.hpp>
#include <boost/gil/extension/toolbox/color_spaces/gray_alpha.hpp>
#include <boost/gil/extension/toolbox/color_spaces/hsl.hpp>
#include <boost/gil/extension/toolbox/color_spaces/hsv.hpp>
#include <boost/gil/extension/toolbox/color_spaces/lab.hpp>
#include <boost/gil/extension/toolbox/color_spaces/xyz.hpp>
#include <boost/gil/extension/toolbox/color_spaces/ycbcr.hpp>
#include <boost/gil/extension/toolbox/dynamic_images.hpp>
#include <boost/gil/extension/toolbox/image_types.hpp>
#include <boost/gil/extension/toolbox/image_types/indexed_image.hpp>
#include <boost/gil/extension/toolbox/image_types/subchroma_image.hpp>
#include <boost/gil/extension/toolbox/metafunctions.hpp>
#include <boost/gil/extension/toolbox/metafunctions/channel_type.hpp>
#include <boost/gil/extension/toolbox/metafunctions/channel_view.hpp>
#include <boost/gil/extension/toolbox/metafunctions/get_num_bits.hpp>
#include <boost/gil/extension/toolbox/metafunctions/get_pixel_type.hpp>
#include <boost/gil/extension/toolbox/metafunctions/is_bit_aligned.hpp>
#include <boost/gil/extension/toolbox/metafunctions/is_homogeneous.hpp>
#include <boost/gil/extension/toolbox/metafunctions/is_similar.hpp>
#include <boost/gil/extension/toolbox/metafunctions/pixel_bit_size.hpp>
#include <boost/gil/extension/toolbox/toolbox.hpp>
#include <boost/gil/io/base.hpp>
#include <boost/gil/io/bit_operations.hpp>
#include <boost/gil/io/conversion_policies.hpp>
#include <boost/gil/io/device.hpp>
#include <boost/gil/io/dynamic_io_new.hpp>
#include <boost/gil/io/error.hpp>
#include <boost/gil/io/get_read_device.hpp>
#include <boost/gil/io/get_reader.hpp>
#include <boost/gil/io/get_write_device.hpp>
#include <boost/gil/io/get_writer.hpp>
#include <boost/gil/io/io.hpp>
#include <boost/gil/io/make_backend.hpp>
#include <boost/gil/io/make_dynamic_image_reader.hpp>
#include <boost/gil/io/make_dynamic_image_writer.hpp>
#include <boost/gil/io/make_reader.hpp>
#include <boost/gil/io/make_scanline_reader.hpp>
#include <boost/gil/io/make_writer.hpp>
#include <boost/gil/io/path_spec.hpp>
#include <boost/gil/io/read_and_convert_image.hpp>
#include <boost/gil/io/read_and_convert_view.hpp>
#include <boost/gil/io/read_image.hpp>
#include <boost/gil/io/read_image_info.hpp>
#include <boost/gil/io/read_view.hpp>
#include <boost/gil/io/reader_base.hpp>
#include <boost/gil/io/row_buffer_helper.hpp>
#include <boost/gil/io/scanline_read_iterator.hpp>
#include <boost/gil/io/typedefs.hpp>
#include <boost/gil/io/write_view.hpp>
#include <boost/gil/extension/io/bmp.hpp>
#include <boost/gil/extension/io/bmp/detail/is_allowed.hpp>
#include <boost/gil/extension/io/bmp/detail/read.hpp>
#include <boost/gil/extension/io/bmp/detail/reader_backend.hpp>
#include <boost/gil/extension/io/bmp/detail/scanline_read.hpp>
#include <boost/gil/extension/io/bmp/detail/supported_types.hpp>
#include <boost/gil/extension/io/bmp/detail/write.hpp>
#include <boost/gil/extension/io/bmp/detail/writer_backend.hpp>
#include <boost/gil/extension/io/bmp/old.hpp>
#include <boost/gil/extension/io/bmp/read.hpp>
#include <boost/gil/extension/io/bmp/tags.hpp>
#include <boost/gil/extension/io/bmp/write.hpp>
#include <boost/gil/extension/io/jpeg.hpp>
#include <boost/gil/extension/io/jpeg/detail/base.hpp>
#include <boost/gil/extension/io/jpeg/detail/is_allowed.hpp>
#include <boost/gil/extension/io/jpeg/detail/read.hpp>
#include <boost/gil/extension/io/jpeg/detail/reader_backend.hpp>
#include <boost/gil/extension/io/jpeg/detail/scanline_read.hpp>
#include <boost/gil/extension/io/jpeg/detail/supported_types.hpp>
#include <boost/gil/extension/io/jpeg/detail/write.hpp>
#include <boost/gil/extension/io/jpeg/detail/writer_backend.hpp>
#include <boost/gil/extension/io/jpeg/old.hpp>
#include <boost/gil/extension/io/jpeg/read.hpp>
#include <boost/gil/extension/io/jpeg/tags.hpp>
#include <boost/gil/extension/io/jpeg/write.hpp>
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/io/png/detail/base.hpp>
#include <boost/gil/extension/io/png/detail/is_allowed.hpp>
#include <boost/gil/extension/io/png/detail/read.hpp>
#include <boost/gil/extension/io/png/detail/reader_backend.hpp>
#include <boost/gil/extension/io/png/detail/scanline_read.hpp>
#include <boost/gil/extension/io/png/detail/supported_types.hpp>
#include <boost/gil/extension/io/png/detail/write.hpp>
#include <boost/gil/extension/io/png/detail/writer_backend.hpp>
#include <boost/gil/extension/io/png/old.hpp>
#include <boost/gil/extension/io/png/read.hpp>
#include <boost/gil/extension/io/png/tags.hpp>
#include <boost/gil/extension/io/png/write.hpp>
#include <boost/gil/extension/io/pnm.hpp>
#include <boost/gil/extension/io/pnm/detail/is_allowed.hpp>
#include <boost/gil/extension/io/pnm/detail/read.hpp>
#include <boost/gil/extension/io/pnm/detail/reader_backend.hpp>
#include <boost/gil/extension/io/pnm/detail/scanline_read.hpp>
#include <boost/gil/extension/io/pnm/detail/supported_types.hpp>
#include <boost/gil/extension/io/pnm/detail/write.hpp>
#include <boost/gil/extension/io/pnm/detail/writer_backend.hpp>
#include <boost/gil/extension/io/pnm/old.hpp>
#include <boost/gil/extension/io/pnm/read.hpp>
#include <boost/gil/extension/io/pnm/tags.hpp>
#include <boost/gil/extension/io/pnm/write.hpp>
#include <boost/gil/extension/io/targa.hpp>
#include <boost/gil/extension/io/targa/detail/is_allowed.hpp>
#include <boost/gil/extension/io/targa/detail/read.hpp>
#include <boost/gil/extension/io/targa/detail/reader_backend.hpp>
#include <boost/gil/extension/io/targa/detail/scanline_read.hpp>
#include <boost/gil/extension/io/targa/detail/supported_types.hpp>
#include <boost/gil/extension/io/targa/detail/write.hpp>
#include <boost/gil/extension/io/targa/detail/writer_backend.hpp>
#include <boost/gil/extension/io/targa/old.hpp>
#include <boost/gil/extension/io/targa/read.hpp>
#include <boost/gil/extension/io/targa/tags.hpp>
#include <boost/gil/extension/io/targa/write.hpp>
#include <boost/gil/extension/io/tiff.hpp>
#include <boost/gil/extension/io/tiff/detail/device.hpp>
#include <boost/gil/extension/io/tiff/detail/is_allowed.hpp>
#include <boost/gil/extension/io/tiff/detail/log.hpp>
#include <boost/gil/extension/io/tiff/detail/read.hpp>
#include <boost/gil/extension/io/tiff/detail/reader_backend.hpp>
#include <boost/gil/extension/io/tiff/detail/scanline_read.hpp>
#include <boost/gil/extension/io/tiff/detail/supported_types.hpp>
#include <boost/gil/extension/io/tiff/detail/write.hpp>
#include <boost/gil/extension/io/tiff/detail/writer_backend.hpp>
#include <boost/gil/extension/io/tiff/old.hpp>
#include <boost/gil/extension/io/tiff/read.hpp>
#include <boost/gil/extension/io/tiff/tags.hpp>
#include <boost/gil/extension/io/tiff/write.hpp>
int main() { return 0; }
