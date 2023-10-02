//
// Copyright 2005-2007 Adobe Systems Incorporated
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_PIXEL_ITERATOR_ADAPTOR_HPP
#define BOOST_GIL_PIXEL_ITERATOR_ADAPTOR_HPP

#include <boost/gil/concepts.hpp>
#include <boost/gil/pixel_iterator.hpp>

#include <boost/stl_interfaces/iterator_interface.hpp>

#include <iterator>

namespace boost { namespace gil {

/// \defgroup PixelIteratorModelDerefPtr dereference_iterator_adaptor
/// \ingroup PixelIteratorModel
/// \brief An iterator that invokes a provided function object upon dereference. Models: IteratorAdaptorConcept, PixelIteratorConcept

/// \ingroup PixelIteratorModelDerefPtr PixelBasedModel
/// \brief An adaptor over an existing iterator that provides for custom filter on dereferencing the object. Models: IteratorAdaptorConcept, PixelIteratorConcept

template <typename Iterator,    // Models Iterator
          typename DFn>  // Models Returns the result of dereferencing a given iterator of type Iterator
class dereference_iterator_adaptor : public stl_interfaces::iterator_interface<
#if !BOOST_STL_INTERFACES_USE_DEDUCED_THIS
    dereference_iterator_adaptor<Iterator,DFn>,
#endif
    typename std::iterator_traits<Iterator>::iterator_category,
    typename DFn::value_type,
    typename DFn::reference>
{
public:
    using parent_t = stl_interfaces::iterator_interface<
#if !BOOST_STL_INTERFACES_USE_DEDUCED_THIS
        dereference_iterator_adaptor<Iterator, DFn>,
#endif
        typename std::iterator_traits<Iterator>::iterator_category,
        typename DFn::value_type,
        typename DFn::reference>;
    using reference = typename DFn::result_type;
    using difference_type = typename std::iterator_traits<Iterator>::difference_type;
    using dereference_fn = DFn;

    dereference_iterator_adaptor() {}
    template <typename Iterator1>
    dereference_iterator_adaptor(const dereference_iterator_adaptor<Iterator1,DFn>& dit) : it_(dit.base()), _deref_fn(dit._deref_fn) {}
    dereference_iterator_adaptor(Iterator it, DFn deref_fn=DFn()) : it_(it), _deref_fn(deref_fn) {}
    template <typename Iterator1, typename DFn1>
    dereference_iterator_adaptor(const dereference_iterator_adaptor<Iterator1,DFn1>& it) : it_(it.base()), _deref_fn(it._deref_fn) {}

    constexpr auto operator*() const noexcept -> reference { return dereference(); }

    Iterator& base()              { return this->base_reference(); }
    const Iterator& base() const  { return this->base_reference(); }
    const DFn& deref_fn() const { return _deref_fn; }

protected:
    Iterator it_;
    DFn _deref_fn;

    constexpr auto base_reference() noexcept -> Iterator& { return it_; }
    constexpr auto base_reference() const noexcept -> Iterator const& { return it_; }

private:
    template <typename Iterator1, typename DFn1>
    friend class dereference_iterator_adaptor;
    friend struct boost::stl_interfaces::access;

    reference dereference() const { return _deref_fn(*(this->base_reference())); }
};

template <typename I, typename DFn>
struct const_iterator_type<dereference_iterator_adaptor<I,DFn> > {
    using type = dereference_iterator_adaptor<typename const_iterator_type<I>::type,typename DFn::const_t>;
};

template <typename I, typename DFn>
struct iterator_is_mutable<dereference_iterator_adaptor<I, DFn>>
    : std::integral_constant<bool, DFn::is_mutable>
{};


template <typename I, typename DFn>
struct is_iterator_adaptor<dereference_iterator_adaptor<I, DFn>> : std::true_type {};

template <typename I, typename DFn>
struct iterator_adaptor_get_base<dereference_iterator_adaptor<I, DFn>>
{
    using type = I;
};

template <typename I, typename DFn, typename NewBaseIterator>
struct iterator_adaptor_rebind<dereference_iterator_adaptor<I,DFn>,NewBaseIterator> {
    using type = dereference_iterator_adaptor<NewBaseIterator,DFn>;
};

/////////////////////////////
//  PixelBasedConcept
/////////////////////////////

template <typename I, typename DFn>
struct color_space_type<dereference_iterator_adaptor<I,DFn> > : public color_space_type<typename DFn::value_type> {};

template <typename I, typename DFn>
struct channel_mapping_type<dereference_iterator_adaptor<I,DFn> > : public channel_mapping_type<typename DFn::value_type> {};

template <typename I, typename DFn>
struct is_planar<dereference_iterator_adaptor<I,DFn> > : public is_planar<typename DFn::value_type> {};

template <typename I, typename DFn>
struct channel_type<dereference_iterator_adaptor<I,DFn> > : public channel_type<typename DFn::value_type> {};


/////////////////////////////
//  MemoryBasedIteratorConcept
/////////////////////////////

template <typename Iterator, typename DFn>
struct byte_to_memunit<dereference_iterator_adaptor<Iterator,DFn>> : public byte_to_memunit<Iterator> {};

template <typename Iterator, typename DFn>
inline auto memunit_step(dereference_iterator_adaptor<Iterator,DFn> const& p)
    -> typename std::iterator_traits<Iterator>::difference_type
{
    return memunit_step(p.base());
}

template <typename Iterator, typename DFn>
inline auto memunit_distance(dereference_iterator_adaptor<Iterator,DFn> const& p1,
                 dereference_iterator_adaptor<Iterator,DFn> const& p2)
    -> typename std::iterator_traits<Iterator>::difference_type
{
    return memunit_distance(p1.base(),p2.base());
}

template <typename Iterator, typename DFn>
inline void memunit_advance(dereference_iterator_adaptor<Iterator,DFn>& p,
                         typename std::iterator_traits<Iterator>::difference_type diff) {
    memunit_advance(p.base(), diff);
}

template <typename Iterator, typename DFn>
inline auto memunit_advanced(dereference_iterator_adaptor<Iterator,DFn> const& p,
              typename std::iterator_traits<Iterator>::difference_type diff)
    -> dereference_iterator_adaptor<Iterator,DFn>
{
    return dereference_iterator_adaptor<Iterator,DFn>(memunit_advanced(p.base(), diff), p.deref_fn());
}


template <typename Iterator, typename DFn>
inline auto memunit_advanced_ref(dereference_iterator_adaptor<Iterator,DFn> const& p,
                  typename std::iterator_traits<Iterator>::difference_type diff)
    -> typename std::iterator_traits<dereference_iterator_adaptor<Iterator,DFn> >::reference
{
    return *memunit_advanced(p, diff);
}

/////////////////////////////
//  HasDynamicXStepTypeConcept
/////////////////////////////

template <typename Iterator, typename DFn>
struct dynamic_x_step_type<dereference_iterator_adaptor<Iterator,DFn> > {
    using type = dereference_iterator_adaptor<typename dynamic_x_step_type<Iterator>::type,DFn>;
};

/// \brief Returns the type (and creates an instance) of an iterator that invokes the given dereference adaptor upon dereferencing
/// \ingroup PixelIteratorModelDerefPtr
template <typename Iterator, typename Deref>
struct iterator_add_deref {
    BOOST_GIL_CLASS_REQUIRE(Deref, boost::gil, PixelDereferenceAdaptorConcept)

    using type = dereference_iterator_adaptor<Iterator, Deref>;

    static type make(const Iterator& it, const Deref& d) { return type(it,d); }
};

/// \ingroup PixelIteratorModelDerefPtr
/// \brief For dereference iterator adaptors, compose the new function object after the old one
template <typename Iterator, typename PREV_DEREF, typename Deref>
struct iterator_add_deref<dereference_iterator_adaptor<Iterator, PREV_DEREF>,Deref> {
//    BOOST_GIL_CLASS_REQUIRE(Deref, boost::gil, PixelDereferenceAdaptorConcept)

    using type = dereference_iterator_adaptor<Iterator, deref_compose<Deref,PREV_DEREF>>;

    static type make(const dereference_iterator_adaptor<Iterator, PREV_DEREF>& it, const Deref& d) {
        return type(it.base(),deref_compose<Deref,PREV_DEREF>(d,it.deref_fn()));
    }
};

}}  // namespace boost::gil

#endif
