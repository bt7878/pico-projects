#include "pico/stdlib.h"
#include <queue>

std::queue<absolute_time_t> echoQueue;

void gpio_irq_callback(uint gpio, uint32_t events)
{
    if (gpio == 12)
    {
        if (events == GPIO_IRQ_EDGE_FALL)
        {
            hardware_alarm_cancel(0);
            echoQueue.push(get_absolute_time());
            gpio_put(11, true);
        }
        else if (events == GPIO_IRQ_EDGE_RISE && !echoQueue.empty())
        {
            echoQueue.push(get_absolute_time());
            gpio_put(11, false);
            hardware_alarm_set_target(0, make_timeout_time_ms(5000));
        }
    }
}

void hardware_alarm_callback(uint alarm_num)
{
    if (alarm_num <= 1)
    {
        if (alarm_num == 0)
        {
            gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, false, gpio_irq_callback);
            gpio_put(11, false);
            gpio_put(PICO_DEFAULT_LED_PIN, true);
        }
        if (!echoQueue.empty())
        {
            gpio_put(20, !gpio_get(20));
            absolute_time_t startTime = echoQueue.front();
            echoQueue.pop();
            if (!echoQueue.empty())
            {
                absolute_time_t endTime = echoQueue.front();
                hardware_alarm_set_target(1, make_timeout_time_us(absolute_time_diff_us(startTime, endTime)));
            }
            else
            {
                gpio_put(20, false);
                gpio_put(PICO_DEFAULT_LED_PIN, false);
                gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true,
                                                   gpio_irq_callback);
            }
        }
        else
        {
            gpio_put(PICO_DEFAULT_LED_PIN, false);
            gpio_put(20, false);
            gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, gpio_irq_callback);
        }
    }
}

int main()
{
    hardware_alarm_set_callback(0, hardware_alarm_callback);
    hardware_alarm_set_callback(1, hardware_alarm_callback);

    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);

    gpio_init(12);
    gpio_set_dir(12, GPIO_IN);
    gpio_pull_up(12);
    gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL | GPIO_IRQ_EDGE_RISE, true, gpio_irq_callback);

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
