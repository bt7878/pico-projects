cmake_minimum_required(VERSION 3.13)
add_executable(finite-state-machine
  finite-state-machine.cpp
)
pico_enable_stdio_usb(finite-state-machine 1)
pico_enable_stdio_uart(finite-state-machine 0)
pico_add_extra_outputs(finite-state-machine)
target_link_libraries(finite-state-machine pico_stdlib)
