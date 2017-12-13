/*
 * Copyright (c) 2016-2017 Chris Johns <chrisj@rtems.org>.
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

#define TARGET_DEBUG 0

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

#if TARGET_DEBUG
#include <rtems/bspIo.h>
#endif

/*
 * ARM Variant controls.
 */
#if defined(__ARM_ARCH_7A__) || \
    defined(__ARM_ARCH_7R__)
  #define ARM_CP15 1
#endif

#if (defined(__ARM_ARCH_7M__) || \
     defined(__ARM_ARCH_7EM__))
  #define ARM_THUMB_ONLY 1
#else
  #define ARM_THUMB_ONLY 0
#endif

#if defined(ARM_MULTILIB_ARCH_V4)
 #define ARM_PSR_HAS_INT_MASK 1
 #define ARM_PSR_HAS_THUMB    1
#else
 #define ARM_PSR_HAS_INT_MASK 0
 #define ARM_PSR_HAS_THUMB    0
#endif

#if ARM_CP15
#include <libcpu/arm-cp15.h>
#endif

/**
 * If thumb build of code switch the asm to thumb as required.
 *
 * If the variant only supports thumb insturctions disable the support.
 */
#define ARM_SWITCH_REG     uint32_t arm_switch_reg
#define ARM_SWITCH_REG_ASM [arm_switch_reg] "=&r" (arm_switch_reg)
#if !ARM_THUMB_ONLY && defined(__thumb__)
  #define ASM_ARM_MODE   ".align 2\nbx pc\n.arm\n"
  #define ASM_THUMB_MODE "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
#else
  #define ASM_ARM_MODE
  #define ASM_THUMB_MODE
#endif

/*
 * Hack to work around ARMv7-M not having a the T and I bits in the PSR.
 *
 * This needs to be fixed when real support for this ARM variant is added.
 */
#if !defined(ARM_PSR_I)
  #define ARM_PSR_I 0
#endif
#if !defined(ARM_PSR_T)
  #define ARM_PSR_T 0
#endif

/*
 * The ARM has 2 interrupt bits.
 */
#define CPSR_IRQ_DISABLE 0x80    /* IIQ disabled when 1 */
#define CPSR_FIQ_DISABLE 0x40    /* FIQ disabled when 1 */
#define CPSR_INTS_MASK   (CPSR_IRQ_DISABLE | CPSR_FIQ_DISABLE)

/*
 * Software breakpoint block size.
 */
#define RTEMS_DEBUGGER_SWBREAK_NUM 64

/*
 * Number of registers.
 */
#define RTEMS_DEBUGGER_NUMREGS 26

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
#define REG_R0    0
#define REG_R1    1
#define REG_R2    2
#define REG_R3    3
#define REG_R4    4
#define REG_R5    5
#define REG_R6    6
#define REG_R7    7
#define REG_R8    8
#define REG_R9    9
#define REG_R10   10
#define REG_R11   11
#define REG_R12   12
#define REG_SP    13
#define REG_LR    14
#define REG_PC    15
#define REG_CPSR  25

/**
 * The various status registers.
 */
#if defined(ARM_MULTILIB_ARCH_V4)
 #define FRAME_SR frame->register_cpsr
#elif defined(ARM_MULTILIB_ARCH_V7M)
 #define FRAME_SR frame->register_xpsr
#else
 #error ARM architecture is not supported.
#endif

/**
 * The breakpoint.
 */
#ifdef __thumb__
 static const uint8_t breakpoint[2] = { 0x55, 0xbe };
#else
 static const uint8_t breakpoint[4] = { 0x75, 0xe0, 0x20, 0xe1 };
#endif

/**
 * Target lock.
 */
RTEMS_INTERRUPT_LOCK_DEFINE(static, target_lock, "target_lock")

/**
 * Is a session active?
 */
static bool debug_session_active;

/*
 * ARM debug hardware.
 */
static int debug_version;
static int debug_revision;;
static int hw_breakpoints;
static int hw_watchpoints;

/**
 * Hardware break and watch points.
 */
typedef struct
{
  bool                 enabled;
  bool                 loaded;
  void*                address;
  size_t               length;
  CPU_Exception_frame* frame;
  uint32_t             control;
  uint32_t             value;
} arm_debug_hwbreak;

#define ARM_HW_BREAKPOINT_MAX (16)
#define ARM_HW_WATCHPOINT_MAX (16)

/*
 * Types of break points. Only the 2 we use listed.
 */
#define ARM_HW_BP_UNLINKED_INSTR_MATCH    (0x00)
#define ARM_HW_BP_UNLINKED_INSTR_MISMATCH (0x04)

/*
 * Privilege levels.
 */
#define ARM_HW_BP_PRIV_PL0_SUP_SYS (0x00)
#define ARM_HW_BP_PRIV_PL1_ONLY    (0x01)
#define ARM_HW_BP_PRIV_PL0_ONLY    (0x02)
#define ARM_HW_BP_PRIV_ALL_MODES   (0x03)

static arm_debug_hwbreak hw_breaks[ARM_HW_BREAKPOINT_MAX];
//static arm_debug_hwbreak hw_watches[ARM_HW_WATCHPOINT_MAX];

#if TARGET_DEBUG
void rtems_debugger_printk_lock(rtems_interrupt_lock_context* lock_context);
void rtems_debugger_printk_unlock(rtems_interrupt_lock_context* lock_context);

