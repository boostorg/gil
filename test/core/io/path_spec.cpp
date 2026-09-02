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
    std::wstring const path = L"/some_path/\u50B3/\u043F\u0440\u0438\u0432\u0435\u0442/qwerty";
    std::string const expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    std::string string = gil::detail::convert_to_string(path);
    BOOST_TEST_EQ( 34, string.size() );
    BOOST_TEST_EQ( expected, string );
}

void test_convert_to_native_string_from_wchar_t_ptr()
{
    wchar_t const* path = L"/some_path/\u50B3/\u043F\u0440\u0438\u0432\u0435\u0442/qwerty";
    char const* expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    char const* string = gil::detail::convert_to_native_string(path);
    BOOST_TEST_EQ( 34, strlen(string) );
    BOOST_TEST_EQ( 0, std::strcmp(expected, string) );
    delete[] string;
}

void test_convert_to_native_string_from_wstring()
{
    std::wstring const path = L"/some_path/\u50B3/\u043F\u0440\u0438\u0432\u0435\u0442/qwerty";
    char const* expected = "/some_path/\xE5\x82\xB3/\xD0\xBF\xD1\x80\xD0\xB8\xD0\xB2\xD0\xB5\xD1\x82/qwerty";

    char const* string = gil::detail::convert_to_native_string(path);
    BOOST_TEST_EQ( 34, strlen(string) );
    BOOST_TEST_EQ( 0, std::strcmp(expected, string) );
    delete[] string;
}

int main()
{
    // On Windows, convert_to_string()/convert_to_native_string() encode
    // UTF-8 directly via WideCharToMultiByte and don't depend on the
    // global locale. On POSIX, they still use wcstombs/wcsrtombs, which
    // require a UTF-8-capable global locale to be active. The exact name
    // of such a locale is platform-dependent (e.g. "C.UTF-8" on glibc
    // systems is not available on macOS), so try a few common candidates;
    // this is a no-op in practice on Windows.
    char const* const utf8_locale_names[] = {
        "C.UTF-8", "en_US.UTF-8", "UTF-8", "en_US.utf8"
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
