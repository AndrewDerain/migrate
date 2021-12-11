#pragma once

#include "../mango_headers.h"
#include <iostream>

char myster[56];

class fixed_string_test
{
public:
    void run()
    {
        mango::foundation::fixed_string<5> s;
        mango::foundation::fixed_string<10> l;

        strcpy_s(myster, 56, "hello , world!xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx");

        mango::utility::shadow(myster) = "hello, world!xxxxxxxxxxxxxxxxxxxxxxxxxxxxxxxx";

        int a = 9;
        std::string h = "jfklasjdf";

        s = "12345";
        l = "00";
        s.assign(l);
        std::cout << s.c_str() << std::endl;
        bool c = l == "12345";
    }
};
