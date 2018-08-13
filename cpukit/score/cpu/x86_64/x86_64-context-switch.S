/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#include <rtems/asm.h>
#include <rtems/score/cpu.h>

#ifndef CPU_STACK_ALIGNMENT
#error "Missing header? CPU_STACK_ALIGNMENT not defined"
#endif

BEGIN_CODE

/*
 *  void _CPU_Context_switch( run_context, heir_context )
 *
 *  This routine performs a normal non-FP context.
 */

.p2align  1
PUBLIC(_CPU_Context_switch)

/* save context argument */
.set RUNCONTEXT_ARG,   REG_ARG0
/* restore context argument */
.set HEIRCONTEXT_ARG,  REG_ARG1

SYM(_CPU_Context_switch):
  movq    RUNCONTEXT_ARG, rax  /* rax = running threads context */

  /* Fill up Context_Control struct */
  pushf
  popq               (0 * CPU_SIZEOF_POINTER)(rax) /* pop rflags into context */
  movq    rbx,       (1 * CPU_SIZEOF_POINTER)(rax)
  movq    rsp,       (2 * CPU_SIZEOF_POINTER)(rax)
  movq    rbp,       (3 * CPU_SIZEOF_POINTER)(rax)
  movq    r12,       (4 * CPU_SIZEOF_POINTER)(rax)
  movq    r13,       (5 * CPU_SIZEOF_POINTER)(rax)
  movq    r14,       (6 * CPU_SIZEOF_POINTER)(rax)
  movq    r15,       (7 * CPU_SIZEOF_POINTER)(rax)

  movq    HEIRCONTEXT_ARG, rax /* rax = heir threads context */

restore:
  pushq (0 * CPU_SIZEOF_POINTER)(rax)       /* push rflags */
  popf                                      /* restore rflags */
  movq  (1 * CPU_SIZEOF_POINTER)(rax), rbx
  movq  (2 * CPU_SIZEOF_POINTER)(rax), rsp
  movq  (3 * CPU_SIZEOF_POINTER)(rax), rbp
  movq  (4 * CPU_SIZEOF_POINTER)(rax), r12
  movq  (5 * CPU_SIZEOF_POINTER)(rax), r13
  movq  (6 * CPU_SIZEOF_POINTER)(rax), r14
  movq  (7 * CPU_SIZEOF_POINTER)(rax), r15

  /* XXX: TLS - load GDT and refresh FS segment selector */

  ret

/*
 *  void _CPU_Context_restore( new_context )
 *
 *  This routine performs a normal non-FP context restore.
 */

PUBLIC(_CPU_Context_restore)

.set NEWCONTEXT_ARG,   REG_ARG0       /* context to restore argument */

SYM(_CPU_Context_restore):
  movq      NEWCONTEXT_ARG, rax  /* rax = running threads context */
  jmp       restore

END_CODE
END
