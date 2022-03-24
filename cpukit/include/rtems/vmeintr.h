/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 * 
 * @brief VMEbus Interface Library
 *
 * This file is the specification for the VMEbus interface library
 * which should be provided by all BSPs for VMEbus Single Board
 * Computers but currently only a few do so.
 */

/*
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
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

#ifndef _RTEMS_VMEINTR_H
#define _RTEMS_VMEINTR_H

#ifdef __cplusplus
extern "C" {
#endif

/*
 *  This defines the mask which is used to determine which
 *  interrupt levels are affected by a call to this package.
 *  The LSB corresponds to VME interrupt 0 and the MSB
 *  to VME interrupt 7.
 *
 */

typedef uint8_t   VME_interrupt_Mask;

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Disable (
  VME_interrupt_Mask                mask                        /* IN  */
);

/*
 *  VME_interrupt_Disable
 *
 */

void VME_interrupt_Enable (
  VME_interrupt_Mask                mask                        /* IN  */
);

#ifdef __cplusplus
}
#endif

#endif /* end of include file */