static void target_printk(const char* format, ...) RTEMS_PRINTFLIKE(1, 2);
static void
target_printk(const char* format, ...)
{
  rtems_interrupt_lock_context lock_context;
  va_list ap;
  va_start(ap, format);
  rtems_debugger_printk_lock(&lock_context);
  vprintk(format, ap);
  rtems_debugger_printk_unlock(&lock_context);
  va_end(ap);
}
static const char*
mode_label(int mode)
{
  switch (mode) {
  case 0x10:
    return "USR";
  case 0x11:
    return "FIQ";
  case 0x12:
    return "IRQ";
  case 0x13:
    return "SVC";
  case 0x16:
    return "MON";
  case 0x17:
    return "ABT";
  case 0x1a:
    return "HYP";
  case 0x1b:
    return "UND";
  case 0x1f:
    return "SYS";
  }
  return "---";
}
#else
#define target_printk(_fmt, ...)
#define mode_labels(_m) NULL
#endif

/*
 * CP register access.
 */
#define ARM_CP_INSTR(_opc, _cp, _op1, _val, _CRn, _CRm, _op2)           \
  #_opc " p" #_cp ", " #_op1 ", %[" #_val "], c" #_CRn ", c" #_CRm ", " #_op2 "\n"

#define ARM_CP_WRITE(_cp, _op1, _val, _CRn, _CRm, _op2)    \
  do {                                                     \
    ARM_SWITCH_REG;                                        \
    asm volatile(                                          \
      ASM_ARM_MODE                                         \
      ARM_CP_INSTR(mcr, _cp, _op1, val, _CRn, _CRm, _op2)  \
      ASM_THUMB_MODE                                       \
      : ARM_SWITCH_REG_ASM                                 \
      : [val] "r" (_val));                                 \
  } while (0)

#define ARM_CP_READ(_cp, _op1, _val, _CRn, _CRm, _op2)     \
  do {                                                     \
    ARM_SWITCH_REG;                                        \
    asm volatile(                                          \
      ASM_ARM_MODE                                         \
      ARM_CP_INSTR(mrc, _cp, _op1, val, _CRn, _CRm, _op2)  \
      ASM_THUMB_MODE                                       \
      : ARM_SWITCH_REG_ASM,                                \
        [val] "=&r" (_val));                               \
  } while (0)

/*
 * Read and write a CP14 register.
 *
 * The software debug event registers are not easy to program because there are
 * up to 32 registers and the instructions have to assembler for each of the 32
 * registers, you cannot program it. This means there is a switch table to do
 * this.
 */
#define ARM_CP14_WRITE(_val, _CRn, _CRm, _op2) \
  ARM_CP_WRITE(14, 0, _val, _CRn, _CRm, _op2)

#define ARM_CP14_READ(_val, _CRn, _CRm, _op2)  \
  ARM_CP_READ(14, 0, _val, _CRn, _CRm, _op2)

/*
 * Read and write a CP15 register.
 *
 * The Context ID register is a process level context and used to scope
 * hardware break points.
 */
#define ARM_CP15_WRITE(_val, _op1, _CRn, _CRm, _op2) \
  ARM_CP_WRITE(15, _op1, _val, _CRn, _CRm, _op2)

#define ARM_CP15_READ(_val, _op1, _CRn, _CRm, _op2)  \
  ARM_CP_READ(15, _op1, _val, _CRn, _CRm, _op2)

static int
arm_debug_probe(rtems_debugger_target* target)
{
  #define ID_VALUE(_i, _h, _l) ((_i >> _l) & ((1 << ((_h - _l) + 1)) -1))
  uint32_t          val;
  const char*       vl = "[Invalid version]";
  const char* const labels[] = {
    "ARMv6 [v6]",
    "ARMv6 [v6.1]",
    "ARMv7 [v7, all CP14 registers]",
    "ARMv7 [v7, baseline CP14 registers]",
    "ARMv7 [v7.1]"
  };
  ARM_CP14_READ(val, 0, 0, 0);
  debug_version = ID_VALUE(val, 19, 16);
  if (debug_version < 1 || debug_version > 5) {
    rtems_debugger_printf("rtems-db: arm debug: (v%d.%d) not supported\n",
                          debug_version, debug_revision);
    errno = EIO;
    return -1;
  }
  vl = labels[debug_version - 1];
  debug_revision = ID_VALUE(val, 3, 0);
  hw_breakpoints = ID_VALUE(val, 27, 24);
  hw_watchpoints = ID_VALUE(val, 31, 28);
  rtems_debugger_printf("rtems-db: arm debug: (v%d.%d) %s breakpoints:%d watchpoints:%d\n",
                        debug_version, debug_revision, vl,
                        hw_breakpoints, hw_watchpoints);
  ARM_CP14_READ(val, 0, 1, 0);
  if ((val & (1 << 15)) == 0) {
    switch (debug_version) {
    case 1:
    case 2:
      ARM_CP14_WRITE(val | (1 << 15), 0, 1, 0);
      break;
    case 3:
    case 4:
    case 5:
    default:
      ARM_CP14_WRITE(val | (1 << 15), 0, 2, 2);
      break;
    }
    ARM_CP14_READ(val, 0, 1, 0);
    if ((val & (1 << 15)) == 0) {
      rtems_debugger_printf("rtems-db: arm debug: cannot enter monitor mode\n");
      errno = EIO;
      return -1;
    }
  }
  return 0;
}

static inline void
arm_debug_break_setup(arm_debug_hwbreak* bp,
                      uint32_t           address,
                      uint32_t           type,
                      uint32_t           byte_address_select,
                      uint32_t           privilege)
{
  bp->control = (((type & 0xf) << 20) |
                 ((byte_address_select & 0xf) << 5) |
                 ((privilege & 0x3) << 1) | 1);
  bp->value = (intptr_t) address;
}

