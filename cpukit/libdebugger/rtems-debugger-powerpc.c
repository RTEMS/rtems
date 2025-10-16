/*
 * Copyright (c) 2024 Contemporary Software
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

#include <rtems/powerpc/registers.h>

/*
 * Hardware breakpoints. Limited by hardware
 */
#define RTEMS_DEBUGGER_HWBREAK_NUM 4

/*
 * Number of registers.
 */
#define RTEMS_DEBUGGER_NUMREGS 72

/*
 * Debugger registers layout.
 */
#define REG_R0            0
#define REG_R1            1
#define REG_R2            2
#define REG_R3            3
#define REG_R4            4
#define REG_R5            5
#define REG_R6            6
#define REG_R7            7
#define REG_R8            8
#define REG_R9            9
#define REG_R10          10
#define REG_R11          11
#define REG_R12          12
#define REG_R13          13
#define REG_R14          14
#define REG_R15          15
#define REG_R16          16
#define REG_R17          17
#define REG_R18          18
#define REG_R19          19
#define REG_R20          20
#define REG_R21          21
#define REG_R22          22
#define REG_R23          23
#define REG_R24          24
#define REG_R25          25
#define REG_R26          26
#define REG_R27          27
#define REG_R28          28
#define REG_R29          29
#define REG_R30          30
#define REG_R31          31
#define REG_F0           32
#define REG_F1           33
#define REG_F2           34
#define REG_F3           35
#define REG_F4           36
#define REG_F5           37
#define REG_F6           38
#define REG_F7           39
#define REG_F8           40
#define REG_F9           41
#define REG_F10          42
#define REG_F11          43
#define REG_F12          44
#define REG_F13          45
#define REG_F14          46
#define REG_F15          47
#define REG_F16          48
#define REG_F17          49
#define REG_F18          50
#define REG_F19          51
#define REG_F20          52
#define REG_F21          53
#define REG_F22          54
#define REG_F23          55
#define REG_F24          56
#define REG_F25          57
#define REG_F26          58
#define REG_F27          59
#define REG_F28          60
#define REG_F29          61
#define REG_F30          62
#define REG_F31          63
#define REG_PC           64
#define REG_MSR          65
#define REG_CND          66
#define REG_LR           67
#define REG_CNT          68
#define REG_XER          69
#define REG_ACC          70
#define REG_SPEFSCR      71

/**
 * Register offset table with the total as the last entry.
 *
 * Check this table in gdb with the command:
 *
 *   maint print remote-registers
 *
 * The important column is the Rmt Nr and g/G offset
 */

/*
 * MPC604/MPC750
 *
 * From the MVME2700 executable (main print architecture):
 *
 *  gdbarch_dump: bfd_arch_info = powerpc:common
 *
 */
static const size_t ppc_common_reg_offsets[RTEMS_DEBUGGER_NUMREGS + 1] =
{
  0,     /* REG_R0      4 uint32_t */
  4,     /* REG_R1      4 uint32_t */
  8,     /* REG_R2      4 uint32_t */
  12,    /* REG_R3      4 uint32_t */
  16,    /* REG_R4      4 uint32_t */
  20,    /* REG_R5      4 uint32_t */
  24,    /* REG_R6      4 uint32_t */
  28,    /* REG_R7      4 uint32_t */
  32,    /* REG_R8      4 uint32_t */
  36,    /* REG_R9      4 uint32_t */
  40,    /* REG_R10     4 uint32_t */
  44,    /* REG_R11     4 uint32_t */
  48,    /* REG_R12     4 uint32_t */
  52,    /* REG_R13     4 uint32_t */
  56,    /* REG_R14     4 uint32_t */
  60,    /* REG_R15     4 uint32_t */
  64,    /* REG_R16     4 uint32_t */
  68,    /* REG_R17     4 uint32_t */
  72,    /* REG_R18     4 uint32_t */
  76,    /* REG_R19     4 uint32_t */
  80,    /* REG_R20     4 uint32_t */
  84,    /* REG_R21     4 uint32_t */
  88,    /* REG_R22     4 uint32_t */
  92,    /* REG_R23     4 uint32_t */
  96,    /* REG_R24     4 uint32_t */
  100,   /* REG_R25     4 uint32_t */
  104,   /* REG_R26     4 uint32_t */
  108,   /* REG_R27     4 uint32_t */
  112,   /* REG_R28     4 uint32_t */
  116,   /* REG_R29     4 uint32_t */
  120,   /* REG_R30     4 uint32_t */
  124,   /* REG_R31     4 uint32_t */
  128,   /* REG_F0      8 long */
  136,   /* REG_F1      8 long */
  144,   /* REG_F2      8 long */
  152,   /* REG_F3      8 long */
  160,   /* REG_F4      8 long */
  168,   /* REG_F5      8 long */
  176,   /* REG_F6      8 long */
  184,   /* REG_F7      8 long */
  192,   /* REG_F8      8 long */
  200,   /* REG_F9      8 long */
  208,   /* REG_F10     8 long */
  216,   /* REG_F11     8 long */
  224,   /* REG_F12     8 long */
  232,   /* REG_F13     8 long */
  240,   /* REG_F14     8 long */
  248,   /* REG_F15     8 long */
  256,   /* REG_F16     8 long */
  264,   /* REG_F17     8 long */
  272,   /* REG_F18     8 long */
  280,   /* REG_F19     8 long */
  288,   /* REG_F20     8 long */
  296,   /* REG_F21     8 long */
  304,   /* REG_F22     8 long */
  312,   /* REG_F23     8 long */
  320,   /* REG_F24     8 long */
  328,   /* REG_F25     8 long */
  336,   /* REG_F26     8 long */
  344,   /* REG_F27     8 long */
  352,   /* REG_F28     8 long */
  360,   /* REG_F29     8 long */
  368,   /* REG_F30     8 long */
  376,   /* REG_F31     8 long */
  384,   /* REG_PC      4 *1 */
  388,   /* REG_MSR     4 uint32_t */
  392,   /* REG_CND     4 uint32_t */
  396,   /* REG_LR      4 *1 */
  400,   /* REG_CNT     4 uint32_t */
  404,   /* REG_XER     4 uint32_t */
  408,   /* REG_ACC     no present */
  408,   /* REG_SPEFSCR 4 long */
  412    /* total size */
};

