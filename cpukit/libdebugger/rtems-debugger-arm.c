/*
 * Copyright (c) 2016-2019 Chris Johns <chrisj@rtems.org>.
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

#define ARM_DUMP_ROM_TABLES 0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

#include <rtems.h>
#include <rtems/score/threadimpl.h>

#include <rtems/debugger/rtems-debugger-bsp.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

#if TARGET_DEBUG
#include <rtems/bspIo.h>
#endif

/*
 * ARM Variant controls.
 */
#if (__ARM_ARCH >= 7) && \
    (__ARM_ARCH_PROFILE == 'A' || __ARM_ARCH_PROFILE == 'R')
  #define ARM_CP15 1
#endif

#if (__ARM_ARCH >= 7) && \
    (__ARM_ARCH_PROFILE == 'M')
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
#define NEEDS_THUMB_SWITCH !ARM_THUMB_ONLY && defined(__thumb__)

#if NEEDS_THUMB_SWITCH
  #define ARM_SWITCH_REG       uint32_t arm_switch_reg
  #define ARM_SWITCH_REG_ASM   [arm_switch_reg] "=&r" (arm_switch_reg)
  #define ARM_SWITCH_REG_ASM_L ARM_SWITCH_REG_ASM,
  #define ASM_ARM_MODE         ".align 2\nbx pc\n.arm\n"
  #define ASM_THUMB_MODE       "add %[arm_switch_reg], pc, #1\nbx %[arm_switch_reg]\n.thumb\n"
#else
  #define ARM_SWITCH_REG
  #define ARM_SWITCH_REG_ASM
  #define ARM_SWITCH_REG_ASM_L
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
 * Number of bytes per type of register.
 */
#define RTEMS_DEBUGGER_REG_BYTES    4
#define RTEMS_DEBUGGER_FP_REG_BYTES 12

/*
 * Debugger registers layout. See arm-core.xml in GDB source.
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
#define REG_F0    16
#define REG_F1    17
#define REG_F2    18
#define REG_F3    19
#define REG_F4    20
#define REG_F5    21
#define REG_F6    22
#define REG_F7    23
#define REG_FPS   24
#define REG_CPSR  25

/**
 * Register offset table with the total as the last entry.
 *
 * Check this table in gdb with the command:
 *
 *   maint print registers
 */
static const size_t arm_reg_offsets[RTEMS_DEBUGGER_NUMREGS + 1] =
{
  0,   /* REG_R0    4 uint32_t */
  4,   /* REG_R1    4 uint32_t */
  8,   /* REG_R2    4 uint32_t */
  12,  /* REG_R3    4 uint32_t */
  16,  /* REG_R4    4 uint32_t */
  20,  /* REG_R5    4 uint32_t */
  24,  /* REG_R6    4 uint32_t */
  28,  /* REG_R7    4 uint32_t */
  32,  /* REG_R8    4 uint32_t */
  36,  /* REG_R9    4 uint32_t */
  40,  /* REG_R10   4 uint32_t */
  44,  /* REG_R11   4 uint32_t */
  48,  /* REG_R12   4 uint32_t */
  52,  /* REG_SP    4 *1 */
  56,  /* REG_LR    4 uint32_t */
  60,  /* REG_PC    4 *1 */
  64,  /* REG_F0   12 _arm_ext */
  76,  /* REG_F1   12 _arm_ext */
  88,  /* REG_F2   12 _arm_ext */
  100, /* REG_F3   12 _arm_ext */
  112, /* REG_F4   12 _arm_ext */
  124, /* REG_F5   12 _arm_ext */
  136, /* REG_F6   12 _arm_ext */
  148, /* REG_F7   12 _arm_ext */
  160, /* REG_FPS   4 uint32_t */
  164, /* REG_CPSR  4 uint32_t */
  168  /* total size */
};

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES arm_reg_offsets[RTEMS_DEBUGGER_NUMREGS]

/**
 * The various status registers.
 */
#if defined(ARM_MULTILIB_ARCH_V4) || defined(ARM_MULTILIB_ARCH_V6M)
 #define FRAME_SR(_frame) (_frame)->register_cpsr
#elif defined(ARM_MULTILIB_ARCH_V7M)
 #define FRAME_SR(_frame) (_frame)->register_xpsr
#else
 #error ARM architecture is not supported.
#endif

/**
 * Print the exception frame.
 */
