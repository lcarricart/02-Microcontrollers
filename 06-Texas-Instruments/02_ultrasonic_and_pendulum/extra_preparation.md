# Microcontrollers LAB2 - Extra Preparation

## Sleep Function
- Which of the GPT modes suits the application best? 
    - One shot
    - Periodic <--- periodic + match value is the compare mode
    - Capture ("stop-watch")
    - Compare ("egg timer")  <---
    - PLL

### What specific values do I want in the MCU app?
- PortB
- Pin 6
- Alternate function 7
- Timer 0A (T0CCP0)
- Prescaler?
- Load value?
- Match value?

I want to wait for X miliseconds
- My timer is 32 bits long
- Reference  = 120 MHz
    - Board crystal/reference source: 25 MHz
    - Default reset oscillator: usually 16 MHz PIOSC
    - Common configured system clock: 120 MHz
    - GPTM timer clock: usually SYSCLK

What's the maximum count I can make with this?
- 16 MHz --> 62,5ns
- 2^(32) * 62,5ns = 268,435 s
Using 16 bits counts a maximum of 4 miliseconds, and therefore we cannot make this timer better. We'd need a faster clock.

This is more than enough 
=> I dont need to make my clock slower
=> I dont need a prescaler

My function should deny values exceeding 268,435 s. It is possible but let's simplify this. Now the user can choose any value in between 0-MAX.
Let's imagine I want to count 3000 ms.
- Load value = 2^(32)

How many steps is 3000 ms?
- 62,5ns * nSteps = 3000 ms
I can change it too this in order to avoid insane floating points
- nSteps = 3000 ms * 16000
- I get the value of nSteps. match_value = load - nSteps. Then I convert this value to binary and store it inside the register.

