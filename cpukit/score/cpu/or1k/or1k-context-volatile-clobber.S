/*
 * COPYRIGHT (c) 2014 Hesham ALMatary <heshamelmatary@gmail.com>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifdef HAVE_CONFIG_H
  #include "config.h"
#endif

#include <rtems/asm.h>

.section .text

PUBLIC(_CPU_Context_volatile_clobber)
SYM(_CPU_Context_volatile_clobber):

.macro clobber_register reg
  l.addi r0, r0, -1
  l.addi  \reg, r0, 0
.endm

  clobber_register  r3
  clobber_register  r4
  clobber_register  r5
  clobber_register  r6
  clobber_register  r7
  clobber_register  r8
  clobber_register  r13
  clobber_register  r31

  l.jr  r9
  l.nop
