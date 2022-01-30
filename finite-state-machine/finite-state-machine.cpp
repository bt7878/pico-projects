#include "pico/stdlib.h"

// Debouncing
uint64_t lastPressTime = to_us_since_boot(get_absolute_time());

void gpio_irq_callback(uint gpio, uint32_t events)
{
    if (gpio == 12 && events == GPIO_IRQ_EDGE_FALL)
    {
        if (to_us_since_boot(get_absolute_time()) - lastPressTime > 200000)
        {
            bool curWhite = gpio_get(11);
            bool curRed = gpio_get(20);
            gpio_put(11, curRed && !curWhite);
            gpio_put(20, !(curRed && curWhite));
            lastPressTime = to_us_since_boot(get_absolute_time());
        }
    }
}

int main()
{
    gpio_init(11);
    gpio_set_dir(11, GPIO_OUT);
    gpio_init(20);
    gpio_set_dir(20, GPIO_OUT);

    gpio_init(12);
    gpio_set_dir(12, GPIO_IN);
    gpio_pull_up(12);
    gpio_set_irq_enabled_with_callback(12, GPIO_IRQ_EDGE_FALL, true, gpio_irq_callback);

    while (true)
    {
        tight_loop_contents();
    }

    return 0;
}
