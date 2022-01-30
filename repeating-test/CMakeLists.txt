cmake_minimum_required(VERSION 3.13)
add_executable(repeating-test
  repeating-test.cpp
)
pico_enable_stdio_usb(repeating-test 1)
pico_enable_stdio_uart(repeating-test 0)
pico_add_extra_outputs(repeating-test)
target_link_libraries(repeating-test pico_stdlib)
