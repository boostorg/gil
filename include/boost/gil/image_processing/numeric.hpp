#include <boost/gil/detail/math.hpp>
#include <cmath>

namespace boost{ namespace gil{

/// \defgroup ImageProcessingMath
/// \brief Math operations for IP algorithms
///
/// This is mostly handful of mathemtical
/// operations that are required by other
/// image processing algorithms

/// \brief Normalized cardinal sine
/// \ingroup ImageProcessingMath
///
/// normalized_sinc(x) = sin(pi * x) / (pi * x)
double normalized_sinc(double x)
{
    return std::sin(x * boost::gil::pi) / (x * boost::gil::pi);
}

/// \brief Lanczos response at point x
/// \ingroup ImageProcessingMath
///
/// Lanczos response is defined as:
/// x == 0: 1
/// -a < x && x < a: 0
/// otherwise: normalized_sinc(x) / normalized_sinc(x / a)
double lanczos(double x, std::ptrdiff_t a)
{
    if (x == 0)
    {
        return 1;
    }
    if (-a < x && x < a)
    {
        return normalized_sinc(x)
               / normalized_sinc(x / static_cast<double>(a));
    }

    return 0;
}
}}
