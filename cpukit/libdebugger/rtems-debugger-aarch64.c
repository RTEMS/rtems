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

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

/* Defined by linkcmds.base. This should be taken from <bsp/linker-symbols.h> */
extern char bsp_section_start_begin[];
extern char bsp_section_start_end[];
extern char bsp_section_text_begin[];
extern char bsp_section_text_end[];
extern char bsp_section_fast_text_begin[];
extern char bsp_section_fast_text_end[];

#include <libcpu/mmu-vmsav8-64.h>

#include <rtems.h>
#include <rtems/score/aarch64-system-registers.h>
#include <rtems/score/cpu.h>
#include <rtems/score/threadimpl.h>

#include <rtems/debugger/rtems-debugger-bsp.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

#if TARGET_DEBUG
#include <rtems/bspIo.h>
#endif

/*
 * Structure used to manage a task executing a function on available cores on
 * a scheduler.
 */
typedef struct {
  rtems_id allCPUsBarrier;
  rtems_task_entry work_function;
  rtems_task_argument arg;
  rtems_status_code sc;
} run_across_cpus_context;

/*
 * The function that runs as the body of the task which moves itself among the
 * various cores registered to a scheduler.
 */
static rtems_task run_across_cpus_task( rtems_task_argument arg )
{
  uint32_t                 released = 0;
  rtems_status_code        sc;
  run_across_cpus_context *ctx = (run_across_cpus_context *) arg;
  cpu_set_t                set;
  cpu_set_t                scheduler_set;
  rtems_id                 scheduler_id;

  sc = rtems_task_get_scheduler( RTEMS_SELF, &scheduler_id );

  if ( sc != RTEMS_SUCCESSFUL ) {
    ctx->sc = sc;
    rtems_task_exit();
  }

  CPU_ZERO( &scheduler_set );
  sc = rtems_scheduler_get_processor_set(
    scheduler_id,
    sizeof( scheduler_set ),
    &scheduler_set
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    ctx->sc = sc;
    rtems_task_exit();
  }

  for (
    int cpu_index = 0;
    cpu_index < rtems_scheduler_get_processor_maximum();
    cpu_index++
  ) {
    if ( !CPU_ISSET( cpu_index, &scheduler_set ) ) {
      continue;
    }

    CPU_ZERO( &set );
    CPU_SET( cpu_index, &set );
    sc = rtems_task_set_affinity( RTEMS_SELF, sizeof( set ), &set );

    if ( sc != RTEMS_SUCCESSFUL ) {
      ctx->sc = sc;
      rtems_task_exit();
    }

    /* execute task on selected CPU */
    ctx->work_function( ctx->arg );
  }

  sc = rtems_barrier_release( ctx->allCPUsBarrier, &released );

  if ( sc != RTEMS_SUCCESSFUL ) {
    ctx->sc = sc;
  }

  rtems_task_exit();
}

/*
 * The function used to run a provided function with arbitrary argument across
 * all cores registered to the current scheduler. This is similar to the Linux
 * kernel's on_each_cpu() call and always waits for the task to complete before
 * returning.
 */
static rtems_status_code run_across_cpus(
  rtems_task_entry    task_entry,
  rtems_task_argument arg
)
{
  rtems_status_code       sc;
  rtems_id                Task_id;
  run_across_cpus_context ctx;

  ctx.work_function = task_entry;
  ctx.arg = arg;
  ctx.sc = RTEMS_SUCCESSFUL;

  memset( &ctx.allCPUsBarrier, 0, sizeof( ctx.allCPUsBarrier ) );
  sc = rtems_barrier_create(
    rtems_build_name( 'B', 'c', 'p', 'u' ),
    RTEMS_BARRIER_MANUAL_RELEASE,
    2,
    &ctx.allCPUsBarrier
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    return sc;
  }

  sc = rtems_task_create(
    rtems_build_name( 'T', 'c', 'p', 'u' ),
    1,
    RTEMS_MINIMUM_STACK_SIZE * 2,
    RTEMS_DEFAULT_MODES,
    RTEMS_FLOATING_POINT | RTEMS_DEFAULT_ATTRIBUTES,
    &Task_id
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_barrier_delete( ctx.allCPUsBarrier );
    return sc;
  }

  sc = rtems_task_start(
    Task_id,
    run_across_cpus_task,
    ( rtems_task_argument ) & ctx
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_task_delete( Task_id );
    rtems_barrier_delete( ctx.allCPUsBarrier );
    return sc;
  }

  /* wait on task */
  sc = rtems_barrier_wait( ctx.allCPUsBarrier, RTEMS_NO_TIMEOUT );

  if ( sc != RTEMS_SUCCESSFUL ) {
    rtems_task_delete( Task_id );
    rtems_barrier_delete( ctx.allCPUsBarrier );
    return sc;
  }

  rtems_barrier_delete( ctx.allCPUsBarrier );

  if ( ctx.sc != RTEMS_SUCCESSFUL ) {
    return ctx.sc;
  }

  return sc;
}

/*
 * Number of registers.
 */
#define RTEMS_DEBUGGER_NUMREGS 68

/*
 * Number of bytes per type of register.
 */
#define RTEMS_DEBUGGER_REG_BYTES    8

/* Debugger registers layout. See aarch64-core.xml in GDB source. */
#define REG_X0    0
#define REG_X1    1
#define REG_X2    2
#define REG_X3    3
#define REG_X4    4
#define REG_X5    5
#define REG_X6    6
#define REG_X7    7
#define REG_X8    8
#define REG_X9    9
#define REG_X10   10
#define REG_X11   11
#define REG_X12   12
#define REG_X13   13
#define REG_X14   14
#define REG_X15   15
#define REG_X16   16
#define REG_X17   17
#define REG_X18   18
#define REG_X19   19
#define REG_X20   20
#define REG_X21   21
#define REG_X22   22
#define REG_X23   23
#define REG_X24   24
#define REG_X25   25
#define REG_X26   26
#define REG_X27   27
#define REG_X28   28
#define REG_FP    29
#define REG_LR    30
#define REG_SP    31
/*
 * PC isn't a real directly accessible register on AArch64, but is exposed via
 * ELR_EL1 in exception context.
 */
#define REG_PC    32
/* CPSR is defined as 32-bit by GDB */
#define REG_CPS   33
/* Debugger registers layout. See aarch64-fpu.xml in GDB source. */
#define REG_V0    34
#define REG_V1    35
#define REG_V2    36
#define REG_V3    37
#define REG_V4    38
#define REG_V5    39
#define REG_V6    40
#define REG_V7    41
#define REG_V8    42
#define REG_V9    43
#define REG_V10   44
#define REG_V11   45
#define REG_V12   46
#define REG_V13   47
#define REG_V14   48
#define REG_V15   49
#define REG_V16   50
#define REG_V17   51
#define REG_V18   52
#define REG_V19   53
#define REG_V20   54
#define REG_V21   55
#define REG_V22   56
#define REG_V23   57
#define REG_V24   58
#define REG_V25   59
#define REG_V26   60
#define REG_V27   61
#define REG_V28   62
#define REG_V29   63
#define REG_V30   64
#define REG_V31   65
/* FPSR and FPCR are defined as 32-bit by GDB */
#define REG_FPS   66
#define REG_FPC   67

/**
 * Register offset table with the total as the last entry.
 *
 * Check this table in gdb with the command:
 *
 *   maint print registers
 */