static void
arm_debug_break_write_control(int bp, uint32_t control)
{
  switch (bp) {
  case 0:
    ARM_CP14_WRITE(control, 0, 0, 5);
    break;
  case 1:
    ARM_CP14_WRITE(control, 0, 1, 5);
    break;
  case 2:
    ARM_CP14_WRITE(control, 0, 2, 5);
    break;
  case 3:
    ARM_CP14_WRITE(control, 0, 3, 5);
    break;
  case 4:
    ARM_CP14_WRITE(control, 0, 4, 5);
    break;
  case 5:
    ARM_CP14_WRITE(control, 0, 5, 5);
    break;
  case 6:
    ARM_CP14_WRITE(control, 0, 6, 5);
    break;
  case 7:
    ARM_CP14_WRITE(control, 0, 7, 5);
    break;
  case 8:
    ARM_CP14_WRITE(control, 0, 8, 5);
    break;
  case 9:
    ARM_CP14_WRITE(control, 0, 9, 5);
    break;
  case 10:
    ARM_CP14_WRITE(control, 0, 10, 5);
    break;
  case 11:
    ARM_CP14_WRITE(control, 0, 11, 5);
    break;
  case 12:
    ARM_CP14_WRITE(control, 0, 12, 5);
    break;
  case 13:
    ARM_CP14_WRITE(control, 0, 13, 5);
    break;
  case 14:
    ARM_CP14_WRITE(control, 0, 14, 5);
    break;
  case 15:
    ARM_CP14_WRITE(control, 0, 15, 5);
    break;
  }
}

static void
arm_debug_break_write_value(int bp, uint32_t value)
{
  switch (bp) {
  case 0:
    ARM_CP14_WRITE(value, 0, 0, 4);
    break;
  case 1:
    ARM_CP14_WRITE(value, 0, 1, 4);
    break;
  case 2:
    ARM_CP14_WRITE(value, 0, 2, 4);
    break;
  case 3:
    ARM_CP14_WRITE(value, 0, 3, 4);
    break;
  case 4:
    ARM_CP14_WRITE(value, 0, 4, 4);
    break;
  case 5:
    ARM_CP14_WRITE(value, 0, 5, 4);
    break;
  case 6:
    ARM_CP14_WRITE(value, 0, 6, 4);
    break;
  case 7:
    ARM_CP14_WRITE(value, 0, 7, 4);
    break;
  case 8:
    ARM_CP14_WRITE(value, 0, 8, 4);
    break;
  case 9:
    ARM_CP14_WRITE(value, 0, 9, 4);
    break;
  case 10:
    ARM_CP14_WRITE(value, 0, 10, 4);
    break;
  case 11:
    ARM_CP14_WRITE(value, 0, 11, 4);
    break;
  case 12:
    ARM_CP14_WRITE(value, 0, 12, 4);
    break;
  case 13:
    ARM_CP14_WRITE(value, 0, 13, 4);
    break;
  case 14:
    ARM_CP14_WRITE(value, 0, 14, 4);
    break;
  case 15:
    ARM_CP14_WRITE(value, 0, 15, 4);
    break;
  }
}

static void
arm_debug_break_clear(void)
{
  rtems_interrupt_lock_context lock_context;
  arm_debug_hwbreak*           bp = &hw_breaks[0];
  int                          i;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  for (i = 0; i < hw_breakpoints; ++i, ++bp) {
    bp->enabled = false;
    bp->loaded = false;
  }
  rtems_interrupt_lock_release(&target_lock, &lock_context);
}

static inline void
arm_debug_set_context_id(const uint32_t id)
{
  ARM_CP15_WRITE(id, 0, 13, 0, 1);
}

/*
 * You can only load the hardware breaks points when in the SVC mode or the
 * single step inverted break point will trigger.
 */
static void
arm_debug_break_load(void)
{
  rtems_interrupt_lock_context lock_context;
  arm_debug_hwbreak*            bp = &hw_breaks[0];
  int                           i;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  if (bp->enabled && !bp->loaded) {
    arm_debug_set_context_id(0xdead1111);
    arm_debug_break_write_value(0, bp->value);
    arm_debug_break_write_control(0, bp->control);
  }
  ++bp;
  for (i = 1; i < hw_breakpoints; ++i, ++bp) {
    if (bp->enabled && !bp->loaded) {
      bp->loaded = true;
      arm_debug_break_write_value(i, bp->value);
      arm_debug_break_write_control(i, bp->control);
    }
  }
  rtems_interrupt_lock_release(&target_lock, &lock_context);
}

static void
arm_debug_break_unload(void)
{
  rtems_interrupt_lock_context lock_context;
  arm_debug_hwbreak*           bp = &hw_breaks[0];
  int                i;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  arm_debug_set_context_id(0);
  for (i = 0; i < hw_breakpoints; ++i, ++bp) {
    bp->loaded = false;
    arm_debug_break_write_control(i, 0);
  }
  rtems_interrupt_lock_release(&target_lock, &lock_context);
}

#if NOT_USED_BUT_KEEPING
static size_t
arm_debug_break_length(void* pc)
{
  arm_debug_hwbreak* bp = &hw_breaks[0];
  int                i;

  for (i = 0; i < hw_breakpoints; ++i, ++bp) {
    if (bp->enabled && bp->address == pc) {
      return bp->length;
    }
  }
  return sizeof(DB_UINT);
}
#endif

int
rtems_debugger_target_configure(rtems_debugger_target* target)
{
  target->capabilities = (RTEMS_DEBUGGER_TARGET_CAP_SWBREAK);
  target->reg_num = RTEMS_DEBUGGER_NUMREGS;
  target->reg_size = sizeof(uint32_t);
  target->breakpoint = &breakpoint[0];
  target->breakpoint_size = sizeof(breakpoint);
  return arm_debug_probe(target);
}

