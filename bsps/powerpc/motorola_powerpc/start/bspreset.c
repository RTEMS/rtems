/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsPowerPCMotorola
 *
 * @brief Motorola PowerPC Reset Support
 */

/*
 * Copyright (C) 2026 UChicago Argonne LLC,
 * as operator of Argonne National Laboratory
 * Author: Vijay Banerjee <vijay@rtems.org>
 *
 * Copyright (C) 2004 On-Line Applications Research Corporation (OAR).
 * Copyright (C) 2001 Till Straumann <strauman@slac.stanford.edu>
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT OWNER OR CONTRIBUTORS BE
 * LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
 * CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
 * SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
 * INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
 * CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
 * ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
 * POSSIBILITY OF SUCH DAMAGE.
 */

#include <bsp.h>
#include <rtems/bspIo.h>
#include <libcpu/stackTrace.h>
#include <bsp/bootcard.h>

/*-------------------------------------------------------------------------+
|         Function: bsp_reset
|      Description: Reboot the PC.
| Global Variables: None.
|        Arguments: None.
|          Returns: Nothing.
+--------------------------------------------------------------------------*/

void bsp_reset( rtems_fatal_source source, rtems_fatal_code code )
{
  (void) source;
  (void) code;

  printk("Printing a stack trace for your convenience :-)\n");
  CPU_print_stack();
  /* shutdown and reboot */

#if defined(mot_ppc_mvme2100)
  out_8((volatile uint8_t *)0xffe00000, 0x80);
#elif defined(mot_ppc_mvme5100)
  out_8((volatile uint8_t *)0xfef880a0, 0x1);
#else
   /* Memory-mapped Port 92 PIB device access
   *(unsigned char*)0x80000092 |= 0x01;
   */
   outb(1, 0x92);

#endif
   RTEMS_UNREACHABLE();
} /* bsp_reset */
