/*
 * Cirrus EP7312 Intererrupt handler
 *
 * Copyright (c) 2010 embedded brains GmbH.
 *
 * Copyright (c) 2002 by Jay Monkman <jtm@smoothsmoothie.com>
 *
 * Copyright (c) 2002 by Charlie Steader <charlies@poliac.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
*/

#ifndef __IRQ_H__
#define __IRQ_H__

#ifndef __asm__

#include <rtems.h>
#include <rtems/irq.h>
#include <rtems/irq-extension.h>

#endif /* __asm__ */

  /* int interrupt status/mask register 1 */
#define BSP_EXTFIQ     0
#define BSP_BLINT      1
#define BSP_WEINT      2
#define BSP_MCINT      3
#define BSP_CSINT      4
#define BSP_EINT1      5
#define BSP_EINT2      6
#define BSP_EINT3      7
#define BSP_TC1OI      8
#define BSP_TC2OI      9
#define BSP_RTCMI     10
#define BSP_TINT      11
#define BSP_UTXINT1   12
#define BSP_URXINT1   13
#define BSP_UMSINT    14
#define BSP_SSEOTI    15
 /* int interrupt status/mask register 2 */
#define BSP_KBDINT    16
#define BSP_SS2RX     17
#define BSP_SS2TX     18
#define BSP_UTXINT2   19
#define BSP_URXINT2   20
 /* int interrupt status/mask register 3 */
#define BSP_DAIINT    21
#define BSP_MAX_INT   22

#define BSP_INTERRUPT_VECTOR_MIN 0

#define BSP_INTERRUPT_VECTOR_MAX (BSP_MAX_INT - 1)

#endif /* __IRQ_H__ */