#define EXC_FRAME_PRINT(_out, _prefix, _frame) \
do { \
  _out(_prefix "  R0 = %08" PRIx32 "  R1 = %08" PRIx32       \
               "  R2 = %08" PRIx32 "  R3 = %08" PRIx32 "\n", \
       _frame->register_r0, _frame->register_r1, \
       _frame->register_r2, _frame->register_r3); \
  _out(_prefix "  R4 = %08" PRIx32 "  R5 = %08" PRIx32       \
               "  R6 = %08" PRIx32 "  R7 = %08" PRIx32 "\n", \
       _frame->register_r4, _frame->register_r5, \
       _frame->register_r6, _frame->register_r7); \
  _out(_prefix "  R8 = %08" PRIx32 "  R9 = %08" PRIx32       \
               " R10 = %08" PRIx32 " R11 = %08" PRIx32 "\n", \
       _frame->register_r8, _frame->register_r9, \
       _frame->register_r10, _frame->register_r11); \
  _out(_prefix " R12 = %08" PRIx32 "  SP = %08" PRIx32       \
               "  LR = %08" PRIxPTR "  PC = %08" PRIxPTR "\n", \
       _frame->register_r12, _frame->register_sp, \
       (intptr_t) _frame->register_lr, (intptr_t) _frame->register_pc); \
  _out(_prefix " CPSR = %08" PRIx32 " %c%c%c%c%c%c%c%c%c%c%c"       \
               " GE:%" PRIx32 " IT:%02" PRIx32 " M:%" PRIx32 " %s\n", \
       FRAME_SR(_frame), \
       (FRAME_SR(_frame) & (1 << 31)) != 0 ? 'N' : '-', \
       (FRAME_SR(_frame) & (1 << 30)) != 0 ? 'Z' : '-', \
       (FRAME_SR(_frame) & (1 << 29)) != 0 ? 'C' : '-', \
       (FRAME_SR(_frame) & (1 << 28)) != 0 ? 'V' : '-', \
       (FRAME_SR(_frame) & (1 << 27)) != 0 ? 'Q' : '-', \
       (FRAME_SR(_frame) & (1 << 24)) != 0 ? 'J' : '-', \
       (FRAME_SR(_frame) & (1 <<  9)) != 0 ? 'E' : '-', \
       (FRAME_SR(_frame) & (1 <<  8)) != 0 ? 'A' : '-', \
       (FRAME_SR(_frame) & (1 <<  7)) != 0 ? 'I' : '-', \
       (FRAME_SR(_frame) & (1 <<  6)) != 0 ? 'F' : '-', \
       (FRAME_SR(_frame) & (1 <<  5)) != 0 ? 'T' : '-', \
       (FRAME_SR(_frame) >> 16) & 0xf, \
       ((FRAME_SR(_frame) >> (25 - 5)) & (0x3 << 5)) | ((FRAME_SR(_frame) >> 10) & 0x1f), \
       FRAME_SR(_frame) & 0x1f, arm_mode_label(FRAME_SR(_frame) & 0x1f)); \
} while (0)

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
 * An exception offset is added to the return address of the PC on an
 * exception's stack frame. The PC needs to be adjusted.
 */
static const size_t exc_offsets[2][5] =
{
  /* ARM undef_ins sup call pref abt data abt */
  {  0,  4,        0,       4,       8 },
  /* TMB undef_ins sup call pref abt data abt */
  {  0,  2,        0,       4,       8 }
};

/**
 * Is a session active?
 */
static bool debug_session_active;

/*
 * ARM debug hardware.
 */
static int   debug_version;
static void* debug_registers;
static int   debug_revision;
static bool  debug_disable_ints;
static int   hw_breakpoints;
static int   hw_watchpoints;

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

/*
 * Method of entry (MOE) to debug mode. Bits [5:2] of DBGDSCR.
 */
#define ARM_HW_DSCR_MOE_HALT_REQUEST       (0x0)
#define ARM_HW_DSCR_MOE_BREAKPOINT_EVENT   (0x1)
#define ARM_HW_DSCR_MOE_ASYNC_WATCHPOINT   (0x2)
#define ARM_HW_DSCR_MOE_BREAKPOINT_INSTR   (0x3)
#define ARM_HW_DSCR_MOE_EXTERNAL           (0x4)
#define ARM_HW_DSCR_MOE_VECTOR_CATCH_EVENT (0x5)
#define ARM_HW_DSCR_MOE_OS_UNLOCK_EVENT    (0x8)
#define ARM_HW_DSCR_MOE_SYNC_WATCHPOINT    (0xa)

/*
 * Use to locally probe and catch exceptions when accessinf suspect addresses.
 */
#if ARM_CP15
static void __attribute__((naked)) arm_debug_unlock_abort(void);
#endif

/*
 * Target debugging support. Use this to debug the backend.
 */
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
#else
#define target_printk(_fmt, ...)
#define mode_labels(_m) NULL
#endif

static const char*
arm_mode_label(int mode)
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

#if TARGET_DEBUG
static const char*
arm_moe_label(uint32_t moe)
{
  switch (moe) {
  case ARM_HW_DSCR_MOE_HALT_REQUEST:
    return "HLT";
  case ARM_HW_DSCR_MOE_BREAKPOINT_EVENT:
    return "BPE";
  case ARM_HW_DSCR_MOE_ASYNC_WATCHPOINT:
    return "AWP";
  case ARM_HW_DSCR_MOE_BREAKPOINT_INSTR:
    return "BPI";
  case ARM_HW_DSCR_MOE_EXTERNAL:
    return "EXT";
  case ARM_HW_DSCR_MOE_VECTOR_CATCH_EVENT:
    return "VCE";
  case ARM_HW_DSCR_MOE_OS_UNLOCK_EVENT:
    return "OUL";
  case ARM_HW_DSCR_MOE_SYNC_WATCHPOINT:
    return "SWP";
  default:
    break;
  }
  return "RSV";
}
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
      : ARM_SWITCH_REG_ASM_L                               \
        [val] "=&r" (_val));                               \
  } while (0)

/*
 * CP14 register access.
 *
 * The registers can be access via the core or they can be memory-mapped.
 */

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

/*
 * Read and write a memory mapped debug register. The register number is a word
 * offset from the base address.
 */
