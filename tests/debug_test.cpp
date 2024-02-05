/**
 * @file This file is used for test of error report and auto stack tracing
 *
 * */

#include <error_handler.h>

void func1();
void func2();

void func3()
{
    func1();
}

void func2()
{
    THROW_ERROR(1, FS_SUCCESSFUL, "Nothing to show here");
}

void func1()
{
    func2();
}

int main()
{
    auto func = [&](){
        func3();
    };

    try {
        func();
    } catch (error_handler_t & err) {
        std::cerr << err.what() << std::endl;
    }
}
