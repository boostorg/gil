/*
    Copyright 2005-2007 Adobe Systems Incorporated
   
    Use, modification and distribution are subject to the Boost Software License,
    Version 1.0. (See accompanying file LICENSE_1_0.txt or copy at
    http://www.boost.org/LICENSE_1_0.txt).

    See http://opensource.adobe.com/gil for most recent version including documentation.
*/
// main.cpp : Runs all GIL tests.
//

void test_channel();
void test_pixel();
void test_pixel_iterator();
void test_image();
void test_image_io();
void test_gimage();

int main(int argc, unsigned char* argv[]) {
    test_pixel();
    test_channel();
    test_pixel_iterator();
    test_image_io();
    test_image();
    return 0;
}