/*
 * MPC604/MPC750
 *
 * From the MVME2700 executable:
 *
 * gdbarch_dump: bfd_arch_info = powerpc:e500
 *
 * Note:
 *  No REG_F?? registers defined. The EV?? registers are defined
 *  and can be viewed in GDB with `info all-registers` however
 *  there is no remote protocol mapping I can see and the data being
 *  display in the EV?? rergisters is an alias of the normal register
 *  data.
 */
static const size_t ppc_e500_reg_offsets[RTEMS_DEBUGGER_NUMREGS + 1] =
{
  0,     /* REG_R0      4 uint32_t */
  4,     /* REG_R1      4 uint32_t */
  8,     /* REG_R2      4 uint32_t */
  12,    /* REG_R3      4 uint32_t */
  16,    /* REG_R4      4 uint32_t */
  20,    /* REG_R5      4 uint32_t */
  24,    /* REG_R6      4 uint32_t */
  28,    /* REG_R7      4 uint32_t */
  32,    /* REG_R8      4 uint32_t */
  36,    /* REG_R9      4 uint32_t */
  40,    /* REG_R10     4 uint32_t */
  44,    /* REG_R11     4 uint32_t */
  48,    /* REG_R12     4 uint32_t */
  52,    /* REG_R13     4 uint32_t */
  56,    /* REG_R14     4 uint32_t */
  60,    /* REG_R15     4 uint32_t */
  64,    /* REG_R16     4 uint32_t */
  68,    /* REG_R17     4 uint32_t */
  72,    /* REG_R18     4 uint32_t */
  76,    /* REG_R19     4 uint32_t */
  80,    /* REG_R20     4 uint32_t */
  84,    /* REG_R21     4 uint32_t */
  88,    /* REG_R22     4 uint32_t */
  92,    /* REG_R23     4 uint32_t */
  96,    /* REG_R24     4 uint32_t */
  100,   /* REG_R25     4 uint32_t */
  104,   /* REG_R26     4 uint32_t */
  108,   /* REG_R27     4 uint32_t */
  112,   /* REG_R28     4 uint32_t */
  116,   /* REG_R29     4 uint32_t */
  120,   /* REG_R30     4 uint32_t */
  124,   /* REG_R31     4 uint32_t */
  128,   /* REG_F0      4 long */
  132,   /* REG_F1      4 long */
  136,   /* REG_F2      4 long */
  140,   /* REG_F3      4 long */
  144,   /* REG_F4      4 long */
  148,   /* REG_F5      4 long */
  152,   /* REG_F6      4 long */
  156,   /* REG_F7      4 long */
  160,   /* REG_F8      4 long */
  164,   /* REG_F9      4 long */
  168,   /* REG_F10     4 long */
  172,   /* REG_F11     4 long */
  176,   /* REG_F12     4 long */
  180,   /* REG_F13     4 long */
  184,   /* REG_F14     4 long */
  188,   /* REG_F15     4 long */
  192,   /* REG_F16     4 long */
  196,   /* REG_F17     4 long */
  200,   /* REG_F18     4 long */
  204,   /* REG_F19     4 long */
  208,   /* REG_F20     4 long */
  212,   /* REG_F21     4 long */
  216,   /* REG_F22     4 long */
  220,   /* REG_F23     4 long */
  224,   /* REG_F24     4 long */
  228,   /* REG_F25     4 long */
  232,   /* REG_F26     4 long */
  236,   /* REG_F27     4 long */
  240,   /* REG_F28     4 long */
  244,   /* REG_F29     4 long */
  248,   /* REG_F30     4 long */
  252,   /* REG_F31     4 long */
  256,   /* REG_PC      4 *1 */
  260,   /* REG_MSR     4 uint32_t */
  264,   /* REG_CND     4 uint32_t */
  268,   /* REG_LR      4 *1 */
  272,   /* REG_CNT     4 uint32_t */
  276,   /* REG_XER     4 uint32_t */
  280,   /* REG_ACC     8 long long */
  288,   /* REG_SPEFSCR 4 long */
  292    /* total size */
};

static const size_t* ppc_reg_offsets;

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES ppc_reg_offsets[RTEMS_DEBUGGER_NUMREGS]

/*
 * Exception handler to hook.
 */
typedef CPU_Exception_frame BSP_Exception_frame;
typedef void (*exception_handler_t)(BSP_Exception_frame*);
extern exception_handler_t globalExceptHdl;

/**
 * The `sc` instruction
 */
#define TARGET_BKPT 0x0ce00000
static const uint8_t breakpoint[4] = { 0x0c, 0xe0, 0x00, 0x00 };

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

/*
 * The CPU Ident code is taken from libcpu/cpuIndent.h because
 * this cpukit code cannot reach over into the BSP headers.
 * Adding the code here is not optimal but it solves the need.
 */
