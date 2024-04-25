cmake_minimum_required(VERSION 3.22)

project(STM32F1xx_HAL_Driver)
add_library(STM32F1xx_HAL_Driver INTERFACE)

# Enable CMake support for ASM and C languages
enable_language(C ASM)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE "Debug")
endif()

target_compile_definitions(STM32F1xx_HAL_Driver INTERFACE 
	USE_HAL_DRIVER 
	STM32F103xB
    $<$<CONFIG:Debug>:DEBUG>
)

target_include_directories(STM32F1xx_HAL_Driver INTERFACE
    include
)

target_sources(STM32F1xx_HAL_Driver INTERFACE
    stm32f1xx_it.c
    system_stm32f1xx.c

    # Src/stm32f1xx_hal_adc.c
    # Src/stm32f1xx_hal_can.c
    # Src/stm32f1xx_hal_cec.c
    Src/stm32f1xx_hal_cortex.c
    # Src/stm32f1xx_hal_crc.c
    # Src/stm32f1xx_hal_dac_ex.c
    # Src/stm32f1xx_hal_dac.c
    # Src/stm32f1xx_hal_dma.c
    # Src/stm32f1xx_hal_eth.c
    # Src/stm32f1xx_hal_flash_ex.c
    # Src/stm32f1xx_hal_flash.c
    # Src/stm32f1xx_hal_gpio_ex.c
    Src/stm32f1xx_hal_gpio.c
    # Src/stm32f1xx_hal_hcd.c
    # Src/stm32f1xx_hal_i2c.c
    # Src/stm32f1xx_hal_i2s.c
    # Src/stm32f1xx_hal_irda.c
    # Src/stm32f1xx_hal_iwdg.c
    # Src/stm32f1xx_hal_mmc.c
    # Src/stm32f1xx_hal_nand.c
    # Src/stm32f1xx_hal_nor.c
    # Src/stm32f1xx_hal_pccard.c
    # Src/stm32f1xx_hal_pcd_ex.c
    # Src/stm32f1xx_hal_pcd.c
    # Src/stm32f1xx_hal_pwr.c
    # Src/stm32f1xx_hal_rcc_ex.c
    Src/stm32f1xx_hal_rcc.c
    # Src/stm32f1xx_hal_rtc_ex.c
    # Src/stm32f1xx_hal_rtc.c
    # Src/stm32f1xx_hal_sd.c
    # Src/stm32f1xx_hal_smartcard.c
    # Src/stm32f1xx_hal_spi.c
    # Src/stm32f1xx_hal_sram.c
    # Src/stm32f1xx_hal_tim_ex.c
    # Src/stm32f1xx_hal_tim.c
    # Src/stm32f1xx_hal_uart.c
    # Src/stm32f1xx_hal_usart.c
    # Src/stm32f1xx_hal_wwdg.c
    Src/stm32f1xx_hal.c
)

target_link_directories(STM32F1xx_HAL_Driver INTERFACE
)

target_link_libraries(STM32F1xx_HAL_Driver INTERFACE
)

# Validate that STM32F1xx_HAL_Driver code is compatible with C standard
if(CMAKE_C_STANDARD LESS 11)
    message(ERROR "Generated code requires C11 or higher")
endif()


