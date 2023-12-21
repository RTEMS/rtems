/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This header file provides interfaces of the hardware initialization
 *   support.
 */

/*
 * Copyright (C) 2022 Airbus U.S. Space & Defense, Inc
 * Copyright (C) 2014 Premysl Houdek <kom541000@gmail.com>
 *
 * Google Summer of Code 2014 at
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
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

#ifndef LIBBSP_ARM_TMS570_HWINIT_H
#define LIBBSP_ARM_TMS570_HWINIT_H

#include <bspopts.h>
#include <stdint.h>
#include <bsp/start.h>

static inline bool tms570_running_from_tcram( void )
{
  uintptr_t fncptr = (uintptr_t)bsp_start_hook_0;
  return (fncptr - TMS570_MEMORY_SRAM_ORIGIN) < TMS570_MEMORY_SRAM_SIZE;
}

static inline bool tms570_running_from_sdram( void )
{
  uintptr_t fncptr = (uintptr_t)bsp_start_hook_0;
  return (fncptr - TMS570_MEMORY_SDRAM_ORIGIN) < TMS570_MEMORY_SDRAM_SIZE;
}

/* Ti TMS570 core setup implemented in assembly */
void _esmCcmErrorsClear_( void );
void _coreEnableEventBusExport_( void );
void _errata_CORTEXR4_66_( void );
void _errata_CORTEXR4_57_( void );
void _coreEnableRamEcc_( void );
void _coreDisableRamEcc_( void );
void _mpuInit_( void );

void tms570_memory_init( uint32_t ram );
void tms570_trim_lpo_init( void );
void tms570_flash_init( void );
void tms570_periph_init( void );
void tms570_system_hw_init( void );
void tms570_esm_init( void );

/*
 * The following functions that must be implemented on a per-board basis for
 * any BSP variant with hardware initialization.  These configure MCU
 * peripherals that are specific to a particular board.
 */

/**
 * @brief Initialize the External Memory InterFace (EMIF) peripheral.
 */
void tms570_emif_sdram_init(void);

/**
 * @brief Initialize PLLs source divider/multipliers.
 */
void tms570_pll_init(void);

/**
 * @brief Initialize the tms570 Global Clock Manager (GCM) registers which
 *   sub-divide the input clock source (generally PLL) into the various
 *   peripheral clocks (VCLK1-3, etc).
 */
void tms570_map_clock_init(void);

/**
 * @brief Initialize the tms570 PINMUX peripheral. This maps signals to pin
 *   terminals.
 */
void tms570_pinmux_init(void);

#endif /* LIBBSP_ARM_TMS570_HWINIT_H */
