//
// Created by shino on 14.08.21.
//

#ifndef BOOST_GIL_MATRIX_HPP
#define BOOST_GIL_MATRIX_HPP

#include <boost/gil/typedefs.hpp>
#include <cmath>
#include <stdexcept>
#include <limits>

namespace boost { namespace gil {
using elem_t = pixel<float, gil::gray_layout_t>;
using elemc_t = elem_t const;
using elem_ref_t = elem_t&;
using elemc_ref_t = elemc_t&;
using elem_ptr_t = elem_t*;
using elemc_ptr_t = elemc_t*;
using elem_step_ptr_t = memory_based_step_iterator<elem_ptr_t>;
using elemc_step_ptr_t = memory_based_step_iterator<elemc_ptr_t>;
using elem_loc_t = memory_based_2d_locator<elem_step_ptr_t>;
using elemc_loc_t = memory_based_2d_locator<elemc_step_ptr_t>;
using matrix = image_view<elem_loc_t>;
using matrixc = image_view<elemc_loc_t>;
using matrix_storage = image<elem_t, false, std::allocator<std::uint8_t>>;
}
}


#endif // BOOST_GIL_MATRIX_HPP
