//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil/histogram.hpp>
#include <boost/gil/image.hpp>
#include <boost/gil/image_view.hpp>
#include <boost/gil/image_view_factory.hpp>
#include <boost/mp11.hpp>

#include <iostream>
#include <tuple>

using namespace boost::gil;

int main() {
    histogram<int,int,int> a,c;
    histogram<int,int,std::string> e;
    // std::cout<<rgb8_pixel_t(1);
    // std::cout<<std::get<2>(a.key_from_pixel(rgb8_pixel_t(1)));
    std::cout<<"\n"<<e.is_pixel_compatible();
    histogram<int,int> b;
    // gray8_image_t v(1,2,gray8_pixel_t(0));
    // fill_histogram<1,2>(const_view(v),a);
    histogram<int>::mapped_type d=12;
    std::cout<<d;
    // a(1) = 9;
    b(1,2) = 1;
    // c = a;
    // std::cout<<a(1)<<a.size();
    // using tt = typename ::key_type;
    std::tuple<int> t;
    t = 1;
    // int b = 
    // std::cout<<(t=={1});
    // std::cout<<t;
    // std::tuple<int, int> e(1,2);
    // boost::hash<std::tuple<int ,int>> h;
    // std::cout<<h(e);

    std::cout<<"New";
    histogram<int> x;
    std::cout<<x.get_dimension()<<"dim";
    gray8_image_t v(1,2,gray8_pixel_t(1));
    rgb8_image_t rv(1,2,rgb8_pixel_t(1, 1, 1));
    const std::size_t k=0;
    // x.fill<0>(const_view(v));
    // x.fill(const_view(v));
    // x.fill<0,2>(const_view(v));
    fill_histogram<0>(const_view(rv),x);
    std::vector<int> vv;
    // fill_histogram(const_view(v), vv, 1);
    std::cout<<x[1];

    std::cout<<"New2";
    rgb8_image_t vx(1,2,rgb8_pixel_t(1,0,2));
    histogram<int, int, int> xx;
    xx.fill<1,0,2>(const_view(vx));
    xx.fill(const_view(vx));
    std::cout<<"Val: "<<xx(0,1,2);
    std::cout<<"Val: "<<xx(1,0,2);
    std::cout<<"Val: "<<xx(1,0,1);
    // rgb8_pixel_t r = rgb8_pixel_t(1,0,2);
    // std::tuple<int,int,int> rt = detail::pixel_to_tuple(r, boost::mp11::index_sequence<1,0,2>{});
    // // std::cout<<std::get<1>(rt);
    // std::cout<<"\n"<<std::get<0>(rt)<<" "<<std::get<1>(rt)<<" "<<std::get<2>(rt)<<" "<<"endl";

    // auto i = xx.key_from_pixel<1,0,2>(rgb8_pixel_t(1,0,2));
    // std::cout<<"\n"<<std::get<0>(i)<<" "<<std::get<1>(i)<<" "<<std::get<2>(i)<<" "<<"endl";

}