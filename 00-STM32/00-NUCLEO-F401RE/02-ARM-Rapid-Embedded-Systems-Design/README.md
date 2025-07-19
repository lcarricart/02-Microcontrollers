# Environment Characteristics
The board being used is an STM32 NUCLEO-F401RE run with Mbed OS6.

## How to make run this files in your microcontroller

1. **Launch Mbed Studio**
   - Open the Mbed Studio IDE on your computer.

2. **Create or Open an Example Project**
   - Go to **File → New Program**.
   - Choose an example project that is configured for Mbed OS 6 and the NUCLEO-F401RE board.
   - This ensures that all necessary OS files and configurations are included.

3. **Integrate Your .cpp File**
   - In the project explorer, locate the `main.cpp` file.
   - Replace its content with your own `.cpp` code or add your file into the project’s source folder.

4. **Compile and Flash**
   - Ensure that the target board is set to **NUCLEO-F401RE** in Mbed Studio.
   - Click the **Compile** button (hammer icon) to build the project.
   - Once compiled, use the **Run/Flash** option to load the program onto your board.