static void
target_exception(CPU_Exception_frame* frame)
{
#if TARGET_DEBUG
  uint32_t ifsr = arm_cp15_get_instruction_fault_status();
#endif

  target_printk("[} frame = %08lx sig=%d vector=%x ifsr=%08lx pra=%08x\n",
                (uint32_t) frame,
                rtems_debugger_target_exception_to_signal(frame),
                frame->vector, ifsr, (intptr_t) frame->register_pc);

  if ((FRAME_SR & (1 <<  5)) == 0)
    frame->register_pc = (void*) ((intptr_t) frame->register_pc - 8);
  else
    frame->register_pc = (void*) ((intptr_t) frame->register_pc - 4);

  target_printk("[}  R0 = %08" PRIx32 "  R1 = %08" PRIx32       \
                "  R2 = %08" PRIx32 "  R3 = %08" PRIx32 "\n",
                frame->register_r0, frame->register_r1,
                frame->register_r2, frame->register_r3);
  target_printk("[}  R4 = %08" PRIx32 "  R5 = %08" PRIx32       \
                "  R6 = %08" PRIx32 "  R7 = %08" PRIx32 "\n",
                frame->register_r4, frame->register_r5,
                frame->register_r6, frame->register_r7);
  target_printk("[}  R8 = %08" PRIx32 "  R9 = %08" PRIx32       \
                " R10 = %08" PRIx32 " R11 = %08" PRIx32 "\n",
                frame->register_r8, frame->register_r9,
                frame->register_r10, frame->register_r11);
  target_printk("[} R12 = %08" PRIx32 "  SP = %08" PRIx32       \
                "  LR = %08" PRIxPTR "  PC = %08" PRIxPTR "\n", \
                frame->register_r12, frame->register_sp,
                (intptr_t) frame->register_lr, (intptr_t) frame->register_pc);
  target_printk("[}  CPSR = %08" PRIx32 " %c%c%c%c%c%c%c%c%c%c%c" \
                " GE:%" PRIx32 " IT:%02" PRIx32 " M:%" PRIx32 " %s\n",
                FRAME_SR,
                (FRAME_SR & (1 << 31)) != 0 ? 'N' : '-',
                (FRAME_SR & (1 << 30)) != 0 ? 'Z' : '-',
                (FRAME_SR & (1 << 29)) != 0 ? 'C' : '-',
                (FRAME_SR & (1 << 28)) != 0 ? 'V' : '-',
                (FRAME_SR & (1 << 27)) != 0 ? 'Q' : '-',
                (FRAME_SR & (1 << 24)) != 0 ? 'J' : '-',
                (FRAME_SR & (1 <<  9)) != 0 ? 'E' : '-',
                (FRAME_SR & (1 <<  8)) != 0 ? 'A' : '-',
                (FRAME_SR & (1 <<  7)) != 0 ? 'I' : '-',
                (FRAME_SR & (1 <<  6)) != 0 ? 'F' : '-',
                (FRAME_SR & (1 <<  5)) != 0 ? 'T' : '-',
                ((FRAME_SR >> (25 - 5)) & (0x3 << 5)) | ((FRAME_SR >> 10) & 0x1f),
                (FRAME_SR >> 16) & 0xf,
                FRAME_SR & 0x1f, mode_label(FRAME_SR & 0x1f));

  arm_debug_break_clear();

  if (!debug_session_active)
    _ARM_Exception_default(frame);

  switch (rtems_debugger_target_exception(frame)) {
  case rtems_debugger_target_exc_consumed:
  default:
    break;
  case rtems_debugger_target_exc_step:
    FRAME_SR |= CPSR_INTS_MASK;
    break;
  case rtems_debugger_target_exc_cascade:
    target_printk("rtems-db: unhandled exception: cascading\n");
    _ARM_Exception_default(frame);
    break;
  }

  target_printk("[} resuming frame = %08lx PC = %08" PRIxPTR " CPSR = %08" PRIx32 "\n",
                (uint32_t) frame, (intptr_t) frame->register_pc, FRAME_SR);
}

/**
 * Exception stack frame size.
 *
 * The size is the exception stack frame plus the CPSR from the exception. We
 * save the CPSR and restore it when we exit the exception.
 */
#define EXCEPTION_FRAME_SIZE (sizeof(CPU_Exception_frame) + sizeof(uint32_t))

/**
 * Exception stack frame FPU offsets and sizes.
 */
#define EXCEPTION_FRAME_FPU_SIZE   ARM_VFP_CONTEXT_SIZE
#define EXCEPTION_FRAME_FPU_OFFSET ARM_EXCEPTION_FRAME_VFP_CONTEXT_OFFSET

/**
 * Exception entry.
 *
 * We have switched from svc (or even user) to an exception mode. Save the
 * current CPSR and create an exception frame on the exception's stack and then
 * copy it to the thread's stack. Switch back to the thread's context and mode
 * to handle the exception to avoid any stack checks thinking the stack is
 * blown. This lets the thread be suspended.
 *
 * The entry is in two parts, the exception mode entry and the trhead mode
 * entry. This lets us disable any hardware breakpoint support. We need to do
 * this because it is enabled in PL0 mode.
 *
 * Note, the code currently assumes cp15 has been set up to match the
 *       instruction set being used.
 */
#define EXCEPTION_ENTRY_EXC()                                           \
  __asm__ volatile(                                                     \
    ASM_ARM_MODE                                                        \
    "sub  sp, %[frame_size]\n"           /* alloc the frame and CPSR */ \
    "stm  sp, {r0-r12}\n"                            /* store r0-r12 */ \
    "sub  sp, #4\n"                                                     \
    "str  lr, [sp]\n"                           /* save the link reg */ \
    ASM_THUMB_MODE                                                      \
    : ARM_SWITCH_REG_ASM                                                \
    : [frame_size] "i" (EXCEPTION_FRAME_SIZE)                           \
    : "memory")

/*
 * FPU entry. Conditionally D16 or D32 support.
 */
