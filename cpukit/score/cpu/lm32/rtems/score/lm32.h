/*  lm32.h
 *
 *  This file sets up basic CPU dependency settings based on
 *  compiler settings.  For example, it can determine if
 *  floating point is available.  This particular implementation
 *  is specified to the NO CPU port.
 *
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Jukka Pietarinen <jukka.pietarinen@mrf.fi>, 2008,
 *  Micro-Research Finland Oy
 */

#ifndef _RTEMS_SCORE_LM32_H
#define _RTEMS_SCORE_LM32_H

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

#if defined(rtems_multilib)
/*
 *  Figure out all CPU Model Feature Flags based upon compiler
 *  predefines.
 */

#define CPU_MODEL_NAME  "rtems_multilib"
#define LM32_HAS_FPU     0

#elif defined(__lm32__)

#define CPU_MODEL_NAME  "lm32"
#define LM32_HAS_FPU     0

#else

#error "Unsupported CPU Model"

#endif

/*
 *  Define the name of the CPU family.
 */

#define CPU_NAME "LM32"

#ifdef __cplusplus
}
#endif

#define lm32_read_interrupts( _ip) \
  __asm__ volatile ("rcsr %0, ip":"=r"(_ip));

#define lm32_disable_interrupts( _level ) \
  do { register uint32_t ie; \
    __asm__ volatile ("rcsr %0,ie":"=r"(ie)); \
    (_level) = ie; \
    ie &= (~0x0001); \
    __asm__ volatile ("wcsr ie,%0"::"r"(ie)); \
  } while (0)

#define lm32_enable_interrupts( _level ) \
  __asm__ volatile ("wcsr ie,%0"::"r"(_level));

#define lm32_flash_interrupts( _level ) \
  do { register uint32_t ie; \
    __asm__ volatile ("wcsr ie,%0"::"r"(_level)); \
    ie = (_level) & (~0x0001); \
    __asm__ volatile ("wcsr ie,%0"::"r"(ie)); \
  } while (0)

#define lm32_interrupt_unmask( _mask ) \
  do { register uint32_t im; \
    __asm__ volatile ("rcsr %0,im":"=r"(im)); \
    im |= _mask; \
    __asm__ volatile ("wcsr im,%0"::"r"(im)); \
  } while (0)

#define lm32_interrupt_mask( _mask ) \
  do { register uint32_t im; \
    __asm__ volatile ("rcsr %0,im":"=r"(im)); \
    im &= ~(_mask); \
    __asm__ volatile ("wcsr im,%0"::"r"(im)); \
  } while (0)

#define lm32_interrupt_ack( _mask ) \
  do { register uint32_t ip = _mask; \
    __asm__ volatile ("wcsr ip,%0"::"r"(ip)); \
  } while (0)

#endif /* _RTEMS_SCORE_LM32_H */
