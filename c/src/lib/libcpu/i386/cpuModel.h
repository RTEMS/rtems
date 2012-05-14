/*  cpuModel.h
 *
 *  This file contains declaration for variables and code
 *  that may be used to get the Intel Cpu identification
 *  that has been performed by checkCPUtypeSetCr0 function.
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
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

extern void printCpuInfo(); /* Display this information on console in ascii form */

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
