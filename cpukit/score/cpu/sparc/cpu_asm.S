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
 *  http://www.rtems.org/license/LICENSE.
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

#if (SPARC_HAS_FPU == 1) && !defined(SPARC_USE_SAFE_FP_SUPPORT)

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
        ld      [%o0], %o1
        std     %f0, [%o1 + FO_F1_OFFSET]
        std     %f2, [%o1 + F2_F3_OFFSET]
        std     %f4, [%o1 + F4_F5_OFFSET]
        std     %f6, [%o1 + F6_F7_OFFSET]
        std     %f8, [%o1 + F8_F9_OFFSET]
        std     %f10, [%o1 + F1O_F11_OFFSET]
        std     %f12, [%o1 + F12_F13_OFFSET]
        std     %f14, [%o1 + F14_F15_OFFSET]
        std     %f16, [%o1 + F16_F17_OFFSET]
        std     %f18, [%o1 + F18_F19_OFFSET]
        std     %f20, [%o1 + F2O_F21_OFFSET]
        std     %f22, [%o1 + F22_F23_OFFSET]
        std     %f24, [%o1 + F24_F25_OFFSET]
        std     %f26, [%o1 + F26_F27_OFFSET]
        std     %f28, [%o1 + F28_F29_OFFSET]
        std     %f30, [%o1 + F3O_F31_OFFSET]
        jmp     %o7 + 8
         st     %fsr, [%o1 + FSR_OFFSET]

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
        ld      [%o0], %o1
        ldd     [%o1 + FO_F1_OFFSET], %f0
        ldd     [%o1 + F2_F3_OFFSET], %f2
        ldd     [%o1 + F4_F5_OFFSET], %f4
        ldd     [%o1 + F6_F7_OFFSET], %f6
        ldd     [%o1 + F8_F9_OFFSET], %f8
        ldd     [%o1 + F1O_F11_OFFSET], %f10
        ldd     [%o1 + F12_F13_OFFSET], %f12
        ldd     [%o1 + F14_F15_OFFSET], %f14
        ldd     [%o1 + F16_F17_OFFSET], %f16
        ldd     [%o1 + F18_F19_OFFSET], %f18
        ldd     [%o1 + F2O_F21_OFFSET], %f20
        ldd     [%o1 + F22_F23_OFFSET], %f22
        ldd     [%o1 + F24_F25_OFFSET], %f24
        ldd     [%o1 + F26_F27_OFFSET], %f26
        ldd     [%o1 + F28_F29_OFFSET], %f28
        ldd     [%o1 + F3O_F31_OFFSET], %f30
        jmp     %o7 + 8
         ld     [%o1 + FSR_OFFSET], %fsr

#endif /* SPARC_HAS_FPU */

/* end of file */