static const size_t aarch64_reg_offsets[ RTEMS_DEBUGGER_NUMREGS + 1 ] = {
  REG_X0 * 8,
  REG_X1 * 8,
  REG_X2 * 8,
  REG_X3 * 8,
  REG_X4 * 8,
  REG_X5 * 8,
  REG_X6 * 8,
  REG_X7 * 8,
  REG_X8 * 8,
  REG_X9 * 8,
  REG_X10 * 8,
  REG_X11 * 8,
  REG_X12 * 8,
  REG_X13 * 8,
  REG_X14 * 8,
  REG_X15 * 8,
  REG_X16 * 8,
  REG_X17 * 8,
  REG_X18 * 8,
  REG_X19 * 8,
  REG_X20 * 8,
  REG_X21 * 8,
  REG_X22 * 8,
  REG_X23 * 8,
  REG_X24 * 8,
  REG_X25 * 8,
  REG_X26 * 8,
  REG_X27 * 8,
  REG_X28 * 8,
  REG_FP * 8,
  REG_LR * 8,
  REG_SP * 8,
  REG_PC * 8,
  REG_CPS * 8,
/* Floating point registers, CPS is 32-bit */
#define V0_OFFSET ( REG_CPS * 8 + 4 )
  V0_OFFSET,
  V0_OFFSET + 16 * ( REG_V1 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V2 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V3 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V4 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V5 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V6 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V7 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V8 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V9 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V10 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V11 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V12 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V13 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V14 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V15 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V16 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V17 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V18 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V19 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V20 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V21 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V22 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V23 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V24 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V25 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V26 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V27 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V28 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V29 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V30 - REG_V0 ),
  V0_OFFSET + 16 * ( REG_V31 - REG_V0 ),
/* FPSR and FPCR are defined as 32-bit by GDB */
#define FPS_OFFSET ( V0_OFFSET + 16 * ( REG_V31 - REG_V0 ) + 16 )
  FPS_OFFSET,
/* FPC follows FPS */
  FPS_OFFSET + 4,
/* Total size */
  FPS_OFFSET + 8,
};

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES \
  aarch64_reg_offsets[ RTEMS_DEBUGGER_NUMREGS ]

/**
 * Print the exception frame.
 */
#define EXC_FRAME_PRINT( _out, _prefix, _frame ) \
  do { \
    _out( \
      _prefix "  X0 = %016" PRIx64 "  X1 = %016" PRIx64 \
      "  X2 = %016" PRIx64 "  X3 = %016" PRIx64 "\n", \
      _frame->register_x0, \
      _frame->register_x1, \
      _frame->register_x2, \
      _frame->register_x3 \
    ); \
    _out( \
      _prefix "  X4 = %016" PRIx64 "  X5 = %016" PRIx64 \
      "  X6 = %016" PRIx64 "  X7 = %016" PRIx64 "\n", \
      _frame->register_x4, \
      _frame->register_x5, \
      _frame->register_x6, \
      _frame->register_x7 \
    ); \
    _out( \
      _prefix "  X8 = %016" PRIx64 "  X9 = %016" PRIx64 \
      " X10 = %016" PRIx64 " X11 = %016" PRIx64 "\n", \
      _frame->register_x8, \
      _frame->register_x9, \
      _frame->register_x10, \
      _frame->register_x11 \
    ); \
    _out( \
      _prefix " X12 = %016" PRIx64 " X13 = %016" PRIx64 \
      " X14 = %016" PRIx64 " X15 = %016" PRIx64 "\n", \
      _frame->register_x12, \
      _frame->register_x13, \
      _frame->register_x14, \
      _frame->register_x15 \
    ); \
    _out( \
      _prefix " X16 = %016" PRIx64 " X17 = %016" PRIx64 \
      " X18 = %016" PRIx64 " X19 = %016" PRIx64 "\n", \
      _frame->register_x16, \
      _frame->register_x17, \
      _frame->register_x18, \
      _frame->register_x19 \
    ); \
    _out( \
      _prefix " X20 = %016" PRIx64 " X21 = %016" PRIx64 \
      " X22 = %016" PRIx64 " X23 = %016" PRIx64 "\n", \
      _frame->register_x20, \
      _frame->register_x21, \
      _frame->register_x22, \
      _frame->register_x23 \
    ); \
    _out( \
      _prefix " X24 = %016" PRIx64 " X25 = %016" PRIx64 \
      " X26 = %016" PRIx64 " X27 = %016" PRIx64 "\n", \
      _frame->register_x24, \
      _frame->register_x25, \
      _frame->register_x26, \
      _frame->register_x27 \
    ); \
    _out( \
      _prefix " X28 = %016" PRIx64 "  FP = %016" PRIx64 \
      "  LR = %016" PRIxPTR "  SP = %016" PRIxPTR "\n", \
      _frame->register_x28, \
      _frame->register_fp, \
      (intptr_t) _frame->register_lr, \
      (intptr_t) _frame->register_sp \
    ); \
    _out( \
      _prefix "  PC = %016" PRIxPTR "\n", \
      (intptr_t) _frame->register_pc \
    ); \
    _out( \
      _prefix " CPSR = %08" PRIx64 " %c%c%c%c%c%c%c%c%c" \
      " M:%" PRIx64 " %s\n", \
      _frame->register_cpsr, \
      ( _frame->register_cpsr & ( 1 << 31 ) ) != 0 ? 'N' : '-', \
      ( _frame->register_cpsr & ( 1 << 30 ) ) != 0 ? 'Z' : '-', \
      ( _frame->register_cpsr & ( 1 << 29 ) ) != 0 ? 'C' : '-', \
      ( _frame->register_cpsr & ( 1 << 28 ) ) != 0 ? 'V' : '-', \
      ( _frame->register_cpsr & ( 1 << 21 ) ) != 0 ? 'S' : '-', \
      ( _frame->register_cpsr & ( 1 << 9 ) ) != 0 ? 'D' : '-', \
      ( _frame->register_cpsr & ( 1 << 8 ) ) != 0 ? 'A' : '-', \
      ( _frame->register_cpsr & ( 1 << 7 ) ) != 0 ? 'I' : '-', \
      ( _frame->register_cpsr & ( 1 << 6 ) ) != 0 ? 'F' : '-', \
      _frame->register_cpsr & 0x1f, \
      aarch64_mode_label( _frame->register_cpsr & 0x1f ) \
    ); \
  } while ( 0 )

/**
 * The breakpoint instruction.
 */
static const uint8_t breakpoint[ 4 ] = { 0x00, 0x00, 0x20, 0xd4 };

/**
 * Target lock.
 */
RTEMS_INTERRUPT_LOCK_DEFINE( static, target_lock, "target_lock" )

/**
 * Is a session active?
 */
static bool debug_session_active;

/*
 * AArch64 debug hardware.
 */
static uint64_t hw_breakpoints;
static uint64_t hw_watchpoints;

#ifdef HARDWARE_BREAKPOINTS_NOT_USED
/**
 * Hardware break and watch points.
 */
typedef struct {
  bool enabled;
  bool loaded;
  void *address;
  size_t length;
  CPU_Exception_frame *frame;
  uint64_t control;
  uint64_t value;
} aarch64_debug_hwbreak;

/*
 * AArch64 guarantees that 2-16 breakpoints will be available in:
 * DBGBCR<0-15>_EL1 (control)
 *   BT: BSP_FLD64(val, 20, 23) (breakpoint type, always 0x0 or 0x4, address match or mismatch)
 *   LBN: BSP_FLD64(val, 16, 19) (linked breakpoint number, always 0x0, not relevant given above)
 *   SSC: BSP_FLD64(val, 14, 15) (security state control, only 0x0 relevant)
 *   HMC: BSP_BIT64(13) (higher mode control, only 0x0 relevant)
 *   BAS: BSP_FLD64(val, 5, 8) (byte address select, always 0xF, other values denote debugging of AArch32 code)
 *   PMC: BSP_FLD64(val, 1, 2) (privelege mode control, only 0x1 relevant)
 *   E: BSP_BIT64(0) (enable, 0x1 when in use, 0x0 when disabled)
 * DBGBVR<0-15>_EL1 (value, address)
 * ID_AA64DFR0_EL1
 *   WRPs: BSP_FLD64(val, 20, 23) (watchpoints implemented - 1, 0x0 reserved so minimum 2)
 *   BRPs: BSP_FLD64(val, 12, 15) (breakpoints implemented - 1, 0x0 reserved so minimum 2)
 *   DebugVer: BSP_FLD64(val, 0, 3) (0x6 - 8, 0x7 - 8 w/ VHE, 0x8 - 8.2, 0x9 - 8.4)
 */
