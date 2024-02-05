#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

/** @file
 * This file defines runtime error class and relevant operations
 */

#include <string>
#include <utility>
#include <cstring>
#include <cstdlib>
#include <debug.h>
#include <cerrno>
#include <cstdint>

/// _ADD_ERROR_INFORMATION_(error_code_name, error_code_value, error_code_text)
#define ADD_ERROR_INFORMATION(code, val, info, _errno)    \
static const char * code##_ERR_TEXT = info;               \
const unsigned long int code = val;                       \
const unsigned int code##_ERRNO_CODE = _errno;

/// Define error information
ADD_ERROR_INFORMATION(FS_SUCCESSFUL, 0x00000000, "Successful", 0)

#undef ADD_ERROR_INFORMATION

/// Filesystem Error Type
class error_handler_t : public std::exception
{
private:
    uint32_t    error_code;
    error_t     _errno = 0;
    std::string info;

public:
    /// Generate a error with error code
    /** @param _code Your error code
     *  @param _info expanded information for error
     **/
    explicit error_handler_t (unsigned int _code = 0, std::string _info = "(no specified information)") noexcept
        : info(std::move(_info))
        {
            error_code = _code;

            if (!__disable_output)
            {
                std::cerr << "Error thrown" << std::endl;
                _output_error_message();
            }
        }

    /// Return explanation of current error
    [[nodiscard]] const char * what() const noexcept override;

    /// Return the explanation of errno snapshoted when the current error is generated
    [[nodiscard]] const char * what_errno() const noexcept { return strerror(_errno); };

private:
    /// output error message
    /// @param msg system message
    void _output_error_message() const noexcept;
};

#endif //ERROR_HANDLER_H
