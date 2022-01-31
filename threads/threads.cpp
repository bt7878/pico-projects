#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include <iostream>
#include <vector>

std::vector<int> toSum(50000, 1);

volatile int sumNaive = 0;
volatile int sumMutex = 0;
auto_init_mutex(m);

void sumUpNaiveCore1()
{
    for (int i = toSum.size() / 2; i < toSum.size(); i++)
    {
        sumNaive += toSum[i];
    }
    multicore_fifo_push_blocking(0);
    while (true)
    {
        tight_loop_contents();
    }
}

void sumUpMutexCore1()
{
    for (int i = toSum.size() / 2; i < toSum.size(); i++)
    {
        mutex_enter_blocking(&m);
        sumMutex += toSum[i];
        mutex_exit(&m);
    }
    multicore_fifo_push_blocking(0);
    while (true)
    {
        tight_loop_contents();
    }
}

int main()
{
    stdio_init_all();

    while (true)
    {
        sumNaive = 0;
        sumMutex = 0;

        multicore_launch_core1(sumUpNaiveCore1);

        for (int i = 0; i < toSum.size() / 2; i++)
        {
            sumNaive += toSum[i];
        }

        assert(multicore_fifo_pop_blocking() == 0);
        multicore_reset_core1();
        multicore_launch_core1(sumUpMutexCore1);

        for (int i = 0; i < toSum.size() / 2; i++)
        {
            mutex_enter_blocking(&m);
            sumMutex += toSum[i];
            mutex_exit(&m);
        }

        assert(multicore_fifo_pop_blocking() == 0);
        multicore_reset_core1();

        std::cout << "Naive sum: " << sumNaive << std::endl;
        std::cout << "Mutex sum: " << sumMutex << std::endl;
        sleep_ms(5000);
    }

    return 0;
}
