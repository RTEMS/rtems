/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSBSPsShared
 *
 * @brief This header file provides a minimal shim for Xilinx drivers.
 */

/*
 * Copyright (C) 2024 On-Line Applications Research Corporation (OAR)
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

#ifndef LIBBSP_SHARED_XIL_COMPAT_COMMON_H
#define LIBBSP_SHARED_XIL_COMPAT_COMMON_H
#include <stdint.h>
typedef uint8_t u8;
typedef int8_t s8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef int32_t s32;
typedef uint64_t u64;
typedef intptr_t INTPTR;
typedef uintptr_t UINTPTR;
#define INLINE inline

#define XST_SUCCESS 0
#define XST_FAILURE 1
#define XST_DEVICE_IS_STARTED 5
#define XST_DEVICE_BUSY 21
#define XST_FLASH_TIMEOUT_ERROR 1134
#define XST_SPI_TRANSFER_DONE 1152
#define XST_SPI_COMMAND_ERROR 1162
#define XST_SPI_POLL_DONE 1163

#include <rtems/score/assert.h>
#define Xil_AssertNonvoid(expr) _Assert(expr);
#define Xil_AssertVoid(expr) _Assert(expr);
#define Xil_AssertVoidAlways() _Assert(false);

#include <stdio.h>
#define xil_printf(args...) printf(args)

#define XIL_COMPONENT_IS_READY 0x1U

#include <string.h>
#define Xil_MemCpy(dest, src, count) memcpy(dest, src, count)

static inline uint32_t Xil_In32(uintptr_t addr)
{
  return *(volatile uint32_t *) addr;
}

static inline void Xil_Out32(uintptr_t addr, uint32_t value)
{
  *(volatile uint32_t *)addr = value;
}

#include <rtems/rtems/cache.h>
#define Xil_DCacheInvalidateRange(addr, len) \
  rtems_cache_flush_multiple_data_lines((void*)addr, len); \
  rtems_cache_invalidate_multiple_data_lines((void*)addr, len)
#define Xil_DCacheFlushRange(addr, len) \
  rtems_cache_flush_multiple_data_lines((void*)addr, len)

#include <unistd.h>
#include <bspopts.h>

#endif
