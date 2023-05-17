#include <iostream>

extern "C"
{
    extern int CoreStartup(int, const char *, const char *, void *, void*, void *, void*);
}

int main()
{
    auto test{ CoreStartup(0x020001, "C:\\", "C:\\", nullptr, nullptr, nullptr, nullptr ) };
    std::cout << test << std::endl;
}