#define AARCH64_HW_BREAKPOINT_MAX ( 16 )

/*
 * Types of break points.
 */
#define AARCH64_HW_BP_TYPE_UNLINKED_INSTR_MATCH ( 0x0 << 20 )
#define AARCH64_HW_BP_TYPE_UNLINKED_INSTR_MISMATCH ( 0x4 << 20 )

/*
 * Byte Address Select
 */
#define AARCH64_HW_BP_BAS_A64 ( 0xF << 5 )

/*
 * Privilege level, corresponds to PMC at 2:1
 */
#define AARCH64_HW_BP_PRIV_EL1 ( 0x1 << 1 )

/*
 * Breakpoint enable.
 */
#define AARCH64_HW_BP_ENABLE ( 0x1 )

static aarch64_debug_hwbreak hw_breaks[ AARCH64_HW_BREAKPOINT_MAX ];
#endif

/*
 * Target debugging support. Use this to debug the backend.
 */
#if TARGET_DEBUG

void rtems_debugger_printk_lock( rtems_interrupt_lock_context *lock_context );

void rtems_debugger_printk_unlock(
  rtems_interrupt_lock_context *lock_context
);

static void target_printk( const char *format, ... ) RTEMS_PRINTFLIKE( 1, 2 );

static void target_printk( const char *format, ... )
{
  rtems_interrupt_lock_context lock_context;
  va_list                      ap;

  va_start( ap, format );
  rtems_debugger_printk_lock( &lock_context );
  vprintk( format, ap );
  rtems_debugger_printk_unlock( &lock_context );
  va_end( ap );
}

#else
#define target_printk( _fmt, ... )
#endif

static const char *aarch64_mode_label( int mode )
{
  switch ( mode ) {
   case 0x0:
     return "EL0t";
   case 0x4:
     return "EL1t";
   case 0x5:
     return "EL1h";
  }

  return "---";
}

static int aarch64_debug_probe( rtems_debugger_target *target )
{
  int                debug_version;
  uint64_t           val;
  const char        *vl = "[Invalid version]";
  const char * const labels[] = {
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    NULL,
    "ARMv8.0",
    "ARMv8.0+VHE",
    "ARMv8.2",
    "ARMv8.4"
  };

  val = _AArch64_Read_midr_el1();
  rtems_debugger_printf(
    "rtems-db: aarch64 core: Architecture: %" PRIu64 " Variant: %" PRIu64 " " \
    "Implementor: %" PRIu64 " Part Number: %" PRIu64 " Revision: %" PRIu64 "\n",
    AARCH64_MIDR_EL1_ARCHITECTURE_GET( val ),
    AARCH64_MIDR_EL1_VARIANT_GET( val ),
    AARCH64_MIDR_EL1_IMPLEMENTER_GET( val ),
    AARCH64_MIDR_EL1_PARTNUM_GET( val ),
    AARCH64_MIDR_EL1_REVISION_GET( val )
  );

  val = _AArch64_Read_id_aa64dfr0_el1();

  debug_version = AARCH64_ID_AA64DFR0_EL1_DEBUGVER_GET( val );

  if ( debug_version < 6 || debug_version > 9 ) {
    rtems_debugger_printf(
      "rtems-db: aarch64 debug: %d not supported\n",
      debug_version
    );
    errno = EIO;
    return -1;
  }

  vl = labels[ debug_version ];
  hw_breakpoints = AARCH64_ID_AA64DFR0_EL1_BRPS_GET( val );
  hw_watchpoints = AARCH64_ID_AA64DFR0_EL1_WRPS_GET( val );

  rtems_debugger_printf(
    "rtems-db: aarch64 debug: %s (%d) " \
    "breakpoints:%" PRIu64 " watchpoints:%" PRIu64 "\n",
    vl,
    debug_version,
    hw_breakpoints,
    hw_watchpoints
  );

  return 0;
}

#ifdef HARDWARE_BREAKPOINTS_NOT_USED
static void aarch64_debug_break_write_control( int bp, uint64_t control )
{
  if ( bp < 15 ) {
    switch ( bp ) {
     case 0:
       _AArch64_Write_dbgbcr0_el1( control );
       break;
     case 1:
       _AArch64_Write_dbgbcr1_el1( control );
       break;
     case 2:
       _AArch64_Write_dbgbcr2_el1( control );
       break;
     case 3:
       _AArch64_Write_dbgbcr3_el1( control );
       break;
     case 4:
       _AArch64_Write_dbgbcr4_el1( control );
       break;
     case 5:
       _AArch64_Write_dbgbcr5_el1( control );
       break;
     case 6:
       _AArch64_Write_dbgbcr6_el1( control );
       break;
     case 7:
       _AArch64_Write_dbgbcr7_el1( control );
       break;
     case 8:
       _AArch64_Write_dbgbcr8_el1( control );
       break;
     case 9:
       _AArch64_Write_dbgbcr9_el1( control );
       break;
     case 10:
       _AArch64_Write_dbgbcr10_el1( control );
       break;
     case 11:
       _AArch64_Write_dbgbcr11_el1( control );
       break;
     case 12:
       _AArch64_Write_dbgbcr12_el1( control );
       break;
     case 13:
       _AArch64_Write_dbgbcr13_el1( control );
       break;
     case 14:
       _AArch64_Write_dbgbcr14_el1( control );
       break;
     case 15:
       _AArch64_Write_dbgbcr15_el1( control );
       break;
    }
  }
}

static void aarch64_debug_break_write_value( int bp, uint64_t value )
{
  if ( bp < 15 ) {
    switch ( bp ) {
     case 0:
       _AArch64_Write_dbgbvr0_el1( value );
       break;
     case 1:
       _AArch64_Write_dbgbvr1_el1( value );
       break;
     case 2:
       _AArch64_Write_dbgbvr2_el1( value );
       break;
     case 3:
       _AArch64_Write_dbgbvr3_el1( value );
       break;
     case 4:
       _AArch64_Write_dbgbvr4_el1( value );
       break;
     case 5:
       _AArch64_Write_dbgbvr5_el1( value );
       break;
     case 6:
       _AArch64_Write_dbgbvr6_el1( value );
       break;
     case 7:
       _AArch64_Write_dbgbvr7_el1( value );
       break;
     case 8:
       _AArch64_Write_dbgbvr8_el1( value );
       break;
     case 9:
       _AArch64_Write_dbgbvr9_el1( value );
       break;
     case 10:
       _AArch64_Write_dbgbvr10_el1( value );
       break;
     case 11:
       _AArch64_Write_dbgbvr11_el1( value );
       break;
     case 12:
       _AArch64_Write_dbgbvr12_el1( value );
       break;
     case 13:
       _AArch64_Write_dbgbvr13_el1( value );
       break;
     case 14:
       _AArch64_Write_dbgbvr14_el1( value );
       break;
     case 15:
       _AArch64_Write_dbgbvr15_el1( value );
       break;
    }
  }
}

