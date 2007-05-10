/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/
// main.cpp : Runs all GIL tests.
//

#include <fstream>
#include <iostream>

void test_channel();
void test_pixel();
void test_pixel_iterator();
void test_image(const char*);
void test_image_io();
void test_gimage();

int main(int argc, char* argv[]) {
    test_pixel();
    test_channel();
    test_pixel_iterator();
    test_image_io();

    const char* local_name = "gil_reference_checksums.txt";
    const char* name_from_status = "../libs/gil/test/gil_reference_checksums.txt";

    std::ifstream file_is_there(local_name);
    if (file_is_there) {
        test_image(local_name);
    } else {
        std::ifstream file_is_there(name_from_status);
        if (file_is_there)
            test_image(name_from_status);
        else {
            std::cerr << "Unable to open gil_reference_checksums.txt"<<std::endl;
            return 1;
        }
    }

    return 0;
}
