/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  This file contains declaration for variables and code
 *  that may be used to get the Intel Cpu identification
 *  that has been performed by checkCPUtypeSetCr0 function.
 */

/*
 * Copyright (c) 1998 Eric Valette <eric.valette@free.fr>
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

#ifndef libcpu_cpuModel_h
#define libcpu_cpuModel_h

/*
 * Tell us the machine setup..
 */

extern char hard_math;	/* floating point coprocessor present indicator */
extern char x86;	/* type of cpu (3 = 386, 4 =486, ...) */
extern char x86_model;
extern char x86_mask;
extern int x86_capability;       /* cpuid:EDX */
extern int x86_capability_x;     /* cpuid:ECX */
extern int x86_capability_ebx;   /* cpuid:EBX */
extern int x86_capability_cores; /* cpuid.(EAX=4, ECX=0) - physical cores */
extern char x86_vendor_id[13];
extern int have_cpuid;
extern unsigned char Cx86_step; /* cyrix processor identification */

/* Display this information on console in ascii form */
extern void printCpuInfo(void);

/* determine if the CPU has a TSC */
#define x86_has_tsc() \
  (x86_capability & (1 << 4))

static inline unsigned long long
rdtsc(void)
{
  /* Return the value of the on-chip cycle counter. */
  unsigned long long result;
  __asm__ volatile(".byte 0x0F, 0x31" : "=A" (result));
  return result;
} /* rdtsc */


#endif
