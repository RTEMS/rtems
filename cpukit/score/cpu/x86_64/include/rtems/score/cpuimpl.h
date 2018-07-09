/**
 * @file
 *
 * @brief CPU Port Implementation API
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
 *
 * Copyright (c) 2013, 2016 embedded brains GmbH
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_CPUIMPL_H
#define _RTEMS_SCORE_CPUIMPL_H

#include <rtems/score/cpu.h>

#define CPU_PER_CPU_CONTROL_SIZE 0

#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

static inline uint8_t inport_byte(uint16_t port)
{
  uint8_t ret;
  __asm__ volatile ( "inb %1, %0"
                     : "=a" (ret)
                     : "Nd" (port) );
  return ret;
}

static inline void outport_byte(uint16_t port, uint8_t val)
{
  __asm__ volatile ( "outb %0, %1" : : "a" (val), "Nd" (port) );
}

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif /* _RTEMS_SCORE_CPUIMPL_H */
