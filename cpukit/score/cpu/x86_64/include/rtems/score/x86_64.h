/*
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
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

#ifndef _RTEMS_SCORE_X86_64_H
#define _RTEMS_SCORE_X86_64_H

#ifdef __cplusplus
extern "C" {
#endif

#define CPU_NAME "x86-64"
#define CPU_MODEL_NAME "amd64"

#define COM1_BASE_IO    0x3F8
#define COM1_CLOCK_RATE (115200 * 16)

#define EFLAGS_INTR_ENABLE 0x200

#define FSBASE_MSR 0xC0000100

#ifndef ASM
#ifdef DEBUG
#include <inttypes.h>
#include <rtems/bspIo.h>
#define DBG_PRINTF(...) printk(__VA_ARGS__)
#else
#define DBG_PRINTF(...)
#endif
#endif

#ifdef __cplusplus
}
#endif

#endif /* _RTEMS_SCORE_X86_64_H */
