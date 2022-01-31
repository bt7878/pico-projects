#include "pico/multicore.h"
#include "pico/stdlib.h"
#include "pico/sync.h"
#include <iostream>
#include <vector>

std::vector<int> toSum(50000, 1);

volatile int sumNaive = 0;
volatile int sumMutex = 0;
auto_init_mutex(m);

void core1_entry()
{
    while (true)
    {
        void (*f)() = (void (*)())multicore_fifo_pop_blocking();
        f();
        multicore_fifo_push_blocking(0);
    }
}

void sumUpNaiveCore1()
{
    for (int i = toSum.size() / 2; i < toSum.size(); i++)
    {
        sumNaive += toSum[i];
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
}

int main()
{
    stdio_init_all();
    multicore_launch_core1(core1_entry);
    volatile int r;

    while (true)
    {
        sumNaive = 0;
        sumMutex = 0;

        multicore_fifo_push_blocking((uintptr_t)&sumUpNaiveCore1);

        for (int i = 0; i < toSum.size() / 2; i++)
        {
            sumNaive += toSum[i];
        }

        r = multicore_fifo_pop_blocking();
        multicore_fifo_push_blocking((uintptr_t)&sumUpMutexCore1);

        for (int i = 0; i < toSum.size() / 2; i++)
        {
            mutex_enter_blocking(&m);
            sumMutex += toSum[i];
            mutex_exit(&m);
        }

        r = multicore_fifo_pop_blocking();

        std::cout << "Naive sum: " << sumNaive << std::endl;
        std::cout << "Mutex sum: " << sumMutex << std::endl;
        sleep_ms(5000);
    }

    return 0;
}