#define ASM_RESET_VECTOR                     0x01
#define ASM_MACH_VECTOR                      0x02
#define ASM_PROT_VECTOR                      0x03
#define ASM_ISI_VECTOR                       0x04
#define ASM_EXT_VECTOR                       0x05
#define ASM_ALIGN_VECTOR                     0x06
#define ASM_PROG_VECTOR                      0x07
#define ASM_FLOAT_VECTOR                     0x08
#define ASM_DEC_VECTOR                       0x09
#define ASM_SYS_VECTOR                       0x0C
#define ASM_TRACE_VECTOR                     0x0D

#define ASM_60X_VEC_VECTOR                   0x0A
#define ASM_60X_PERFMON_VECTOR               0x0F
#define ASM_60X_IMISS_VECTOR                 0x10
#define ASM_60X_DLMISS_VECTOR                0x11
#define ASM_60X_DSMISS_VECTOR                0x12
#define ASM_60X_ADDR_VECTOR                  0x13
#define ASM_60X_SYSMGMT_VECTOR               0x14
#define ASM_60X_VEC_ASSIST_VECTOR            0x16
#define ASM_60X_ITM_VECTOR                   0x17

#define LAST_VALID_EXC                       0x1F

typedef enum
{
  PPC_601 = 0x1,
  PPC_5XX = 0x2,
  PPC_603 = 0x3,
  PPC_604 = 0x4,
  PPC_603e = 0x6,
  PPC_603ev = 0x7,
  PPC_750 = 0x8,
  PPC_750_IBM = 0x7000,
  PPC_604e = 0x9,
  PPC_604r = 0xA,
  PPC_7400 = 0xC,
  PPC_405  = 0x2001,  /* Xilinx Virtex-II Pro or -4 */
  PPC_405EX = 0x1291,   /* + 405EXr */
  PPC_405GP = 0x4011,   /* + 405CR */
  PPC_405GPr = 0x5091,
  PPC_405EZ = 0x4151,
  PPC_405EP = 0x5121,
  PPC_440 = 0x7ff2,  /* Xilinx Virtex-5*/
  PPC_7455 = 0x8001, /* Kate Feng */
  PPC_7457 = 0x8002,
  PPC_620 = 0x16,
  PPC_860 = 0x50,
  PPC_821 = PPC_860,
  PPC_823 = PPC_860,
  PPC_8260 = 0x81,
  PPC_8240 = PPC_8260,
  PPC_8245 = 0x8081,
  PPC_8540 = 0x8020,
  PPC_e500v2 = 0x8021,
  PPC_e6500 = 0x8040,
  PPC_603le = 0x8082, /* 603le core, in MGT5100 and MPC5200 */
  PPC_e300c1  = 0x8083, /* e300c1  core, in MPC83xx*/
  PPC_e300c2  = 0x8084, /* e300c2  core */
  PPC_e300c3  = 0x8085, /* e300c3  core */
  PPC_e200z0  = 0x8170,
  PPC_e200z1  = 0x8140,
  PPC_e200z4  = 0x8150,
  PPC_e200z6  = 0x8110,
  PPC_e200z7  = 0x8160,
  PPC_PSIM    = 0xfffe,  /* GDB PowerPC simulator -- fake version */
  PPC_UNKNOWN = 0xffff
} ppc_cpu_id;

#define PPC_BOOKE_405   1 /* almost like booke but with some significant differences */
#define PPC_BOOKE_STD   2
#define PPC_BOOKE_E500  3 /* bookE with extensions */

typedef struct {
  volatile bool type_1_complete;
  volatile uint32_t msr;
} powerpc_stepping;

static ppc_cpu_id ppc_cpu;
static int is_bookE;
static powerpc_stepping stepping_instr;

