/*  cpu_asm.s
 *
 *  This file contains the basic algorithms for all assembly code used
 *  in an specific CPU port of RTEMS.  These algorithms must be implemented
 *  in assembly language.
 *
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Ported to ERC32 implementation of the SPARC by On-Line Applications
 *  Research Corporation (OAR) under contract to the European Space
 *  Agency (ESA).
 *
 *  ERC32 modifications of respective RTEMS file: COPYRIGHT (c) 1995.
 *  European Space Agency.
 */

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <rtems/asm.h>
#include <rtems/system.h>

#if (SPARC_HAS_FPU == 1)

/*
 *  void _CPU_Context_save_fp(
 *    void **fp_context_ptr
 *  )
 *
 *  This routine is responsible for saving the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  NOTE: See the README in this directory for information on the
 *        management of the "EF" bit in the PSR.
 */

        .align 4
        PUBLIC(_CPU_Context_save_fp)
SYM(_CPU_Context_save_fp):
        save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE, %sp

        /*
         *  The following enables the floating point unit.
         */

        mov     %psr, %l0
        sethi   %hi(SPARC_PSR_EF_MASK), %l1
        or      %l1, %lo(SPARC_PSR_EF_MASK), %l1
        or      %l0, %l1, %l0
        mov     %l0, %psr                  ! **** ENABLE FLOAT ACCESS ****
	nop; nop; nop;			   ! Need three nops before EF is
        ld      [%i0], %l0		   ! active due to pipeline delay!!!
        std     %f0, [%l0 + FO_F1_OFFSET]
        std     %f2, [%l0 + F2_F3_OFFSET]
        std     %f4, [%l0 + F4_F5_OFFSET]
        std     %f6, [%l0 + F6_F7_OFFSET]
        std     %f8, [%l0 + F8_F9_OFFSET]
        std     %f10, [%l0 + F1O_F11_OFFSET]
        std     %f12, [%l0 + F12_F13_OFFSET]
        std     %f14, [%l0 + F14_F15_OFFSET]
        std     %f16, [%l0 + F16_F17_OFFSET]
        std     %f18, [%l0 + F18_F19_OFFSET]
        std     %f20, [%l0 + F2O_F21_OFFSET]
        std     %f22, [%l0 + F22_F23_OFFSET]
        std     %f24, [%l0 + F24_F25_OFFSET]
        std     %f26, [%l0 + F26_F27_OFFSET]
        std     %f28, [%l0 + F28_F29_OFFSET]
        std     %f30, [%l0 + F3O_F31_OFFSET]
        st      %fsr, [%l0 + FSR_OFFSET]
        ret
        restore

/*
 *  void _CPU_Context_restore_fp(
 *    void **fp_context_ptr
 *  )
 *
 *  This routine is responsible for restoring the FP context
 *  at *fp_context_ptr.  If the point to load the FP context
 *  from is changed then the pointer is modified by this routine.
 *
 *  NOTE: See the README in this directory for information on the
 *        management of the "EF" bit in the PSR.
 */

        .align 4
        PUBLIC(_CPU_Context_restore_fp)
SYM(_CPU_Context_restore_fp):
        save    %sp, -CPU_MINIMUM_STACK_FRAME_SIZE , %sp

        /*
         *  The following enables the floating point unit.
         */

        mov     %psr, %l0
        sethi   %hi(SPARC_PSR_EF_MASK), %l1
        or      %l1, %lo(SPARC_PSR_EF_MASK), %l1
        or      %l0, %l1, %l0
        mov     %l0, %psr                  ! **** ENABLE FLOAT ACCESS ****
	nop; nop; nop;			   ! Need three nops before EF is
        ld      [%i0], %l0		   ! active due to pipeline delay!!!
        ldd     [%l0 + FO_F1_OFFSET], %f0
        ldd     [%l0 + F2_F3_OFFSET], %f2
        ldd     [%l0 + F4_F5_OFFSET], %f4
        ldd     [%l0 + F6_F7_OFFSET], %f6
        ldd     [%l0 + F8_F9_OFFSET], %f8
        ldd     [%l0 + F1O_F11_OFFSET], %f10
        ldd     [%l0 + F12_F13_OFFSET], %f12
        ldd     [%l0 + F14_F15_OFFSET], %f14
        ldd     [%l0 + F16_F17_OFFSET], %f16
        ldd     [%l0 + F18_F19_OFFSET], %f18
        ldd     [%l0 + F2O_F21_OFFSET], %f20
        ldd     [%l0 + F22_F23_OFFSET], %f22
        ldd     [%l0 + F24_F25_OFFSET], %f24
        ldd     [%l0 + F26_F27_OFFSET], %f26
        ldd     [%l0 + F28_F29_OFFSET], %f28
        ldd     [%l0 + F3O_F31_OFFSET], %f30
        ld      [%l0 + FSR_OFFSET], %fsr
        ret
        restore

#endif /* SPARC_HAS_FPU */

/* end of file */
