//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_EXTENSION_NUMERIC_KERNEL_HPP
#define BOOST_GIL_EXTENSION_NUMERIC_KERNEL_HPP

#include <boost/gil/utilities.hpp>

#include <boost/assert.hpp>

#include <algorithm>
#include <array>
#include <cstddef>
#include <memory>
#include <vector>

namespace boost { namespace gil {

// Definitions of 1D fixed-size and variable-size kernels and related operations

namespace detail {

/// \brief kernel adaptor for one-dimensional cores
/// Core needs to provide size(),begin(),end(),operator[],
/// value_type,iterator,const_iterator,reference,const_reference
template <typename Core>
class kernel_1d_adaptor : public Core
{
public:
    kernel_1d_adaptor() = default;

    explicit kernel_1d_adaptor(std::size_t center)
        : center_(center)
    {
        BOOST_ASSERT(center_ < this->size());
    }

    kernel_1d_adaptor(std::size_t size, std::size_t center)
        : Core(size)
        , center_(center)
    {
        BOOST_ASSERT(this->size() > 0);
        BOOST_ASSERT(center_ < this->size()); // also implies `size() > 0`
    }

    kernel_1d_adaptor(kernel_1d_adaptor const& other)
        : Core(other), center_(other.center_)
    {
        BOOST_ASSERT(this->size() > 0);
        BOOST_ASSERT(center_ < this->size()); // also implies `size() > 0`
    }

    kernel_1d_adaptor& operator=(kernel_1d_adaptor const& other)
    {
        Core::operator=(other);
        center_ = other.center_;
        return *this;
    }

    std::size_t left_size() const
    {
        BOOST_ASSERT(center_ < this->size());
        return center_;
    }

    std::size_t right_size() const
    {
        BOOST_ASSERT(center_ < this->size());
        return this->size() - center_ - 1;
    }

    auto center() -> std::size_t&
    {
        BOOST_ASSERT(center_ < this->size());
        return center_;
    }

    auto center() const -> std::size_t const&
    {
        BOOST_ASSERT(center_ < this->size());
        return center_;
    }

private:
    std::size_t center_{0};
};

} // namespace detail

/// \brief variable-size kernel
template <typename T, typename Allocator = std::allocator<T> >
class kernel_1d : public detail::kernel_1d_adaptor<std::vector<T, Allocator>>
{
    using parent_t = detail::kernel_1d_adaptor<std::vector<T, Allocator>>;
public:

    kernel_1d() = default;
    kernel_1d(std::size_t size, std::size_t center) : parent_t(size, center) {}

    template <typename FwdIterator>
    kernel_1d(FwdIterator elements, std::size_t size, std::size_t center)
        : parent_t(size, center)
    {
        detail::copy_n(elements, size, this->begin());
    }

    kernel_1d(kernel_1d const& other) : parent_t(other) {}
    kernel_1d& operator=(kernel_1d const& other) = default;
};

/// \brief static-size kernel
template <typename T,std::size_t Size>
class kernel_1d_fixed : public detail::kernel_1d_adaptor<std::array<T, Size>>
{
    using parent_t = detail::kernel_1d_adaptor<std::array<T, Size>>;
public:
    static constexpr std::size_t static_size = Size;
    static_assert(static_size > 0, "kernel must have size greater than 0");
    static_assert(static_size % 2 == 1, "kernel size must be odd to ensure validity at the center");

    kernel_1d_fixed() = default;
    explicit kernel_1d_fixed(std::size_t center) : parent_t(center) {}

    template <typename FwdIterator>
    explicit kernel_1d_fixed(FwdIterator elements, std::size_t center)
        : parent_t(center)
    {
        detail::copy_n(elements, Size, this->begin());
    }

    kernel_1d_fixed(kernel_1d_fixed const& other) : parent_t(other) {}
    kernel_1d_fixed& operator=(kernel_1d_fixed const& other) = default;
};

// TODO: This data member is odr-used and definition at namespace scope
// is required by C++11. Redundant and deprecated in C++17.
template <typename T,std::size_t Size>
constexpr std::size_t kernel_1d_fixed<T, Size>::static_size;

/// \brief reverse a kernel
template <typename Kernel>
inline Kernel reverse_kernel(Kernel const& kernel)
{
    Kernel result(kernel);
    result.center() = kernel.right_size();
    std::reverse(result.begin(), result.end());
    return result;
}

}} // namespace boost::gil

#endif
