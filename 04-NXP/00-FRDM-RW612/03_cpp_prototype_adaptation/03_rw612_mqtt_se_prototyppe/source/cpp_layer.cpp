/******************************************************************************
 * @conversion
 * Press File > New > Other > C/C++ > Convert to a C/C++ Project > Select target project
 * This adds the correct compilers and linker configuration.
 *  - C++ Compiler will come with no include paths or symbols
 *  - C++ Linker will come with no linker flags and linker script configuration
 *
 * So if you need a specific include in your .cpp files, modify this accordingly. It is
 * possible to just copy paste all the paths and symbols from C to C++ compilers.
 ******************************************************************************
 * @integration
 *      - Check the arm-none-eabi-gcc toolchain. I will likely need the arm-none-eabi-g++
 *      - Why not make the header a .hpp file? That means "compile this file as C++ code"
 *        A wrapper <extern "C" { }> needs to be added to the .h file. main() needs a
 *        declaration that is C-valid. .hpp is possible when only C++ files include it.
 *      - <extern "C" { }> code cannot contain normal C++ things like classes, overloaded
 *        functions, templates, namespaces, etc.
 *
 * Proposed project structure:
 * SDK / board / drivers       --> stays C
 * MQTT connection code        --> mostly stays C initially
 * C bridge layer              --> exposes simple functions
 * C++ application layer       --> owns your logic/classes/state machines
 ******************************************************************************
 * @compiler/linker
 * The configuration should look like this
 *      .c files    → use arm-none-eabi-gcc (C compilation)
 *      .cpp files  → use arm-none-eabi-c++ (C++ compilation)
 *      final link  → preferably use arm-none-eabi-c++
 ******************************************************************************
 *
 */

#include "cpp_layer.h"

extern "C" {
#include "fsl_debug_console.h" /* PRINTF definition */
}

void test_function(void) {
  PRINTF("It's me, the C++ function! \r\n");
}