#ifdef ARM_MULTILIB_VFP
#ifdef ARM_MULTILIB_VFP_D32
#define FPU_ENTRY_VFP_D32                                               \
    "vstmia  r5!, {d16-d31}\n"
#else  /* ARM_MULTILIB_VFP_D32 */
#define FPU_ENTRY_VFP_D32                                               \
    "mov    r3, #0\n"                                                   \
    "mov    r4, #0\n"                                                   \
    "adds   r6, r5, #128\n"                                             \
    "3:\n"                                                              \
    "stmia  r5!, {r3-r4}\n"                                             \
    "cmp    r5, r6\n"                                                   \
    "bne    3b\n"
#endif /* ARM_MULTILIB_VFP_D32 */
#define EXCEPTION_ENTRY_FPU(frame_fpu_size)                             \
    "sub    sp, %[frame_fpu_size]\n" /* size includes alignment size */ \
    "add    r5, sp, #4\n"                        /* up to align down */ \
    "bic    r5, r5, #7\n"                     /* align the FPU frame */ \
    "str    r5, [r2]\n"               /* store the FPU frame pointer */ \
    "vmrs   r3, FPEXC\n"                                                \
    "vmrs   r4, FPSCR\n"                                                \
    "stmia  r5!, {r3-r4}\n"                                             \
    "vstmia r5!, {d0-d15}\n"                                            \
    FPU_ENTRY_VFP_D32
#else  /* ARM_MULTILIB_VFP */
#define EXCEPTION_ENTRY_FPU(frame_fpu_size)
#endif /* ARM_MULTILIB_VFP */

#define EXCEPTION_ENTRY_THREAD(_frame)                                  \
  __asm__ volatile(                                                     \
    ASM_ARM_MODE                                                        \
    "ldr  lr, [sp]\n"                        /* recover the link reg */ \
    "add  sp, #4\n"                                                     \
    "add  r0, sp, %[r0_r12_size]\n"       /* get the sp in the frame */ \
    "mrs  r1, spsr\n"                            /* get the saved sr */ \
    "mov  r6, r1\n"                            /* stash it for later */ \
    "bic  r1, r1, %[psr_t]\n"         /* clear thumb mode, not sure? */ \
    "orr  r1, r1, %[psr_i]\n"                           /* mask irqs */ \
    "mrs  r2, cpsr\n"                          /* get the current sr */ \
    "str  r2, [sp, %[frame_cpsr]]\n"          /* save for exc return */ \
    "msr  cpsr, r1\n"                         /* switch to user mode */ \
    "mov  r3, sp\n"                         /* get the stack pointer */ \
    "mov  r4, lr\n"                              /* get the link reg */ \
    "msr  cpsr, r2\n"                            /* back to exc mode */ \
    "mov  r5, lr\n"                                   /* get the PRA */ \
    "stm  r0, {r3-r6}\n"                      /* save into the frame */ \
    "sub  r4, r3, %[frame_size]\n"            /* destination address */ \
    "mov  r6, r4\n"                                /* save the frame */ \
    "sub  r4, #1\n"                          /* one before the start */ \
    "add  r3, #1\n"                              /* one past the end */ \
    "sub  r5, sp, #1\n"                            /* source address */ \
    "1:\n"                                                              \
    "ldrb r0, [r5, #1]!\n"                             /* get a byte */ \
    "strb r0, [r4, #1]!\n"                           /* put the byte */ \
    "cmp  r3, r4\n"                                      /* the end? */ \
    "bne  1b\n"                                                         \
    "add  sp, %[frame_size]\n"            /* free the frame and CPSR */ \
    "mrs  r1, spsr\n"                   /* get the thread's saved sr */ \
    "orr  r2, r1, %[psr_i]\n"                           /* mask irqs */ \
    "msr  cpsr, r2\n"         /* switch back to the thread's context */ \
    "sub  sp, %[frame_size]\n"          /* alloc in the thread stack */ \
    "mov  %[o_frame], sp\n"                        /* save the frame */ \
    "add  r2, sp, %[o_frame_fpu]\n"            /* get the FPU offset */ \
    "mov  r3, #0\n"                                                     \
    "str  r3, [r2]\n"                 /* clear the FPU frame pointer */ \
    EXCEPTION_ENTRY_FPU(frame_fpu_size)                                 \
    "bic  r1, r1, %[psr_i]\n"        /* clear irq mask, debug checks */ \
    "msr  cpsr, r1\n"       /* restore the state with irq mask clear */ \
    ASM_THUMB_MODE                                                      \
    : ARM_SWITCH_REG_ASM,                                               \
      [o_frame] "=r" (_frame)                                           \
    : [psr_t] "i" (ARM_PSR_T),                                          \
      [psr_i] "i" (ARM_PSR_I),                                          \
      [r0_r12_size] "i" (13 * sizeof(uint32_t)),                        \
      [frame_cpsr] "i" (EXCEPTION_FRAME_SIZE - sizeof(uint32_t)),       \
      [frame_size] "i" (EXCEPTION_FRAME_SIZE),                          \
      [o_frame_fpu] "i" (EXCEPTION_FRAME_FPU_OFFSET),                   \
      [frame_fpu_size] "i" (EXCEPTION_FRAME_FPU_SIZE + 4)               \
    : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "memory")

/*
 * FPU exit. Conditionally D16 or D32 support.
 */
#ifdef ARM_MULTILIB_VFP
#ifdef ARM_MULTILIB_VFP_D32
#define FPU_EXIT_VFP_D32                                                \
    "vldmia r0, {d16-d31}\n"