static inline void aarch64_debug_break_setup(
  uint8_t  index,
  uint64_t address
)
{
  aarch64_debug_hwbreak *bp = &hw_breaks[ index ];

  bp->control = AARCH64_HW_BP_TYPE_UNLINKED_INSTR_MISMATCH |
                AARCH64_HW_BP_BAS_A64 |
                AARCH64_HW_BP_PRIV_EL1 |
                AARCH64_HW_BP_ENABLE;
  uint64_t address_mask = 0x3;

  bp->value = (intptr_t) ( address & ~address_mask );
  aarch64_debug_break_write_value( index, bp->value );
  aarch64_debug_break_write_control( index, bp->control );
}

static void aarch64_debug_break_clear( void )
{
  rtems_interrupt_lock_context lock_context;
  aarch64_debug_hwbreak       *bp = &hw_breaks[ 0 ];
  int                          i;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );

  for ( i = 0; i < hw_breakpoints; ++i, ++bp ) {
    bp->enabled = false;
    bp->loaded = false;
  }

  rtems_interrupt_lock_release( &target_lock, &lock_context );
}

static void aarch64_debug_break_load( void )
{
  rtems_interrupt_lock_context lock_context;
  aarch64_debug_hwbreak       *bp = &hw_breaks[ 0 ];
  int                          i;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );

  if ( bp->enabled && !bp->loaded ) {
    aarch64_debug_set_context_id( 0xdead1111 );
    aarch64_debug_break_write_value( 0, bp->value );
    aarch64_debug_break_write_control( 0, bp->control );
  }

  ++bp;

  for ( i = 1; i < hw_breakpoints; ++i, ++bp ) {
    if ( bp->enabled && !bp->loaded ) {
      bp->loaded = true;
      aarch64_debug_break_write_value( i, bp->value );
      aarch64_debug_break_write_control( i, bp->control );
    }
  }

  rtems_interrupt_lock_release( &target_lock, &lock_context );
}

static void aarch64_debug_break_unload( void )
{
  rtems_interrupt_lock_context lock_context;
  aarch64_debug_hwbreak       *bp = &hw_breaks[ 0 ];
  int                          i;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );
  aarch64_debug_set_context_id( 0 );

  for ( i = 0; i < hw_breakpoints; ++i, ++bp ) {
    bp->loaded = false;
    aarch64_debug_break_write_control( i, 0 );
  }

  rtems_interrupt_lock_release( &target_lock, &lock_context );
}

static void aarch64_debug_break_dump( void )
{
#if TARGET_DEBUG
  aarch64_debug_hwbreak *bp = &hw_breaks[ 0 ];
  int                    i;

  for ( i = 0; i < hw_breakpoints; ++i, ++bp ) {
    if ( bp->enabled ) {
      target_printk(
        "[} bp: %d: control: %016" PRIx64 " addr: %016" PRIxPTR "\n",
        i,
        bp->control,
        (uintptr_t) bp->value
      );
    }
  }

#endif
}
#endif

static void aarch64_debug_disable_interrupts( void )
{
  __asm__ volatile ( "msr DAIFSet, #0x2" );
}

static void aarch64_debug_enable_interrupts( void )
{
  __asm__ volatile ( "msr DAIFClr, #2\n" );
}

static void aarch64_debug_disable_debug_exceptions( void )
{
  __asm__ volatile ( "msr DAIFSet, #0x8" );
}

static inline void aarch64_debug_set_context_id( const uint32_t id )
{
  _AArch64_Write_contextidr_el1( id );
}

int rtems_debugger_target_configure( rtems_debugger_target *target )
{
  target->capabilities = ( RTEMS_DEBUGGER_TARGET_CAP_SWBREAK );
  target->reg_num = RTEMS_DEBUGGER_NUMREGS;
  target->reg_offset = aarch64_reg_offsets;
  target->breakpoint = &breakpoint[ 0 ];
  target->breakpoint_size = sizeof( breakpoint );
  return aarch64_debug_probe( target );
}

static void target_print_frame( CPU_Exception_frame *frame )
{
  EXC_FRAME_PRINT( target_printk, "[} ", frame );
}

/* returns true if cascade is required */
static bool target_exception( CPU_Exception_frame *frame )
{
  target_printk(
    "[} > frame = %016" PRIxPTR \
    " sig=%d" \
    " pra=%016" PRIxPTR "\n" \
    "[} >   esr=%016" PRIx64 \
    " far=%016" PRIxPTR "\n",
    (uintptr_t) frame,
    rtems_debugger_target_exception_to_signal( frame ),
    (uintptr_t) frame->register_pc,
    (uint64_t) frame->register_syndrome,
    (uintptr_t) frame->register_fault_address
  );

  target_print_frame( frame );

  switch ( rtems_debugger_target_exception( frame ) ) {
   case rtems_debugger_target_exc_consumed:
   default:
     break;
   case rtems_debugger_target_exc_step:
     break;
   case rtems_debugger_target_exc_cascade:
     target_printk( "rtems-db: unhandled exception: cascading\n" );
     /* Continue in fatal error handler chain */
     return true;
     break;
  }

  target_printk(
    "[} < resuming frame = %016" PRIxPTR "\n",
    (uintptr_t) frame
  );
  target_print_frame( frame );

#if TARGET_DEBUG
  uint64_t mdscr = _AArch64_Read_mdscr_el1();
#endif
  target_printk(
    "[} global stepping: %s\n",
    mdscr & AARCH64_MDSCR_EL1_SS ? "yes" : "no"
  );
  target_printk(
    "[} kernel self-debug: %s\n",
    mdscr & AARCH64_MDSCR_EL1_KDE ? "yes" : "no"
  );
  target_printk(
    "[} non-step/non-BRK debug events: %s\n",
    mdscr & AARCH64_MDSCR_EL1_MDE ? "yes" : "no"
  );
  target_printk(
    "[} OSLSR(should be 0x8): 0x%016" PRIx64 "\n",
    _AArch64_Read_oslsr_el1()
  );
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_dump();
#endif
  return false;
}

#define xstr( a ) str( a )
#define str( a ) #a
#define FRAME_SIZE_STR xstr( AARCH64_EXCEPTION_FRAME_SIZE )

/*
 * This block of assembly must have a target branch function because GCC
 * requires that SP not accumulate changes across an ASM block. Instead of
 * changing the SP, we branch to a new function and never return since it was
 * never going to return anyway.
 */
#define SWITCH_STACKS_AND_ALLOC( new_mode, old_frame, jump_target )                              \
  __asm__ volatile (                                                                            \
  "msr spsel, #" new_mode "\n"                         /* switch to thread stack */            \
  "sub sp, sp, #" FRAME_SIZE_STR "\n"   /* reserve space for CEF */             \
  "mov x0, sp\n"                                       /* Set x0 to the new exception frame */ \
  "mov x1, %[old_frame]\n"                             /* Set x1 to the old exception frame */ \
  "b " #jump_target "\n"                               /* Jump to the specified function */    \
  :                                                                                          \
  : [ old_frame ] "r" ( old_frame )                                                              \
  : "x0", "x1" )

#define SWITCH_STACKS_AND_ALLOC_WITH_CASCADE( new_mode, app_frame, \
                                              jump_target )                 \
  __asm__ volatile (                                                                            \
  "msr spsel, #" new_mode "\n"                         /* switch to thread stack */            \
  "sub sp, sp, #" FRAME_SIZE_STR "\n"   /* reserve space for CEF */             \
  "mov x0, sp\n"                                       /* Set x0 to the new exception frame */ \
  "mov x1, %[app_frame]\n"                             /* Set x1 to the old exception frame */ \
  "mov x2, %[needs_cascade]\n"                         /* pass on whether cascade is needed */ \
  "b " #jump_target "\n"                               /* Jump to the specified function */    \
  :                                                                                          \
  : [ app_frame ] "r" ( app_frame ),                                                             \
  [ needs_cascade ] "r" ( needs_cascade )                                                      \
  : "x0", "x1" )

