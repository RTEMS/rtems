/*
 * Copyright (c) 2016 Chris Johns <chrisj@rtems.org>.
 * All rights reserved.
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

#define TARGET_DEBUG 1

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/score/threadimpl.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

/*
 * Hardware breakpoints. Limited by hardware
 */
#define RTEMS_DEBUGGER_HWBREAK_NUM 4

/*
 * Number of registers.
 */
#define RTEMS_DEBUGGER_NUMREGS 16

/*
 * Number of bytes per register.
 */
#define RTEMS_DEBUGGER_REGBYTES 4

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES \
  (RTEMS_DEBUGGER_NUMREGS * RTEMS_DEBUGGER_REGBYTES)

/*
 * Debugger registers layout.
 */
#define REG_EAX    0
#define REG_ECX    1
#define REG_EDX    2
#define REG_EBX    3
#define REG_ESP    4
#define REG_EBP    5
#define REG_ESI    6
#define REG_EDI    7
#define REG_PC     8
#define REG_EIP    REG_PC
#define REG_PS     9
#define REG_EFLAGS REG_PS
#define REG_CS     10
#define REG_SS     11
#define REG_DS     12
#define REG_ES     13
#define REG_FS     14
#define REG_GS     15

/**
 * The int 3 opcode.
 */
#define TARGET_BKPT 0xcc

static const uint8_t breakpoint[1] = { TARGET_BKPT };

/*
 * Get a copy of a register.
 */
#define GET_REG(_r, _v) asm volatile("pushl %%" #_r "; popl %0" : "=rm" (_v))

/*
 * Get a copy of a segment register.
 */
#define GET_SEG_REG(_r, _v) \
  do {                      \
    int _i;                 \
    GET_REG(_r, _i);        \
    _v = _i & 0xffff;       \
  } while (0)

/**
 * Target lock.
 */
RTEMS_INTERRUPT_LOCK_DEFINE(static, target_lock, "target_lock")

/**
 * The orginal exception handler.
 */
static void (*orig_currentExcHandler)(CPU_Exception_frame* frame);

