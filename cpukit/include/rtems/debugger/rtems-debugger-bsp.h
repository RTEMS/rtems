/*
 * Copyright (c) 2019 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 */

/*
 * Debugger BSP support.
 */

#ifndef _RTEMS_DEBUGGER_BSP_h
#define _RTEMS_DEBUGGER_BSP_h

#include <rtems/rtems-debugger.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/**
 * ARM
 */

/**
 * Debug registers
 *
 * Return the debugger registers for you BSP if the SoC requires memory mapped
 * registers and the DBGDRAR and DBGDSAR are not support or return 0. This
 * function is provided in the ARM backend and declared weak.
 */
void* rtems_debugger_arm_debug_registers(void);

/**
 * Configure the debug interface.
 *
 * Some ARM devices need special configurations to enable the debugging
 * hardware. The device are often locked down for securty reasons and
 * the debug hardware in the ARM needs to be enabled (asserting DBGEN).
 */
bool rtems_debugger_arm_debug_configure(void);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif
