/** @file
 *  this file implements functions for HTMPFS_error_t
 */

#include <error_handler.h>

/// used for error_t::what()
#define ERROR_SWITCH_START \
    switch (error_code) { __asm__("nop")
/// used for error_t::what()
#define ERROR_SWITCH_CASE(val_name) case val_name : return val_name##_ERR_TEXT
/// used for error_t::what()
#define ERROR_SWITCH_END default : return "Unknown error"; } __asm__("nop")

const char *error_handler_t::what() const noexcept
{
    ERROR_SWITCH_START;
            ERROR_SWITCH_CASE(FS_SUCCESSFUL);
    ERROR_SWITCH_END;
}

void error_handler_t::_output_error_message() const noexcept
{
    std::cerr <<
            "Error: 0x" << std::uppercase << std::hex << error_code << " (" << what() << ")" << "\n" <<
            "System error: " << what_errno() << "\n" <<
            "Specified information: " << info << "\n" << std::flush;
}