#if TARGET_DEBUG
#include <rtems/bspIo.h>
static void target_printk(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
static void
target_printk(const char* format, ...)
{
  va_list ap;
  va_start(ap, format);
  vprintk(format, ap);
  va_end(ap);
}
#else
#define target_printk(_fmt, ...)
#endif

#if TODO
/**
 * Target description.
 */
static const char* const target_xml =
"<?xml version=\"1.0\">                        \
 <!DOCTYPE target SYSTEM \"gdb-target.dtd\">   \
 <target version=\"1.0\">                      \
  <architecture>i386</architecture>            \
  <xi:include href=\"32bit-core.xml\"/>        \
  <xi:include href=\"32bit-sse.xml\"/>         \
</target>";
#endif

int
rtems_debugger_target_configure(rtems_debugger_target* target)
{
  target->capabilities = (RTEMS_DEBUGGER_TARGET_CAP_SWBREAK);
  target->reg_num = RTEMS_DEBUGGER_NUMREGS;
  target->reg_size = sizeof(uint32_t);
  target->breakpoint = &breakpoint[0];
  target->breakpoint_size = sizeof(breakpoint);
  return 0;
}

static void
target_exception(CPU_Exception_frame* frame)
{
  target_printk("[} frame = %08lx sig=%d (%lx)\n",
                (uint32_t) frame,
                rtems_debugger_target_exception_to_signal(frame),
                frame->idtIndex);
  target_printk("[} EAX = %" PRIx32 " EBX = %" PRIx32 \
                " ECX = %" PRIx32 " EDX = %" PRIx32 "\n",
                frame->eax, frame->ebx, frame->ecx, frame->edx);
  target_printk("[} ESI = %" PRIx32 " EDI = %" PRIx32           \
                " EBP = %" PRIx32 " ESP = %" PRIx32 "\n",
                frame->esi, frame->edi, frame->ebp, frame->esp0);
  target_printk("[} EIP = %" PRIx32"\n", frame->eip);

  frame->eflags &= ~EFLAGS_TRAP;

  switch (rtems_debugger_target_exception(frame)) {
  case rtems_debugger_target_exc_consumed:
  default:
    break;
  case rtems_debugger_target_exc_step:
    frame->eflags |= EFLAGS_TRAP;
    break;
  case rtems_debugger_target_exc_cascade:
    orig_currentExcHandler(frame);
    break;
  }
}

int
rtems_debugger_target_enable(void)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  if (orig_currentExcHandler == NULL) {
    orig_currentExcHandler = _currentExcHandler;
    _currentExcHandler = target_exception;
  }
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_disable(void)
{
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  if (orig_currentExcHandler != NULL)
    _currentExcHandler = orig_currentExcHandler;
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_read_regs(rtems_debugger_thread* thread)
{
  if (!rtems_debugger_thread_flag(thread,
                                  RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID)) {
    uint32_t* regs = &thread->registers[0];
    size_t    i;

    for (i = 0; i < rtems_debugger_target_reg_num(); ++i)
      regs[i] = 0xdeaddead;

    if (thread->frame) {
      CPU_Exception_frame* frame = thread->frame;
      regs[REG_EAX]    = frame->eax;
      regs[REG_ECX]    = frame->ecx;
      regs[REG_EDX]    = frame->edx;
      regs[REG_EBX]    = frame->ebx;
      regs[REG_ESP]    = frame->esp0;
      regs[REG_EBP]    = frame->ebp;
      regs[REG_ESI]    = frame->esi;
      regs[REG_EDI]    = frame->edi;
      regs[REG_EIP]    = frame->eip;
      regs[REG_EFLAGS] = frame->eflags;
      regs[REG_CS]     = frame->cs;

      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal(frame);
    }
    else {
      regs[REG_EBX]    = thread->tcb->Registers.ebx;
      regs[REG_ESI]    = thread->tcb->Registers.esi;
      regs[REG_EDI]    = thread->tcb->Registers.edi;
      regs[REG_EFLAGS] = thread->tcb->Registers.eflags;
      regs[REG_ESP]    = (intptr_t) thread->tcb->Registers.esp;
      regs[REG_EBP]    = (intptr_t) thread->tcb->Registers.ebp;
      regs[REG_EIP]    = *((DB_UINT*) thread->tcb->Registers.esp);
      regs[REG_EAX]    = (intptr_t) thread;

      GET_SEG_REG(CS, regs[REG_CS]);

      /*
       * Blocked threads have no signal.
       */
      thread->signal = 0;
    }

    GET_SEG_REG(SS, regs[REG_SS]);
    GET_SEG_REG(DS, regs[REG_DS]);
    GET_SEG_REG(ES, regs[REG_ES]);
    GET_SEG_REG(FS, regs[REG_FS]);
    GET_SEG_REG(GS, regs[REG_GS]);

    thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID;
    thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
  }

  return 0;
}

int
rtems_debugger_target_write_regs(rtems_debugger_thread* thread)
{
  if (rtems_debugger_thread_flag(thread,
                                 RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY)) {
    uint32_t* regs = &thread->registers[0];

    /*
     * Only write to debugger controlled threads. Do not touch the registers
     * for threads blocked in the context switcher.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      CPU_Exception_frame* frame = thread->frame;
      frame->eax    = regs[REG_EAX];
      frame->ecx    = regs[REG_ECX];
      frame->edx    = regs[REG_EDX];
      frame->ebx    = regs[REG_EBX];
      frame->esp0   = regs[REG_ESP];
      frame->ebp    = regs[REG_EBP];
      frame->esi    = regs[REG_ESI];
      frame->edi    = regs[REG_EDI];
      frame->eip    = regs[REG_EIP];
      frame->eflags = regs[REG_EFLAGS];
      frame->cs     = regs[REG_CS];
    }
    thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
  }
  return 0;
}

DB_UINT
rtems_debugger_target_reg_pc(rtems_debugger_thread* thread)
{
  int r;
  r = rtems_debugger_target_read_regs(thread);
  if (r >= 0) {
    uint32_t* regs = &thread->registers[0];
    return regs[REG_EIP];
  }
  return 0;
}

DB_UINT
rtems_debugger_target_frame_pc(CPU_Exception_frame* frame)
{
  return (DB_UINT) frame->eip;
}

DB_UINT
rtems_debugger_target_reg_sp(rtems_debugger_thread* thread)
{
  int r;
  r = rtems_debugger_target_read_regs(thread);
  if (r >= 0) {
    uint32_t* regs = &thread->registers[0];
    return regs[REG_ESP];
  }
  return 0;
}

DB_UINT
rtems_debugger_target_tcb_sp(rtems_debugger_thread* thread)
{
  return (DB_UINT) thread->tcb->Registers.esp;
}

int
rtems_debugger_target_thread_stepping(rtems_debugger_thread* thread)
{
  if (rtems_debugger_thread_flag(thread,
                                 (RTEMS_DEBUGGER_THREAD_FLAG_STEP |
                                  RTEMS_DEBUGGER_THREAD_FLAG_STEPPING))) {
    CPU_Exception_frame* frame = thread->frame;
    /*
     * Single step instructions with interrupts masked to avoid stepping into
     * an interrupt handler.
     */
    if ((frame->eflags & EFLAGS_INTR_ENABLE) == 0)
      thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED;
    else
      frame->eflags &= ~EFLAGS_INTR_ENABLE;
    frame->eflags |= EFLAGS_TRAP;
  }
  return 0;
}

int
rtems_debugger_target_exception_to_signal(CPU_Exception_frame* frame)
{
  int sig = RTEMS_DEBUGGER_SIGNAL_HUP;
  switch (frame->idtIndex) {
  case 1:  /* debug exception */
  case 3:  /* breakpoint int3 */
    sig = RTEMS_DEBUGGER_SIGNAL_TRAP;
    break;
  case 4:  /* int overflow    */
  case 5:  /* out-of-bounds   */
    sig = RTEMS_DEBUGGER_SIGNAL_URG;
    break;
  case 6:  /* invalid opcode  */
    sig = RTEMS_DEBUGGER_SIGNAL_ILL;
    break;
  case 8:  /* double fault    */
  case 16: /* fp error        */
    sig = RTEMS_DEBUGGER_SIGNAL_EMT;
    break;
  case 0:  /* divide by zero  */
  case 7:  /* FPU not avail.  */
    sig = RTEMS_DEBUGGER_SIGNAL_FPE;
    break;
  case 9:  /* i387 seg overr. */
  case 10: /* Invalid TSS     */
  case 11: /* seg. not pres.  */
  case 12: /* stack except.   */
  case 13: /* general prot.   */
  case 14: /* page fault      */
  case 17: /* alignment check */
    sig = RTEMS_DEBUGGER_SIGNAL_SEGV;
    break;
  case 2:  /* NMI             */
  case 18: /* machine check   */
    sig = RTEMS_DEBUGGER_SIGNAL_BUS;
    break;
  default:
    break;
  }
  return sig;
}

int
rtems_debugger_target_hwbreak_insert(void)
{
  /*
   * Do nothing, load on exit of the exception handler.
   */
  return 0;
}

int
rtems_debugger_target_hwbreak_remove(void)
{
  return 0;
}

int
rtems_debugger_target_hwbreak_control(rtems_debugger_target_watchpoint wp,
                                      bool                             insert,
                                      DB_UINT                          addr,
                                      DB_UINT                          kind)
{
  /*
   * To do.
   */
  return 0;
}

int
rtems_debugger_target_cache_sync(rtems_debugger_target_swbreak* swbreak)
{
  /*
   * Nothing to do on an i386.
   */
  return 0;
}
