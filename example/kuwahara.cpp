//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
#include <boost/gil/extension/io/png.hpp>
#include <boost/gil/extension/numeric/kernel.hpp>
#include <iostream>
#include <vector>
using namespace boost::gil;
using namespace std;

template <typename SrcView, typename DstView>
void kuwahara_filter(SrcView const &src_view, DstView const &dst_view,
                     std::size_t kernel_size) {
  std::size_t half_kernel_size = kernel_size / 2;
  std::size_t a_val = kernel_size =
      (kernel_size - 1) / 2; // 2a + 1 = kernel_size
  std::size_t quadrant_size = a_val + 1;

  std::vector<float> values;
  std::vector<float> std_deviations;
  std::vector<float> means;

  std_deviations.reserve(4);
  means.reserve(4);

  values.reserve(quadrant_size);

  for (std::ptrdiff_t y = 0; y < src_view.height(); y++) {
    typename DstView::x_iterator dst_it = dst_view.row_begin(y);

    for (std::ptrdiff_t x = 0; x < src_view.width(); x++) {
      for (int x_i = -half_kernel_size; x_i <= 0; x_i += half_kernel_size) {
        for (int y_i = -half_kernel_size; y_i <= 0; y_i += half_kernel_size) {
          auto sub_view = subimage_view(src_view, x + x_i, y + y_i,
                                        quadrant_size, quadrant_size);

          values.assign(sub_view.begin(), sub_view.end());
          auto mean = std::accumulate(values.begin(), values.end(), 0.0) /
                      values.size();

          std::vector<float> diff(values.size());
          std::transform(values.begin(), values.end(), diff.begin(),
                         [mean](float x) { return x - mean; });

          auto sq_sum =
              std::inner_product(diff.begin(), diff.end(), diff.begin(), 0.0);
          auto stdev = std::sqrt(sq_sum / values.size());

          std_deviations.push_back(stdev);
          means.push_back(mean);
        }
      }

      auto min_idx =
          std::min_element(std_deviations.begin(), std_deviations.end());
      dst_it[x] = means[std::distance(std_deviations.begin(), min_idx)];

      std_deviations.clear();
      means.clear();
    }
  }
}

int main() {
  gray8_image_t img;
  read_image("test_adaptive.png", img, png_tag{});

  kuwahara_filter(const_view(img), view(img), 9);
  write_view("out_kuwahara_filter.png", view(img), png_tag{});
  return 0;
}
