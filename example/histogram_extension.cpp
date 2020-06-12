//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil.hpp>
#include <boost/gil/extension/histogram/stl_histogram.hpp>
#include <string>
#include <array>
#include <vector>
#include <map>
#include <unordered_map>

using namespace boost::gil;

int main()
{	
	gray8_image_t img(4,4,gray8_pixel_t(1));
	gray8_image_t img1(4,4,gray8_pixel_t(1));
	
	// Checking calls with different overloaded container types

	std::vector<int> v;
	fill_histogram(const_view(img), v);  			
	fill_histogram(const_view(img), v, true);       

	std::map<int,int> m;
	fill_histogram(view(img),m);  					
	fill_histogram(view(img),m, true);				

	std::array<int,100> arr;
	fill_histogram(view(img), arr); 				
	fill_histogram(view(img), arr, true); 			

	std::unordered_map<int,int> mp;
	fill_histogram(view(img), mp);  				
	fill_histogram(view(img), mp, true);			
	return 0;
}
