/** @file
 *
 * This file defines operations for runtime debug support
 */

#include <debug.h>
#include <cstdio>
#include <vector>
#include <memory>
#include <stdexcept>
#include <string>
#include <array>
#include <chrono>
#include <cxxabi.h>
#include <cstdlib>

bool __is_addr2line_available = false;
bool __is_time_enabled = false;
bool __disable_output = false;

std::string __exec_cmd(const std::string& cmd)
{
    std::array<char, 512> buffer{};
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd.c_str(), "r"), pclose);
    if (!pipe)
    {
        throw std::runtime_error("popen() failed!");
    }

    while (fgets(buffer.data(), buffer.size(), pipe.get()) != nullptr)
    {
        result += buffer.data();
    }

    return result;
}

std::string __get_addr(const std::string& input)
{
    std::string output_ad, output_sq;
    bool start = false;

    for (auto i : input)
    {
        if (i == '+')
        {
            start = true;
            continue;
        }

        if (start)
        {
            if (i != ')')
            {
                output_ad += i;
            }
            else
            {
                break;
            }
        }
    }

    start = false;

    for (auto i : input)
    {
        if (i == '[')
        {
            start = true;
            continue;
        }

        if (start)
        {
            if (i != ']')
            {
                output_ad += i;
            }
            else
            {
                break;
            }
        }
    }

    if (!output_ad.empty())
    {
        return output_ad;
    }

    return output_sq;
}

std::string __get_path(const std::string& input)
{
    std::string pathname;
    for (auto i : input)
    {
        if (i != '(')
        {
            pathname += i;
        }
        else
        {
            break;
        }
    }

    return pathname;
}

std::vector < std::string > str2lines(const std::string& input)
{
    std::string line;
    std::vector < std::string > ret;

    for (auto i : input)
    {
        if (i == '\n')
        {
            ret.emplace_back(line);
            line.clear();
            continue;
        }

        line += i;
    }

    return ret;
}

std::string __clean_addr2line_output(const std::string& input)
{
    auto find_name = [&](std::string & line)->std::string {
        std::string i;
        for (auto & j : line)
        {
            if (j != ' ')
            {
                i += j;
                continue;
            }
            else
            {
                break;
            }
        }

        return i;
    };

    std::vector < std::string > line = str2lines(input);
    for (auto i : line)
    {
        if (i[0] == '?' && i[1] == '?')
        {
            continue;
        }

        auto name = find_name(i);
        auto r_name = __demangle(name.c_str());
        std::string del_head;
        for (int c = 0; c < i.length() - name.length() + 1; c++)
        {
            del_head += i[name.length() + 1 + c];
        }

        r_name += " ";
        r_name += del_head;

        return r_name;
    }

    return "(no specific information)";
}

void __check_addr2line()
{
    __is_addr2line_available = (system("addr2line --help >/dev/null 2>/dev/null") == 0);
}

std::string __current_time()
{
    auto time = std::chrono::system_clock::to_time_t(std::chrono::high_resolution_clock::now());
    return std::ctime(&time);
}

std::string __demangle(const char* mangledName)
{
    int status = -1;
    char* demangled = abi::__cxa_demangle(mangledName, nullptr, nullptr, &status);
    std::string result = (status == 0) ? demangled : mangledName;
    std::free(demangled); // Free the memory allocated by __cxa_demangle
    return result;
}
