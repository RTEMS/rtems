/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsARMTMS570
 *
 * @brief This source file contains the bsp_start_hook_1(0 implementation.
 */

/*
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

#include <bsp/start.h>
#include <libcpu/arm-cp15.h>

BSP_START_TEXT_SECTION void bsp_start_hook_1( void )
{
  uint32_t ctrl;
  size_t size;

  ctrl = arm_cp15_get_control();

  if ( ( ctrl & ARM_CP15_CTRL_I ) == 0 ) {
    rtems_cache_invalidate_entire_instruction();
    ctrl |= ARM_CP15_CTRL_I;
    arm_cp15_set_control(ctrl);
  }

  if ( ( ctrl & ARM_CP15_CTRL_C ) == 0 ) {
    rtems_cache_invalidate_entire_data();
    ctrl |= ARM_CP15_CTRL_C;
    arm_cp15_set_control(ctrl);
  }

  bsp_start_copy_sections_compact();
  bsp_start_clear_bss();

  size =(size_t) bsp_section_fast_text_size;
  RTEMS_OBFUSCATE_VARIABLE( size );

  if ( size != 0 ) {
    rtems_cache_flush_multiple_data_lines( bsp_section_fast_text_begin, size );
  }
}
