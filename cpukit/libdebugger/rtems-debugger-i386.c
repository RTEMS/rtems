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
#define RTEMS_DEBUGGER_REG_BYTES 4

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
 * Register offset table with the total as the last entry.
 *
 * Check this table in gdb with the command:
 *
 *   maint print registers
 */
static const size_t i386_reg_offsets[RTEMS_DEBUGGER_NUMREGS + 1] =
{
  0,   /* REG_EAX    4 uint32_t */
  4,   /* REG_ECX    4 uint32_t */
  8,   /* REG_EDX    4 uint32_t */
  12,  /* REG_EBX    4 uint32_t */
  16,  /* REG_ESP    4 uint32_t */
  20,  /* REG_EBP    4 uint32_t */
  24,  /* REG_ESI    4 uint32_t */
  28,  /* REG_EDI    4 uint32_t */
  32,  /* REG_EIP    4 *1 */
  36,  /* REG_EFLAGS 4 uint32_t */
  40,  /* REG_CS     4 uint32_t */
  44,  /* REG_SS     4 uint32_t */
  48,  /* REG_DS     4 uint32_t */
  52,  /* REG_ES     4 uint32_t */
  56,  /* REG_FS     4 uint32_t */
  60,  /* REG_GS     4 uint32_t */
  64   /* total size */
};

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES i386_reg_offsets[RTEMS_DEBUGGER_NUMREGS]

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
 * A function to get a segment register.
 */
static inline uint32_t
get_seg_reg(size_t reg)
{
  int v = 0;
  switch (reg) {
    case REG_CS:
      GET_REG(CS, v);
      break;
    case REG_SS:
      GET_REG(SS, v);
      break;
    case REG_DS:
      GET_REG(DS, v);
      break;
    case REG_ES:
      GET_REG(ES, v);
      break;
    case REG_FS:
      GET_REG(FS, v);
      break;
    case REG_GS:
      GET_REG(GS, v);
      break;
  }
  return v & 0xffff;
}

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
  target->reg_offset = i386_reg_offsets;
  target->breakpoint = &breakpoint[0];
  target->breakpoint_size = sizeof(breakpoint);
  return 0;
}

