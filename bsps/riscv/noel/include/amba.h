/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsRISCVGRLIBAMBA
 */

/*
 *  AMBA Plag & Play Bus Driver Macros
 *
 *  Macros used for AMBA Plug & Play bus scanning
 *
 *  COPYRIGHT (c) 2026.
 *  Frontgrade Gaisler AB
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

#ifndef __AMBA_H__
#define __AMBA_H__

/**
 * @defgroup RTEMSBSPsRISCVGRLIBAMBA RISC-V AMBA Driver Handler
 *
 * @ingroup RTEMSBSPsRISCVNOEL
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief AMBA Plag & Play Bus Driver Macros
 *
 * @{
 */

#ifndef GRLIB_IO_AREA
#define GRLIB_IO_AREA 0xfff00000
#endif

#define GRLIB_CONF_AREA           0xff000
#define GRLIB_AHB_SLAVE_CONF_AREA ( 1 << 11 )

#define GRLIB_AHB_CONF_WORDS 8
#define GRLIB_APB_CONF_WORDS 2
#define GRLIB_AHB_MASTERS    64
#define GRLIB_AHB_SLAVES     64
#define GRLIB_APB_SLAVES     16

#define GRLIB_GPTIMER_0_FREQUENCY_SET_BY_BOOT_LOADER 1000000

#include <grlib/ambapp.h>
#include <grlib/grlib.h>
#include <rtems/rtems/intr.h>

#ifdef __cplusplus
extern "C" {
#endif

extern uint32_t GRLIB_Cpu_Index;

extern struct ambapp_dev            *GRLIB_IrqCtrl_Adev;
extern volatile struct gptimer_regs *GRLIB_Timer_Regs;
extern struct ambapp_dev            *GRLIB_Timer_Adev;
void                                 gptimer_initialize( void );

static inline uint32_t grlib_up_counter_frequency( void )
{
  /*
   * For simplicity, assume that the interrupt controller uses the processor
   * clock.
   */
  return ambapp_freq_get( ambapp_plb(), GRLIB_IrqCtrl_Adev );
}

#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __AMBA_H__ */
