/*  cpu_asm.c  ===> cpu_asm.S or cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language
 *
 *  NOTE:  This is supposed to be a .S or .s file NOT a C file.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  This file adapted from no_bsp board library of the RTEMS distribution.
 *  The body has been modified for the Bender Or1k implementation by
 *  Chris Ziomkowski. <chris@asics.ws>
 */

/*
 *  This is supposed to be an assembly file.  This means that system.h
 *  and cpu.h should not be included in a "real" cpu_asm file.  An
 *  implementation in assembly should include "cpu_asm.h>
 */

#include <rtems/system.h>
#include <rtems/score/cpu.h>
/* #include "cpu_asm.h> */

/*
 *  _CPU_Context_save_fp_context
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  Sometimes a macro implementation of this is in cpu.h which dereferences
 *  the ** and a similarly named routine in this file is passed something
 *  like a (Context_Control_fp *).  The general rule on making this decision
 *  is to avoid writing assembly language.
 *
 *  or1k specific Information:
 *
 *  This implementation of RTEMS considers the concept of
 *  "fast context switching", as defined in the or1k architecture
 *  specification. Whether or not this makes a significant
 *  impact on speed is dubious, however it is not a significant
 *  impediment to include it. It probably wastes a few cycles on
 *  every floating point context switch.
 *
 *  This implementation will currently not work on a processor where
 *  the integer unit and floating point unit are not the same size. I
 *  am waiting on an architecture change to make this feasible. It
 *  should work fine on 64 bit architectures, except for the fact that
 *  the variables are declared as 32 bits. This shouldn't really make
 *  a difference, as the fact that they must be registers should force
 *  them into a 64 bit word anyway.
 *
 *  The decision as to whether to do 32 or 64 bit saves is performed
 *  at run time based on the configuration of the CPUCFGR register. This
 *  takes a performance hit of a few cycles, but this should be a very
 *  small percentage of the total number of cycles necessary to do the
 *  save, and doesn't require special code for 32 or 64 bit versions.
 *
 *  ADDITIONAL INFORMATION:
 *
 *  It has been unanimously agreed that floating point will not be
 *  included in the initial releases of the Or1k chips, and that
 *  significant changes to the floating point architecture may
 *  occur before any such release will ever be implemented. The code
 *  below is therefore never called and never used.
 */

void _CPU_Context_save_fp(
  void **fp_context_ptr
)
{
  register unsigned32 temp;
  register unsigned32 address = (unsigned32)(*fp_context_ptr);
  register unsigned32 xfer;
  register unsigned32 loop;

  /* %0 is a temporary register which is used for several
     values throughout the code. %3 contains the address
     to save the context, and is modified during the course
     of the context save. %1 is a second dummy register
     which is used during transfer of the floating point
     value to memory. %2 is an end of loop marker which
     is compared against the pointer %3. */

  asm volatile ("l.mfspr  %0,r0,0x02    \n\t"  /* CPUCFGR */
	       "l.andi   %0,%0,0x380   \n\t"  /* OF32S or OV64S or OF64S */
	       "l.sfnei  %0,0x0        \n\t"
	       "l.bf     _L_nofps      \n\t"  /* exit if no floating point  */
	       "l.sfeqi  %0,0x080      \n\t"  /* (DELAY) single precision?  */
	       "l.mfspr  %0,r0,0x11    \n\t"  /* Load Status Register       */
	       "l.srli   %0,%0,58      \n\t"  /* Move CID into low byte*32  */
	       "l.bnf    _L_spfp_loops \n\t"  /* Branch on single precision */
	       "l.addi   %2,%0,0x20    \n"    /* Terminating condition      */
	       /**** Double Precision Floating Point Section ****/
	       "_L_dpfp_loops:         \n\t"
	       "l.mfspr  %1,%0,0x600   \n\t"  /* Load VFRx                  */
	       "l.sd     0(%3),%1      \n\t"  /* Save VFRx                  */
	       "l.addi   %0,%0,0x01    \n\t"  /* Increment counter          */
	       "l.sfeq   %0,%2         \n\t"  /* Branch if incomplete       */
	       "l.bf     _L_dpfp_loops \n\t"
	       "l.addi   %3,%3,0x08    \n\t"  /* (DELAY) update pointer     */
	       "l.bnf    _L_nofps      \n\t"  /* exit                       */
	       "l.nop                  \n"
	       /**** Single Precision Floating Point Section ****/
	       "_L_spfp_loops:         \n\t"
	       "l.mfspr  %1,%0,0x600   \n\t"  /* Load VFRx                  */
	       "l.sw     0(%3),%1      \n\t"  /* Save VFRx                  */
	       "l.addi   %0,%0,0x01    \n\t"  /* Increment counter          */
	       "l.sfeq   %0,%2         \n\t"  /* Branch if incomplete       */
	       "l.bf     _L_spfp_loops \n\t"
	       "l.addi   %3,%3,0x04    \n"    /* (DELAY) update pointer     */
	       "_L_nofps:              \n\t"  /* End of context save        */
		: "=&r" (temp), "=r" (xfer), "=&r" (loop), "+r" (address));
}

