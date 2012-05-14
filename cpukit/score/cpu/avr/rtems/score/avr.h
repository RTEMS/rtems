/**
 * @file rtems/score/avr.h
 */

/*
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the avr port.
 *
 *  COPYRIGHT 2004, Ralf Corsepius, Ulm, Germany.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */

#ifndef _RTEMS_SCORE_AVR_H
#define _RTEMS_SCORE_AVR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This file contains the information required to build
 *  RTEMS for a particular member of the NO CPU family.
 *  It does this by setting variables to indicate which
 *  implementation dependent features are present in a particular
 *  member of the family.
 *
 *  This is a good place to list all the known CPU models
 *  that this port supports and which RTEMS CPU model they correspond
 *  to.
 */

/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */
#if defined(__AVR__)

#if defined(__AVR_ARCH__)
#if   __AVR_ARCH__ == 1
#define CPU_MODEL_NAME  "avr1"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 2
#define CPU_MODEL_NAME  "avr2"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 3
#define CPU_MODEL_NAME  "avr3"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 4
#define CPU_MODEL_NAME  "avr4"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 5
#define CPU_MODEL_NAME  "avr5"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 25
#define CPU_MODEL_NAME  "avr25"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 31
#define CPU_MODEL_NAME  "avr31"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 35
#define CPU_MODEL_NAME  "avr35"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 51
#define CPU_MODEL_NAME  "avr51"
#define AVR_HAS_FPU     1

#elif __AVR_ARCH__ == 6
#define CPU_MODEL_NAME  "avr6"
#define AVR_HAS_FPU     1

#else
#error "Unsupported __AVR_ARCH__"
#endif
#else
#error "__AVR_ARCH__ undefined"
#endif

#else

#error "Unsupported CPU Model"

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "avr"

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_AVR_H */
