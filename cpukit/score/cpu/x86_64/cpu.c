/**
 *  @file
 *
 *  @brief x86_64 Dependent Source
 */

/*
 * Copyright (c) 2018.
 * Amaan Cheval <amaan.cheval@gmail.com>
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

#include <rtems/system.h>
#include <rtems/score/isr.h>
#include <rtems/score/wkspace.h>
#include <rtems/score/tls.h>

Context_Control_fp _CPU_Null_fp_context;

void _CPU_Exception_frame_print(const CPU_Exception_frame *ctx)
{
}

void _CPU_Initialize(void)
{
}

uint32_t _CPU_ISR_Get_level(void)
{
  return 0;
}

void _CPU_ISR_install_raw_handler(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
}

void _CPU_ISR_install_vector(
  uint32_t    vector,
  proc_ptr    new_handler,
  proc_ptr   *old_handler
)
{
}

void _CPU_Install_interrupt_stack(void)
{
}

void *_CPU_Thread_Idle_body(uintptr_t ignored)
{
  for( ; ; ) { }
}
