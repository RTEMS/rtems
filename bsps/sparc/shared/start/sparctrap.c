/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file leontrap.c
 * @ingroup RTEMSBSPsSPARCERC32
 * @ingroup RTEMSBSPsLEON2
 * @ingroup RTEMSBSPsLEON3
 * @ingroup RTEMSBSPsSPARCShared
 * @brief Clears and unmasks a SPARC interrupt on LEON.
 */

/*
 * Copyright (c) 2025 On-Line Applications Research Corporation (OAR)
 * Copyright (c) 2025 Sunil Hegde <sunil1hegde@gmail.com>
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

void SPARC_Clear_and_unmask_interrupt(rtems_vector_number vector)
{
  if (SPARC_IS_INTERRUPT_TRAP(vector)) {
    uint32_t source = SPARC_INTERRUPT_TRAP_TO_SOURCE(vector);
    BSP_Clear_interrupt(source);
    BSP_Unmask_interrupt(source);
  }
}