#define xppc_read_spr(reg, val) \
        __asm__ __volatile__("mfspr %0,"#reg : "=r" (val))
#define xppc_write_spr(reg, val) \
        __asm__ __volatile__("mtspr "#reg",%0" : : "r" (val))
#define ppc_read_spr(reg, val) xppc_read_spr(reg, val)
#define ppc_write_spr(reg, val) xppc_write_spr(reg, val)

static const char *ppc_get_cpu_type_name(ppc_cpu_id cpu)
{
  switch (cpu) {
    case PPC_405:     return "PPC405";
    case PPC_405GP:   return "PPC405GP";
    case PPC_405EX:   return "PPC405EX";
    case PPC_440:     return "PPC440";
    case PPC_601:     return "MPC601";
    case PPC_5XX:     return "MPC5XX";
    case PPC_603:     return "MPC603";
    case PPC_603ev:   return "MPC603ev";
    case PPC_604:     return "MPC604";
    case PPC_750:     return "MPC750";
    case PPC_750_IBM: return "IBM PPC750";
    case PPC_7400:    return "MPC7400";
    case PPC_7455:    return "MPC7455";
    case PPC_7457:    return "MPC7457";
    case PPC_603le:   return "MPC603le";
    case PPC_604e:    return "MPC604e";
    case PPC_604r:    return "MPC604r";
    case PPC_620:     return "MPC620";
    case PPC_860:     return "MPC860";
    case PPC_8260:    return "MPC8260";
    case PPC_8245:    return "MPC8245";
    case PPC_8540:    return "MPC8540";
    case PPC_PSIM:    return "PSIM";
    case PPC_e200z0:  return "e200z0";
    case PPC_e200z1:  return "e200z1";
    case PPC_e200z4:  return "e200z4";
    case PPC_e200z6:  return "e200z6";
    case PPC_e200z7:  return "e200z7";
    case PPC_e500v2:  return "e500v2";
    case PPC_e6500:   return "e6500";
    default:
      break;
  }
  return "unknown";
}

static bool ppc_is_bookE(void)
{
  return is_bookE != 0;
}

static bool ppc_is_bookE_405(void)
{
  return is_bookE == PPC_BOOKE_405;
}

static int ppc_probe_cpu_type(void)
{
  /*
   * cpu types listed here have the lowermost nibble as a version identifier
   * we will tweak them to the standard version
   */
  const uint32_t ppc_cpu_id_version_nibble[] = {
    PPC_e200z0,
    PPC_e200z1,
    PPC_e200z4,
    PPC_e200z6,
    PPC_e200z7
  };
  #define NUM_CPU_ID_VERSION \
    (sizeof(ppc_cpu_id_version_nibble) / sizeof(ppc_cpu_id_version_nibble[0]))

  uint32_t pvr;
  int i;

  ppc_read_spr(PPC_PVR, pvr);
  pvr >>= 16;

  /*
   * apply tweaks to ignore version
   */
  for (i = 0; i < NUM_CPU_ID_VERSION; ++i) {
    if ((pvr & 0xfff0) == (ppc_cpu_id_version_nibble[i] & 0xfff0)) {
      pvr = ppc_cpu_id_version_nibble[i];
      break;
    }
  }

  ppc_cpu = (ppc_cpu_id) pvr;

  switch (pvr) {
    case PPC_405:
    case PPC_405GP:
    case PPC_405EX:
    case PPC_440:
    case PPC_601:
    case PPC_5XX:
    case PPC_603:
    case PPC_603ev:
    case PPC_603le:
    case PPC_604:
    case PPC_604r:
    case PPC_750:
    case PPC_750_IBM:
    case PPC_7400:
    case PPC_7455:
    case PPC_7457:
    case PPC_604e:
    case PPC_620:
    case PPC_860:
    case PPC_8260:
    case PPC_8245:
    case PPC_PSIM:
    case PPC_8540:
    case PPC_e200z0:
    case PPC_e200z1:
    case PPC_e200z4:
    case PPC_e200z6:
    case PPC_e200z7:
    case PPC_e300c1:
    case PPC_e300c2:
    case PPC_e300c3:
    case PPC_e500v2:
    case PPC_e6500:
      break;
    default:
      rtems_debugger_printf("rtems-db: powerpc: unknown CPU\n");
      return -1;
  }

  switch (ppc_cpu) {
    case PPC_405:
    case PPC_405GP:
    case PPC_405EX:
      is_bookE = PPC_BOOKE_405;
      break;
    case PPC_440:
      is_bookE = PPC_BOOKE_STD;
      break;
    case PPC_8540:
    case PPC_e200z0:
    case PPC_e200z1:
    case PPC_e200z4:
    case PPC_e200z6:
    case PPC_e200z7:
    case PPC_e500v2:
    case PPC_e6500:
      is_bookE = PPC_BOOKE_E500;
      break;
    default:
      break;
  }

  rtems_debugger_printf("rtems-db: powerpc: %s %s\n",
                        ppc_get_cpu_type_name(ppc_cpu),
                        ppc_is_bookE() ? "(book E)" : "");

  return 0;
}

static void ppc_set_dbsr(uint32_t dbsr)
{
  if (ppc_is_bookE()) {
    if (ppc_is_bookE_405()) {
      ppc_write_spr(0x3f0, dbsr);
    } else {
      ppc_write_spr(BOOKE_DBSR, dbsr);
    }
  }
}

static void ppc_set_dbcr0(uint32_t dbcr)
{
  if (ppc_is_bookE()) {
    if (ppc_is_bookE_405()) {
      ppc_write_spr(0x3f2, dbcr);
    } else {
      ppc_write_spr(BOOKE_DBCR0, dbcr);
    }
  }
}

static int ppc_debug_probe(rtems_debugger_target* target)
{
  (void) target;

  if (ppc_probe_cpu_type() != 0) {
    return -1;
  }
  ppc_set_dbsr(0xffffffffUL);
  ppc_set_dbcr0(0);
  return 0;
}

static int ppc_code_writer(void* address, const void* data, size_t size) {
  uint32_t current_level;
  uint32_t addr;
  uint32_t val;
  addr = (uint32_t) (intptr_t) address;
  val = *((uint32_t*) data);
  target_printk("[] powerpc: code_ writer: %08x -> %p size=%zu\n", val, address, size);
  if (size != 4) {
    rtems_debugger_printf("rtems-db: powerpc: invalid code write size: size=%zu\n", size);
    return -1;
  }
  /*
   * Disable interrupts and MMU to work around write-protection.
   *
   * This hack is due to the lack of a proper MMU API for the older
   * PPC hardware. Normally libdebugger makes the code section
   * read/write so breakpoint insertion and removal is fast. The lack
   * of a MMU API and the way some of the BSPs MMU is set up means it
   * not easy to update the BSPs.
   *
   * The following is based on Till's `do_patch` implementation.
   */
  current_level = ppc_interrupt_disable();
  asm volatile(
    "   mfmsr 0         \n"
    "   andc  7,0,%0    \n"
    "   mtmsr 7         \n" /* msr is exec. synchronizing; rval access complete */
    "   isync           \n" /* context sync.; DR off after this                 */
    "   stw   %2,0(%1)  \n"
    "   dcbst 0,%1      \n" /* write out data cache line (addr)                 */
    "   icbi  0,%1      \n" /* invalidate instr. cache line (addr)              */
    "   mtmsr 0         \n" /* msr is exec. synchr.; mem access completed       */
    "   sync            \n" /* probably not necessary                           */
    "   isync           \n" /* context sync.; MMU on after this                 */
    /* add 'key' to input operands to make sure this asm is not
     * moved around
     */
    ::"r"(ppc_is_bookE() ? 0 : MSR_DR), "b"(addr), "r"(val), "r"(current_level)
    :"r0","r7");
  ppc_interrupt_enable(current_level);
  return 0;
}

int
rtems_debugger_target_configure(rtems_debugger_target* target)
{
  if (is_bookE) {
    ppc_reg_offsets = ppc_e500_reg_offsets;
  } else {
    ppc_reg_offsets = ppc_common_reg_offsets;
  }
  target->capabilities = (RTEMS_DEBUGGER_TARGET_CAP_SWBREAK);
  target->reg_num = RTEMS_DEBUGGER_NUMREGS;
  target->reg_offset = ppc_reg_offsets;
  target->breakpoint = &breakpoint[0];
  target->breakpoint_size = sizeof(breakpoint);
  target->code_writer = ppc_code_writer;
  return ppc_debug_probe(target);
}

static void powerpc_print_exception_frame(CPU_Exception_frame* frame) {
  uintptr_t* gpr;
  int r;
  target_printk("[} frame = %08" PRIx32 " sig=%d (0x%" PRIx32 ")\n",
                (uint32_t) frame,
                rtems_debugger_target_exception_to_signal(frame),
                frame->_EXC_number);
#ifndef __SPE__
  target_printk("[} SRR0 = %08" PRIx32 " SRR1 = %08" PRIx32 "\n",
                frame->EXC_SRR0, frame->EXC_SRR1);
#else
  target_printk("[} SRR0 = %08" PRIx32 " SRR1 = %08" PRIx32 \
                " SPEFSCR = %08" PRIx32 " ACC = %08" PRIx32 "\n",
                frame->EXC_SRR0, frame->EXC_SRR1,
                frame->EXC_SPEFSCR, frame->EXC_ACC);
#endif
  target_printk("[} LR = %08" PRIx32 " CR = %08" PRIx32 \
                " XER = %08" PRIx32 " CTR = %08" PRIx32 "\n",
                frame->EXC_LR, frame->EXC_CR, frame->EXC_XER, frame->EXC_CTR);
  gpr = &frame->GPR0;
  for (r= 0; r < 32; r += 4, gpr += 4) {
    target_printk("[} R%-2d = %08" PRIx32 " R%-2d = %08" PRIx32 \
                  " R%-2d = %08" PRIx32 " R%-2d = %08" PRIx32 "\n",
                  r, *gpr, r + 1, *(gpr + 1), r + 2,
                  *(gpr + 2), r + 3, *(gpr + 3));
  }
}

/*
 * NXP BOOK_EUM.PDF Programmeing Note, Chapter 9 Debug Facilities
 *
 * There are two classes of debug exception types:
 *
 *   Type 1: exception before instruction
 *      Type 2: exception after instruction
 *
 * Almost all debug exceptions fall into the first category. That is,
 * they all take the interrupt upon encountering an instruction having
 * the exception without updating any architectural state (other than
 * DBSR, CSRR0, CSRR1, MSR) for that instruction.
 *
 * The CSRR0 for this type of exception points to the instruction that
 * encountered the exception. This includes IAC, DAC, branch taken,
 * etc.
 *
 * The only exception which fall into the second category is the
 * instruction complete debug exception. This exception is taken upon
 * completing and updating one instruction and then pointing CSRR0 to
 * the next instruction to execute.
 */
static bool
powerpc_stepping_exception(CPU_Exception_frame* frame)
{
  bool r = false;
  frame->EXC_SRR1 &= ~MSR_SE;
  switch (frame->_EXC_number) {
  default:
    stepping_instr.type_1_complete = false;
    break;
  case ASM_TRACE_VECTOR:
    if (stepping_instr.type_1_complete) {
      /*
       * Type 2 exception after instruction
       */
      stepping_instr.type_1_complete = false;
      frame->EXC_SRR1 |= stepping_instr.msr;
    } else {
      stepping_instr.type_1_complete = true;
      frame->EXC_SRR1 &= ~ppc_interrupt_get_disable_mask();
      frame->EXC_SRR1 |= MSR_SE;
      r = true;
    }
    break;
  case ASM_PROG_VECTOR:
    /*
     * Breakpoint.
     */
    break;
  }
  return r;
}

static void
powerpc_stepping_frame(CPU_Exception_frame* frame)
{
  if (ppc_is_bookE()) {
    stepping_instr.type_1_complete = false;
  } else {
    stepping_instr.type_1_complete = true;
  }
  stepping_instr.msr =
    frame->EXC_SRR1 & ppc_interrupt_get_disable_mask();;
  frame->EXC_SRR1 &= ~ppc_interrupt_get_disable_mask();
  frame->EXC_SRR1 |= MSR_SE;
}

static void
target_exception(BSP_Exception_frame* bsp_frame)
{
  CPU_Exception_frame* frame = bsp_frame;

  target_printk("[} powerpc target exc: entry\n");
  powerpc_print_exception_frame(frame);

  if (!powerpc_stepping_exception(bsp_frame)) {
    switch (rtems_debugger_target_exception(frame)) {
    case rtems_debugger_target_exc_consumed:
    default:
      break;
    case rtems_debugger_target_exc_step:
      powerpc_stepping_frame(frame);
      break;
    case rtems_debugger_target_exc_cascade:
      if (orig_currentExcHandler != NULL) {
        orig_currentExcHandler(bsp_frame);
      }
      break;
    }
  }
}

static bool
rtems_debugger_is_int_reg(size_t reg)
{
  const size_t size = ppc_reg_offsets[reg + 1] - ppc_reg_offsets[reg];
  return size == RTEMS_DEBUGGER_NUMREGBYTES;
}

static void
rtems_debugger_set_int_reg(rtems_debugger_thread* thread,
                           const uintptr_t        reg,
                           const uint32_t         value)
{
  const size_t offset = ppc_reg_offsets[reg];
  /*
   * Use memcpy to avoid alignment issues.
   */
  memcpy(&thread->registers[offset], &value, sizeof(uintptr_t));
}

static uintptr_t
rtems_debugger_get_int_reg(rtems_debugger_thread* thread, const size_t reg)
{
  const size_t offset = ppc_reg_offsets[reg];
  uintptr_t value;
  memcpy(&value, &thread->registers[offset], sizeof(uintptr_t));
  return value;
}

static void rtems_debugger_acquire_exc(void) {
  if (orig_currentExcHandler == NULL) {
    orig_currentExcHandler = globalExceptHdl;
    globalExceptHdl = target_exception;
  }
}

static void rtems_debugger_release_exc(void) {
  if (orig_currentExcHandler != NULL) {
    globalExceptHdl = orig_currentExcHandler;
  }
}

int
rtems_debugger_target_enable(void)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  rtems_debugger_acquire_exc();
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_disable(void)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  rtems_interrupt_lock_context lock_context;
  rtems_interrupt_lock_acquire(&target_lock, &lock_context);
  ppc_set_dbsr(0xffffffffUL);
  ppc_set_dbcr0(0);
  rtems_debugger_release_exc();
  rtems_interrupt_lock_release(&target_lock, &lock_context);
  return 0;
}

int
rtems_debugger_target_read_regs(rtems_debugger_thread* thread)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  if (!rtems_debugger_thread_flag(thread,
                                  RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID)) {
    static const uintptr_t good_address = (uintptr_t) &good_address;
    size_t i;

    for (i = 0; i < rtems_debugger_target_reg_num(); ++i) {
      if (rtems_debugger_is_int_reg(i)) {
        rtems_debugger_set_int_reg(thread, i, (uintptr_t) &good_address);
      }
    }

    if (thread->frame) {
      CPU_Exception_frame* frame = thread->frame;
      rtems_debugger_set_int_reg(thread, REG_R0, frame->GPR0);
      rtems_debugger_set_int_reg(thread, REG_R1, frame->GPR1);
      rtems_debugger_set_int_reg(thread, REG_R2, frame->GPR2);
      rtems_debugger_set_int_reg(thread, REG_R3, frame->GPR3);
      rtems_debugger_set_int_reg(thread, REG_R4, frame->GPR4);
      rtems_debugger_set_int_reg(thread, REG_R5, frame->GPR5);
      rtems_debugger_set_int_reg(thread, REG_R6, frame->GPR6);
      rtems_debugger_set_int_reg(thread, REG_R7, frame->GPR7);
      rtems_debugger_set_int_reg(thread, REG_R8, frame->GPR8);
      rtems_debugger_set_int_reg(thread, REG_R9, frame->GPR9);
      rtems_debugger_set_int_reg(thread, REG_R10, frame->GPR10);
      rtems_debugger_set_int_reg(thread, REG_R11, frame->GPR11);
      rtems_debugger_set_int_reg(thread, REG_R12, frame->GPR12);
      rtems_debugger_set_int_reg(thread, REG_R13, frame->GPR13);
      rtems_debugger_set_int_reg(thread, REG_R14, frame->GPR14);
      rtems_debugger_set_int_reg(thread, REG_R15, frame->GPR15);
      rtems_debugger_set_int_reg(thread, REG_R16, frame->GPR16);
      rtems_debugger_set_int_reg(thread, REG_R17, frame->GPR17);
      rtems_debugger_set_int_reg(thread, REG_R18, frame->GPR18);
      rtems_debugger_set_int_reg(thread, REG_R19, frame->GPR19);
      rtems_debugger_set_int_reg(thread, REG_R20, frame->GPR20);
      rtems_debugger_set_int_reg(thread, REG_R21, frame->GPR21);
      rtems_debugger_set_int_reg(thread, REG_R22, frame->GPR22);
      rtems_debugger_set_int_reg(thread, REG_R23, frame->GPR23);
      rtems_debugger_set_int_reg(thread, REG_R24, frame->GPR24);
      rtems_debugger_set_int_reg(thread, REG_R25, frame->GPR25);
      rtems_debugger_set_int_reg(thread, REG_R26, frame->GPR26);
      rtems_debugger_set_int_reg(thread, REG_R27, frame->GPR27);
      rtems_debugger_set_int_reg(thread, REG_R28, frame->GPR28);
      rtems_debugger_set_int_reg(thread, REG_R29, frame->GPR29);
      rtems_debugger_set_int_reg(thread, REG_R30, frame->GPR30);
      rtems_debugger_set_int_reg(thread, REG_R31, frame->GPR31);

      rtems_debugger_set_int_reg(thread, REG_PC, frame->EXC_SRR0);
      rtems_debugger_set_int_reg(thread, REG_MSR, frame->EXC_SRR1);
      rtems_debugger_set_int_reg(thread, REG_CND, frame->EXC_CR);
      rtems_debugger_set_int_reg(thread, REG_LR, frame->EXC_LR);
      rtems_debugger_set_int_reg(thread, REG_CNT, frame->EXC_CTR);
      rtems_debugger_set_int_reg(thread, REG_XER, frame->EXC_XER);

      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal(frame);
    }
    else {
      ppc_context* thread_ctx = ppc_get_context(&thread->tcb->Registers);
      rtems_debugger_set_int_reg(thread, REG_R1,  thread_ctx->gpr1);
      rtems_debugger_set_int_reg(thread, REG_R14, thread_ctx->gpr14);
      rtems_debugger_set_int_reg(thread, REG_R15, thread_ctx->gpr15);
      rtems_debugger_set_int_reg(thread, REG_R16, thread_ctx->gpr16);
      rtems_debugger_set_int_reg(thread, REG_R17, thread_ctx->gpr17);
      rtems_debugger_set_int_reg(thread, REG_R18, thread_ctx->gpr18);
      rtems_debugger_set_int_reg(thread, REG_R19, thread_ctx->gpr19);
      rtems_debugger_set_int_reg(thread, REG_R20, thread_ctx->gpr20);
      rtems_debugger_set_int_reg(thread, REG_R21, thread_ctx->gpr21);
      rtems_debugger_set_int_reg(thread, REG_R22, thread_ctx->gpr22);
      rtems_debugger_set_int_reg(thread, REG_R23, thread_ctx->gpr23);
      rtems_debugger_set_int_reg(thread, REG_R24, thread_ctx->gpr24);
      rtems_debugger_set_int_reg(thread, REG_R25, thread_ctx->gpr25);
      rtems_debugger_set_int_reg(thread, REG_R26, thread_ctx->gpr26);
      rtems_debugger_set_int_reg(thread, REG_R27, thread_ctx->gpr27);
      rtems_debugger_set_int_reg(thread, REG_R28, thread_ctx->gpr28);
      rtems_debugger_set_int_reg(thread, REG_R29, thread_ctx->gpr29);
      rtems_debugger_set_int_reg(thread, REG_R30, thread_ctx->gpr30);
      rtems_debugger_set_int_reg(thread, REG_R31, thread_ctx->gpr31);

      rtems_debugger_set_int_reg(thread, REG_PC, thread_ctx->lr);
      rtems_debugger_set_int_reg(thread, REG_MSR, thread_ctx->msr);
      rtems_debugger_set_int_reg(thread, REG_CND, thread_ctx->cr);
      rtems_debugger_set_int_reg(thread, REG_LR, thread_ctx->lr);
      rtems_debugger_set_int_reg(thread, REG_CNT, 0);
      rtems_debugger_set_int_reg(thread, REG_XER, 0);
      rtems_debugger_set_int_reg(thread, REG_SPEFSCR, 0);

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
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  if (rtems_debugger_thread_flag(thread,
                                 RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY)) {
    /*
     * Only write to debugger controlled threads. Do not touch the registers
     * for threads blocked in the context switcher.
     */
    if (rtems_debugger_thread_flag(thread,
                                   RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION)) {
      CPU_Exception_frame* frame = thread->frame;
      frame->GPR0 = rtems_debugger_get_int_reg(thread, REG_R0);
      frame->GPR1 = rtems_debugger_get_int_reg(thread, REG_R1);
      frame->GPR2 = rtems_debugger_get_int_reg(thread, REG_R2);
      frame->GPR3 = rtems_debugger_get_int_reg(thread, REG_R3);
      frame->GPR4 = rtems_debugger_get_int_reg(thread, REG_R4);
      frame->GPR5 = rtems_debugger_get_int_reg(thread, REG_R5);
      frame->GPR6 = rtems_debugger_get_int_reg(thread, REG_R6);
      frame->GPR7 = rtems_debugger_get_int_reg(thread, REG_R7);
      frame->GPR8 = rtems_debugger_get_int_reg(thread, REG_R8);
      frame->GPR9 = rtems_debugger_get_int_reg(thread, REG_R9);
      frame->GPR10 = rtems_debugger_get_int_reg(thread, REG_R10);
      frame->GPR11 = rtems_debugger_get_int_reg(thread, REG_R11);
      frame->GPR12 = rtems_debugger_get_int_reg(thread, REG_R12);
      frame->GPR13 = rtems_debugger_get_int_reg(thread, REG_R13);
      frame->GPR14 = rtems_debugger_get_int_reg(thread, REG_R14);
      frame->GPR15 = rtems_debugger_get_int_reg(thread, REG_R15);
      frame->GPR16 = rtems_debugger_get_int_reg(thread, REG_R16);
      frame->GPR17 = rtems_debugger_get_int_reg(thread, REG_R17);
      frame->GPR18 = rtems_debugger_get_int_reg(thread, REG_R18);
      frame->GPR19 = rtems_debugger_get_int_reg(thread, REG_R19);
      frame->GPR20 = rtems_debugger_get_int_reg(thread, REG_R20);
      frame->GPR21 = rtems_debugger_get_int_reg(thread, REG_R21);
      frame->GPR22 = rtems_debugger_get_int_reg(thread, REG_R22);
      frame->GPR23 = rtems_debugger_get_int_reg(thread, REG_R23);
      frame->GPR24 = rtems_debugger_get_int_reg(thread, REG_R24);
      frame->GPR25 = rtems_debugger_get_int_reg(thread, REG_R25);
      frame->GPR26 = rtems_debugger_get_int_reg(thread, REG_R26);
      frame->GPR27 = rtems_debugger_get_int_reg(thread, REG_R27);
      frame->GPR28 = rtems_debugger_get_int_reg(thread, REG_R28);
      frame->GPR29 = rtems_debugger_get_int_reg(thread, REG_R29);
      frame->GPR30 = rtems_debugger_get_int_reg(thread, REG_R30);
      frame->GPR31 = rtems_debugger_get_int_reg(thread, REG_R31);

      frame->EXC_SRR0 = rtems_debugger_get_int_reg(thread, REG_PC);
      frame->EXC_SRR1 = rtems_debugger_get_int_reg(thread, REG_MSR);
      frame->EXC_CR = rtems_debugger_get_int_reg(thread, REG_CND);
      frame->EXC_LR = rtems_debugger_get_int_reg(thread, REG_LR);
      frame->EXC_CTR = rtems_debugger_get_int_reg(thread, REG_CNT);
      frame->EXC_XER = rtems_debugger_get_int_reg(thread, REG_XER);
    }
    thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
  }
  return 0;
}

uintptr_t
rtems_debugger_target_reg_pc(rtems_debugger_thread* thread)
{
  int r;
  r = rtems_debugger_target_read_regs(thread);
  if (r >= 0) {
    return rtems_debugger_get_int_reg(thread, REG_PC);
  }
  return 0;
}

uintptr_t
rtems_debugger_target_frame_pc(CPU_Exception_frame* frame)
{
  return (uintptr_t) frame->EXC_SRR0;
}

uintptr_t
rtems_debugger_target_reg_sp(rtems_debugger_thread* thread)
{
  int r;
  r = rtems_debugger_target_read_regs(thread);
  if (r >= 0) {
    return rtems_debugger_get_int_reg(thread, REG_R1);
  }
  return 0;
}

uintptr_t
rtems_debugger_target_tcb_sp(rtems_debugger_thread* thread)
{
  ppc_context* thread_ctx = ppc_get_context(&thread->tcb->Registers);
  return (DB_UINT) thread_ctx->gpr1;
}

int
rtems_debugger_target_thread_stepping(rtems_debugger_thread* thread)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  if (rtems_debugger_thread_flag(thread,
                                 RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR)) {
    CPU_Exception_frame* frame = thread->frame;
    /*
     * There maybe no frame, ie connect and then enter `si`
     */
    if (frame != NULL) {
      /*
       * Single step instructions with interrupts masked to avoid
       * stepping into an interrupt handler.
       */
      if ((frame->EXC_SRR1 & MSR_EE) == 0) {
        thread->flags |= RTEMS_DEBUGGER_THREAD_FLAG_INTS_DISABLED;
      }
      powerpc_stepping_frame(frame);
    }
  }
  return 0;
}

int
rtems_debugger_target_exception_to_signal(CPU_Exception_frame* frame)
{
  int sig = RTEMS_DEBUGGER_SIGNAL_HUP;
  switch (frame->_EXC_number) {
  case ASM_MACH_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_BUS;
    break;
  case ASM_PROT_VECTOR:
  case ASM_ISI_VECTOR:
  case ASM_ALIGN_VECTOR:
  case ASM_60X_IMISS_VECTOR:
  case ASM_60X_DLMISS_VECTOR:
  case ASM_60X_DSMISS_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_SEGV;
    break;
  case ASM_PROG_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_TRAP;
    break;
  case ASM_FLOAT_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_FPE;
    break;
  case ASM_DEC_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_ALRM;
    break;
  case ASM_SYS_VECTOR:
  case ASM_TRACE_VECTOR:
    sig = RTEMS_DEBUGGER_SIGNAL_TRAP;
    break;
  default:
    break;
  }
  return sig;
}

void
rtems_debugger_target_exception_print(CPU_Exception_frame* frame)
{
  uintptr_t* gpr = &frame->GPR0;
  int r = 0;
#ifndef __SPE__
  rtems_debugger_printf("SRR0 = %08" PRIx32 " SRR1 = %08" PRIx32 "\n",
                        frame->EXC_SRR0, frame->EXC_SRR1);
#else
  rtems_debugger_printf("SRR0 = %08" PRIx32 " SRR1 = %08" PRIx32 \
                        " SPEFSCR = %08" PRIx32 " ACC = %08" PRIx32 "\n",
                        frame->EXC_SRR0, frame->EXC_SRR1,
                        frame->EXC_SPEFSCR, frame->EXC_ACC);
#endif
  rtems_debugger_printf("LR = %08" PRIx32 " CR = %08" PRIx32 \
                        " XER = %08" PRIx32 " CTR = %08" PRIx32 "\n",
                        frame->EXC_LR, frame->EXC_CR,
                        frame->EXC_XER, frame->EXC_CTR);
  gpr = &frame->GPR0;
  for (r= 0; r < 32; r += 4, gpr += 4) {
    rtems_debugger_printf("R%-2d = %08" PRIx32 " R%-2d = %08" PRIx32     \
                  " R%-2d = %08" PRIx32 " R%-2d = %08" PRIx32 "\n",
                  r, *gpr, r + 1, *(gpr + 1), r + 2,
                  *(gpr + 2), r + 3, *(gpr + 3));
  }
}

int
rtems_debugger_target_hwbreak_insert(void)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  /*
   * Do nothing, load on exit of the exception handler.
   */
  return 0;
}

int
rtems_debugger_target_hwbreak_remove(void)
{
  target_printk("]] rtems-db: powerpc: %s\n", __func__);
  return 0;
}

int
rtems_debugger_target_hwbreak_control(rtems_debugger_target_watchpoint wp,
                                      bool                             insert,
                                      uintptr_t                        addr,
                                      DB_UINT                          kind)
{
  (void) wp;
  (void) insert;
  (void) addr;
  (void) kind;

  target_printk("]] rtems-db: powerpc: %s\n", __func__);
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
  rtems_cache_flush_multiple_data_lines(
    swbreak->address,
    sizeof( breakpoint )
  );
  rtems_cache_instruction_sync_after_code_change(
    swbreak->address,
    sizeof( breakpoint )
  );
  return 0;
}
