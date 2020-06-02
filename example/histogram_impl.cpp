//
// Copyright 2020 Debabrata Mandal <mandaldebabrata123@gmail.com>
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//

#include <boost/gil.hpp>
#include <vector>
#include <map>
#include <list>
#include <array>
#include <deque>
#include <set>
#include <string>
#include <unordered_set>
#include <unordered_map>

using namespace boost::gil;

int main()
{	
	gray8_image_t img(4,4,gray8_pixel_t(1));
	gray8_image_t img1(4,4,gray8_pixel_t(1));
	
	// Checking calls with different compatible container types

	std::vector<int> v;
	v = image_histogram<std::vector<int>>(view(img));  // Works
	image_histogram(view(img),v);             		  // Works

	std::map<int,int> m;
	m = image_histogram<std::map<int,int>>(view(img));  // Works
	image_histogram(view(img),m);					   // Works

	std::deque<int> deq;
	deq = image_histogram<std::deque<int>>(view(img));  // Works
	image_histogram(view(img),deq);						// Works

	std::array<int,100> arr;
	image_histogram(view(img),arr); 					// Works

	std::unordered_map<int,int> mp;
	mp = image_histogram<std::unordered_map<int,int>>(view(img));  // Works
	image_histogram(view(img),mp);								   // Works

	// Checking calls with different incompatible container types

	// std::vector<std::vector<int> > vv;
	// vv = image_histogram<std::vector<std::vector<int>>>(view(img));  // Does not work
	// image_histogram(view(img),vv);  									// Does not work

	// std::map<std::string,int> ms;
	// ms = image_histogram<std::map<std::string,int>>(view(img));  	// Does not Work
	// image_histogram(view(img),ms);					   				// Does not Work

	// std::map<int, int*> ms1;
	// ms1 = image_histogram<std::map<int,int*>>(view(img));  			// Does not Work
	// image_histogram(view(img),ms1);					   				// Does not Work

	// std::map<int,std::map<int,int> > mm;
	// mm = image_histogram<std::map<int,std::map<int,int>>>(view(img));  // Does not Work
	// image_histogram(view(img),mm);									  // Does not Work
 
	// std::set<int> ss;
	// ss = image_histogram<std::set<int>>(view(img));   				// Does not Work
	// image_histogram(view(img),ss);					 				// Does not Work
	
	// std::set<std::set<int>> ss2;
	// ss2 = image_histogram<std::set<std::set<int>>>(view(img)); 		// Does not Work
	// image_histogram(view(img),ss2);							   		// Does not Work
	
	// std::list<int> l;
	// l = image_histogram<std::list<int>>(view(img));  				// Does not Work
	// image_histogram(view(img),l);									// Does not Work
	
	// std::list<std::list<int>> l2;
	// l = image_histogram<std::list<std::list<int>>>(view(img));  		// Does not Work
	// image_histogram(view(img),l);									// Does not Work

	// std::deque<std::deque<int>> deq2;
	// deq2 = image_histogram<std::deque<std::deque<int>>>(view(img));  // Does not Work
	// image_histogram(view(img),deq2);									// Does not Work

	// std::array<std::array<int, 3>, 3> arr2;
	// image_histogram(view(img),arr2);									// Does not Work

	// std::unordered_map<int,std::unordered_map<int,int>> mp2;
	// mp2 = image_histogram<std::unordered_map<int,std::unordered_map<int,int>>>(view(img));  // Does not Work
	// image_histogram(view(img),mp2);														   // Does not Work

	// std::unordered_set<int> st;
	// st = image_histogram<std::unordered_set<int>>(view(img)); 		// Does not Work
	// image_histogram(view(img),st);							  		// Does not Work

	return 0;
}