#else  /* ARM_MULTILIB_VFP_D32 */
#define FPU_EXIT_VFP_D32
#endif /* ARM_MULTILIB_VFP_D32 */
#define EXCEPTION_EXIT_FPU(frame_fpu_size)                              \
    "ldmia  r0!, {r1-r2}\n"                                             \
    "vldmia r0!, {d0-d15}\n"                                            \
    FPU_EXIT_VFP_D32                                                    \
    "vmsr   FPEXC, r1\n"                                                \
    "vmsr   FPSCR, r2\n"                                                \
    "add    sp, %[frame_fpu_size]\n" /* size includes alignment size */
#else  /* ARM_MULTILIB_VFP */
#define EXCEPTION_EXIT_FPU(frame_fpu_size)
#endif /* ARM_MULTILIB_VFP */

/**
 * Exception exit.
 *
 * The thread is to be resumed so we are still in the thread's mode. Copy the
 * exception frame from the thread's stack back to the exception's stack and
 * restore the thread's context before returning from the exception to the
 * thread.
 *
 * Note, the code currently assumes cp15 has been set up to match the
 *       instruction set being used.
 */
#define EXCEPTION_EXIT_THREAD(_frame)                                   \
  __asm__ volatile(                                                     \
    ASM_ARM_MODE                                                        \
    "mov  r0, %[i_frame]\n"                         /* get the frame */ \
    "ldr  r0, [r0, %[frame_fpu]]\n"     /* recover FPU frame pointer */ \
    EXCEPTION_EXIT_FPU(frame_fpu_size)                                  \
    "ldr  r2, [sp, %[frame_cpsr]]\n" /* recover exc CPSR from thread */ \
    "mov  r0, sp\n"                  /* get the thread frame pointer */ \
    "msr  cpsr, r2\n"            /* switch back to the exc's context */ \
    "add  r3, sp, #1\n"                               /* get the end */ \
    "sub  sp, %[frame_size]\n"                    /* alloc the frame */ \
    "sub  r4, sp, #1\n"                       /* destination address */ \
    "sub  r5, r0, #1\n"                            /* source address */ \
    "1:\n"                                                              \
    "ldrb r1, [r5, #1]!\n"                             /* get a byte */ \
    "strb r1, [r4, #1]!\n"                           /* put the byte */ \
    "cmp  r3, r4\n"                                      /* the end? */ \
    "bne  1b\n"                                                         \
    "add  r1, r0, %[r0_r12_size]\n"       /* get the sp in the frame */ \
    "ldm  r1, {r3-r6}\n"                   /* recover from the frame */ \
    "orr  r1, r6, %[psr_i]\n"  /* get the thread's psr and mask irqs */ \
    "msr  cpsr, r1\n"                         /* switch to user mode */ \
    "mov  sp, r3\n"                         /* set the stack pointer */ \
    "mov  lr, r4\n"                              /* set the link reg */ \
    "msr  cpsr, r2\n"            /* switch back to the exc's context */ \
    "msr  spsr, r6\n"                       /* set the thread's CPSR */ \
    "sub  sp, #4\n"                                                     \
    "mov  lr, r5\n"                                    /* get the PC */ \
    "str  lr, [sp]\n"                           /* save the link reg */ \
    ASM_THUMB_MODE                                                      \
    : ARM_SWITCH_REG_ASM                                                \
    : [psr_i] "i" (ARM_PSR_I),                                          \
      [r0_r12_size] "i" (13 * sizeof(uint32_t)),                        \
      [frame_cpsr] "i" (EXCEPTION_FRAME_SIZE - sizeof(uint32_t)),       \
      [frame_size] "i" (EXCEPTION_FRAME_SIZE),                          \
      [frame_fpu] "i" (EXCEPTION_FRAME_FPU_OFFSET),                     \
      [frame_fpu_size] "i" (EXCEPTION_FRAME_FPU_SIZE + 4),              \
      [i_frame] "r" (_frame)                                            \
    : "r0", "r1", "r2", "r3", "r4", "r5", "r6", "memory")

#define EXCEPTION_EXIT_EXC()                                            \
  __asm__ volatile(                                                     \
    ASM_ARM_MODE                                                        \
    "ldr  lr, [sp]\n"                        /* recover the link reg */ \
    "add  sp, #4\n"                                                     \
    "ldm  sp, {r0-r12}\n"            /* restore the trhead's context */ \
    "add  sp, %[frame_size]\n"                     /* free the frame */ \
    "subs pc, lr, #0\n"                       /* return from the exc */ \
    :                                                                   \
    : [frame_size] "i" (EXCEPTION_FRAME_SIZE)                           \
    : "memory")


static void __attribute__((naked))
target_exception_undefined_instruction(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 1;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_break_load();
  EXCEPTION_EXIT_EXC();
}

static void __attribute__((naked))
target_exception_supervisor_call(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  /*
   * The PC offset needs to be review so we move past a svc instruction. This
   * can then used as a user breakpoint. The issue is this exception is used by
   * the BKPT instruction in the prefetch abort handler to signal a TRAP.
   */
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 2;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_break_load();
  EXCEPTION_EXIT_EXC();
}

static void __attribute__((naked))
target_exception_prefetch_abort(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  EXCEPTION_ENTRY_THREAD(frame);
#if ARM_CP15
  if ((arm_cp15_get_instruction_fault_status() & 0x1f) == 0x02)
    frame->vector = 2;
  else
    frame->vector = 3;
#else
  frame->vector = 3;
#endif
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_break_load();
  EXCEPTION_EXIT_EXC();
}

static void __attribute__((naked))
target_exception_data_abort(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 4;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_break_load();
  EXCEPTION_EXIT_EXC();
}

#if ARM_CP15
/**
 * The init value for the text section.
 */
static uint32_t text_section_flags;

/* Defined by linkcmds.base */
extern char bsp_section_text_begin[];
extern char bsp_section_text_end[];