#define ARM_MMAP_ADDR(reg)       (((volatile uint32_t*) debug_registers) + (reg))
#define ARM_MMAP_WRITE(reg, val) *ARM_MMAP_ADDR(reg) = (val); _ARM_Data_synchronization_barrier()
#define ARM_MMAP_READ(reg)       *ARM_MMAP_ADDR(reg)

static bool
arm_debug_authentication(uint32_t dbgauthstatus)
{
  bool granted = (dbgauthstatus & (1 << 0)) != 0;
  rtems_debugger_printf("rtems-db: arm debug: authentication: %s " \
                        "(%s %s %s %s %s %s %s %s)\n",
                        granted ? "granted" : "denied",
                        (dbgauthstatus & (1 << 0)) == 0 ? "-" : "NSE",
                        (dbgauthstatus & (1 << 1)) == 0 ? "-" : "NSI",
                        (dbgauthstatus & (1 << 2)) == 0 ? "-" : "NSNE",
                        (dbgauthstatus & (1 << 3)) == 0 ? "-" : "NSNI",
                        (dbgauthstatus & (1 << 4)) == 0 ? "-" : "SE",
                        (dbgauthstatus & (1 << 5)) == 0 ? "-" : "SI",
                        (dbgauthstatus & (1 << 6)) == 0 ? "-" : "SNE",
                        (dbgauthstatus & (1 << 7)) == 0 ? "-" : "SNI");
  return granted;
}

static int
arm_debug_cp14_enable(rtems_debugger_target* target)
{
  uint32_t val;
  ARM_CP14_READ(val, 7, 14, 6);
  if (!arm_debug_authentication(val))
    return -1;
  ARM_CP14_READ(val, 0, 1, 0);
  if ((val & (1 << 15)) == 0) {
    switch (debug_version) {
    case 1:
    case 2:
      ARM_CP14_WRITE(val | (1 << 15), 0, 1, 0);
      break;
    case 3:
    case 5:
    default:
      ARM_CP14_WRITE(val | (1 << 15), 0, 2, 2);
      break;
    case 4:
      rtems_debugger_printf("rtems-db: arm debug: no cp14 access with version 4\n");
      return -1;
    }
    ARM_CP14_READ(val, 0, 1, 0);
    if ((val & (1 << 15)) == 0) {
      rtems_debugger_printf("rtems-db: arm debug: cannot enter monitor mode\n");
      errno = EIO;
      return -1;
    }
  }
  rtems_debugger_printf("rtems-db: arm debug: using cp14 register access\n");
  return 0;
}

/*
 * The write access to the software unlock register can cause an abort. Absorb
 * it.
 */
static jmp_buf unlock_abort_jmpbuf;
static size_t  arm_debug_retries;