static void
target_exception(CPU_Exception_frame* frame)
{
  target_printk("[} frame = %08" PRIx32 " sig=%d (%" PRIx32 ")\n",
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

static bool
rtems_debugger_is_int_reg(size_t reg)
{
  const size_t size = i386_reg_offsets[reg + 1] - i386_reg_offsets[reg];
  return size == RTEMS_DEBUGGER_REG_BYTES;
}

static void
rtems_debugger_set_int_reg(rtems_debugger_thread* thread,
                           size_t                 reg,
                           const uint32_t         value)
{
  const size_t offset = i386_reg_offsets[reg];
  /*
   * Use memcpy to avoid alignment issues.
   */
  memcpy(&thread->registers[offset], &value, sizeof(uint32_t));
}

static const uint32_t
rtems_debugger_get_int_reg(rtems_debugger_thread* thread, size_t reg)
{
  const size_t offset = i386_reg_offsets[reg];
  uint32_t     value;
  memcpy(&value, &thread->registers[offset], sizeof(uint32_t));
  return value;
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
    size_t i;

    for (i = 0; i < rtems_debugger_target_reg_num(); ++i) {
      if (rtems_debugger_is_int_reg(i))
        rtems_debugger_set_int_reg(thread, i, 0xdeaddead);
    }

    if (thread->frame) {
      CPU_Exception_frame* frame = thread->frame;

      rtems_debugger_set_int_reg(thread, REG_EAX,    frame->eax);
      rtems_debugger_set_int_reg(thread, REG_ECX,    frame->ecx);
      rtems_debugger_set_int_reg(thread, REG_EDX,    frame->edx);
      rtems_debugger_set_int_reg(thread, REG_EBX,    frame->ebx);
      rtems_debugger_set_int_reg(thread, REG_ESP,    frame->esp0);
      rtems_debugger_set_int_reg(thread, REG_EBP,    frame->ebp);
      rtems_debugger_set_int_reg(thread, REG_ESI,    frame->esi);
      rtems_debugger_set_int_reg(thread, REG_EDI,    frame->edi);
      rtems_debugger_set_int_reg(thread, REG_EIP,    frame->eip);
      rtems_debugger_set_int_reg(thread, REG_EFLAGS, frame->eflags);
      rtems_debugger_set_int_reg(thread, REG_CS,     frame->cs);

      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal(frame);
    }
    else {
      rtems_debugger_set_int_reg(thread, REG_EBX,    thread->tcb->Registers.ebx);
      rtems_debugger_set_int_reg(thread, REG_ESI,    thread->tcb->Registers.esi);
      rtems_debugger_set_int_reg(thread, REG_EDI,    thread->tcb->Registers.edi);
      rtems_debugger_set_int_reg(thread, REG_EFLAGS, thread->tcb->Registers.eflags);
      rtems_debugger_set_int_reg(thread, REG_ESP,    (intptr_t) thread->tcb->Registers.esp);
      rtems_debugger_set_int_reg(thread, REG_EBP,    (intptr_t) thread->tcb->Registers.ebp);
      rtems_debugger_set_int_reg(thread, REG_EIP,    *((DB_UINT*) thread->tcb->Registers.esp));
      rtems_debugger_set_int_reg(thread, REG_EAX,    (intptr_t) thread);

      rtems_debugger_set_int_reg(thread, REG_CS, get_seg_reg(REG_CS));

      /*
       * Blocked threads have no signal.
       */
      thread->signal = 0;
    }

    rtems_debugger_set_int_reg(thread, REG_SS, get_seg_reg(REG_SS));
    rtems_debugger_set_int_reg(thread, REG_DS, get_seg_reg(REG_DS));
    rtems_debugger_set_int_reg(thread, REG_ES, get_seg_reg(REG_ES));
    rtems_debugger_set_int_reg(thread, REG_FS, get_seg_reg(REG_FS));
    rtems_debugger_set_int_reg(thread, REG_GS, get_seg_reg(REG_GS));

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
    /*
     * Only write to debugger controlled threads. Do not touch the registers
     * for threads blocked in the context switcher.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      CPU_Exception_frame* frame = thread->frame;
      frame->eax    = rtems_debugger_get_int_reg(thread, REG_EAX);
      frame->ecx    = rtems_debugger_get_int_reg(thread, REG_ECX);
      frame->edx    = rtems_debugger_get_int_reg(thread, REG_EDX);
      frame->ebx    = rtems_debugger_get_int_reg(thread, REG_EBX);
      frame->esp0   = rtems_debugger_get_int_reg(thread, REG_ESP);
      frame->ebp    = rtems_debugger_get_int_reg(thread, REG_EBP);
      frame->esi    = rtems_debugger_get_int_reg(thread, REG_ESI);
      frame->edi    = rtems_debugger_get_int_reg(thread, REG_EDI);
      frame->eip    = rtems_debugger_get_int_reg(thread, REG_EIP);
      frame->eflags = rtems_debugger_get_int_reg(thread, REG_EFLAGS);
      frame->cs     = rtems_debugger_get_int_reg(thread, REG_CS);
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
    return rtems_debugger_get_int_reg(thread, REG_EIP);
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
    return rtems_debugger_get_int_reg(thread, REG_ESP);
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

void
rtems_debugger_target_exception_print(CPU_Exception_frame* frame)
{
  rtems_debugger_printf(" EAX = %" PRIx32 " EBX = %" PRIx32           \
                        " ECX = %" PRIx32 " EDX = %" PRIx32 "\n",
                        frame->eax, frame->ebx, frame->ecx, frame->edx);
  rtems_debugger_printf(" ESI = %" PRIx32 " EDI = %" PRIx32           \
                        " EBP = %" PRIx32 " ESP = %" PRIx32 "\n",
                        frame->esi, frame->edi, frame->ebp, frame->esp0);
  rtems_debugger_printf(" EIP = %" PRIx32"\n", frame->eip);
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