static void
rtems_debugger_target_set_vectors(void)
{
  void* text_begin;
  void* text_end;
  text_begin = &bsp_section_text_begin[0];
  text_end = &bsp_section_text_end[0];
  text_section_flags =
    arm_cp15_set_translation_table_entries(text_begin,
                                           text_end,
                                           ARMV7_MMU_DATA_READ_WRITE_CACHED);
  arm_cp15_set_exception_handler(ARM_EXCEPTION_UNDEF,
                                 target_exception_undefined_instruction);
  arm_cp15_set_exception_handler(ARM_EXCEPTION_SWI,
                                 target_exception_supervisor_call);
  arm_cp15_set_exception_handler(ARM_EXCEPTION_PREF_ABORT,
                                 target_exception_prefetch_abort);
  arm_cp15_set_exception_handler(ARM_EXCEPTION_DATA_ABORT,
                                 target_exception_data_abort);
}
#else
static void
rtems_debugger_target_set_vectors(void)
{
  /*
   * Dummy, please add support for your ARM variant.
   */
  void* ui = target_exception_undefined_instruction;
  void* sc = target_exception_supervisor_call;
  void* pa = target_exception_prefetch_abort;
  void* da = target_exception_data_abort;
  (void) ui;
  (void) sc;
  (void) pa;
  (void) da;
}
#endif

