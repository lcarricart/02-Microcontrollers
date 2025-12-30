# STM32 CubeIDE Development Guide

A comprehensive guide for writing clean, maintainable STM32 applications using CubeIDE, focusing on best practices and architectural patterns.

---

## Table of Contents
1. [Project Structure](#project-structure)
2. [Keeping Code Clean](#keeping-code-clean)
3. [Using BSP Layer](#using-bsp-layer)
4. [Mbed vs CubeIDE Comparison](#mbed-vs-cubeide-comparison)
5. [Working with CubeMX Regeneration](#working-with-cubemx-regeneration)
6. [Flashing Your Device](#flashing-your-device)

---

## Project Structure

### Standard STM32 Project Layout

```
Core/
├── Inc/
│   ├── main.h              (CubeMX-generated)
│   ├── app.h               (Your application header)
│   └── stm32f4xx_nucleo.h  (BSP layer - optional)
├── Src/
│   ├── main.c              (CubeMX-generated - keep minimal)
│   ├── app.c               (Your application logic)
│   └── stm32f4xx_nucleo.c  (BSP layer - optional)
Drivers/
├── STM32F4xx_HAL_Driver/   (HAL peripheral drivers)
└── CMSIS/                  (ARM Cortex-M core files)
```

### Key Architectural Layers

```
┌─────────────────────────────┐
│   Application Layer         │  ← Your code (app.c)
│   (app.c, state machines)   │
├─────────────────────────────┤
│   BSP Layer (Optional)      │  ← Board-specific (stm32f4xx_nucleo.c)
│   (LED, Button helpers)     │
├─────────────────────────────┤
│   HAL/LL Layer             │  ← STM32 peripheral drivers
│   (GPIO, I2C, SPI, etc.)   │
├─────────────────────────────┤
│   CMSIS Layer              │  ← ARM Cortex-M core
│   (Startup, interrupts)    │
└─────────────────────────────┘
```

**Layer Responsibilities:**
- **Application**: Business logic, state machines, algorithms
- **BSP**: Board-specific abstractions (LED_On, Button_Read)
- **HAL**: Peripheral drivers (GPIO_WritePin, I2C_Transmit)
- **CMSIS**: Low-level ARM core support

---

## Keeping Code Clean

### The Problem: Everything in `main.c`

Developers **don't** write everything in `main.c`. They treat it as a **thin entry point** and move application logic into separate files.

### Recommended Pattern

#### 1. Create Application Files

**`Core/Inc/app.h`**
```c
#ifndef APP_H
#define APP_H

void app_init(void);
void app_loop(void);

#endif
```

**`Core/Src/app.c`**
```c
#include "app.h"
#include "main.h"   // Gives you HAL_*, GPIO ports/pins

void app_init(void)
{
    // One-time initialization beyond CubeMX init
}

void app_loop(void)
{
    HAL_GPIO_TogglePin(LD2_GPIO_Port, LD2_Pin);
    HAL_Delay(500);
}
```

#### 2. Keep `main.c` Minimal

**In `main.c`, only touch `USER CODE` blocks:**

```c
/* USER CODE BEGIN Includes */
#include "app.h"
/* USER CODE END Includes */

int main(void)
{
    HAL_Init();
    SystemClock_Config();
    MX_GPIO_Init();
    
    /* USER CODE BEGIN 2 */
    app_init();
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        app_loop();
        /* USER CODE END 3 */
    }
}
```

**Result:** `main.c` stays generated and readable, real program lives in `app.c`.

---

## Using BSP Layer

### Why BSP?

The HAL is verbose: `HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET)` for every LED toggle gets noisy in complex applications.

**BSP (Board Support Package)** provides board-specific, higher-level APIs:
- `BSP_LED_On(LED2)` instead of `HAL_GPIO_WritePin(...)`
- `BSP_PB_GetState(BUTTON_USER)` instead of `HAL_GPIO_ReadPin(...)`

### How to Add BSP Files

#### Step 1: Locate BSP Files

BSP files **already exist** in STM32Cube but aren't included by default:

```
C:\Users\<username>\STM32Cube\Repository\STM32Cube_FW_F4_V1.28.3\Drivers\BSP\
```

For NUCLEO-F401RE:
- `stm32f4xx_nucleo.h`
- `stm32f4xx_nucleo.c`

#### Step 2: Copy to Project

Copy files into your project:
- `stm32f4xx_nucleo.h` → `Core/Inc/`
- `stm32f4xx_nucleo.c` → `Core/Src/`

#### Step 3: Include in Code

**In `main.h`:**
```c
/* USER CODE BEGIN Includes */
#include "stm32f4xx_nucleo.h"
/* USER CODE END Includes */
```

### BSP Example: SOS Blinker

**Without BSP (verbose HAL):**
```c
while (1)
{
    if (HAL_GPIO_ReadPin(B1_GPIO_Port, B1_Pin) == GPIO_PIN_RESET)
    {
        for (int i = 0; i < 3; i++) {
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_SET);
            HAL_Delay(200);
            HAL_GPIO_WritePin(LD2_GPIO_Port, LD2_Pin, GPIO_PIN_RESET);
            HAL_Delay(200);
        }
    }
}
```

**With BSP (clean):**
```c
/* Once at init */
BSP_LED_Init(LED2);
BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

while (1)
{
    if (BSP_PB_GetState(BUTTON_USER) == 0)   // pressed (low active)
    {
        // S: ...
        for (int i = 0; i < 3; i++) {
            BSP_LED_On(LED2);
            HAL_Delay(200);
            BSP_LED_Off(LED2);
            HAL_Delay(200);
        }
        HAL_Delay(1000);

        // O: ---
        for (int i = 0; i < 3; i++) {
            BSP_LED_On(LED2);
            HAL_Delay(1000);
            BSP_LED_Off(LED2);
            HAL_Delay(200);
        }
        HAL_Delay(1000);

        // S: ...
        for (int i = 0; i < 3; i++) {
            BSP_LED_On(LED2);
            HAL_Delay(200);
            BSP_LED_Off(LED2);
            HAL_Delay(200);
        }

        // Wait for button release
        while (BSP_PB_GetState(BUTTON_USER) == 0) {
            HAL_Delay(10);
        }
    }
}
```

---

## Mbed vs CubeIDE Comparison

### Example 1: SOS Blinker

#### Mbed (High-Level Abstraction)
```cpp
#include "mbed.h"

#define SHORT_BLINK_RATE    200ms
#define LONG_BLINK_RATE     1000ms
#define INTER_LETTER_DELAY  1000ms

int main()
{
    DigitalOut led(D10);
    DigitalIn user_button(BUTTON1);  // Mbed maps BUTTON1 automatically

    while (true)
    {
        if (user_button == 0)  // Active-low
        {    
            // S: ...
            for (int i = 0; i < 3; i++) {
                led = 1;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
                led = 0;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
            }
            ThisThread::sleep_for(INTER_LETTER_DELAY);

            // O: ---
            for (int i = 0; i < 3; i++) {
                led = 1;
                ThisThread::sleep_for(LONG_BLINK_RATE);
                led = 0;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
            }
            ThisThread::sleep_for(INTER_LETTER_DELAY);

            // S: ...
            for (int i = 0; i < 3; i++) {
                led = 1;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
                led = 0;
                ThisThread::sleep_for(SHORT_BLINK_RATE);
            }
        }
    }
}
```

#### CubeIDE with BSP (Closest to Mbed)
```c
BSP_LED_Init(LED2);
BSP_PB_Init(BUTTON_USER, BUTTON_MODE_GPIO);

while (1)
{
    if (BSP_PB_GetState(BUTTON_USER) == 0)
    {
        // S: ...
        for (int i = 0; i < 3; i++) {
            BSP_LED_On(LED2);
            HAL_Delay(200);
            BSP_LED_Off(LED2);
            HAL_Delay(200);
        }
        HAL_Delay(1000);
        
        // (rest similar to above)
    }
}
```

### Example 2: Analog Joystick

#### Mbed (Simple)
```cpp
#include "mbed.h"

AnalogIn joystickX(A0);
AnalogIn joystickY(A1);

int main() {
    while (true)
    {
        int8_t x = joystickX.read() * 100;
        int8_t y = joystickY.read() * 100;

        char buffer[4];
        sprintf(buffer, "%d", x);
        printf("Position X: %s", buffer);
        sprintf(buffer, "%d", y);
        printf("Position Y: %s", buffer);

        ThisThread::sleep_for(500ms);
    }
}
```

#### CubeIDE with HAL
```c
/* In main.c - USER CODE BEGIN 2 */
MX_ADC1_Init();

/* In app.c */
uint16_t read_adc_once(uint32_t channel)
{
    ADC_ChannelConfTypeDef sConfig = {0};
    sConfig.Channel = channel;
    sConfig.Rank = 1;
    sConfig.SamplingTime = ADC_SAMPLETIME_15CYCLES;
    HAL_ADC_ConfigChannel(&hadc1, &sConfig);
    
    HAL_ADC_Start(&hadc1);
    HAL_ADC_PollForConversion(&hadc1, 10);
    uint16_t value = HAL_ADC_GetValue(&hadc1);
    HAL_ADC_Stop(&hadc1);
    
    return value;
}

void app_loop(void)
{
    // Example for A0=ADC1_IN0 and A1=ADC1_IN1
    uint16_t raw_x = read_adc_once(ADC_CHANNEL_0);
    uint16_t raw_y = read_adc_once(ADC_CHANNEL_1);

    // Scale 0..4095 to -100..+100
    int16_t x = (int16_t)(((int32_t)raw_x - 2048) * 100 / 2048);
    int16_t y = (int16_t)(((int32_t)raw_y - 2048) * 100 / 2048);

    printf("X=%d Y=%d\r\n", x, y);
    HAL_Delay(500);
}
```

**Note:** CubeIDE requires more HAL knowledge but offers finer control.

---

## Alternative High-Level Frameworks

If you want Mbed-like simplicity with STM32:

### A) Zephyr RTOS
Higher-level, consistent APIs across different MCUs:
```c
gpio_pin_set(...);
gpio_pin_toggle(...);
k_msleep(...);
```
Built on device tree model, abstracts hardware differences.

### B) Arduino for STM32 (stm32duino)
Official Arduino core for STM32:
```c
digitalWrite(LED_BUILTIN, HIGH);
pinMode(...);
delay(...);
```
Easiest transition from Arduino, still uses STM32Cube HAL underneath.

---

## Working with CubeMX Regeneration

### Critical Rule: Use `USER CODE` Blocks

**Problem:** When you regenerate code from CubeMX mid-development, it overwrites `main.c`, `stm32f4xx_it.c`, etc.

**Solution:** Only write code inside `USER CODE BEGIN/END` markers.

### Protected Sections in Generated Files

**Example in `main.c`:**
```c
/* USER CODE BEGIN Includes */
#include "app.h"  // ✅ Safe - won't be deleted
/* USER CODE END Includes */

/* Auto-generated includes here - DON'T TOUCH */

/* USER CODE BEGIN 0 */
void my_custom_function(void) {
    // ✅ Safe
}
/* USER CODE END 0 */

int main(void)
{
    HAL_Init();  // Auto-generated
    
    /* USER CODE BEGIN 2 */
    app_init();  // ✅ Safe
    /* USER CODE END 2 */

    while (1)
    {
        /* USER CODE BEGIN 3 */
        app_loop();  // ✅ Safe
        /* USER CODE END 3 */
    }
}
```

### Best Practices

1. **Never modify auto-generated code** outside `USER CODE` blocks
2. **Put application logic in `app.c`** - those files are never touched by CubeMX
3. **Use BSP files** - they're manually added, so CubeMX won't overwrite them
4. **Always check after regeneration** - verify your code is still present

### What Gets Regenerated

| File | Regenerated? | Protection |
|------|--------------|------------|
| `main.c` | ✅ Yes | Use `USER CODE` blocks |
| `main.h` | ✅ Yes | Use `USER CODE` blocks |
| `stm32f4xx_it.c` | ✅ Yes | Use `USER CODE` blocks |
| `stm32f4xx_hal_msp.c` | ✅ Yes | Use `USER CODE` blocks |
| `app.c` / `app.h` | ❌ No | Fully safe |
| BSP files | ❌ No | Fully safe |

---

## Flashing Your Device

### Standard Method

1. **Build** your project:
   - Click hammer icon 🔨, or
   - Press `Ctrl+B`

2. **Flash** to device:
   - Click **Run** icon ▶️ (normal execution), or
   - Click **Debug** icon 🐞 (with debugger)

### No Additional Configuration Needed

CubeIDE automatically:
- Detects ST-Link programmer
- Selects correct flash settings
- Programs and verifies

### Troubleshooting

**If flashing fails:**
1. Check USB cable connection
2. Verify ST-Link driver installed
3. Try: Project → Clean → Rebuild
4. Check Debug configuration matches your board

---

## Summary

### Key Takeaways

1. **Separate concerns**: Keep `main.c` minimal, put logic in `app.c`
2. **Use BSP layer**: Higher abstraction = cleaner code
3. **Protect your code**: Always use `USER CODE` blocks in generated files
4. **Consider frameworks**: Zephyr/Arduino offer Mbed-like simplicity
5. **HAL is powerful**: More verbose but gives full control

### Recommended Project Template

```
Core/
├── Inc/
│   ├── app.h                    (Your application API)
│   ├── stm32f4xx_nucleo.h       (BSP layer)
│   └── main.h                   (CubeMX-generated)
├── Src/
│   ├── app.c                    (Your application logic)
│   ├── stm32f4xx_nucleo.c       (BSP implementation)
│   └── main.c                   (Thin entry point)
```

**This structure:**
- ✅ Survives CubeMX regeneration
- ✅ Keeps code organized and readable
- ✅ Provides good abstraction layers
- ✅ Scales well to complex projects

---

## Additional Resources

- [STM32CubeIDE User Guide](https://www.st.com/resource/en/user_manual/um2609-stm32cubeide-user-guide-stmicroelectronics.pdf)
- [STM32 HAL Documentation](https://www.st.com/resource/en/user_manual/dm00105879-description-of-stm32f4-hal-and-ll-drivers-stmicroelectronics.pdf)
- [Zephyr RTOS](https://docs.zephyrproject.org/)
- [Arduino STM32](https://github.com/stm32duino/Arduino_Core_STM32)

---

**Happy Coding!** 🚀
