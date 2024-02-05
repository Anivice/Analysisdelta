#ifndef DEBUG_H
#define DEBUG_H

/** @file
 *
 * This file defines operations for runtime debug support
 */

#include <iostream>
#include <execinfo.h>
#include <sys/xattr.h>
#include <sys/sysinfo.h>
#include <string>

// override debug flag
//#undef CMAKE_BUILD_DEBUG

#ifdef CMAKE_BUILD_DEBUG

/// check if addr2line command is available, filled by __check_addr2line()
extern bool __is_addr2line_available;

/// if output time is enabled, filled by main
extern bool __is_time_enabled;

/// check addr2line availability
void __check_addr2line();

/// temporarily disable debug output
extern bool __disable_output;

/// execute a command and get its output
/// @param cmd command for SHELL
/// @return SHELL output
std::string __exec_cmd(const std::string& cmd);

/// get address in stack frame output
/// @param input raw stack frame input
/// @return address of stack frame
std::string __get_addr(const std::string& input);

/// get pathname of elf file in stack frame output
/// @param input raw stack frame input
/// @return pathname of elf file
std::string __get_path(const std::string& input);

/// clean addr2line output, remove "?? ??:0"
/// @param input addr2line raw output
/// @return useful information provided by addr2line, or "(no specific information)" if not
std::string __clean_addr2line_output(const std::string& input);

/// get current time
/// @param none
/// @return std::string of current time
std::string __current_time();

/// convert C++ demangled names into ones that are human-readable
/// @param mangledName name
/// @return converted name
std::string __demangle(const char* mangledName);

/// automatically check availability of addr2line (before main)
static class __exec_check_addr2line__
{
public:
    __exec_check_addr2line__() noexcept { __check_addr2line(); }
} __init_check_addr2line__;

# define BACKTRACE_SZ 64    /* Max stack frame size */
/// Automatically obtain stack frame
/// OBTAIN_STACK_FRAME(output_pipe)
/// output_pipe == 1: stdout, output_pipe == 2, stderr
/// other value will be ignored and pipe will be default to stdout
# define OBTAIN_STACK_FRAME(fd)                                                         \
if (!__disable_output)                                                                  \
{                                                                                       \
    auto * output_pipe = &std::cout;                                                    \
    if ((fd) == 2)                                                                      \
    {                                                                                   \
        output_pipe = &std::cerr;                                                       \
    }                                                                                   \
    void *__array[BACKTRACE_SZ];                                                        \
    int __size, __i;                                                                    \
    char **__strings;                                                                   \
    __size = backtrace (__array, BACKTRACE_SZ);                                         \
    __strings = backtrace_symbols (__array, __size);                                    \
                                                                                        \
    if (__is_time_enabled)                                                              \
    {                                                                                   \
        *output_pipe << std::endl << __current_time();                                  \
    }                                                                                   \
                                                                                        \
    if (__strings != nullptr)                                                           \
    {                                                                                   \
        *output_pipe   << "Obtained stack frame(s):"                                    \
                       << std::endl;                                                    \
        for (__i = 0; __i < __size; __i++)                                              \
        {                                                                               \
            *output_pipe << "\t" << __strings[__i];                                     \
            if (__is_addr2line_available)                                               \
            {                                                                           \
                *output_pipe << " " << __clean_addr2line_output(                        \
                                    __exec_cmd(std::string("addr2line a -f -p -e ")     \
                                        + __get_path(__strings[__i]) + " "              \
                                        + __get_addr(__strings[__i])                    \
                                    )                                                   \
                                );                                                      \
            }                                                                           \
                                                                                        \
            *output_pipe << "\n";                                                       \
        }                                                                               \
        free (__strings);                                                               \
        *output_pipe << std::endl << std::endl;                                         \
    }                                                                                   \
} __asm__("nop") /* suppress IDE "empty statement" warning */


/// output function name, file, and line location
/// FUNCTION_INFO(output_pipe)
/// output_pipe == 1: stdout, output_pipe == 2, stderr
/// other value will be ignored and pipe will be default to stdout
# define FUNCTION_INFO(fd)                              \
if (!__disable_output)                                  \
{                                                       \
    auto * output_pipe = &std::cout;                    \
    if ((fd) == 2)                                      \
    {                                                   \
        output_pipe = &std::cerr;                       \
    }                                                   \
                                                        \
    *output_pipe                                        \
              << "\nFrom " << __FILE__ << ":"           \
              << __LINE__ << ": "                       \
              << __FUNCTION__ << ": ";                  \
} __asm__("nop") /* suppress IDE "empty statement" warning */

# define INSTANCE(MESSAGE) \
    std::cout << "\033[01;33m" << (MESSAGE) << "\033[0m" << std::endl

#else // CMAKE_BUILD_DEBUG

# define OBTAIN_STACK_FRAME(fd) __asm__("nop") /* suppress IDE "empty statement" warning */
# define FUNCTION_INFO(fd)      __asm__("nop") /* suppress IDE "empty statement" warning */
# define INSTANCE(MESSAGE)      __asm__("nop") /* suppress IDE "empty statement" warning */

#endif // CMAKE_BUILD_DEBUG

/// this section is defining a marco with variable parameter count
#define _THROW_ERROR_2(_code, _info)  throw error_handler_t(_code, _info)
#define _THROW_ERROR_1(_code)         throw error_handler_t(_code)
#define _THROW_ERROR_0()              throw error_handler_t()

#define _FUNC_CHOOSER(_f1, _f2, _f3, ...) _f3
#define _FUNC_RECOMPOSER(argsWithParentheses) _FUNC_CHOOSER argsWithParentheses
#define _CHOOSE_FROM_ARG_COUNT(...) _FUNC_RECOMPOSER((__VA_ARGS__, _THROW_ERROR_2, _THROW_ERROR_1, ))
#define _NO_ARG_EXPANDER() ,,_THROW_ERROR_0
#define _MACRO_CHOOSER(...) _CHOOSE_FROM_ARG_COUNT(_NO_ARG_EXPANDER __VA_ARGS__ ())

/// generate error using error_handler_t
#define THROW_ERROR(fd, ...)                                    \
            FUNCTION_INFO(fd);                                  \
            OBTAIN_STACK_FRAME(fd);                             \
            _MACRO_CHOOSER(__VA_ARGS__)(__VA_ARGS__)

#endif // DEBUG_H