int
rtems_debugger_target_enable(void)
{
  rtems_interrupt_lock_context lock_context;
  debug_session_active = true;
  arm_debug_break_unload();
  arm_debug_break_clear();
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  rtems_debugger_target_set_vectors();
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_disable(void)
{
  rtems_interrupt_lock_context lock_context;
#if DOES_NOT_WORK
  void*                        text_begin;
  void*                        text_end;
#endif
  arm_debug_break_unload();
  arm_debug_break_clear();
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  debug_session_active = false;
#if DOES_NOT_WORK
  text_begin = &bsp_section_text_begin[0];
  text_end = &bsp_section_text_end[0];
  arm_cp15_set_translation_table_entries(text_begin,
                                         text_end,
                                         text_section_flags);
#endif
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_read_regs(rtems_debugger_thread* thread)
{
  if (!rtems_debugger_thread_flag(thread,
                                  RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID)) {
    static const uint32_t good_address = (uint32_t) &good_address;
    uint32_t*             regs = &thread->registers[0];
    int                   i;

    for (i = 0; i < RTEMS_DEBUGGER_NUMREGS; ++i)
      regs[i] = (uint32_t) &good_address;

    if (thread->frame) {
      CPU_Exception_frame* frame = thread->frame;

      /*
       * Assume interrupts are not masked and if masked set them to the saved
       * value.
       */
      FRAME_SR &= ~CPSR_INTS_MASK;

      if (rtems_debugger_thread_flag(thread,
                                     RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED)) {
        FRAME_SR |=
          (thread->flags >> RTEMS_DEBUGGER_THREAD_FLAG_TARGET_BASE) & CPSR_INTS_MASK;
        thread->flags = ~RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED;
      }

      regs[REG_R0]   = frame->register_r0;
      regs[REG_R1]   = frame->register_r1;
      regs[REG_R2]   = frame->register_r2;
      regs[REG_R3]   = frame->register_r3;
      regs[REG_R4]   = frame->register_r4;
      regs[REG_R5]   = frame->register_r5;
      regs[REG_R6]   = frame->register_r6;
      regs[REG_R7]   = frame->register_r7;
      regs[REG_R8]   = frame->register_r8;
      regs[REG_R9]   = frame->register_r9;
      regs[REG_R10]  = frame->register_r10;
      regs[REG_R11]  = frame->register_r11;
      regs[REG_R12]  = frame->register_r12;
      regs[REG_SP]   = frame->register_sp;
      regs[REG_LR]   = (uint32_t) frame->register_lr;
      regs[REG_PC]   = (uint32_t) frame->register_pc;
      regs[REG_CPSR] = FRAME_SR;
      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal(frame);
    }
    else {
#if defined(ARM_MULTILIB_ARCH_V4)
      regs[REG_R4]  = thread->tcb->Registers.register_r4;
      regs[REG_R5]  = thread->tcb->Registers.register_r5;
      regs[REG_R6]  = thread->tcb->Registers.register_r6;
      regs[REG_R7]  = thread->tcb->Registers.register_r7;
      regs[REG_R8]  = thread->tcb->Registers.register_r8;
      regs[REG_R9]  = thread->tcb->Registers.register_r9;
      regs[REG_R10] = thread->tcb->Registers.register_r10;
      regs[REG_R11] = thread->tcb->Registers.register_fp;
      regs[REG_LR]  = (intptr_t) thread->tcb->Registers.register_lr;
      regs[REG_PC]  = (intptr_t) thread->tcb->Registers.register_lr;
      regs[REG_SP]  = (intptr_t) thread->tcb->Registers.register_sp;
#elif defined(ARM_MULTILIB_ARCH_V7M)
      regs[REG_R4]  = thread->tcb->Registers.register_r4;
      regs[REG_R5]  = thread->tcb->Registers.register_r5;
      regs[REG_R6]  = thread->tcb->Registers.register_r6;
      regs[REG_R7]  = thread->tcb->Registers.register_r7;
      regs[REG_R8]  = thread->tcb->Registers.register_r8;
      regs[REG_R9]  = thread->tcb->Registers.register_r9;
      regs[REG_R10] = thread->tcb->Registers.register_r10;
      regs[REG_R11] = thread->tcb->Registers.register_r11;
      regs[REG_LR]  = (intptr_t) thread->tcb->Registers.register_lr;
      regs[REG_PC]  = (intptr_t) thread->tcb->Registers.register_lr;
      regs[REG_SP]  = (intptr_t) thread->tcb->Registers.register_sp;
#endif
      /*
       * Blocked threads have no signal.
       */
      thread->signal = 0;
    }

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
     * Only write to debugger controlled exception threads. Do not touch the
     * registers for threads blocked in the context switcher.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      CPU_Exception_frame* frame = thread->frame;
      frame->register_r0  = regs[REG_R0];
      frame->register_r1  = regs[REG_R1];
      frame->register_r2  = regs[REG_R2];
      frame->register_r3  = regs[REG_R3];
      frame->register_r4  = regs[REG_R4];
      frame->register_r5  = regs[REG_R5];
      frame->register_r6  = regs[REG_R6];
      frame->register_r7  = regs[REG_R7];
      frame->register_r8  = regs[REG_R8];
      frame->register_r9  = regs[REG_R9];
      frame->register_r10 = regs[REG_R10];
      frame->register_r11 = regs[REG_R11];
      frame->register_r12 = regs[REG_R12];
      frame->register_sp  = regs[REG_SP];
      frame->register_lr  = (void*) regs[REG_LR];
      frame->register_pc  = (void*) regs[REG_PC];
      FRAME_SR            = regs[REG_CPSR];
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
    return regs[REG_PC];
  }
  return 0;
}

DB_UINT
rtems_debugger_target_frame_pc(CPU_Exception_frame* frame)
{
  return (DB_UINT) frame->register_pc;
}

DB_UINT
rtems_debugger_target_reg_sp(rtems_debugger_thread* thread)
{
  int r;
  r = rtems_debugger_target_read_regs(thread);
  if (r >= 0) {
    uint32_t* regs = &thread->registers[0];
    return regs[REG_SP];
  }
  return 0;
}

DB_UINT
rtems_debugger_target_tcb_sp(rtems_debugger_thread* thread)
{
  return (DB_UINT) thread->tcb->Registers.register_sp;
}

int
rtems_debugger_target_thread_stepping(rtems_debugger_thread* thread)
{
  if (rtems_debugger_thread_flag(thread, RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR)) {
    /*
     * Single stepping and range stepping uses hardware debug breakpoint
     * 0. This is reserved for single stepping.
     */
    CPU_Exception_frame* frame = thread->frame;
    arm_debug_hwbreak*   bp = &hw_breaks[0];
    target_printk("[} stepping: %s\n", bp->enabled ? "yes" : "no");
    if (!bp->enabled) {
      const uint32_t addr = (intptr_t) frame->register_pc;
      const bool     thumb = (FRAME_SR & (1 << 5)) != 0 ? true : false;
      uint32_t       bas;

      bp->enabled = true;
      bp->loaded = false;
      bp->address = frame->register_pc;
      bp->frame = frame;
      bp->length = sizeof(uint32_t);

      if (thumb) {
        uint16_t instr = *((uint16_t*) frame->register_pc);
        switch (instr & 0xf800) {
        case 0xe800:
        case 0xf000:
        case 0xf800:
          break;
        default:
          bp->length = sizeof(uint16_t);
          break;
        }
      }

      /*
       * See table C3-2 Effect of byte address selection on Breakpoint
       * generation and "Instruction address comparision programming
       * examples.
       */
      if (thumb) {
        if ((addr & (1 << 1)) == 0) {
          bas = 0x3; /* b0011 */
        }
        else {
          bas = 0xc; /* b1100 */
        }
      }
      else {
        bas = 0xf; /* b1111 */
      }

      arm_debug_break_setup(bp,
                            addr & ~0x3,
                            ARM_HW_BP_UNLINKED_INSTR_MISMATCH,
                            bas,
                            ARM_HW_BP_PRIV_PL0_SUP_SYS);

      /*
       * Save the interrupt state before stepping if set.
       */
#if ARM_PSR_HAS_INT_MASK
      if ((FRAME_SR & CPSR_INTS_MASK) != 0) {
        uint32_t int_state;
        int_state =
          (frame->register_cpsr & CPSR_INTS_MASK) << RTEMS_DEBUGGER_THREAD_FLAG_TARGET_BASE;
        thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED | int_state;
      }
      /*
       * Mask the interrupt when stepping.
       */
      FRAME_SR |= CPSR_INTS_MASK;
#endif
    }
  }
  return 0;
}

int
rtems_debugger_target_exception_to_signal(CPU_Exception_frame* frame)
{
  int sig = RTEMS_DEBUGGER_SIGNAL_HUP;
#if defined(ARM_MULTILIB_ARCH_V4)
  switch (frame->vector) {
  case ARM_EXCEPTION_RESET:
  case ARM_EXCEPTION_SWI:
    sig = RTEMS_DEBUGGER_SIGNAL_TRAP;
    break;
  case ARM_EXCEPTION_UNDEF:
    sig = RTEMS_DEBUGGER_SIGNAL_ILL;
    break;
  case ARM_EXCEPTION_FIQ:
    sig = RTEMS_DEBUGGER_SIGNAL_FPE;
    break;
  case ARM_EXCEPTION_PREF_ABORT:
  case ARM_EXCEPTION_DATA_ABORT:
    sig = RTEMS_DEBUGGER_SIGNAL_SEGV;
    break;
  case ARM_EXCEPTION_RESERVED:
  case ARM_EXCEPTION_IRQ:
    sig = RTEMS_DEBUGGER_SIGNAL_BUS;
    break;
  default:
    break;
  }
#endif
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
  arm_debug_break_unload();
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
   * Flush the data cache and invalidate the instruction cache.
   */
  rtems_cache_flush_multiple_data_lines(swbreak->address,
                                        sizeof(breakpoint));
  rtems_cache_instruction_sync_after_code_change(swbreak->address,
                                                 sizeof(breakpoint));
  return 0;
}