static void
arm_debug_dump_rom_table(uint32_t* rom, size_t depth)
{
  uint32_t pidr[7];
  uint32_t cidr[4];
  uint32_t memtype;
  uint32_t pidr4_4KB_count;
  size_t   r;

  static const char *table_class[16] = {
    "reserved",
    "ROM table",
    "reserved", "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "reserved",
    "CoreSight component",
    "reserved",
    "Peripheral Test Block",
    "reserved",
    "OptimoDE DESS",
    "Generic IP component",
    "PrimeCell or System component"
  };

  #define ROM_READ(b_, o_, r_) b_[((o_) / sizeof(uint32_t)) + (r_)]

  if (depth > 16) {
    rtems_debugger_printf("]] rom: too deep\n");
    return;
  }

  for (r = 0; r < 4; ++r)
    pidr[r] = ROM_READ(rom, 0xfe0, r) & 0xff;
  for (r = 0; r < 3; ++r)
    pidr[r + 4] = ROM_READ(rom, 0xfd0, r) & 0xff;
  for (r = 0; r < 4; ++r)
    cidr[r] = ROM_READ(rom, 0xff0, r) & 0xff;
  memtype = ROM_READ(rom, 0xfcc, 0);

  pidr4_4KB_count = pidr[4] & (((1 << (7 - 4)) - 1) >> 4);

  rtems_debugger_printf("]] rom = %p\n", rom);
  rtems_debugger_printf("   PIDR: %08x %08x %08x %08x %08x %08x %08x\n",
                        pidr[0], pidr[1], pidr[2], pidr[3],
                        pidr[4], pidr[5], pidr[6]);
  rtems_debugger_printf("   CIDR: %08x %08x %08x %08x\n",
                        cidr[0], cidr[1], cidr[2], cidr[3]);
  rtems_debugger_printf("   4KB count: %u\n", pidr4_4KB_count);

  if ((memtype & 0x01) != 0)
    rtems_debugger_printf("   MEMTYPE sys memory present on bus\n");
  else
    rtems_debugger_printf("   MEMTYPE sys memory not present: dedicated debug bus\n");

  /*
   * Read ROM table entries until we get 0
   */
  for (r = 0; rom[r] != 0; ++r) {
    uint32_t  romentry = rom[r];
    uint32_t  c_pidr[7];
    uint32_t  c_cidr[4];
    uint32_t* c_base;
    uint32_t  table_type;
    size_t    i;

    c_base = (uint32_t*) ((intptr_t) rom + (romentry & 0xFFFFF000));

    /*
     * Read the IDs.
     */
    for (i = 0; i < 4; ++i)
      c_pidr[i] = ROM_READ(c_base, 0xfe0, i) & 0xff;
    for (i = 0; i < 3; ++i)
      c_pidr[i + 4] = ROM_READ(c_base, 0xfd0, i) & 0xff;
    for (i = 0; i < 4; ++i)
      c_cidr[i] = ROM_READ(c_base, 0xff0, i) & 0xff;

    table_type = ROM_READ(c_base, 0xfcc, 0);

    rtems_debugger_printf("   > Base: %p, start: 0x%" PRIx32 "\n",
                          c_base,
                          /* component may take multiple 4K pages */
                          (uint32_t)((intptr_t) c_base - 0x1000 * (c_pidr[4] >> 4)));
    rtems_debugger_printf("     Class is 0x%x, %s\n",
                          (c_cidr[1] >> 4) & 0xf, table_class[(c_cidr[1] >> 4) & 0xf]);

    if (((c_cidr[1] >> 4) & 0x0f) == 1) {
      arm_debug_dump_rom_table(c_base, depth + 1);
    }
    else if (((c_cidr[1] >> 4) & 0x0f) == 9) {
      const char* major = "reserved";
      const char* subtype = "reserved";
      unsigned    minor = (table_type >> 4) & 0x0f;

      switch (table_type & 0x0f) {
      case 0:
        major = "Miscellaneous";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 4:
          subtype = "Validation component";
          break;
        }
        break;
      case 1:
        major = "Trace Sink";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Port";
          break;
        case 2:
          subtype = "Buffer";
          break;
        case 3:
          subtype = "Router";
          break;
        }
        break;
      case 2:
        major = "Trace Link";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Funnel, router";
          break;
        case 2:
          subtype = "Filter";
          break;
        case 3:
          subtype = "FIFO, buffer";
          break;
        }
        break;
      case 3:
        major = "Trace Source";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Processor";
          break;
        case 2:
          subtype = "DSP";
          break;
        case 3:
          subtype = "Engine/Coprocessor";
          break;
        case 4:
          subtype = "Bus";
          break;
        case 6:
          subtype = "Software";
          break;
        }
        break;
      case 4:
        major = "Debug Control";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Trigger Matrix";
          break;
        case 2:
          subtype = "Debug Auth";
          break;
        case 3:
          subtype = "Power Requestor";
          break;
        }
        break;
      case 5:
        major = "Debug Logic";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Processor";
          break;
        case 2:
          subtype = "DSP";
          break;
        case 3:
          subtype = "Engine/Coprocessor";
          break;
        case 4:
          subtype = "Bus";
          break;
        case 5:
          subtype = "Memory";
        }
        break;
      case 6:
        major = "Perfomance Monitor";
        switch (minor) {
        case 0:
          subtype = "other";
          break;
        case 1:
          subtype = "Processor";
          break;
        case 2:
          subtype = "DSP";
          break;
        case 3:
          subtype = "Engine/Coprocessor";
          break;
        case 4:
          subtype = "Bus";
          break;
        case 5:
          subtype = "Memory";
          break;
        }
        break;
      }

      rtems_debugger_printf("     Type: 0x%02" PRIx32 ", %s, %s\n",
                            table_type & 0xff, major, subtype);
      rtems_debugger_printf("     PID[4..0]: %02x %02x %02x %02x %02x\n",
                            c_pidr[4], c_pidr[3], c_pidr[2], c_pidr[1], c_pidr[0]);

      if (((c_cidr[1] >> 4) & 0x0f) == 1) {
        arm_debug_dump_rom_table(c_base, depth + 1);
      }
    }
  }
}

static int
arm_debug_rom_discover(uint32_t* rom, uint32_t comp, uint32_t** addr, int* index)
{
  size_t r = 0;
  *addr = 0;
  while ((rom[r] & 1) != 0) {
    uint32_t* c_base = (uint32_t*) ((intptr_t) rom + (rom[r] & 0xfffff000));
    uint32_t  c_cid1 = c_base[0xff4 / sizeof(uint32_t)];
    uint32_t  type;
    if (((c_cid1 >> 4) & 0x0f) == 1) {
      if (arm_debug_rom_discover(c_base, comp, addr, index))
        return true;
    }
    type = c_base[0xfcc / sizeof(uint32_t)] & 0xff;
    if (comp == type) {
      if (*index > 0)
        --(*index);
      else {
        *addr = c_base;
        return true;
      }
    }
    ++r;
  }
  return false;
}