/*
 *  _CPU_Context_restore_fp_context
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  
 */

void _CPU_Context_restore_fp(
  void **fp_context_ptr
)
{
  register unsigned32 temp;
  register unsigned32 address = (unsigned32)(*fp_context_ptr);
  register unsigned32 xfer;
  register unsigned32 loop;

  /* The reverse of Context_save_fp */
  /* %0 is a temporary register which is used for several
     values throughout the code. %1 contains the address
     to save the context, and is modified during the course
     of the context save. %2 is a second dummy register
     which is used during transfer of the floating point
     value to memory. %3 is an end of loop marker which
     is compared against the pointer %1. */

  asm volatile ("l.mfspr  %0,r0,0x02    \n\t"  /* CPUCFGR */
	       "l.andi   %0,%0,0x380   \n\t"  /* OF32S or OV64S or OF64S */
	       "l.sfnei  %0,0x0        \n\t"
	       "l.bf     _L_nofpr      \n\t"  /* exit if no floating point  */
	       "l.sfeqi  %0,0x080      \n\t"  /* (DELAY) single precision?  */
	       "l.mfspr  %0,r0,0x11    \n\t"  /* Load Status Register       */
	       "l.srli   %0,%0,58      \n\t"  /* Move CID into low byte*32  */
	       "l.bnf    _L_spfp_loopr \n\t"  /* Branch on single precision */
	       "l.addi   %3,%0,0x20    \n"    /* Terminating condition      */
	       /**** Double Precision Floating Point Section ****/
	       "_L_dpfp_loopr:          \n\t"
	       "l.mfspr  %2,%0,0x600   \n\t"  /* Load VFRx                  */
	       "l.sd     0(%1),%2      \n\t"  /* Save VFRx                  */
	       "l.addi   %0,%0,0x01    \n\t"  /* Increment counter          */
	       "l.sfeq   %0,%3         \n\t"  /* Branch if incomplete       */
	       "l.bf     _L_dpfp_loopr \n\t"
	       "l.addi   %1,%1,0x08    \n\t"  /* (DELAY) update pointer     */
	       "l.bnf    _L_nofpr      \n\t"  /* exit                       */
	       "l.nop                  \n"
	       /**** Single Precision Floating Point Section ****/
	       "_L_spfp_loopr:         \n\t"
	       "l.mfspr  %2,%0,0x600   \n\t"  /* Load VFRx                  */
	       "l.sw     0(%1),%2      \n\t"  /* Save VFRx                  */
	       "l.addi   %0,%0,0x01    \n\t"  /* Increment counter          */
	       "l.sfeq   %0,%3         \n\t"  /* Branch if incomplete       */
	       "l.bf     _L_spfp_loopr \n\t"
	       "l.addi   %1,%1,0x04    \n"    /* (DELAY) update pointer     */
	       "_L_nofpr:              \n\t"  /* End of context save        */
	       : "=&r" (temp), "+r" (address), "=r" (xfer), "=&r" (loop));
}

/*  _CPU_Context_switch
 *
 *  This routine performs a normal non-FP context switch.
 *
 *  NO_CPU Specific Information:
 *
 *  XXX document implementation including references if appropriate
 */

