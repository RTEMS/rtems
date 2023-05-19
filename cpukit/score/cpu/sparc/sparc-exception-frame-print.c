/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUSPARC
 *
 * @brief This source file contains the SPARC-specific implementation of
 *   _CPU_Exception_frame_print().
 */

/*
 * Copyright (C) 2021 embedded brains GmbH & Co. KG
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpu.h>
#include <rtems/bspIo.h>
#include <inttypes.h>

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  size_t i;
  size_t j;
  const char *desc;

  switch ( SPARC_REAL_TRAP_NUMBER( frame->trap ) ) {
    case 0x01:
      desc = " (instruction access exception)";
      break;
    case 0x02:
      desc = " (illegal instruction)";
      break;
    case 0x03:
      desc = " (privileged instruction)";
      break;
    case 0x04:
      desc = " (fp disabled)";
      break;
    case 0x05:
      desc = " (window overflow)";
      break;
    case 0x06:
      desc = " (window underflow)";
      break;
    case 0x07:
      desc = " (memory address not aligned)";
      break;
    case 0x08:
      desc = " (fp exception)";
      break;
    case 0x09:
      desc = " (data access exception)";
      break;
    case 0x0A:
      desc = " (tag overflow)";
      break;
    case 0x11:
    case 0x12:
    case 0x13:
    case 0x14:
    case 0x15:
    case 0x16:
    case 0x17:
    case 0x18:
    case 0x19:
    case 0x1A:
    case 0x1B:
    case 0x1C:
    case 0x1D:
    case 0x1E:
    case 0x1F:
      desc = " (external interrupt)";
      break;
    case 0x24:
      desc = " (cp disabled)";
      break;
    case 0x28:
      desc = " (cp exception)";
      break;
    default:
      desc = "";
      break;
  }

  printk(
    "\n"
    "unexpected trap %" PRIu32 "%s\n"
    "PSR = 0x%08" PRIx32 "\n"
    "PC = 0x%08" PRIx32 "\n"
    "nPC = 0x%08" PRIx32 "\n"
    "WIM = 0x%08" PRIx32 "\n"
    "Y = 0x%08" PRIx32 "\n",
    frame->trap,
    desc,
    frame->psr,
    frame->pc,
    frame->npc,
    frame->wim,
    frame->y
  );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( frame->global ); ++i ) {
    printk( "g%zu = 0x%08" PRIx32 "\n", i, frame->global[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( frame->output ); ++i ) {
    printk( "o%zu[CWP - 0] = 0x%08" PRIx32 "\n", i, frame->output[ i ] );
  }

  for ( i = 0; i < RTEMS_ARRAY_SIZE( frame->windows ); ++i ) {
    const SPARC_Register_window *win;

    win = &frame->windows[ i ];

    for ( j = 0; j < RTEMS_ARRAY_SIZE( win->local ); ++j ) {
      printk( "l%zu[CWP - %zu] = 0x%08" PRIx32 "\n", j, i, win->local[ j ] );
    }

    for ( j = 0; j < RTEMS_ARRAY_SIZE( win->input ); ++j ) {
      printk( "i%zu[CWP - %zu] = 0x%08" PRIx32 "\n", j, i, win->input[ j ] );
    }
  }

#if SPARC_HAS_FPU == 1
  printk( "FSR = 0x%08" PRIx32 "\n", frame->fsr );

  for ( i = 0; i < RTEMS_ARRAY_SIZE( frame->fp ); ++i ) {
    j = i * 2;
    printk( "fp%zu:fp%zu = 0x%016" PRIx64 "\n", j, j + 1, frame->fp[ i ] );
  }
#endif
}
