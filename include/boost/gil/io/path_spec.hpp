//
// Copyright 2007-2008 Andreas Pokorny, Christian Henning
// Copyright 2024 Dirk Stolle
//
// Distributed under the Boost Software License, Version 1.0
// See accompanying file LICENSE_1_0.txt or copy at
// http://www.boost.org/LICENSE_1_0.txt
//
#ifndef BOOST_GIL_IO_PATH_SPEC_HPP
#define BOOST_GIL_IO_PATH_SPEC_HPP

#include <boost/gil/io/detail/filesystem.hpp>

#include <cstdlib>
#include <cstring>
#include <cwchar>
#include <string>
#include <type_traits>

#if defined(_WIN32)
#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif
#ifndef NOMINMAX
#define NOMINMAX
#endif
#include <windows.h>
#endif

namespace boost { namespace gil { namespace detail {

#if defined(_WIN32)
// Converts a wide string (UTF-16 on Windows) to UTF-8 without relying on
// the current C/global locale. wcstombs/wcsrtombs depend on setlocale()
// state, and MSVC's setlocale() parser is stricter than std::locale's own
// constructor about which locale-name strings it accepts, so a locale that
// constructs successfully can still leave the C runtime on the system ANSI
// codepage instead of UTF-8. WideCharToMultiByte with CP_UTF8 sidesteps
// that mismatch entirely.
inline std::string wide_to_utf8(wchar_t const* str, std::size_t len)
{
    if (len == 0)
    {
        return std::string();
    }
    int const size = ::WideCharToMultiByte(
        CP_UTF8, 0, str, static_cast<int>(len), nullptr, 0, nullptr, nullptr);
    std::string result(static_cast<std::size_t>(size), '\0');
    ::WideCharToMultiByte(
        CP_UTF8, 0, str, static_cast<int>(len), &result[0], size, nullptr, nullptr);
    return result;
}
#endif

template<typename P> struct is_supported_path_spec              : std::false_type {};
template<> struct is_supported_path_spec< std::string >         : std::true_type {};
template<> struct is_supported_path_spec< const std::string >   : std::true_type {};
template<> struct is_supported_path_spec< std::wstring >        : std::true_type {};
template<> struct is_supported_path_spec< const std::wstring >  : std::true_type {};
template<> struct is_supported_path_spec< char const* >         : std::true_type {};
template<> struct is_supported_path_spec< char* >               : std::true_type {};
template<> struct is_supported_path_spec< const wchar_t* >      : std::true_type {};
template<> struct is_supported_path_spec< wchar_t* >            : std::true_type {};

template<int i> struct is_supported_path_spec<const char [i]>       : std::true_type {};
template<int i> struct is_supported_path_spec<char [i]>             : std::true_type {};
template<int i> struct is_supported_path_spec<const wchar_t [i]>    : std::true_type {};
template<int i> struct is_supported_path_spec<wchar_t [i]>          : std::true_type {};

template<> struct is_supported_path_spec<filesystem::path> : std::true_type {};
template<> struct is_supported_path_spec<filesystem::path const> : std::true_type {};

inline std::string convert_to_string( std::string const& obj)
{
   return obj;
}

inline std::string convert_to_string( std::wstring const& s )
{
#if defined(_WIN32)
    return wide_to_utf8( s.c_str(), s.size() );
#else
    std::mbstate_t state = std::mbstate_t();
    const wchar_t* str = s.c_str();
    const std::size_t len = std::wcsrtombs(nullptr, &str, 0, &state);
    std::string result(len, '\0');
    std::wcstombs( &result[0], s.c_str(), len );

    return result;
#endif
}

inline std::string convert_to_string( char const* str )
{
    return std::string( str );
}

inline std::string convert_to_string( char* str )
{
    return std::string( str );
}

inline std::string convert_to_string(filesystem::path const& path)
{
    return convert_to_string(path.string());
}

inline char const* convert_to_native_string( char* str )
{
    return str;
}

inline char const* convert_to_native_string( char const* str )
{
    return str;
}

inline char const* convert_to_native_string( const std::string& str )
{
   return str.c_str();
}

inline char const* convert_to_native_string( const wchar_t* str )
{
#if defined(_WIN32)
    std::string const utf8 = wide_to_utf8( str, std::wcslen( str ) );
    char* c = new char[utf8.size() + 1];
    std::memcpy( c, utf8.c_str(), utf8.size() + 1 );

    return c;
#else
    std::mbstate_t state = std::mbstate_t();
    const std::size_t len = std::wcsrtombs(nullptr, &str, 0, &state) + 1;
    char* c = new char[len];
    std::wcstombs( c, str, len );

    return c;
#endif
}

inline char const* convert_to_native_string( std::wstring const& str )
{
#if defined(_WIN32)
    return convert_to_native_string( str.c_str() );
#else
    std::mbstate_t state = std::mbstate_t();
    const wchar_t* wstr = str.c_str();
    const std::size_t len = std::wcsrtombs(nullptr, &wstr, 0, &state) + 1;
    char* c = new char[len];
    std::wcstombs( c, str.c_str(), len );

    return c;
#endif
}

}}} // namespace boost::gil::detail

#endif
