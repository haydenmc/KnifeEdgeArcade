#include <iostream>
#include "m64p/Mupen64PlusCore.h"

int main()
{
    m64p::Mupen64PlusCore m64{{
        "C:\\", // Config path
        "C:\\", // Data path
    }};
}