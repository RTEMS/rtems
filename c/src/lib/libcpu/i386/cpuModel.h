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
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

/*
 * Tell us the machine setup..
 */
#include <stdio.h>
#include <libcpu/cpu.h>
#include <string.h>

extern char hard_math;	/* flotting point coprocessor present indicator */
extern char x86;	/* type of cpu (3 = 386, 4 =486, ...) */
extern char x86_model;
extern char x86_mask;
extern int x86_capability;
extern char x86_vendor_id[13];
extern int have_cpuid;  
extern unsigned char Cx86_step; /* cyrix processor identification */

extern voidget_cpuinfo(); /* Display this information in ascii form */
