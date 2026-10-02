#pragma once

//--------------------------------------------------------------------------------

#include <source_location>
#include <string_view>
#include <format>
#include <iostream>

//--------------------------------------------------------------------------------

namespace error_handle
{

//--------------------------------------------------------------------------------

constexpr std::string_view COLOR_RED   = "\033[31m";
constexpr std::string_view COLOR_RESET = "\033[0m";

//--------------------------------------------------------------------------------

class ErrorFormat
{
    std::string_view     format_;
    std::source_location location_;

public:
    ErrorFormat (const char* format,
                 std::source_location location = 
                 std::source_location::current ()) :
        format_(format),
        location_(location)
    {}

    std::source_location GetLocation () const
    {
        return location_;
    }
    std::string_view GetFormat () const
    {
        return format_;
    }

}; // class ErrorFormat

//--------------------------------------------------------------------------------

template <typename... Args>
void PrintError (const ErrorFormat fmt, Args&&... args)
{
    std::cerr << COLOR_RED << "[ERROR] At: "
              << fmt.GetLocation ().file_name ()     << ":"
              << fmt.GetLocation ().line ()          << ":" 
              << fmt.GetLocation ().column ()        << ": " 
              << fmt.GetLocation ().function_name () << "\n"
              << std::vformat (fmt.GetFormat (), std::make_format_args (args...)) 
              << std::endl << COLOR_RESET;
} 

//--------------------------------------------------------------------------------

}; // namespace error_handle;

//--------------------------------------------------------------------------------