


#include <mango/mango.h>

#include <set>
#include <vector>
#include <fstream>

#include "Timer.h"

int count = 0;

bool quit;

void read_thread(std::shared_ptr<mango::interprocess::eipc::session> ssc)
{
    mango::interprocess::eipc::content_ptr content;
    std::vector<data_t> v_delay;
    v_delay.resize(100000);
    int v_delay_index = 0;

    std::chrono::steady_clock::time_point start = std::chrono::steady_clock::now();
    while (!quit)
    {
        if (ssc->fast_read(content) == mango::interprocess::eipc::session::read_result_t::noerror)
        {
            //std::cout << "tag:[" << content->tag() << "]" << " size:[" << content->size() << "]" << std::endl;
            //std::cout << "    data:[" << ((data_t*)content->data())->msg << "]" << std::endl;

            ((data_t*)content->data())->delay.Stop();
            v_delay[v_delay_index] = (*(data_t*)content->data());
            v_delay_index++;

            ssc->fast_read_release(content);
        }
    }
    std::chrono::steady_clock::time_point end = std::chrono::steady_clock::now();

    std::ofstream fout("delay.csv", std::ios::out);

    for (auto& item : v_delay)
    {
        fout << std::fixed << item.msg << ", " << item.delay.start().QuadPart << ", " << item.delay.end().QuadPart << ", " << item.delay.GetElapsedMilliseconds() << "," << std::endl;
    }

    std::cout << "count: " << count << std::endl;
    std::cout << "over: " << std::chrono::duration<double>(end - start).count() << std::endl;
}

void on_connect(std::shared_ptr<mango::interprocess::eipc::session> ssc)
{
    std::cout << __FUNCTION__ << " " << ssc->name() << std::endl;
    //ssc_map.insert(std::make_pair(ssc->name(), ssc));

    quit = false;
    std::thread{ read_thread, ssc }.detach();
}

void on_disconnect(std::shared_ptr<mango::interprocess::eipc::session> ssc)
{
    std::cout << __FUNCTION__ << " " << ssc->name() << std::endl;
    //ssc_map.erase(ssc->name());

    quit = true;
}

int main()
{
    mango::interprocess::eipc::server server;
    server.bind("TEST_SERVER", on_connect, on_disconnect);
      
    std::cout << "server: " << server.name() << " is listening." << std::endl;
    server.listen();

    system("pause");
}

void on_connected()
{

}

void on_return_depth_market_data()
{

}