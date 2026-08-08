//
// Copyright 2024 Dirk Stolle
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// https://www.boost.org/LICENSE_1_0.txt

#include <boost/core/lightweight_test.hpp>
#include <boost/gil/io/path_spec.hpp>
#include <cstring>
#include <locale>
#include <string>

namespace gil = boost::gil;

void test_convert_to_string_from_wstring()
{
    std::wstring const path = L"/some_path/傳/привет/qwerty";
    std::string const expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    std::string string = gil::detail::convert_to_string(path);
    BOOST_TEST_EQ( 34, string.size() );
    BOOST_TEST_EQ( expected, string );
}

void test_convert_to_native_string_from_wchar_t_ptr()
{
    wchar_t const* path = L"/some_path/傳/привет/qwerty";
    char const* expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    char const* string = gil::detail::convert_to_native_string(path);
    BOOST_TEST_EQ( 34, strlen(string) );
    BOOST_TEST_EQ( 0, std::strcmp(expected, string) );
    delete[] string;
}

void test_convert_to_native_string_from_wstring()
{
    std::wstring const path = L"/some_path/傳/привет/qwerty";
    char const* expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    char const* string = gil::detail::convert_to_native_string(path);
    BOOST_TEST_EQ( 34, strlen(string) );
    BOOST_TEST_EQ( 0, std::strcmp(expected, string) );
    delete[] string;
}

int main()
{
    // Set global locale to one that uses UTF-8. The exact name of such a
    // locale is platform-dependent (e.g. "C.UTF-8" on glibc systems is not
    // available on macOS), so try a few common candidates and fall back to
    // the current locale if none of them exist on this system.
    // ".UTF-8" is tried first: it is the cross-platform-safe form documented
    // by Microsoft (since VS2015 Update 2 / Windows 10 1803) that is
    // recognized by both std::locale and the underlying setlocale() call it
    // triggers; POSIX-style names such as "en_US.UTF-8" can construct a
    // valid std::locale on MSVC without actually switching the C runtime's
    // multibyte conversion (used by wcstombs) to UTF-8, silently leaving it
    // on the system ANSI codepage.
    char const* const utf8_locale_names[] = {
        ".UTF-8", "C.UTF-8", "en_US.UTF-8", "UTF-8", "en_US.utf8"
    };
    for (char const* name : utf8_locale_names)
    {
        try
        {
            std::locale::global(std::locale(name));
            break;
        }
        catch (std::runtime_error const&)
        {
            // Try the next candidate.
        }
    }

    test_convert_to_string_from_wstring();
    test_convert_to_native_string_from_wchar_t_ptr();
    test_convert_to_native_string_from_wstring();

    return boost::report_errors();
}
