#include <iostream>
#include <random>

#include "consistenthashing.h"


int main()
{
    consistent_hashing<> hash;
    hash.insertNode("server0", 100);
    hash.insertNode("server1", 100);
    hash.insertNode("server2", 100);
    hash.insertNode("server3", 100);
    hash.insertNode("server4", 100);
    hash.insertNode("server5", 100);
    hash.insertNode("server6", 100);
    hash.insertNode("server7", 100);
    hash.insertNode("server8", 100);
    hash.insertNode("server9", 100);

    std::default_random_engine generator;
    std::uniform_int_distribution<uint32_t> distribution(0, 0xFFFFFFFFFFFF);

    uint32_t count[10] = {0};
    auto print = [&count]()
    {
        for(int i = 0; i < 10; ++i)
            std::cout << "server" << i << ">>" << count[i] << std::endl;
    };

    char buf[128] = {0};
    for (int i = 0; i < 100000; ++i)
    {
        std::string id;
        itoa(distribution(generator), buf, 10);
        id = hash.find(buf);
        for(int i = 0; i < 10; ++i)
        {
            itoa(i, buf, 10);
            if(id == std::string("server") + buf )
            {
                ++count[i];
                break;
            }
        }
    }

    print();
    std::cout << "remove server1" << std::endl;
    for(int i = 0; i < 10; ++i)
        count[i] = 0;
    hash.removeNode("server1");

    for (int i = 0; i < 100000; ++i)
    {
        std::string id;
        itoa(distribution(generator), buf, 10);
        id = hash.find(buf);
        for(int i = 0; i < 10; ++i)
        {
            itoa(i, buf, 10);
            if(id == std::string("server") + buf )
            {
                ++count[i];
                break;
            }
        }
    }
    print();

    return 0;
}

