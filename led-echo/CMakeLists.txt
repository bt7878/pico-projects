cmake_minimum_required(VERSION 3.13)
add_executable(led-echo
  led-echo.cpp
)
pico_enable_stdio_usb(led-echo 1)
pico_enable_stdio_uart(led-echo 0)
pico_add_extra_outputs(led-echo)
target_link_libraries(led-echo pico_stdlib)
