/*
 *  zero the various bss areas.
 *
 *  COPYRIGHT (c) 1989-1998.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modified to support the MCP750.
 *  Modifications Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 */

#include <string.h>

/* prevent these from being accessed in the short data areas */
extern unsigned long __bss_start[], __SBSS_START__[], __SBSS_END__[];
extern unsigned long __SBSS2_START__[], __SBSS2_END__[];
extern unsigned long __bss_end[];

void zero_bss(void)
{
  memset(__SBSS_START__, 0, ((unsigned) __SBSS_END__) - ((unsigned)__SBSS_START__));
  memset(__SBSS2_START__, 0, ((unsigned) __SBSS2_END__) - ((unsigned)__SBSS2_START__));
  memset(__bss_start, 0, ((unsigned) __bss_end) - ((unsigned)__bss_start));
}