/*
 * This block does not have an overall effect on SP since the spsel mode change
 * preserves the original SP
 */
#define DROP_OLD_FRAME( old_frame, old_mode, new_mode )                                                 \
  __asm__ volatile (                                                                                   \
  "msr spsel, #" old_mode "\n"                         /* switch to exception stack */                \
  "mov sp, %0\n"                                       /* Reset SP to the beginning of the CEF */     \
  "add sp, sp, #" FRAME_SIZE_STR "\n"   /* release space for CEF on exception stack */ \
  "msr spsel, #" new_mode "\n"                         /* switch to thread stack */                   \
  :                                                                                                 \
  : "r" ( old_frame ) )                                                                             \

#define THREAD_MODE "1"
#define EXCEPTION_MODE "0"

void target_exception_stack_stage_3(
  CPU_Exception_frame *exc_frame,
  CPU_Exception_frame *app_frame,
  bool                 needs_cascade
);

void target_exception_stack_stage_3(
  CPU_Exception_frame *exc_frame,
  CPU_Exception_frame *app_frame,
  bool                 needs_cascade
)
{
  _AArch64_Exception_frame_copy( exc_frame, app_frame );
  DROP_OLD_FRAME( app_frame, THREAD_MODE, EXCEPTION_MODE );

  if ( needs_cascade ) {
    /* does not return */
    _AArch64_Exception_default( exc_frame );
  }

  /* does not return */
  _CPU_Exception_resume( exc_frame );
}

void target_exception_stack_stage_2(
  CPU_Exception_frame *app_frame,
  CPU_Exception_frame *exc_frame
);

void target_exception_stack_stage_2(
  CPU_Exception_frame *app_frame,
  CPU_Exception_frame *exc_frame
)
{
  _AArch64_Exception_frame_copy( app_frame, exc_frame );
  DROP_OLD_FRAME( exc_frame, EXCEPTION_MODE, THREAD_MODE );
  /* breakpoints must be disabled here since other tasks could run that don't have debug masked */
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_unload();
#endif
  /* enable interrupts here to allow this thread to be suspended as necessary */
  aarch64_debug_enable_interrupts();
  bool needs_cascade = target_exception( app_frame );

  /* disable interrupts to return to normal operation */
  aarch64_debug_disable_interrupts();
  /* re-enable breakpoints disabled above */
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_load();
#endif
  SWITCH_STACKS_AND_ALLOC_WITH_CASCADE(
    EXCEPTION_MODE,
    app_frame,
    target_exception_stack_stage_3
  );
}

/* not allowed to return since it unwinds the stack */
static void target_exception_thread_stack( CPU_Exception_frame *old_frame )
{
  SWITCH_STACKS_AND_ALLOC(
    THREAD_MODE,
    old_frame,
    target_exception_stack_stage_2
  );
}

static void target_exception_application( CPU_Exception_frame *ef )
{
  /* Continue in fatal error handler chain */
  if ( !debug_session_active ) {
    /* does not return */
    _AArch64_Exception_default( ef );
  }

  /*
   * Set CPSR.D to disable single-step operation, this will be cleared before
   * the thread is resumed if necessary.
   */
  ef->register_cpsr |= AARCH64_DSPSR_EL0_D;

  /*
   * Switching to the user stack is not possible if the stack pointer is bad.
   * This should be a relatively rare occurrance and signals a severe problem
   * with the application code or system.
   */
  if ( AARCH64_ESR_EL1_EC_GET( ef->register_syndrome ) == 0x26 ) {
    if ( target_exception( ef ) ) {
      /* does not return */
      _AArch64_Exception_default( ef );
    }

    /* does not return */
    _CPU_Exception_resume( ef );
  }

  target_exception_thread_stack( ef );
}

static void target_exception_kernel( CPU_Exception_frame *ef )
{
  /*
   * If there is a stack alignment problem in exception mode, it really
   * shouldn't happen and execution won't even make it this far.
   */
  if ( !debug_session_active ) {
    /* does not return */
    _AArch64_Exception_default( ef );
  }

  /*
   * Set CPSR.D to disable single-step operation, this will be cleared before
   * the thread is resumed if necessary.
   */
  ef->register_cpsr |= AARCH64_DSPSR_EL0_D;

  if ( target_exception( ef ) ) {
    /* does not return */
    _AArch64_Exception_default( ef );
  }

  /* does not return */
  _CPU_Exception_resume( ef );
}

static void rtems_debugger_target_set_vectors( void )
{
  /* Set vectors for both application and kernel modes */
  AArch64_set_exception_handler(
    AARCH64_EXCEPTION_SPx_SYNCHRONOUS,
    (void *) target_exception_application
  );
  AArch64_set_exception_handler(
    AARCH64_EXCEPTION_SP0_SYNCHRONOUS,
    (void *) target_exception_kernel
  );
}

static bool rtems_debugger_is_int_reg( size_t reg )
{
  const size_t size = aarch64_reg_offsets[ reg + 1 ] -
                      aarch64_reg_offsets[ reg ];

  return size == RTEMS_DEBUGGER_REG_BYTES;
}

static void rtems_debugger_set_int_reg(
  rtems_debugger_thread *thread,
  size_t                 reg,
  const uint64_t         value
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];

  memcpy( &thread->registers[ offset ], &value, sizeof( uint64_t ) );
}

static const uint64_t rtems_debugger_get_int_reg(
  rtems_debugger_thread *thread,
  size_t                 reg
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];
  uint64_t     value;

  memcpy( &value, &thread->registers[ offset ], sizeof( uint64_t ) );
  return value;
}

static void rtems_debugger_set_halfint_reg(
  rtems_debugger_thread *thread,
  size_t                 reg,
  const uint32_t         value
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];

  memcpy( &thread->registers[ offset ], &value, sizeof( uint32_t ) );
}

static const uint32_t rtems_debugger_get_halfint_reg(
  rtems_debugger_thread *thread,
  size_t                 reg
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];
  uint32_t     value;

  memcpy( &value, &thread->registers[ offset ], sizeof( uint32_t ) );
  return value;
}

static void rtems_debugger_set_fp_reg(
  rtems_debugger_thread *thread,
  size_t                 reg,
  const uint128_t        value
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];

  memcpy( &thread->registers[ offset ], &value, sizeof( uint128_t ) );
}

static const uint128_t rtems_debugger_get_fp_reg(
  rtems_debugger_thread *thread,
  size_t                 reg
)
{
  const size_t offset = aarch64_reg_offsets[ reg ];
  uint128_t    value;

  memcpy( &value, &thread->registers[ offset ], sizeof( uint128_t ) );
  return value;
}

static rtems_status_code rtems_debugger_target_set_text_writable(
  bool writable
)
{
  uintptr_t         start_begin = (uintptr_t) bsp_section_start_begin;
  uintptr_t         start_end = (uintptr_t) bsp_section_start_end;
  uintptr_t         text_begin = (uintptr_t) bsp_section_text_begin;
  uintptr_t         text_end = (uintptr_t) bsp_section_text_end;
  uintptr_t         fast_text_begin = (uintptr_t) bsp_section_fast_text_begin;
  uintptr_t         fast_text_end = (uintptr_t) bsp_section_fast_text_end;
  uint64_t          mmu_flags = AARCH64_MMU_CODE_RW_CACHED;
  rtems_status_code sc;

  if ( !writable ) {
    mmu_flags = AARCH64_MMU_CODE_CACHED;
  }

  target_printk(
    "[} MMU edit: start_begin: 0x%016" PRIxPTR
    " start_end: 0x%016" PRIxPTR "\n",
    start_begin,
    start_end
  );
  sc = aarch64_mmu_map(
    start_begin,
    start_end - start_begin,
    mmu_flags
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    target_printk( "[} MMU edit failed\n" );
    return sc;
  }

  target_printk(
    "[} MMU edit: text_begin: 0x%016" PRIxPTR
    " text_end: 0x%016" PRIxPTR "\n",
    text_begin,
    text_end
  );
  sc = aarch64_mmu_map(
    text_begin,
    text_end - text_begin,
    mmu_flags
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    target_printk( "[} MMU edit failed\n" );
    return sc;
  }

  target_printk(
    "[} MMU edit: fast_text_begin: 0x%016" PRIxPTR
    " fast_text_end: 0x%016" PRIxPTR "\n",
    fast_text_begin,
    fast_text_end
  );
  sc = aarch64_mmu_map(
    fast_text_begin,
    fast_text_end - fast_text_begin,
    mmu_flags
  );

  if ( sc != RTEMS_SUCCESSFUL ) {
    target_printk( "[} MMU edit failed\n" );
  }

  return sc;
}

