#include <iostream>
#include <thread>
#include <vector>
#include <mutex>
#include <chrono>

int gs_your_money = 0;
int thread_count = 10; // 线程数
int N = 100000; // 每个线程执行的次数

void add_money()
{
    for(int i = 0; i < N; ++i)
    {
        ++gs_your_money;
    }
}

int main()
{
    std::vector<std::thread> threads;
    for (int i = 0; i < thread_count; ++i) {
        threads.emplace_back(add_money);
    }
    for (auto& t : threads) {
        t.join();
    }
    std::cout << "------------------------------: " << gs_your_money << std::endl;  
    return 0;
}
