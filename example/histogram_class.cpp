//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <cstddef>

// For ADL lookup need to define before including histogram.hpp
struct TestClass;

namespace boost {
std::size_t hash_value(TestClass const&);
}

#include <boost/gil/histogram.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>

#include <boost/mp11.hpp>
#include <boost/container_hash/hash.hpp>

#include <iostream>
#include <tuple>

using namespace boost::gil;
using namespace boost::mp11;

struct TestClass 
{
    int a;
};

namespace boost {

std::size_t hash_value(TestClass const&)
{
    return 100;
}

} // namespace boost

int main() {
    //Check key_from_pixel
    histogram<int,int,int> a,c;
    int i = 0;
    std::cout<<"Val: "<<(i = std::get<2>(a.key_from_pixel(rgb8_pixel_t(1))))<<"\n";

    histogram<int,int,std::string> e;

    rgb8_image_t v1(1,2,rgb8_pixel_t(2));
    // fill_histogram<1,2>(const_view(v1),a);   //Fails

    a(1, 1, 1) = 9;
    histogram<int, int> b;
    b(1,2) = 1;
    c = a;
    std::cout<<"Val :"<<a(1, 1, 1)<<" "<<a.size()<<"\n";
    
    // Some std::tuple checks
    std::tuple<int> t;
    t = 1;
    // std::cout<<(t=={1});     // Fails
    // std::cout<<t;            // Fails

    histogram<int, int, int> h;
    h(1,1,1)=1;
    h(1,2,1)=1;
    h(3,3,1)=1;
    h(1,21,21)=1;
    // auto h1 = h.sub_histogram<0>();
    // std::cout<<"sub : "<<h1(1)<<"\n";

    auto h2 = h.sub_histogram<0,2>(std::tuple<int, int, int>(1, 0, 1));
    std::cout<<"sub : "<<h2(1, 1, 1)<<"\n";
    std::cout<<"sub : "<<h2(1, 2, 1)<<"\n";
    std::cout<<"sub : "<<h2(3, 3, 1)<<"\n";

    histogram<int, int, int> ha;
    std::tuple<int, int, int> ta;
    std::cout<<ha.is_tuple_compatible(ta);
    
    std::size_t seed=0;
    TestClass tc;
    boost::hash<TestClass> tc_hasher;
    tc_hasher(tc);
    boost::hash_combine(seed, tc);
    return 0;
}
