/**
 * Configuration file for the SSD1306 OLED library.
 *
 * Hardware:
 * - MCU: STM32F401RE
 * - Interface: I2C1
 * - OLED controller: SSD1306
 * - Resolution: 128 x 64
 * - I2C address: 0x3C
 */

#ifndef __SSD1306_CONF_H__
#define __SSD1306_CONF_H__

/* Select the STM32 family */
#define STM32F4

/* Select the communication interface */
#define SSD1306_USE_I2C
/* #define SSD1306_USE_SPI */

/* I2C configuration */
#define SSD1306_I2C_PORT    hi2c1
#define SSD1306_I2C_ADDR    (0x3C << 1)

/* Display dimensions */
#define SSD1306_WIDTH       128
#define SSD1306_HEIGHT      64

/* Optional screen orientation settings */
/* #define SSD1306_MIRROR_VERT */
/* #define SSD1306_MIRROR_HORIZ */

/* Optional inverse-color mode */
/* #define SSD1306_INVERSE_COLOR */

/* Include only the fonts you need */
#define SSD1306_INCLUDE_FONT_6x8
#define SSD1306_INCLUDE_FONT_7x10
#define SSD1306_INCLUDE_FONT_11x18
#define SSD1306_INCLUDE_FONT_16x26

#endif /* __SSD1306_CONF_H__ */
