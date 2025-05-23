/* arm_acle.h - Compatibility Macros for ARM Compiler v6 */

#ifndef __ARM_ACLE_H
#define __ARM_ACLE_H

#define __isb(i) __builtin_arm_isb(i)
#define __dsb(i) __builtin_arm_dsb(i)
#define __dmb(i) __builtin_arm_dmb(i)

#endif /* __ARM_ACLE_H */