void _CPU_Context_switch(
  Context_Control  *run,
  Context_Control  *heir
)
{
  register unsigned32 temp1 = 0;
  register unsigned32 temp2 = 0;

  /* This function is really tricky. When this function is called,
     we should save our state as we need it, and then grab the
     new state from the pointer. We then do a longjump to this
     code, replacing the current stack pointer with the new
     environment. This function never returns. Instead, at some
     later time, another person will call context switch with
     our pointer in the heir variable, and they will longjump
     to us. We will then continue. Let's see how this works... */

  /* Technically, we could probably not worry about saving r3
     and r4, since these are parameters guaranteed to be saved
     by the calling function. We could also probably get away
     without saving r11, as that is filled in by the return
     statement. But as a first cut I'm in favor of just saving
     everything.... */

  /* We could be more efficient and use compile time directives
     for 32 or 64 bit, but this will allow the code to run on
     everything without modification. Feel free to comment the
     relevant sections out if you don't need it. */

  /* We should probably write this whole routine in assembly
     so that we can have seperate entry points for self restore
     or context switch. You can't jump to local labels from
     inline assembly across function calls, and I don't feel
     like embedding all the .global directives here...it really
     screws up the debugger. Oh well, what's 2 more instructions
     and a branch really cost... */

  /* One thing which we should do is check for 32 or 64 bit models
     first, and then do one branch to the appropriate code section.
     Currently, we check the architecture bit in CPUCFGR twice. Once
     during the load section and again during restore. That is inefficient,
     and considering this code is huge anyway, saving the few bytes
     simply doesn't make any practical sense. FIX THIS LATER. */

  /* Note that this routine assumes software context switches are
     done with the same CID. In other words, it will not manage
     the CIDs and assign a new one as necessary. If you tell it
     to restore a context at CID 2, and the current one is at CID
     4, it will do what it is told. It will overwrite the registers
     for context ID 2, meaning they are irretrievably lost. I hope
     you saved them earlier.... */
 
  /* Note that you can have a context jump anywhere you want, although
     by default we will jump to the L_restore label. If you then modify
     the location in the Context_Control structure, it will continue
     whereever you told it to go. Note however that you had better
     also have cleaned up the stack and frame pointers though, because
     they are probably still set with the values obtained from
     entering this function... */

  asm volatile ("l.sfeqi   %3,0x0        \n\t"  /* Is this a self restore? */
	       "l.bf     _L_restore    \n\t"  /* Yes it is...go there */
	       "l.nop                  \n\t"

	       "l.lwz    %0,0(%3)      \n\t"  /* Prefetch new context */
               "l.mfspr  %2,r0,0x11    \n\t"  /* Status Register */
	       "l.sw     0(%1),%2      \n\t"  /* Save it */
	       "l.srli   %2,%2,28      \n\t"  /* Move CID into low byte */
	       "l.mfspr  %0,%2,0x20    \n\t"  /* Offset from EPCR */
	       "l.sw     4(%1),%0      \n\t"  /* Store it */
	       "l.mfspr  %0,%2,0x30    \n\t"  /* Offset from EEAR */
	       "l.sw     8(%1),%0      \n\t"  /* Store it */
	       "l.mfspr  %0,%2,0x40    \n\t"  /* Offset from ESR */
	       "l.sw     12(%1),%0     \n\t"  /* Store it */
               "l.mfspr  %0,r0,0x02    \n\t"  /* CPUCFGR */
	       "l.andi   %0,%0,0x40    \n\t"  /* OB64S */
	       "l.sfnei  %0,0x0        \n\t"
	       "l.bf     _L_64bit      \n\t"  /* 64 bit architecture */
	       "l.movhi  %0,hi(_L_restore)\n\t"

	       /****  32 bit implementation  ****/
	       "l.ori    %0,%0,lo(_L_restore)\n\t"
	       "l.sw     140(%1),%0    \n\t"   /* Save the PC */
	       "l.lwz    %0,140(%3)    \n\t"   /* New PC. Expect cache miss */
	       "l.sw     16(%1),r1     \n\t"
	       "l.sw     20(%1),r2     \n\t"
	       "l.sw     24(%1),r3     \n\t"
	       "l.sw     28(%1),r4     \n\t"
	       "l.sw     32(%1),r5     \n\t"
	       "l.sw     36(%1),r6     \n\t"
	       "l.sw     40(%1),r7     \n\t"
	       "l.sw     44(%1),r8     \n\t"
	       "l.sw     48(%1),r9     \n\t"
	       "l.sw     52(%1),r10    \n\t"
	       "l.sw     56(%1),r11    \n\t"
	       "l.sw     60(%1),r12    \n\t"
	       "l.sw     64(%1),r13    \n\t"
	       "l.sw     68(%1),r14    \n\t"
	       "l.sw     72(%1),r15    \n\t"
	       "l.sw     76(%1),r16    \n\t"
	       "l.sw     80(%1),r17    \n\t"
	       "l.sw     84(%1),r18    \n\t"
	       "l.sw     88(%1),r19    \n\t"
	       "l.sw     92(%1),r20    \n\t"
	       "l.sw     96(%1),r21    \n\t"
	       "l.sw     100(%1),r22   \n\t"
	       "l.sw     104(%1),r23   \n\t"
	       "l.sw     108(%1),r24   \n\t"
	       "l.sw     112(%1),r25   \n\t"
	       "l.sw     116(%1),r26   \n\t"
	       "l.sw     120(%1),r27   \n\t"
	       "l.sw     124(%1),r28   \n\t"
	       "l.sw     128(%1),r29   \n\t"
	       "l.sw     132(%1),r30   \n\t"
	       "l.jr     %0            \n\t"  /* Go there */
	       "l.sw     136(%1),r31   \n"    /* Store the last reg */

	       /**** 64 bit implementation ****/
	       "_L_64bit:              \n\t"
	       "l.ori    %0,%0,lo(_L_restore)\n\t"
	       "l.sw     264(%1),%0    \n\t"
	       "l.sd     16(%1),r1     \n\t"
	       "l.sd     24(%1),r2     \n\t"
	       "l.sd     32(%1),r3     \n\t"
	       "l.sd     40(%1),r4     \n\t"
	       "l.sd     48(%1),r5     \n\t"
	       "l.sd     56(%1),r6     \n\t"
	       "l.sd     64(%1),r7     \n\t"
	       "l.sd     72(%1),r8     \n\t"
	       "l.sd     80(%1),r9     \n\t"
	       "l.sd     88(%1),r10    \n\t"
	       "l.sd     96(%1),r11    \n\t"
	       "l.sd     104(%1),r12   \n\t"
	       "l.sd     112(%1),r13   \n\t"
	       "l.sd     120(%1),r14   \n\t"
	       "l.sd     128(%1),r15   \n\t"
	       "l.sd     136(%1),r16   \n\t"
	       "l.sd     144(%1),r17   \n\t"
	       "l.sd     152(%1),r18   \n\t"
	       "l.sd     160(%1),r19   \n\t"
	       "l.sd     168(%1),r20   \n\t"
	       "l.sd     176(%1),r21   \n\t"
	       "l.sd     184(%1),r22   \n\t"
	       "l.sd     192(%1),r23   \n\t"
	       "l.sd     200(%1),r24   \n\t"
	       "l.sd     208(%1),r25   \n\t"
	       "l.sd     216(%1),r26   \n\t"
	       "l.sd     224(%1),r27   \n\t"
	       "l.sd     232(%1),r28   \n\t"
	       "l.sd     240(%1),r29   \n\t"
	       "l.sd     248(%1),r30   \n\t"
	       "l.jr     %0            \n\t"  /* Go to the new PC */
	       "l.sd     256(%1),r31   \n"    /* Store the last reg */

	       /**** The restoration routine. ****/
	       
	       /* Note that when we return from this function,
		  we will actually be returning to a different
		  context than when we left. The debugger might
		  have conniptions over this, but we'll have to
		  reengineer that later. The stack and status
		  registers will all be changed, however we
		  will not touch the global interrupt mask. */

	       /* Also note, when doing any restore, the most
		  important registers are r1, r2, and r9. These
		  will be accessed immediately upon exiting the
		  routine, and so we want to make sure we load
		  them as early as possible in case they are 
		  not in cache */

	       "_L_restore:            \n\t"  /* Restore "heir" */
               "l.mfspr  %2,r0,0x11    \n\t"  /* Status Register */
	       "l.movhi  %0,0x07FF     \n\t"  /* ~SR mask  */
	       "l.ori    %0,%0,0xD1FF  \n\t"
	       "l.and    %2,%0,%2      \n\t"  /* save the global bits */
	       "l.movhi  %0,0xF800     \n\t"  /* SR mask  */
	       "l.ori    %0,%0,0x2E00  \n\t"
	       "l.lwz    %1,0(%3)      \n\t"  /* Get the previous SR */
	       "l.and    %0,%1,%0      \n\t"  /* Mask out the global bits */
	       "l.or     %2,%2,%0      \n\t"  /* Combine local/global */
	       "l.mtspr  r0,%2,0x11    \n\t"  /* Restore the status register */

               "l.mfspr  %0,r0,0x02    \n\t"  /* CPUCFGR */
	       "l.andi   %0,%0,0x40    \n\t"  /* OB64S */
	       "l.sfnei  %0,0x0        \n\t"  /* Save the 64 bit flag */

	       "l.srli   %2,%2,28      \n\t"  /* Move CID into low byte */
	       "l.lwz    %0,4(%3)      \n\t"
	       "l.mtspr  %2,%0,0x20    \n\t"  /* Offset from EPCR */
	       "l.lwz    %0,8(%3)      \n\t"
	       "l.mtspr  %2,%0,0x30    \n\t"  /* Offset from EEAR */
	       "l.lwz    %0,12(%3)     \n\t"

	       "l.bf     _L_r64bit     \n\t"  /* 64 bit architecture */
	       "l.mtspr  %2,%0,0x30    \n\t"  /* Offset from EEAR (DELAY) */

	       /**** 32 bit restore ****/
	       "l.lwz   r1,16(%3)      \n\t"
	       "l.lwz   r2,20(%3)      \n\t"
	       "l.lwz   r9,48(%3)      \n\t"
	       "l.lwz   r3,24(%3)      \n\t"
	       "l.lwz   r4,28(%3)      \n\t"
	       "l.lwz   r5,32(%3)      \n\t"
	       "l.lwz   r6,36(%3)      \n\t"
	       "l.lwz   r7,40(%3)      \n\t"
	       "l.lwz   r8,44(%3)      \n\t"
	       "l.lwz   r10,52(%3)     \n\t"
	       "l.lwz   r11,56(%3)     \n\t"
	       "l.lwz   r12,60(%3)     \n\t"
	       "l.lwz   r13,64(%3)     \n\t"
	       "l.lwz   r14,68(%3)     \n\t"
	       "l.lwz   r15,72(%3)     \n\t"
	       "l.lwz   r16,76(%3)     \n\t"
	       "l.lwz   r17,80(%3)     \n\t"
	       "l.lwz   r18,84(%3)     \n\t"
	       "l.lwz   r19,88(%3)     \n\t"
	       "l.lwz   r20,92(%3)     \n\t"
	       "l.lwz   r21,96(%3)     \n\t"
	       "l.lwz   r22,100(%3)    \n\t"
	       "l.lwz   r23,104(%3)    \n\t"
	       "l.lwz   r24,108(%3)    \n\t"
	       "l.lwz   r25,112(%3)    \n\t"
	       "l.lwz   r26,116(%3)    \n\t"
	       "l.lwz   r27,120(%3)    \n\t"
	       "l.lwz   r28,124(%3)    \n\t"
	       "l.lwz   r29,128(%3)    \n\t"
	       "l.lwz   r30,132(%3)    \n\t"
	       "l.j     _L_return      \n\t"
	       "l.lwz   r31,136(%3)    \n"

	       /****  64 bit restore ****/
	       "_L_r64bit:             \n\t"
	       "l.ld    r1,16(%3)      \n\t"
	       "l.ld    r2,24(%3)      \n\t"
	       "l.ld   r9,80(%3)       \n\t"
	       "l.ld   r3,32(%3)       \n\t"
	       "l.ld   r4,40(%3)       \n\t"
	       "l.ld   r5,48(%3)       \n\t"
	       "l.ld   r6,56(%3)       \n\t"
	       "l.ld   r7,64(%3)       \n\t"
	       "l.ld   r8,72(%3)       \n\t"
	       "l.ld   r10,88(%3)      \n\t"
	       "l.ld   r11,96(%3)      \n\t"
	       "l.ld   r12,104(%3)     \n\t"
	       "l.ld   r13,112(%3)     \n\t"
	       "l.ld   r14,120(%3)     \n\t"
	       "l.ld   r15,128(%3)     \n\t"
	       "l.ld   r16,136(%3)     \n\t"
	       "l.ld   r17,144(%3)     \n\t"
	       "l.ld   r18,152(%3)     \n\t"
	       "l.ld   r19,160(%3)     \n\t"
	       "l.ld   r20,168(%3)     \n\t"
	       "l.ld   r21,176(%3)     \n\t"
	       "l.ld   r22,184(%3)     \n\t"
	       "l.ld   r23,192(%3)     \n\t"
	       "l.ld   r24,200(%3)     \n\t"
	       "l.ld   r25,208(%3)     \n\t"
	       "l.ld   r26,216(%3)     \n\t"
	       "l.ld   r27,224(%3)     \n\t"
	       "l.ld   r28,232(%3)     \n\t"
	       "l.ld   r29,240(%3)     \n\t"
	       "l.ld   r30,248(%3)     \n\t"
	       "l.ld   r31,256(%3)     \n"

	       "_L_return:             \n\t"  /* End of routine */
	       
		: "=&r" (temp1), "+r" (run), "=&r" (temp2)
		: "r" (heir));
  
  /* Note that some registers were used for parameter passing and
     temporary registeres (temp1 and temp2). These values were
     saved and restored across context calls, but the values that
     the caller needs should have been stored on the stack. The
     C code should now restore these from the stack, since r1 and
     r2 have been restored, and return to the location specified
     by r9. Then, all should be happy in the world. */
}