static rtems_task setup_debugger_on_cpu( rtems_task_argument arg )
{
  rtems_status_code            sc;
  rtems_status_code           *init_error = (rtems_status_code *) arg;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );
  sc = rtems_debugger_target_set_text_writable( true );

  if ( sc != RTEMS_SUCCESSFUL ) {
    *init_error = sc;
  }

  rtems_debugger_target_set_vectors();

  /* enable single-step debugging */
  uint64_t mdscr = _AArch64_Read_mdscr_el1();

  mdscr |= AARCH64_MDSCR_EL1_SS;
  mdscr |= AARCH64_MDSCR_EL1_KDE;
  mdscr |= AARCH64_MDSCR_EL1_MDE;
  _AArch64_Write_mdscr_el1( mdscr );

  /* clear the OS lock */
  _AArch64_Write_oslar_el1( 0 );
  rtems_interrupt_lock_release( &target_lock, &lock_context );
}

int rtems_debugger_target_enable( void )
{
  rtems_status_code sc;
  rtems_status_code init_error = RTEMS_SUCCESSFUL;

  debug_session_active = true;
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_unload();
  aarch64_debug_break_clear();
#endif
  aarch64_debug_disable_debug_exceptions();
  sc = run_across_cpus(
    setup_debugger_on_cpu,
    ( rtems_task_argument ) & init_error
  );

  if ( init_error != RTEMS_SUCCESSFUL ) {
    return init_error;
  }

  return sc;
}

static rtems_task teardown_debugger_on_cpu( rtems_task_argument arg )
{
  rtems_status_code            sc;
  rtems_status_code           *deinit_error = (rtems_status_code *) arg;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );
  sc = rtems_debugger_target_set_text_writable( false );

  if ( sc != RTEMS_SUCCESSFUL ) {
    *deinit_error = sc;
  }

  /* disable single-step debugging */
  uint64_t mdscr = _AArch64_Read_mdscr_el1();

  mdscr &= ~AARCH64_MDSCR_EL1_SS;
  mdscr &= ~AARCH64_MDSCR_EL1_KDE;
  mdscr &= ~AARCH64_MDSCR_EL1_MDE;
  _AArch64_Write_mdscr_el1( mdscr );

  rtems_interrupt_lock_release( &target_lock, &lock_context );
}

int rtems_debugger_target_disable( void )
{
  rtems_status_code sc;
  rtems_status_code deinit_error = RTEMS_SUCCESSFUL;

  debug_session_active = false;
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_unload();
  aarch64_debug_break_clear();
#endif
  sc = run_across_cpus(
    teardown_debugger_on_cpu,
    ( rtems_task_argument ) & deinit_error
  );

  if ( deinit_error != RTEMS_SUCCESSFUL ) {
    return deinit_error;
  }

  return sc;
}

