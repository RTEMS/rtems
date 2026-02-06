/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @brief U-Boot Support for qoriq bsp
 */

/*
 * COPYRIGHT (C) 2025, Ran Hong
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

#include <stdint.h>

#include <bsp.h>

#if defined(HAS_UBOOT)
/* Base address of U-Boot environment variables
 * The default base address 0xfff00000 is the starting address of the MRAM
 * of board mvme2500 according to its manual. MRAM is a type of nonvolatile
 * memory. 
 *
 * For other qoriq boards, users should update the base address accordingly.
 * User should export the U-Boot environment to this base address in U-Boot 
 * terminal before accessing it in RTEMS.
 */
const uint8_t *uboot_environment = (const uint8_t *) 0xfff00000;

/* Length of area reserved for U-Boot environment variables */
const size_t  uboot_environment_size = 0x20000;
#endif
