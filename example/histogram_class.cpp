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
    //Check key_from_pixel
    histogram<int,int,int> a,c;
    int i = 0;
    std::cout<<"Val: "<<(i = std::get<2>(a.key_from_pixel(rgb8_pixel_t(1))))<<"\n";

    histogram<int,int,std::string> e;

    gray8_image_t v1(1,2,gray8_pixel_t(0));
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
}