static int
arm_debug_mmap_enable(rtems_debugger_target* target, uint32_t dbgdidr)
{
  uint32_t val;
  int      rc = -1;

#if ARM_CP15
  void* abort_handler;
#endif

  /*
   * File scope as setjmp/longjmp effect the local stack variables.
   */
  arm_debug_retries = 5;

  /*
   * The DBGDSAR (DSAR) is a signed offset from DBGDRAR. Both need to
   * be valid for the debug register address to be valid. Read the
   * DBGRAR first.
   */
  ARM_CP14_READ(val, 1, 0, 0);
  if ((val & 3) == 3) {
    uint32_t* rom = (uint32_t*) (val & 0xfffff000);
    uint32_t* comp_base = NULL;
    int       core = (int) _SMP_Get_current_processor();

    if (ARM_DUMP_ROM_TABLES)
      arm_debug_dump_rom_table(rom, 0);

    debug_registers = NULL;

    if (arm_debug_rom_discover(rom, 0x15, &comp_base, &core)) {
      debug_registers = comp_base;
      rtems_debugger_printf("rtems-db: ram debug: ROM Base: %p\n", comp_base);
    } else {
      ARM_CP14_READ(val, 2, 0, 0);
      if ((val & 3) == 3 ) {
        debug_registers = (void*) ((intptr_t) rom + (val & ~3));
      }
    }
  }

  if (debug_registers == NULL) {
    debug_registers = rtems_debugger_arm_debug_registers();
    if (debug_registers == NULL) {
      rtems_debugger_printf("rtems-db: arm debug: no valid register map\n");
      return -1;
    }
    rtems_debugger_printf("rtems-db: arm debug: BSP Base: %p\n", debug_registers);
  }

  /*
   * Make sure the memory mapped registers return the same ID.
   */
  if (ARM_MMAP_READ(0) != dbgdidr) {
    debug_registers = NULL;
    rtems_debugger_printf("rtems-db: arm debug: debug reg map not verified: " \
                          "0x%08x\n", ARM_MMAP_READ(0));
    return -1;
  }

  if (!arm_debug_authentication(ARM_MMAP_READ(1006)))
    return -1;

#if ARM_CP15
  abort_handler =
    arm_cp15_set_exception_handler(ARM_EXCEPTION_DATA_ABORT,
                                   arm_debug_unlock_abort);
#endif

  while (arm_debug_retries-- > 0) {
    if (setjmp(unlock_abort_jmpbuf) == 0) {
      /*
       * If there is a software lock and it is locked unlock it.
       *
       * On the TI am335x this can cause a data abort which we catch and retry
       * which seems to make the debug hardware work.
       */
      if (ARM_MMAP_READ(1005) == 3) {
        ARM_MMAP_WRITE(1004, 0xC5ACCE55);
      }
      /*
       * Are we already in debug mode?
       */
      val = ARM_MMAP_READ(34);
      if ((val & (1 << 15)) == 0) {
        rtems_debugger_printf("rtems-db: arm debug: enable debug mode\n");
        val = ARM_MMAP_READ(34);
        ARM_MMAP_WRITE(34, ARM_MMAP_READ(34) | (1 << 15));
        arm_debug_retries = 0;
      }
    }
  }

#if ARM_CP15
  arm_cp15_set_exception_handler(ARM_EXCEPTION_DATA_ABORT, abort_handler);
#endif

  if (arm_debug_retries > 0) {
    rtems_debugger_printf("rtems-db: arm debug: using debug register access\n");
    rc = 0;
  }
  else {
    rtems_debugger_printf("rtems-db: arm debug: cannot enter debug mode\n");
  }

  val = ARM_MMAP_READ(1006);

  return rc;
}

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
  int rc = -1;

#if ARM_CP15
  ARM_CP15_READ(val, 0, 0, 0, 0);
  rtems_debugger_printf("rtems-db: arm core: Architecture: %d Variant: %d " \
                        "Implementor: %d Part Number: %d Revision: %d\n",
                        (val >> 16) & ((1 << (19 - 16 + 1)) - 1),
                        (val >> 20) & ((1 << (23 - 20 + 1)) - 1),
                        (val >> 24) & ((1 << (31 - 24 + 1)) - 1),
                        (val >>  4) & ((1 << (15 -  4 + 1)) - 1),
                        (val >>  0) & ((1 << ( 3 -  0 + 1)) - 1));
#endif

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

  rtems_debugger_printf("rtems-db: arm debug: (v%d.%d) %s " \
                        "breakpoints:%d watchpoints:%d\n",
                        debug_version, debug_revision, vl,
                        hw_breakpoints, hw_watchpoints);

  if (!rtems_debugger_arm_debug_configure())
    return -1;

  switch (debug_version) {
    case 1:
    case 2:
    case 3:
    case 5:
    default:
      rc = arm_debug_mmap_enable(target, val);
      if (rc != 0)
        rc =  arm_debug_cp14_enable(target);
      break;
    case 4:
      rc = arm_debug_mmap_enable(target, val);
      break;
  }

  return rc;
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
  if (bp < 15) {
    if (debug_registers != NULL) {
      ARM_MMAP_WRITE(80 + bp, control);
    }
    else {
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
  }
}

