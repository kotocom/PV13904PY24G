// CONFIG_HPP
#pragma once

// Display params.
#define DISPLAY_RESOLUTION (454 * 454)

// Helpers for SystemInitError().
#define SYSTEM_INIT_ERROR_FLASH 0x01
#define SYSTEM_INIT_ERROR_PLL 0x02
#define SYSTEM_INIT_ERROR_CLKSRC 0x04
#define SYSTEM_INIT_ERROR_HSI 0x08

// Stat LED pin.
#define PD0 0
#define LED_TOGGLE GPIOD->ODR ^= (1 << PD0)

// CONFIG_HPP_END
