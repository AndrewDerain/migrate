


#include <mango/mango.h>
#include <chrono>
#include <cstdio>
#include "Timer.h"


int main()
{
    mango::interprocess::eipc::client client;
    mango::math::interval<int> j;

    std::string client_name;

    std::cout << "imput client name: ";
    std::cin >> client_name;
    client.connect("TEST_SERVER", client_name, 2048);

    data_t data;
    mango::utility::shadow(data.msg) = "this is client";

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    for (int i = 0; i < 100000; )
    {
        //sprintf(data.msg, "msg=%d", i);
        data.delay.Start();
        if (client.write(1, data) == mango::interprocess::eipc::client::write_result_t::noerror)
        {
            int while_counter = 2000;  while (while_counter--) {}
            ++i;
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();
    
    std::cout << "over: " << std::chrono::duration<double>(end - start).count() << std::endl;
    
    system("pause");
}