int rtems_debugger_target_read_regs( rtems_debugger_thread *thread )
{
  if (
    !rtems_debugger_thread_flag(
      thread,
      RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID
    )
  ) {
    static const uintptr_t good_address = (uintptr_t) &good_address;
    int                    i;

    memset( &thread->registers[ 0 ], 0, RTEMS_DEBUGGER_NUMREGBYTES );

    /* set all integer register to a known valid address */
    for ( i = 0; i < RTEMS_DEBUGGER_NUMREGS; ++i ) {
      if ( rtems_debugger_is_int_reg( i ) ) {
        rtems_debugger_set_int_reg( thread, i, (uintptr_t) &good_address );
      }
    }

    if ( thread->frame ) {
      CPU_Exception_frame *frame = thread->frame;

      *( (CPU_Exception_frame *) thread->registers ) = *frame;
      rtems_debugger_set_int_reg( thread, REG_X0, frame->register_x0 );
      rtems_debugger_set_int_reg( thread, REG_X1, frame->register_x1 );
      rtems_debugger_set_int_reg( thread, REG_X2, frame->register_x2 );
      rtems_debugger_set_int_reg( thread, REG_X3, frame->register_x3 );
      rtems_debugger_set_int_reg( thread, REG_X4, frame->register_x4 );
      rtems_debugger_set_int_reg( thread, REG_X5, frame->register_x5 );
      rtems_debugger_set_int_reg( thread, REG_X6, frame->register_x6 );
      rtems_debugger_set_int_reg( thread, REG_X7, frame->register_x7 );
      rtems_debugger_set_int_reg( thread, REG_X8, frame->register_x8 );
      rtems_debugger_set_int_reg( thread, REG_X9, frame->register_x9 );
      rtems_debugger_set_int_reg( thread, REG_X10, frame->register_x10 );
      rtems_debugger_set_int_reg( thread, REG_X11, frame->register_x11 );
      rtems_debugger_set_int_reg( thread, REG_X12, frame->register_x12 );
      rtems_debugger_set_int_reg( thread, REG_X13, frame->register_x13 );
      rtems_debugger_set_int_reg( thread, REG_X14, frame->register_x14 );
      rtems_debugger_set_int_reg( thread, REG_X15, frame->register_x15 );
      rtems_debugger_set_int_reg( thread, REG_X16, frame->register_x16 );
      rtems_debugger_set_int_reg( thread, REG_X17, frame->register_x17 );
      rtems_debugger_set_int_reg( thread, REG_X18, frame->register_x18 );
      rtems_debugger_set_int_reg( thread, REG_X19, frame->register_x19 );
      rtems_debugger_set_int_reg( thread, REG_X20, frame->register_x20 );
      rtems_debugger_set_int_reg( thread, REG_X21, frame->register_x21 );
      rtems_debugger_set_int_reg( thread, REG_X22, frame->register_x22 );
      rtems_debugger_set_int_reg( thread, REG_X23, frame->register_x23 );
      rtems_debugger_set_int_reg( thread, REG_X24, frame->register_x24 );
      rtems_debugger_set_int_reg( thread, REG_X25, frame->register_x25 );
      rtems_debugger_set_int_reg( thread, REG_X26, frame->register_x26 );
      rtems_debugger_set_int_reg( thread, REG_X27, frame->register_x27 );
      rtems_debugger_set_int_reg( thread, REG_X28, frame->register_x28 );
      rtems_debugger_set_int_reg( thread, REG_FP, frame->register_fp );
      rtems_debugger_set_int_reg(
        thread,
        REG_LR,
        (intptr_t) frame->register_lr
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_SP,
        (intptr_t) frame->register_sp
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_PC,
        (intptr_t) frame->register_pc
      );
      /* GDB considers CPSR to be 32-bit because bits 63:32 are RES0 */
      rtems_debugger_set_halfint_reg(
        thread,
        REG_CPS,
        (uint32_t) frame->register_cpsr
      );
      rtems_debugger_set_fp_reg( thread, REG_V0, frame->register_q0 );
      rtems_debugger_set_fp_reg( thread, REG_V1, frame->register_q1 );
      rtems_debugger_set_fp_reg( thread, REG_V2, frame->register_q2 );
      rtems_debugger_set_fp_reg( thread, REG_V3, frame->register_q3 );
      rtems_debugger_set_fp_reg( thread, REG_V4, frame->register_q4 );
      rtems_debugger_set_fp_reg( thread, REG_V5, frame->register_q5 );
      rtems_debugger_set_fp_reg( thread, REG_V6, frame->register_q6 );
      rtems_debugger_set_fp_reg( thread, REG_V7, frame->register_q7 );
      rtems_debugger_set_fp_reg( thread, REG_V8, frame->register_q8 );
      rtems_debugger_set_fp_reg( thread, REG_V9, frame->register_q9 );
      rtems_debugger_set_fp_reg( thread, REG_V10, frame->register_q10 );
      rtems_debugger_set_fp_reg( thread, REG_V11, frame->register_q11 );
      rtems_debugger_set_fp_reg( thread, REG_V12, frame->register_q12 );
      rtems_debugger_set_fp_reg( thread, REG_V13, frame->register_q13 );
      rtems_debugger_set_fp_reg( thread, REG_V14, frame->register_q14 );
      rtems_debugger_set_fp_reg( thread, REG_V15, frame->register_q15 );
      rtems_debugger_set_fp_reg( thread, REG_V16, frame->register_q16 );
      rtems_debugger_set_fp_reg( thread, REG_V17, frame->register_q17 );
      rtems_debugger_set_fp_reg( thread, REG_V18, frame->register_q18 );
      rtems_debugger_set_fp_reg( thread, REG_V19, frame->register_q19 );
      rtems_debugger_set_fp_reg( thread, REG_V20, frame->register_q20 );
      rtems_debugger_set_fp_reg( thread, REG_V21, frame->register_q21 );
      rtems_debugger_set_fp_reg( thread, REG_V22, frame->register_q22 );
      rtems_debugger_set_fp_reg( thread, REG_V23, frame->register_q23 );
      rtems_debugger_set_fp_reg( thread, REG_V24, frame->register_q24 );
      rtems_debugger_set_fp_reg( thread, REG_V25, frame->register_q25 );
      rtems_debugger_set_fp_reg( thread, REG_V26, frame->register_q26 );
      rtems_debugger_set_fp_reg( thread, REG_V27, frame->register_q27 );
      rtems_debugger_set_fp_reg( thread, REG_V28, frame->register_q28 );
      rtems_debugger_set_fp_reg( thread, REG_V29, frame->register_q29 );
      rtems_debugger_set_fp_reg( thread, REG_V30, frame->register_q30 );
      rtems_debugger_set_fp_reg( thread, REG_V31, frame->register_q31 );
      /* GDB considers FPSR and FPCR to be 32-bit because bits 63:32 are RES0 */
      rtems_debugger_set_halfint_reg( thread, REG_FPS, frame->register_fpsr );
      rtems_debugger_set_halfint_reg( thread, REG_FPC, frame->register_fpcr );
      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal( frame );
    } else {
      rtems_debugger_set_int_reg(
        thread,
        REG_X19,
        thread->tcb->Registers.register_x19
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X20,
        thread->tcb->Registers.register_x20
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X21,
        thread->tcb->Registers.register_x21
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X22,
        thread->tcb->Registers.register_x22
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X23,
        thread->tcb->Registers.register_x23
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X24,
        thread->tcb->Registers.register_x24
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X25,
        thread->tcb->Registers.register_x25
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X26,
        thread->tcb->Registers.register_x26
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X27,
        thread->tcb->Registers.register_x27
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_X28,
        thread->tcb->Registers.register_x28
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_FP,
        thread->tcb->Registers.register_fp
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_LR,
        (intptr_t) thread->tcb->Registers.register_lr
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_SP,
        (intptr_t) thread->tcb->Registers.register_sp
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_PC,
        (intptr_t) thread->tcb->Registers.register_lr
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V8,
        thread->tcb->Registers.register_d8
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V9,
        thread->tcb->Registers.register_d9
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V10,
        thread->tcb->Registers.register_d10
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V11,
        thread->tcb->Registers.register_d11
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V12,
        thread->tcb->Registers.register_d12
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V13,
        thread->tcb->Registers.register_d13
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V14,
        thread->tcb->Registers.register_d14
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_V15,
        thread->tcb->Registers.register_d15
      );
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

int rtems_debugger_target_write_regs( rtems_debugger_thread *thread )
{
  if (
    rtems_debugger_thread_flag(
      thread,
      RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY
    )
  ) {
    /*
     * Only write to debugger controlled exception threads. Do not touch the
     * registers for threads blocked in the context switcher.
     */
    if (
      rtems_debugger_thread_flag(
        thread,
        RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION
      )
    ) {
      CPU_Exception_frame *frame = thread->frame;
      frame->register_x0 = rtems_debugger_get_int_reg( thread, REG_X0 );
      frame->register_x1 = rtems_debugger_get_int_reg( thread, REG_X1 );
      frame->register_x2 = rtems_debugger_get_int_reg( thread, REG_X2 );
      frame->register_x3 = rtems_debugger_get_int_reg( thread, REG_X3 );
      frame->register_x4 = rtems_debugger_get_int_reg( thread, REG_X4 );
      frame->register_x5 = rtems_debugger_get_int_reg( thread, REG_X5 );
      frame->register_x6 = rtems_debugger_get_int_reg( thread, REG_X6 );
      frame->register_x7 = rtems_debugger_get_int_reg( thread, REG_X7 );
      frame->register_x8 = rtems_debugger_get_int_reg( thread, REG_X8 );
      frame->register_x9 = rtems_debugger_get_int_reg( thread, REG_X9 );
      frame->register_x10 = rtems_debugger_get_int_reg( thread, REG_X10 );
      frame->register_x11 = rtems_debugger_get_int_reg( thread, REG_X11 );
      frame->register_x12 = rtems_debugger_get_int_reg( thread, REG_X12 );
      frame->register_x13 = rtems_debugger_get_int_reg( thread, REG_X13 );
      frame->register_x14 = rtems_debugger_get_int_reg( thread, REG_X14 );
      frame->register_x15 = rtems_debugger_get_int_reg( thread, REG_X15 );
      frame->register_x16 = rtems_debugger_get_int_reg( thread, REG_X16 );
      frame->register_x17 = rtems_debugger_get_int_reg( thread, REG_X17 );
      frame->register_x18 = rtems_debugger_get_int_reg( thread, REG_X18 );
      frame->register_x19 = rtems_debugger_get_int_reg( thread, REG_X19 );
      frame->register_x20 = rtems_debugger_get_int_reg( thread, REG_X20 );
      frame->register_x21 = rtems_debugger_get_int_reg( thread, REG_X21 );
      frame->register_x22 = rtems_debugger_get_int_reg( thread, REG_X22 );
      frame->register_x23 = rtems_debugger_get_int_reg( thread, REG_X23 );
      frame->register_x24 = rtems_debugger_get_int_reg( thread, REG_X24 );
      frame->register_x25 = rtems_debugger_get_int_reg( thread, REG_X25 );
      frame->register_x26 = rtems_debugger_get_int_reg( thread, REG_X26 );
      frame->register_x27 = rtems_debugger_get_int_reg( thread, REG_X27 );
      frame->register_x28 = rtems_debugger_get_int_reg( thread, REG_X28 );
      frame->register_fp = (uintptr_t) rtems_debugger_get_int_reg(
        thread,
        REG_FP
      );
      frame->register_lr = (void *) (uintptr_t) rtems_debugger_get_int_reg(
        thread,
        REG_LR
      );
      frame->register_sp = (uintptr_t) rtems_debugger_get_int_reg(
        thread,
        REG_SP
      );
      frame->register_pc = (void *) (uintptr_t) rtems_debugger_get_int_reg(
        thread,
        REG_PC
      );
      frame->register_cpsr = rtems_debugger_get_halfint_reg( thread, REG_CPS );
      frame->register_q0 = rtems_debugger_get_fp_reg( thread, REG_V0 );
      frame->register_q1 = rtems_debugger_get_fp_reg( thread, REG_V1 );
      frame->register_q2 = rtems_debugger_get_fp_reg( thread, REG_V2 );
      frame->register_q3 = rtems_debugger_get_fp_reg( thread, REG_V3 );
      frame->register_q4 = rtems_debugger_get_fp_reg( thread, REG_V4 );
      frame->register_q5 = rtems_debugger_get_fp_reg( thread, REG_V5 );
      frame->register_q6 = rtems_debugger_get_fp_reg( thread, REG_V6 );
      frame->register_q7 = rtems_debugger_get_fp_reg( thread, REG_V7 );
      frame->register_q8 = rtems_debugger_get_fp_reg( thread, REG_V8 );
      frame->register_q9 = rtems_debugger_get_fp_reg( thread, REG_V9 );
      frame->register_q10 = rtems_debugger_get_fp_reg( thread, REG_V10 );
      frame->register_q11 = rtems_debugger_get_fp_reg( thread, REG_V11 );
      frame->register_q12 = rtems_debugger_get_fp_reg( thread, REG_V12 );
      frame->register_q13 = rtems_debugger_get_fp_reg( thread, REG_V13 );
      frame->register_q14 = rtems_debugger_get_fp_reg( thread, REG_V14 );
      frame->register_q15 = rtems_debugger_get_fp_reg( thread, REG_V15 );
      frame->register_q16 = rtems_debugger_get_fp_reg( thread, REG_V16 );
      frame->register_q17 = rtems_debugger_get_fp_reg( thread, REG_V17 );
      frame->register_q18 = rtems_debugger_get_fp_reg( thread, REG_V18 );
      frame->register_q19 = rtems_debugger_get_fp_reg( thread, REG_V19 );
      frame->register_q20 = rtems_debugger_get_fp_reg( thread, REG_V20 );
      frame->register_q21 = rtems_debugger_get_fp_reg( thread, REG_V21 );
      frame->register_q22 = rtems_debugger_get_fp_reg( thread, REG_V22 );
      frame->register_q23 = rtems_debugger_get_fp_reg( thread, REG_V23 );
      frame->register_q24 = rtems_debugger_get_fp_reg( thread, REG_V24 );
      frame->register_q25 = rtems_debugger_get_fp_reg( thread, REG_V25 );
      frame->register_q26 = rtems_debugger_get_fp_reg( thread, REG_V26 );
      frame->register_q27 = rtems_debugger_get_fp_reg( thread, REG_V27 );
      frame->register_q28 = rtems_debugger_get_fp_reg( thread, REG_V28 );
      frame->register_q29 = rtems_debugger_get_fp_reg( thread, REG_V29 );
      frame->register_q30 = rtems_debugger_get_fp_reg( thread, REG_V30 );
      frame->register_q31 = rtems_debugger_get_fp_reg( thread, REG_V31 );
      frame->register_fpsr = rtems_debugger_get_halfint_reg( thread, REG_FPS );
      frame->register_fpcr = rtems_debugger_get_halfint_reg( thread, REG_FPC );
    }

    thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
  }

  return 0;
}

uintptr_t rtems_debugger_target_reg_pc( rtems_debugger_thread *thread )
{
  int r;

  r = rtems_debugger_target_read_regs( thread );

  if ( r >= 0 ) {
    return rtems_debugger_get_int_reg( thread, REG_PC );
  }

  return 0;
}

uintptr_t rtems_debugger_target_frame_pc( CPU_Exception_frame *frame )
{
  return (uintptr_t) frame->register_pc;
}

uintptr_t rtems_debugger_target_reg_sp( rtems_debugger_thread *thread )
{
  int r;

  r = rtems_debugger_target_read_regs( thread );

  if ( r >= 0 ) {
    return rtems_debugger_get_int_reg( thread, REG_SP );
  }

  return 0;
}

uintptr_t rtems_debugger_target_tcb_sp( rtems_debugger_thread *thread )
{
  return (uintptr_t) thread->tcb->Registers.register_sp;
}

int rtems_debugger_target_thread_stepping( rtems_debugger_thread *thread )
{
  CPU_Exception_frame *frame = thread->frame;

  if ( rtems_debugger_thread_flag(
    thread,
    RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR
       ) ) {
    /* Especially on first startup, frame isn't guaranteed to be non-NULL */
    if ( frame == NULL ) {
      return -1;
    }

    /*
     * Single stepping uses AArch64-specific single-step mode and does not
     * involve hardware breakpoints.
     */

    /* Breakpoint instruction exceptions occur even when D is not set. */
    uint64_t stepping_enabled =
      !( frame->register_cpsr & AARCH64_DSPSR_EL0_D );

    target_printk( "[} stepping: %s\n", stepping_enabled ? "yes" : "no" );

    /*
     * This field is unset by the CPU during the software step process and must
     * be set again each time the debugger needs to advance one instruction. If
     * this is not set each time, the software step exception will trigger
     * before executing an instruction.
     */
    frame->register_cpsr |= AARCH64_DSPSR_EL0_SS;

    if ( !stepping_enabled ) {
      /*
       * Clear CPSR.D to enable single-step operation. The debug mask flag is
       * set on taking an exception to prevent unwanted stepping. The way
       * single-stepping works will need to change if hardware breakpoints and
       * watchpoints are ever used.
       */
      frame->register_cpsr &= ~AARCH64_DSPSR_EL0_D;
    }
  }

  return 0;
}

int rtems_debugger_target_exception_to_signal( CPU_Exception_frame *frame )
{
  uint64_t EC = AARCH64_ESR_EL1_EC_GET( frame->register_syndrome );

  switch ( EC ) {
    case 0x1:   /* WFI */
    case 0x7:   /* SVE/SIMD/FP */
    case 0xa:   /* LD64B/ST64B* */
    case 0x15:
    case 0x18:  /* MSR/MRS/system instruction */
    case 0x19:  /* SVE */
    case 0x31:
    case 0x33:
    case 0x35:
    case 0x3c:
      return RTEMS_DEBUGGER_SIGNAL_TRAP;

    case 0x2c:
      return RTEMS_DEBUGGER_SIGNAL_FPE;

    case 0x21:
    case 0x25:
      return RTEMS_DEBUGGER_SIGNAL_SEGV;

    default:
      /*
       * Covers unknown, SP/PC alignment, illegal execution state, and any new
       * exception classes that get added.
       */
      return RTEMS_DEBUGGER_SIGNAL_ILL;
  }
}

void rtems_debugger_target_exception_print( CPU_Exception_frame *frame )
{
  EXC_FRAME_PRINT( rtems_debugger_printf, "", frame );
}

int rtems_debugger_target_hwbreak_insert( void )
{
  /*
   * Do nothing, these are loaded elsewhere if needed.
   */
  return 0;
}

int rtems_debugger_target_hwbreak_remove( void )
{
#ifdef HARDWARE_BREAKPOINTS_NOT_USED
  aarch64_debug_break_unload();
#endif
  return 0;
}

int rtems_debugger_target_hwbreak_control(
  rtems_debugger_target_watchpoint wp,
  bool                             insert,
  uintptr_t                        addr,
  DB_UINT                          kind
)
{
  /* To do. */
  return 0;
}

int rtems_debugger_target_cache_sync( rtems_debugger_target_swbreak *swbreak )
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
