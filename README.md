## DAC8532 STM32 Driver
A serial peripheral interface (SPI) driver for the [DAC8532 dual channel, low power, 16-bit, serial input digital-to-analog converter](https://www.ti.com/lit/ds/symlink/dac8532.pdf?ts=1714978561542&ref_url=https%253A%252F%252Fwww.ti.com%252Fproduct%252FDAC8532) written in C for the ARM-based Cortex-M7 MCU running on STM32F7 devices. This driver has been tested using the STM NUCLEO-F746ZG board.

### Including the driver in your STM32 project
If you are building your STM32 project using the STM32CubeIDE, simply place the DAC8532.c and DAC8532.h files within the Src and Inc directories of your project, respectively.

In the future, time permitting, I will look into building this driver as a static library through the use of the [stm32-cmake project](https://github.com/ObKo/stm32-cmake/tree/master).

### Notice on STM32F7 HAL
This driver was originally written using the STM32F7 (hardware abstraction layer) HAL functions for communicating over SPI. After some of my own testing, with confirmation from the [community](https://community.st.com/t5/stm32-mcus-products/spi-too-slow/m-p/251638), it has been made clear that the STM32F7 HAL is insufficient for high-speed applications. Thus, I have replaced all HAL function calls with their equivalent register calls.

#### Author: Ethan Garnier