/*
 *  _CPU_Context_restore
 *
 *  This routine is generally used only to restart self in an
 *  efficient manner.  It may simply be a label in _CPU_Context_switch.
 *
 *  NOTE: May be unnecessary to reload some registers.
 *
 *  Or1k Specific Information:
 *
 *  In our implementation, this simply redirects to swich context
 */

void _CPU_Context_restore(
  Context_Control  *run
)
{
  _CPU_Context_switch(run,NULL);
}


/*  void __ISR_Handler()
 *
 *  This routine provides the RTEMS interrupt management.
 *
 *  Or1k Specific Information:
 *
 *  Based on the Or1k interrupt architecture described in chapter 16
 *  and the exception architecture described in chapter 9
 */

void _ISR_Handler(unsigned32 vector,unsigned32 ProgramCounter,
		  unsigned32 EffectiveAddress,unsigned32 StatusRegister)
{
   /*
    *  This discussion ignores a lot of the ugly details in a real
    *  implementation such as saving enough registers/state to be
    *  able to do something real.  Keep in mind that the goal is
    *  to invoke a user's ISR handler which is written in C and
    *  uses a certain set of registers.
    *
    *  Also note that the exact order is to a large extent flexible.
    *  Hardware will dictate a sequence for a certain subset of
    *  _ISR_Handler while requirements for setting
    */

  /*
   *  At entry to "common" _ISR_Handler, the vector number must be
   *  available.  On some CPUs the hardware puts either the vector
   *  number or the offset into the vector table for this ISR in a
   *  known place.  If the hardware does not give us this information,
   *  then the assembly portion of RTEMS for this port will contain
   *  a set of distinct interrupt entry points which somehow place
   *  the vector number in a known place (which is safe if another
   *  interrupt nests this one) and branches to _ISR_Handler.
   *
   *  save some or all context on stack
   *  may need to save some special interrupt information for exit
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    if ( _ISR_Nest_level == 0 )
   *      switch to software interrupt stack
   *  #endif
   *
   *  _ISR_Nest_level++;
   *
   *  _Thread_Dispatch_disable_level++;
   *
   *  (*_ISR_Vector_table[ vector ])( vector );
   *
   *  --_ISR_Nest_level;
   *
   *  if ( _ISR_Nest_level )
   *    goto the label "exit interrupt (simple case)"
   *
   *  #if ( CPU_HAS_SOFTWARE_INTERRUPT_STACK == TRUE )
   *    restore stack
   *  #endif
   *  
   *  if ( !_Context_Switch_necessary )
   *    goto the label "exit interrupt (simple case)"
   *  
   *  if ( !_ISR_Signals_to_thread_executing )
   *    _ISR_Signals_to_thread_executing = FALSE;
   *    goto the label "exit interrupt (simple case)"
   *
   *  call _Thread_Dispatch() or prepare to return to _ISR_Dispatch
   *
   *  prepare to get out of interrupt
   *  return from interrupt  (maybe to _ISR_Dispatch)
   *
   *  LABEL "exit interrupt (simple case):
   *  prepare to get out of interrupt
   *  return from interrupt
   */

  /* In the Or1k architecture, exceptions are handled in the
     startup code of the board support package. Thus, this
     routine is never called. Or1k exception routines are called
     with the following prototype:

     function(int vector#, int PC, int Address, int StatusRegister);

     These parameters are snapshots of the system when the exception
     was encountered. If virtual memory is active, things like the
     PC and Address may have little meaning, as they are referenced
     in physical space, not the virtual space of the process.
  */
}

