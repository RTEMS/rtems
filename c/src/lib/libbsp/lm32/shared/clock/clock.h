/*
 *  This file contains definitions for LatticeMico32 Timer (Clock)
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

#ifndef _BSPCLOCK_H
#define _BSPCLOCK_H

/* Status Register */

#define LM32_CLOCK_SR       (0x0000)
#define LM32_CLOCK_SR_TO    (0x0001)
#define LM32_CLOCK_SR_RUN   (0x0002)

/* Control Register */

#define LM32_CLOCK_CR       (0x0004)
#define LM32_CLOCK_CR_ITO   (0x0001)
#define LM32_CLOCK_CR_CONT  (0x0002)
#define LM32_CLOCK_CR_START (0x0004)
#define LM32_CLOCK_CR_STOP  (0x0008)

/* Period Register */

#define LM32_CLOCK_PERIOD   (0x0008)

/* Snapshot Register */

#define LM32_CLOCK_SNAPSHOT (0x000C)

#endif /* _BSPCLOCK_H */
