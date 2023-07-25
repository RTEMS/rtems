/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSScoreCPUAArch64
 *
 * @brief Implementation of _CPU_Exception_frame_print.
 *
 * This file implements _CPU_Exception_frame_print for use in fatal output.
 * It dumps all standard integer and floating point registers as well as some
 * of the more important system registers.
 */

/*
 * Copyright (C) 2020 On-Line Applications Research Corporation (OAR)
 * Written by Kinsey Moore <kinsey.moore@oarcorp.com>
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

#include <inttypes.h>

#include <rtems/score/cpu.h>
#include <rtems/dev/io.h>
#include <rtems/bspIo.h>

typedef struct {
	char *s;
	size_t n;
} String_Context;

static void _CPU_Put_char( int c, void *arg )
{
  String_Context *sctx = arg;
  size_t n = sctx->n;

  if (n > 0) {
    char *s = sctx->s;
    *s = (char) c;
    sctx->s = s + 1;
    sctx->n = n - 1;
  }
}

static void _CPU_Binary_sprintf(
  char *s,
  size_t maxlen,
  uint32_t num_bits,
  uint32_t value
)
{
  String_Context sctx;
  uint32_t mask;

  sctx.s = s;
  sctx.n = maxlen;

  mask = 1 << (num_bits - 1);

  while ( mask != 0 ) {
    _IO_Printf( _CPU_Put_char, &sctx, "%d", (value & mask ? 1 : 0) );
    mask >>= 1;
  }

  s[num_bits] = '\0';
}

static const char* _CPU_Exception_class_to_string( uint16_t exception_class )
{
  /* The 80 character limit is intentionally ignored for these strings. */
  switch ( exception_class ) {
    case 0x01:
      return "Trapped WFI or WFE instruction";
    case 0x03:
      return "Trapped MCR or MRC access with (coproc==0b1111)";
    case 0x04:
      return "Trapped MCRR or MRRC access with (coproc==0b1111)";
    case 0x05:
      return "Trapped MCR or MRC access with (coproc==0b1110)";
    case 0x06:
      return "Trapped LDC or STC access";
    case 0x0c:
      return "Trapped MRRC access with (coproc==0b1110)";
    case 0x0e:
      return "Illegal Execution state";
    case 0x18:
      return "Trapped MSR, MRS, or System instruction";
    case 0x20:
      return "Instruction Abort from a lower Exception level";
    case 0x21:
      return "Instruction Abort taken without a change in Exception level";
    case 0x22:
      return "PC alignment fault";
    case 0x24:
      return "Data Abort from a lower Exception level";
    case 0x25:
      return "Data Abort taken without a change in Exception level";
    case 0x26:
      return "SP alignment fault";
    case 0x30:
      return "Breakpoint exception from a lower Exception level";
    case 0x31:
      return "Breakpoint exception taken without a change in Exception level";
    case 0x32:
      return "Software Step exception from a lower Exception level";
    case 0x33:
      return "Software Step exception taken without a change in Exception level";
    case 0x34:
      return "Watchpoint exception from a lower Exception level";
    case 0x35:
      return "Watchpoint exception taken without a change in Exception level";
    case 0x38:
      return "BKPT instruction execution in AArch32 state";
    case 0x3c:
      return "BRK instruction execution in AArch64 state";
    default:
      return "Unknown";
  }
}

void _CPU_Exception_frame_print( const CPU_Exception_frame *frame )
{
  uint32_t ec;
  uint32_t il;
  uint32_t iss;
  char     ec_str[7];
  char     iss_str[26];
  int      i;
  const uint128_t *qx;

  printk(
    "\n"
    "X0   = 0x%016" PRIx64  " X17  = 0x%016" PRIx64 "\n"
    "X1   = 0x%016" PRIx64  " X18  = 0x%016" PRIx64 "\n"
    "X2   = 0x%016" PRIx64  " X19  = 0x%016" PRIx64 "\n"
    "X3   = 0x%016" PRIx64  " X20  = 0x%016" PRIx64 "\n"
    "X4   = 0x%016" PRIx64  " X21  = 0x%016" PRIx64 "\n"
    "X5   = 0x%016" PRIx64  " X22  = 0x%016" PRIx64 "\n"
    "X6   = 0x%016" PRIx64  " X23  = 0x%016" PRIx64 "\n"
    "X7   = 0x%016" PRIx64  " X24  = 0x%016" PRIx64 "\n"
    "X8   = 0x%016" PRIx64  " X25  = 0x%016" PRIx64 "\n"
    "X9   = 0x%016" PRIx64  " X26  = 0x%016" PRIx64 "\n"
    "X10  = 0x%016" PRIx64  " X27  = 0x%016" PRIx64 "\n"
    "X11  = 0x%016" PRIx64  " X28  = 0x%016" PRIx64 "\n"
    "X12  = 0x%016" PRIx64  " FP   = 0x%016" PRIx64 "\n"
    "X13  = 0x%016" PRIx64  " LR   = 0x%016" PRIxPTR "\n"
    "X14  = 0x%016" PRIx64  " SP   = 0x%016" PRIxPTR "\n"
    "X15  = 0x%016" PRIx64  " PC   = 0x%016" PRIxPTR "\n"
    "X16  = 0x%016" PRIx64  " DAIF = 0x%016" PRIx64 "\n"
    "VEC  = 0x%016" PRIxPTR " CPSR = 0x%016" PRIx64 "\n",
    frame->register_x0, frame->register_x17,
    frame->register_x1, frame->register_x18,
    frame->register_x2, frame->register_x19,
    frame->register_x3, frame->register_x20,
    frame->register_x4, frame->register_x21,
    frame->register_x5, frame->register_x22,
    frame->register_x6, frame->register_x23,
    frame->register_x7, frame->register_x24,
    frame->register_x8, frame->register_x25,
    frame->register_x9, frame->register_x26,
    frame->register_x10, frame->register_x27,
    frame->register_x11, frame->register_x28,
    frame->register_x12, frame->register_fp,
    frame->register_x13, (intptr_t) frame->register_lr,
    frame->register_x14, (intptr_t) frame->register_sp,
    frame->register_x15, (intptr_t) frame->register_pc,
    frame->register_x16, frame->register_daif,
    (intptr_t) frame->vector, frame->register_cpsr
  );

  ec = frame->register_syndrome >> 26 & 0x3f;
  il = frame->register_syndrome >> 25 & 0x1;
  iss = frame->register_syndrome & 0x1ffffff;

  _CPU_Binary_sprintf( ec_str, sizeof( ec_str ), sizeof( ec_str ) - 1, ec );
  _CPU_Binary_sprintf( iss_str, sizeof( iss_str ), sizeof( iss_str ) - 1, iss );

  printk(
    "ESR  = EC: 0b%s" " IL: 0b%d" " ISS: 0b%s" "\n"
    "       %s\n",
    ec_str, il, iss_str, _CPU_Exception_class_to_string( ec )
  );

  printk( "FAR  = 0x%016" PRIx64 "\n", frame->register_fault_address );

  qx = &frame->register_q0;

  printk(
    "FPCR = 0x%016" PRIx64 " FPSR = 0x%016" PRIx64 "\n",
    frame->register_fpcr, frame->register_fpsr
  );

  for ( i = 0 ; i < 32 ; ++i ) {
    uint64_t low = (uint64_t) qx[i];
    uint64_t high = (uint64_t) (qx[i] >> 64);

    printk( "Q%02i  = 0x%016" PRIx64 "%016" PRIx64 "\n", i, high, low );
  }
}
