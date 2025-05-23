#include <stdint.h>

// Define the base addresses for RCC and GPIOA
#define RCC_BASE        0x40023800
#define GPIOA_BASE      0x40020000

// Define offsets for the registers
#define RCC_AHB1ENR_OFFSET   0x30
#define GPIOA_MODER_OFFSET   0x00
#define GPIOA_ODR_OFFSET     0x14

// Define pointers to the registers
#define RCC_AHB1ENR  (*(volatile uint32_t *)(RCC_BASE + RCC_AHB1ENR_OFFSET))
#define GPIOA_MODER  (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_MODER_OFFSET))
#define GPIOA_ODR    (*(volatile uint32_t *)(GPIOA_BASE + GPIOA_ODR_OFFSET))

void delay(void) {
    for (volatile int i = 0; i < 1000000; i++) {
        // Do nothing, just waste some time
    }
}

void Reset_Handler(void);
extern unsigned long _estack;

// Vector table
__attribute__((section(".isr_vector")))
unsigned long *vector_table[] = {
    (unsigned long *) &_estack,
    (unsigned long *) Reset_Handler
};

void Reset_Handler(void) {
    // Enable the GPIOA clock
    RCC_AHB1ENR |= (1 << 0);

    // Set PA5 as output
    GPIOA_MODER |= (1 << 10);
    GPIOA_MODER &= ~(1 << 11);

    while (1) {
        // Toggle PA5
        GPIOA_ODR ^= (1 << 5);
        delay();
    }
}

int main(void) {
    while (1) {
        // Your main code here (never reached in this setup)
    }
}