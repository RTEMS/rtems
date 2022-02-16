/* SPDX-License-Identifier: BSD-2-Clause */

/*
 *  systrap.S
 *
 *  This file contains emulated system calls using software trap 0.
 *  The following calls are supported:
 *
 *    + SYS_exit  (halt)
 *    + SYS_irqdis  (disable interrupts)
 *    + SYS_irqset  (set interrupt level)
 * 
 *  COPYRIGHT (c) 2010. Gedare Bloom.
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

#include <rtems/asm.h>
#include "sparc64-syscall.h"


.seg    "text"
/*
 *  system call
 *
 *  On entry:
 *      g4[AG | GL=1] = tstate (from trap table)
 *      g2[AG | GL=1] = trap vector # (256)
 *      g3[AG | GL=1] = address of SYM(syscall)
 *      g4[AG | GL-1] = system call id
 *    if arch = sun4v:
 *      We need to back to GL-1 to read the system call id. 
 *   on sun4u:
 *      We need to go back to the normal globals to read the system call id.
 *    
 *    First thing is to return to the previous set of globals, so 
 *    that the system call id can be read.  The syscall code needs 
 *    to re-read tstate.
 *
 *    syscall should only ever be entered by ta 0 being called explicitly
 *    by a function that knows what is happening.  This means the syscall
 *    code can safely use any scratch registers and the %o registers.
 */


PUBLIC(syscall)

  
  SYM(syscall):
    mov   %g0, %g4  ! clear %g4 at this GL
#if defined (SUN4U)
    rdpr  %pstate, %g1
    andn  %g1, SPARC_PSTATE_AG_MASK, %g1
    wrpr  %g1, %g0, %pstate                 ! go to regular globals
#elif defined (SUN4V)
    rdpr  %gl, %g1
    dec   %g1
    wrpr  %g0, %g1, %gl                     ! go back to GL = GL - 1
#endif

    subcc %g4, 2, %g0                     
    bne   3f
    rdpr  %tstate, %g5                      ! re-read tstate, use delay slot

    ! syscall 2, disable interrupts
    rdpr  %pil, %g1
    and   %g5, SPARC_TSTATE_IE_MASK, %o0
    or    %o0, %g1, %o0                     ! return TSTATE_IE | PIL
    wrpr  %g0, 0xf, %pil                    ! set PIL to 15
    andn  %g5, SPARC_TSTATE_IE_MASK, %g1
    wrpr  %g0, %g1, %tstate                 ! disable interrupts in trap state
    ba,a  9f

    3:  ! syscall 3, enable interrupts
    subcc %g4, 3, %g0                     
    bne   1f
    and   %o0, 0xf, %g1
    wrpr  %g0, %g1, %pil                    ! restore PIL
!    and   %o0, SPARC_TSTATE_IE_MASK, %g1
!    or    %g5, %g1, %g1                     ! restore saved IE
    or    %g5, SPARC_TSTATE_IE_MASK, %g1    ! restore IE (safe?)
    wrpr  %g0, %g1, %tstate     
    ba,a  9f

    1:          
    ba,a  1b                                ! spin. taking a trap here -> htrap

    9:                                      ! leave
    mov  0, %g4                             ! clear %g4
    DONE

PUBLIC(sparc_disable_interrupts)

  SYM(sparc_disable_interrupts):
    mov  SYS_irqdis, %g4
    ta  0
#if 0
    rdpr  %pstate, %g5
    rdpr  %pil, %g1
    and   %g5, SPARC_PSTATE_IE_MASK, %o0
    or    %o0, %g1, %o0                     ! return PSTATE_IE | PIL
    wrpr  %g0, 0xf, %pil                    ! set PIL to 15
    andn  %g5, SPARC_PSTATE_IE_MASK, %g1
    wrpr  %g0, %g1, %pstate                 ! disable interrupts
#endif
    retl  
    nop

PUBLIC(sparc_enable_interrupts)

  SYM(sparc_enable_interrupts):
    mov  SYS_irqen, %g4
    ta  0
#if 0
    rdpr  %pstate, %g5
    and   %o0, 0xf, %g1
    wrpr  %g0, %g1, %pil                    ! restore PIL
    and   %o0, SPARC_PSTATE_IE_MASK, %g1
    or    %g5, %g1, %g1                     ! restore saved IE
!    or    %g5, SPARC_PSTATE_IE_MASK, %g1    ! set IE regardless of old (safe?)
    wrpr  %g0, %g1, %pstate
#endif
    retl  
    nop

    /* end of file */
