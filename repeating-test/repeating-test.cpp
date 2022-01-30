#include "pico/stdlib.h"
#include <cmath>
#include <vector>

#define TOLERENCE_US 500000

std::vector<absolute_time_t> keyInput;
std::vector<absolute_time_t> testInput;

bool testing = false;
int toChangeFlash = 0;

void gpio_irq_callback(uint gpio, uint32_t events)
{
    if (gpio == 12 && !toChangeFlash)
    {
        if (testing)
        {
            if (events == GPIO_IRQ_EDGE_FALL)
            {
                testInput.push_back(get_absolute_time());
                gpio_put(20, true);
            }
            else if (events == GPIO_IRQ_EDGE_RISE && !testInput.empty())
            {
                testInput.push_back(get_absolute_time());
                gpio_put(20, false);
            }
            if (keyInput.size() == testInput.size())
            {
                gpio_put(20, false);
                toChangeFlash = 4;
                for (int i = 0; i < keyInput.size() - 1; i++)
                {
                    if (abs(absolute_time_diff_us(keyInput[i], keyInput[i + 1]) -
                            absolute_time_diff_us(testInput[i], testInput[i + 1])) > TOLERENCE_US)
                    {
                        toChangeFlash = 2;
                    }
                }
                keyInput.clear();
                testInput.clear();
                hardware_alarm_set_target(0, make_timeout_time_ms(500));
            }
        }
        else
        {
            if (events == GPIO_IRQ_EDGE_FALL)
            {
                hardware_alarm_cancel(0);
                keyInput.push_back(get_absolute_time());
                gpio_put(20, true);
            }
            else if (events == GPIO_IRQ_EDGE_RISE && !keyInput.empty())
            {
                keyInput.push_back(get_absolute_time());
                gpio_put(20, false);
                hardware_alarm_set_target(0, make_timeout_time_ms(5000));
            }
        }
    }
}

void hardware_alarm_callback(uint alarm_num)
{
    if (!testing && !keyInput.empty())
    {
        gpio_put(PICO_DEFAULT_LED_PIN, true);
        testing = true;
    }
    else if (testing)
    {
        if (toChangeFlash == 1)
        {
            testing = false;
            gpio_put(11, false);
            gpio_put(PICO_DEFAULT_LED_PIN, false);
        }
        else
        {
            gpio_put(11, !gpio_get(11));
            hardware_alarm_set_target(0, make_timeout_time_ms(500));
        }
        toChangeFlash--;
    }
}

int main()
{
    hardware_alarm_set_callback(0, hardware_alarm_callback);

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
