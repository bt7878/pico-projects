cmake_minimum_required(VERSION 3.13)
add_executable(threads
  threads.cpp
)
pico_enable_stdio_usb(threads 1)
pico_enable_stdio_uart(threads 0)
pico_add_extra_outputs(threads)
target_link_libraries(threads pico_stdlib pico_multicore pico_sync)
