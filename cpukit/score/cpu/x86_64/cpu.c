/**
 *  @file
 *
 *  @brief x86_64 Dependent Source
 */

/*
 * Copyright (c) 2018 Amaan Cheval <amaan.cheval@gmail.com>
 *
 * Copyright (c) 1989-1999.
 * On-Line Applications Research Corporation (OAR).
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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/score/cpuimpl.h>
#include <rtems/score/idt.h>
#include <rtems/score/isr.h>
#include <rtems/score/tls.h>

#include <rtems/bspIo.h>

#include <inttypes.h>

static const char *const amd64_exception_names[] = {
  "divide error",
  "debug",
  "non-maskable interrupt",
  "breakpoint",
  "overflow",
  "bound range exceeded",
  "invalid opcode",
  "device not available",
  "double fault",
  "coprocessor segment overrun",
  "invalid TSS",
  "segment not present",
  "stack-segment fault",
  "general protection",
  "page fault",
  "reserved",
  "x87 floating-point error",
  "alignment check",
  "machine check",
  "SIMD floating-point error",
  "virtualization",
  "control protection",
  "reserved",
  "reserved",
  "reserved",
  "reserved",
  "reserved",
  "reserved",
  "hypervisor injection",
  "VMM communication",
  "security",
  "reserved"
};

void _CPU_Exception_frame_print(const CPU_Exception_frame *ctx)
{
  const char *name;

  if (ctx->vector < RTEMS_ARRAY_SIZE(amd64_exception_names)) {
    name = amd64_exception_names[ctx->vector];
  } else {
    name = "unknown";
  }

  printk(
    "exception vector: %" PRIu64 " (%s)\n"
    "error code: 0x%016" PRIx64 "\n"
    "RIP: 0x%016" PRIx64 " CS:  0x%016" PRIx64 "\n"
    "RFLAGS: 0x%016" PRIx64 " RSP: 0x%016" PRIx64 " SS: 0x%016" PRIx64 "\n"
    "RAX: 0x%016" PRIx64 " RBX: 0x%016" PRIx64 " RCX: 0x%016" PRIx64 "\n"
    "RDX: 0x%016" PRIx64 " RSI: 0x%016" PRIx64 " RDI: 0x%016" PRIx64 "\n"
    "RBP: 0x%016" PRIx64 " R8:  0x%016" PRIx64 " R9:  0x%016" PRIx64 "\n"
    "R10: 0x%016" PRIx64 " R11: 0x%016" PRIx64 " R12: 0x%016" PRIx64 "\n"
    "R13: 0x%016" PRIx64 " R14: 0x%016" PRIx64 " R15: 0x%016" PRIx64 "\n",
    ctx->vector, name,
    ctx->error_code,
    ctx->rip, ctx->cs,
    ctx->rflags, ctx->rsp, ctx->ss,
    ctx->rax, ctx->rbx, ctx->rcx,
    ctx->rdx, ctx->rsi, ctx->rdi,
    ctx->rbp, ctx->r8, ctx->r9,
    ctx->r10, ctx->r11, ctx->r12,
    ctx->r13, ctx->r14, ctx->r15
  );
}

Context_Control_fp _CPU_Null_fp_context;

void _CPU_Initialize(void)
{
 /*
  * Save the FP context intialized by the UEFI firmware in "_CPU_Null_fp_context"
  * which is given to each task at start and restart time.
  * According to the UEFI specification this should mean that:
  * _CPU_Null_fp_context.mxcsr = 0x1F80
  * _CPU_Null_fp_context.fpucw = 0x37F
  */
  asm volatile( "stmxcsr %0" : "=m"(_CPU_Null_fp_context.mxcsr) );
  asm volatile( "fstcw %0" : "=m"(_CPU_Null_fp_context.fpucw) );
}
