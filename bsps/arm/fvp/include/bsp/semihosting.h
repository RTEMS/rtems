/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMFVP
 *
 * @brief This header file provides the semihosting API.
 */

/*
 * Copyright (C) 2020 embedded brains GmbH (http://www.embedded-brains.de)
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

#ifndef _BSPS_ARM_FVP_SEMIHOSTING_H
#define _BSPS_ARM_FVP_SEMIHOSTING_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @addtogroup RTEMSBSPsARMFVP
 *
 * @{
 */

#define SYS_CLOCK 0x10
#define SYS_CLOSE 0x02
#define SYS_ELAPSED 0x30
#define SYS_ERRNO 0x13
#define SYS_EXIT 0x18
#define SYS_EXIT_EXTENDED 0x20
#define SYS_FLEN 0x0c
#define SYS_GET_CMDLINE 0x15
#define SYS_HEAPINFO 0x16
#define SYS_ISERROR 0x08
#define SYS_ISTTY 0x09
#define SYS_OPEN 0x01
#define SYS_READ 0x06
#define SYS_READC 0x07
#define SYS_REMOVE 0x0e
#define SYS_RENAME 0x0f
#define SYS_SEEK 0x0a
#define SYS_SYSTEM 0x12
#define SYS_TICKFREQ 0x31
#define SYS_TIME 0x11
#define SYS_TMPNAM 0x0d
#define SYS_WRITE 0x05
#define SYS_WRITEC 0x03
#define SYS_WRITE0 0x04

#define ADP_Stopped_BranchThroughZero 0x20000
#define ADP_Stopped_UndefinedInstr 0x20001
#define ADP_Stopped_SoftwareInterrupt 0x20002
#define ADP_Stopped_PrefetchAbort 0x20003
#define ADP_Stopped_DataAbort 0x20004
#define ADP_Stopped_AddressException 0x20005
#define ADP_Stopped_IRQ 0x20006
#define ADP_Stopped_FIQ 0x20007
#define ADP_Stopped_BreakPoint 0x20020
#define ADP_Stopped_WatchPoint 0x20021
#define ADP_Stopped_StepComplete 0x20022
#define ADP_Stopped_RunTimeErrorUnknown 0x20023
#define ADP_Stopped_InternalError 0x20024
#define ADP_Stopped_UserInterruption 0x20025
#define ADP_Stopped_ApplicationExit 0x20026
#define ADP_Stopped_StackOverflow 0x20027
#define ADP_Stopped_DivisionByZero 0x20028
#define ADP_Stopped_OSSpecific 0x20029

static inline uintptr_t arm_fvp_semihosting_call(
  uintptr_t op,
  uintptr_t params
)
{
  register uintptr_t op_and_return_reg __asm__("r0");
  register uintptr_t params_reg __asm__("r1");

  op_and_return_reg = op;
  params_reg = params;

  __asm__ volatile (
#ifdef __thumb__
    "svc #0xab"
#else
    "svc #0x123456"
#endif
    : "=r" (op_and_return_reg)
    : "0" (op_and_return_reg), "r" (params_reg)
    : "memory"
  );

  return op_and_return_reg;
}

void arm_fvp_console_output( char c );

int arm_fvp_console_input( void );

/** @} */

#ifdef __cplusplus
}
#endif

#endif /* _BSPS_ARM_FVP_SEMIHOSTING_H */