static void
arm_debug_break_write_value(int bp, uint32_t value)
{
  if (bp < 15) {
    if (debug_registers != NULL) {
      ARM_MMAP_WRITE(64 + bp, value);
    }
    else {
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
  }
}

static uint32_t
arm_debug_dbgdscr_read(void)
{
  uint32_t val;
  if (debug_registers != NULL) {
    val = ARM_MMAP_READ(34);
  }
  else {
    ARM_CP14_READ(val, 0, 1, 0);
  }
  return val;
}

static void
arm_debug_dbgdscr_write(uint32_t val)
{
  if (debug_registers != NULL) {
    ARM_MMAP_WRITE(34, val);
  }
  else {
    ARM_CP14_WRITE(val, 0, 1, 0);
  }
}

static uint32_t
arm_debug_method_of_entry(void)
{
  return (arm_debug_dbgdscr_read() >> 2) & 0xf;
}

static void
arm_debug_disable_interrupts(void)
{
  debug_disable_ints = true;
}

static void
arm_debug_commit_interrupt_disable(void)
{
  if (debug_disable_ints) {
    arm_debug_dbgdscr_write(arm_debug_dbgdscr_read() | (1 << 11));
    debug_disable_ints = false;
  }
}

static void
arm_debug_enable_interrupts(void)
{
  arm_debug_dbgdscr_write(arm_debug_dbgdscr_read() & ~(1 << 11));
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
#if ARM_CP15
  ARM_CP15_WRITE(id, 0, 13, 0, 1);
#endif
}

/*
 * You can only load the hardware breaks points when in the SVC mode or the
 * single step inverted break point will trigger.
 */
static void
arm_debug_break_load(void)
{
  rtems_interrupt_lock_context lock_context;
  arm_debug_hwbreak*           bp = &hw_breaks[0];
  int                          i;
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
  int                          i;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  arm_debug_set_context_id(0);
  for (i = 0; i < hw_breakpoints; ++i, ++bp) {
    bp->loaded = false;
    arm_debug_break_write_control(i, 0);
  }
  rtems_interrupt_lock_release(&target_lock, &lock_context);
}

static void
arm_debug_break_dump(void)
{
#if TARGET_DEBUG
  arm_debug_hwbreak* bp = &hw_breaks[0];
  int                i;
  for (i = 0; i < hw_breakpoints; ++i, ++bp) {
    if (bp->enabled) {
      target_printk("[} bp: %d: control: %08x addr: %08x\n",
                    i, bp->control, bp->value);
    }
  }
#endif
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
  target->reg_offset = arm_reg_offsets;
  target->breakpoint = &breakpoint[0];
  target->breakpoint_size = sizeof(breakpoint);
  return arm_debug_probe(target);
}

static void
target_print_frame(CPU_Exception_frame* frame)
{
  EXC_FRAME_PRINT(target_printk, "[} ", frame);
}

static const size_t
target_exc_offset(CPU_Exception_frame* frame)
{
  size_t thumb = (FRAME_SR(frame) & (1 << 5)) == 0 ? 0 : 1;
  return exc_offsets[thumb][frame->vector];
}

static void
target_exception(CPU_Exception_frame* frame)
{
#if TARGET_DEBUG
#if ARM_CP15
  const uint32_t ifsr = arm_cp15_get_instruction_fault_status();
#else
  const uint32_t ifsr = 0;
#endif
  const uint32_t mvector = frame->vector;
  const uint32_t dbgdscr = arm_debug_dbgdscr_read();
#endif

  const uint32_t moe = arm_debug_method_of_entry();
  const size_t exc_offset = target_exc_offset(frame);

  switch (moe){
  case ARM_HW_DSCR_MOE_BREAKPOINT_EVENT:
  case ARM_HW_DSCR_MOE_BREAKPOINT_INSTR:
  case ARM_HW_DSCR_MOE_ASYNC_WATCHPOINT:
  case ARM_HW_DSCR_MOE_SYNC_WATCHPOINT:
    frame->vector = 2;
    break;
  case ARM_HW_DSCR_MOE_HALT_REQUEST:
  case ARM_HW_DSCR_MOE_EXTERNAL:
  case ARM_HW_DSCR_MOE_VECTOR_CATCH_EVENT:
  case ARM_HW_DSCR_MOE_OS_UNLOCK_EVENT:
  default:
    break;
  }

  target_printk("[} > frame = %08" PRIx32 \
                " sig=%d vector=%u (%u) dbgdscr=%08" PRIx32 " moe=%s" \
                " ifsr=%08" PRIx32 " pra=%08x\n",
                (uint32_t) frame,
                rtems_debugger_target_exception_to_signal(frame),
                frame->vector, mvector, dbgdscr, arm_moe_label(moe),
                ifsr, (intptr_t) frame->register_pc);

  frame->register_pc = (void*) ((intptr_t) frame->register_pc - exc_offset);

  target_print_frame(frame);

  arm_debug_enable_interrupts();
  arm_debug_break_clear();

  if (!debug_session_active)
    _ARM_Exception_default(frame);

  switch (rtems_debugger_target_exception(frame)) {
  case rtems_debugger_target_exc_consumed:
  default:
    break;
  case rtems_debugger_target_exc_step:
    break;
  case rtems_debugger_target_exc_cascade:
    target_printk("rtems-db: unhandled exception: cascading\n");
    _ARM_Exception_default(frame);
    break;
  }

  target_printk("[} < resuming frame = %08" PRIx32 \
                " PC = %08" PRIxPTR " CPSR = %08" PRIx32 "\n",
                (uint32_t) frame, (intptr_t) frame->register_pc, FRAME_SR(frame));
  target_print_frame(frame);
  arm_debug_break_dump();
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

#define ARM_CLEAR_THUMB_MODE "bic  r1, r1, %[psr_t]\n" /* clear thumb */

#define EXCEPTION_ENTRY_THREAD(_frame)                                  \
  __asm__ volatile(                                                     \
    ASM_ARM_MODE                                                        \
    "ldr  lr, [sp]\n"                        /* recover the link reg */ \
    "add  sp, #4\n"                                                     \
    "add  r0, sp, %[r0_r12_size]\n"       /* get the sp in the frame */ \
    "mrs  r1, spsr\n"                            /* get the saved sr */ \
    "mov  r6, r1\n"                            /* stash it for later */ \
    ARM_CLEAR_THUMB_MODE                         /* clear thumb mode */ \
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
    : ARM_SWITCH_REG_ASM_L                                              \
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
    "mov  r0, %[i_frame]\n"                         /* get the frame */ \
    "add  r1, r0, %[r0_r12_size]\n"       /* get the sp in the frame */ \
    "ldm  r1, {r3-r6}\n"                 /* recover sp, lr, pc, cpsr */ \
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
    "ldm  sp, {r0-r12}\n"            /* restore the thread's context */ \
    "add  sp, %[frame_size]\n"                     /* free the frame */ \
    "subs pc, lr, #0\n"                       /* return from the exc */ \
    :                                                                   \
    : [frame_size] "i" (EXCEPTION_FRAME_SIZE)                           \
    : "memory")

/*
 * This is used to catch faulting accesses.
 */
#if ARM_CP15
static void __attribute__((naked))
arm_debug_unlock_abort(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  EXCEPTION_ENTRY_EXC();
  EXCEPTION_ENTRY_THREAD(frame);
  longjmp(unlock_abort_jmpbuf, -1);
}
#endif

static void __attribute__((naked))
target_exception_undefined_instruction(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  arm_debug_enable_interrupts();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 1;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_commit_interrupt_disable();
  arm_debug_break_load();
  EXCEPTION_EXIT_EXC();
}

static void __attribute__((naked))
target_exception_supervisor_call(void)
{
  CPU_Exception_frame* frame;
  ARM_SWITCH_REG;
  /*
   * The PC offset needs to be reviewed so we move past a svc
   * instruction. This can then be used as a user breakpoint. The issue is
   * this exception is used by the BKPT instruction in the prefetch abort
   * handler to signal a TRAP.
   */
  EXCEPTION_ENTRY_EXC();
  arm_debug_break_unload();
  arm_debug_enable_interrupts();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 2;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_commit_interrupt_disable();
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
  arm_debug_enable_interrupts();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 3;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_commit_interrupt_disable();
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
  arm_debug_enable_interrupts();
  EXCEPTION_ENTRY_THREAD(frame);
  frame->vector = 4;
  target_exception(frame);
  EXCEPTION_EXIT_THREAD(frame);
  arm_debug_commit_interrupt_disable();
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
rtems_debugger_target_set_mmu(void)
{
  void* text_begin;
  void* text_end;
  text_begin = &bsp_section_text_begin[0];
  text_end = &bsp_section_text_end[0];
  target_printk("[} MMU edit: text_begin: %p text_end: %p\n",
                text_begin, text_end);
  text_section_flags =
    arm_cp15_set_translation_table_entries(text_begin,
                                           text_end,
                                           ARMV7_MMU_DATA_READ_WRITE_CACHED);
}

static void
rtems_debugger_target_set_vectors(void)
{
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

static void
rtems_debugger_target_set_mmu(void)
{
}
#endif

static bool
rtems_debugger_is_int_reg(size_t reg)
{
  const size_t size = arm_reg_offsets[reg + 1] - arm_reg_offsets[reg];
  return size == RTEMS_DEBUGGER_REG_BYTES;
}

static void
rtems_debugger_set_int_reg(rtems_debugger_thread* thread,
                           size_t                 reg,
                           const uint32_t         value)
{
  const size_t offset = arm_reg_offsets[reg];
  /*
   * Use memcpy to avoid alignment issues.
   */
  memcpy(&thread->registers[offset], &value, sizeof(uint32_t));
}

static const uint32_t
rtems_debugger_get_int_reg(rtems_debugger_thread* thread, size_t reg)
{
  const size_t offset = arm_reg_offsets[reg];
  uint32_t     value;
  memcpy(&value, &thread->registers[offset], sizeof(uint32_t));
  return value;
}

int
rtems_debugger_target_enable(void)
{
  rtems_interrupt_lock_context lock_context;
  debug_session_active = true;
  arm_debug_break_unload();
  arm_debug_break_clear();
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  rtems_debugger_target_set_mmu();
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
    int                   i;

    memset(&thread->registers[0], 0, RTEMS_DEBUGGER_NUMREGBYTES);

    for (i = 0; i < RTEMS_DEBUGGER_NUMREGS; ++i) {
      if (rtems_debugger_is_int_reg(i))
        rtems_debugger_set_int_reg(thread, i, (uint32_t) &good_address);
    }

    if (thread->frame) {
      CPU_Exception_frame* frame = thread->frame;

      /*
       * Assume interrupts are not masked and if masked set them to the saved
       * value.
       */
      FRAME_SR(frame) &= ~CPSR_INTS_MASK;

      if (rtems_debugger_thread_flag(thread,
                                     RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED)) {
        FRAME_SR(frame) |=
          (thread->flags >> RTEMS_DEBUGGER_THREAD_FLAG_TARGET_BASE) & CPSR_INTS_MASK;
        thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED;
      }

      rtems_debugger_set_int_reg(thread, REG_R0,   frame->register_r0);
      rtems_debugger_set_int_reg(thread, REG_R1,   frame->register_r1);
      rtems_debugger_set_int_reg(thread, REG_R2,   frame->register_r2);
      rtems_debugger_set_int_reg(thread, REG_R3,   frame->register_r3);
      rtems_debugger_set_int_reg(thread, REG_R4,   frame->register_r4);
      rtems_debugger_set_int_reg(thread, REG_R5,   frame->register_r5);
      rtems_debugger_set_int_reg(thread, REG_R6,   frame->register_r6);
      rtems_debugger_set_int_reg(thread, REG_R7,   frame->register_r7);
      rtems_debugger_set_int_reg(thread, REG_R8,   frame->register_r8);
      rtems_debugger_set_int_reg(thread, REG_R9,   frame->register_r9);
      rtems_debugger_set_int_reg(thread, REG_R10,  frame->register_r10);
      rtems_debugger_set_int_reg(thread, REG_R11,  frame->register_r11);
      rtems_debugger_set_int_reg(thread, REG_R12,  frame->register_r12);
      rtems_debugger_set_int_reg(thread, REG_SP,   frame->register_sp);
      rtems_debugger_set_int_reg(thread, REG_LR,   (uint32_t) frame->register_lr);
      rtems_debugger_set_int_reg(thread, REG_PC,   (uint32_t) frame->register_pc);
      rtems_debugger_set_int_reg(thread, REG_CPSR, FRAME_SR(frame));
      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal(frame);
    }
    else {
#if defined(ARM_MULTILIB_ARCH_V4)
      rtems_debugger_set_int_reg(thread, REG_R4,  thread->tcb->Registers.register_r4);
      rtems_debugger_set_int_reg(thread, REG_R5,  thread->tcb->Registers.register_r5);
      rtems_debugger_set_int_reg(thread, REG_R6,  thread->tcb->Registers.register_r6);
      rtems_debugger_set_int_reg(thread, REG_R7,  thread->tcb->Registers.register_r7);
      rtems_debugger_set_int_reg(thread, REG_R8,  thread->tcb->Registers.register_r8);
      rtems_debugger_set_int_reg(thread, REG_R9,  thread->tcb->Registers.register_r9);
      rtems_debugger_set_int_reg(thread, REG_R10, thread->tcb->Registers.register_r10);
      rtems_debugger_set_int_reg(thread, REG_R11, thread->tcb->Registers.register_fp);
      rtems_debugger_set_int_reg(thread, REG_LR,  (intptr_t) thread->tcb->Registers.register_lr);
      rtems_debugger_set_int_reg(thread, REG_PC,  (intptr_t) thread->tcb->Registers.register_lr);
      rtems_debugger_set_int_reg(thread, REG_SP,  (intptr_t) thread->tcb->Registers.register_sp);
#elif defined(ARM_MULTILIB_ARCH_V7M)
      rtems_debugger_set_int_reg(thread, REG_R4,  thread->tcb->Registers.register_r4);
      rtems_debugger_set_int_reg(thread, REG_R5,  thread->tcb->Registers.register_r5);
      rtems_debugger_set_int_reg(thread, REG_R6,  thread->tcb->Registers.register_r6);
      rtems_debugger_set_int_reg(thread, REG_R7,  thread->tcb->Registers.register_r7);
      rtems_debugger_set_int_reg(thread, REG_R8,  thread->tcb->Registers.register_r8);
      rtems_debugger_set_int_reg(thread, REG_R9,  thread->tcb->Registers.register_r9);
      rtems_debugger_set_int_reg(thread, REG_R10, thread->tcb->Registers.register_r10);
      rtems_debugger_set_int_reg(thread, REG_R11, thread->tcb->Registers.register_r11);
      rtems_debugger_set_int_reg(thread, REG_LR,  (intptr_t) thread->tcb->Registers.register_lr);
      rtems_debugger_set_int_reg(thread, REG_PC,  (intptr_t) thread->tcb->Registers.register_lr);
      rtems_debugger_set_int_reg(thread, REG_SP,  (intptr_t) thread->tcb->Registers.register_sp);
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
    /*
     * Only write to debugger controlled exception threads. Do not touch the
     * registers for threads blocked in the context switcher.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      CPU_Exception_frame* frame = thread->frame;
      frame->register_r0  = rtems_debugger_get_int_reg(thread, REG_R0);
      frame->register_r1  = rtems_debugger_get_int_reg(thread, REG_R1);
      frame->register_r2  = rtems_debugger_get_int_reg(thread, REG_R2);
      frame->register_r3  = rtems_debugger_get_int_reg(thread, REG_R3);
      frame->register_r4  = rtems_debugger_get_int_reg(thread, REG_R4);
      frame->register_r5  = rtems_debugger_get_int_reg(thread, REG_R5);
      frame->register_r6  = rtems_debugger_get_int_reg(thread, REG_R6);
      frame->register_r7  = rtems_debugger_get_int_reg(thread, REG_R7);
      frame->register_r8  = rtems_debugger_get_int_reg(thread, REG_R8);
      frame->register_r9  = rtems_debugger_get_int_reg(thread, REG_R9);
      frame->register_r10 = rtems_debugger_get_int_reg(thread, REG_R10);
      frame->register_r11 = rtems_debugger_get_int_reg(thread, REG_R11);
      frame->register_r12 = rtems_debugger_get_int_reg(thread, REG_R12);
      frame->register_sp  = rtems_debugger_get_int_reg(thread, REG_SP);
      frame->register_lr  = (void*) rtems_debugger_get_int_reg(thread, REG_LR);
      frame->register_pc  = (void*) rtems_debugger_get_int_reg(thread, REG_PC);
      FRAME_SR(frame)     = rtems_debugger_get_int_reg(thread, REG_CPSR);
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
    return rtems_debugger_get_int_reg(thread, REG_PC);
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
    return rtems_debugger_get_int_reg(thread, REG_SP);
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
      const bool     thumb = (FRAME_SR(frame) & (1 << 5)) != 0 ? true : false;
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

      arm_debug_disable_interrupts();
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

void
rtems_debugger_target_exception_print(CPU_Exception_frame* frame)
{
  EXC_FRAME_PRINT(rtems_debugger_printf, "", frame);
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
