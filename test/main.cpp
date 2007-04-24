/*
    Copyright 2005-2007 Adobe Systems Incorporated
    Distributed under the MIT License (see accompanying file LICENSE_1_0_0.txt
    or a copy at http://opensource.adobe.com/licenses.html)
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
