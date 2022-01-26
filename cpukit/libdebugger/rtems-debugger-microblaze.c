/* SPDX-License-Identifier: BSD-2-Clause */

/**
 * @file
 *
 * @ingroup RTEMSLibdebugger
 *
 * @brief MicroBlaze libdebugger implementation
 */

/*
 * Copyright (C) 2022 On-Line Applications Research Corporation (OAR)
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

#define TARGET_DEBUG 0

#ifdef HAVE_CONFIG_H
#include "config.h"
#endif

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>

/* Defined by linkcmds.base */
extern char bsp_section_text_begin[];
extern char bsp_section_text_end[];
extern char bsp_section_fast_text_begin[];
extern char bsp_section_fast_text_end[];

#include <rtems.h>
#include <rtems/score/cpu.h>
#include <rtems/score/threadimpl.h>
#include <rtems/score/userextimpl.h>

#include <rtems/debugger/rtems-debugger-bsp.h>

#include "rtems-debugger-target.h"
#include "rtems-debugger-threads.h"

#if TARGET_DEBUG
#include <rtems/bspIo.h>
#endif

/*
 * Number of registers.
 */
#define RTEMS_DEBUGGER_NUMREGS 57

/*
 * Number of bytes per type of register.
 */
#define RTEMS_DEBUGGER_REG_BYTES    4

/* Debugger registers layout. See microblaze-core.xml in GDB source. */
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
#define REG_R13   13
#define REG_R14   14
#define REG_R15   15
#define REG_R16   16
#define REG_R17   17
#define REG_R18   18
#define REG_R19   19
#define REG_R20   20
#define REG_R21   21
#define REG_R22   22
#define REG_R23   23
#define REG_R24   24
#define REG_R25   25
#define REG_R26   26
#define REG_R27   27
#define REG_R28   28
#define REG_R29   29
#define REG_R30   30
#define REG_R31   31
#define REG_PC    32
#define REG_MS    33
#define REG_EA    34
#define REG_ES    35
#define REG_FS    36
#define REG_BT    37
#define REG_PV0   38
#define REG_PV1   39
#define REG_PV2   40
#define REG_PV3   41
#define REG_PV4   42
#define REG_PV5   43
#define REG_PV6   44
#define REG_PV7   45
#define REG_PV8   46
#define REG_PV9   47
#define REG_PV10  48
#define REG_PV11  49
#define REG_ED    50
#define REG_PID   51
#define REG_ZP    52
#define REG_TBLX  53
#define REG_TBLSX 54
#define REG_TBLLO 55
#define REG_TBLHI 56

/**
 * Register offset table with the total as the last entry.
 *
 * Check this table in gdb with the command:
 *
 *   maint print registers
 */
static const size_t microblaze_reg_offsets[ RTEMS_DEBUGGER_NUMREGS + 1 ] = {
  REG_R0 * 4,
  REG_R1 * 4,
  REG_R2 * 4,
  REG_R3 * 4,
  REG_R4 * 4,
  REG_R5 * 4,
  REG_R6 * 4,
  REG_R7 * 4,
  REG_R8 * 4,
  REG_R9 * 4,
  REG_R10 * 4,
  REG_R11 * 4,
  REG_R12 * 4,
  REG_R13 * 4,
  REG_R14 * 4,
  REG_R15 * 4,
  REG_R16 * 4,
  REG_R17 * 4,
  REG_R18 * 4,
  REG_R19 * 4,
  REG_R20 * 4,
  REG_R21 * 4,
  REG_R22 * 4,
  REG_R23 * 4,
  REG_R24 * 4,
  REG_R25 * 4,
  REG_R26 * 4,
  REG_R27 * 4,
  REG_R28 * 4,
  REG_R29 * 4,
  REG_R30 * 4,
  REG_R31 * 4,
  REG_PC * 4,
  REG_MS * 4,
  REG_EA * 4,
  REG_ES * 4,
  REG_FS * 4,
  REG_BT * 4,
  REG_PV0 * 4,
  REG_PV1 * 4,
  REG_PV2 * 4,
  REG_PV3 * 4,
  REG_PV4 * 4,
  REG_PV5 * 4,
  REG_PV6 * 4,
  REG_PV7 * 4,
  REG_PV8 * 4,
  REG_PV9 * 4,
  REG_PV10 * 4,
  REG_PV11 * 4,
  REG_ED * 4,
  REG_PID * 4,
  REG_ZP * 4,
  REG_TBLX * 4,
  REG_TBLSX * 4,
  REG_TBLLO * 4,
  REG_TBLHI * 4,
/* Total size */
  REG_TBLHI * 4 + 4,
};

/*
 * Number of bytes of registers.
 */
#define RTEMS_DEBUGGER_NUMREGBYTES \
  microblaze_reg_offsets[ RTEMS_DEBUGGER_NUMREGS ]

/**
 * Print the exception frame.
 */
#define EXC_FRAME_PRINT( _out, _prefix, _frame ) \
  do { \
    _out( \
      _prefix "  R0 = 0x%08" PRIx32 "  R1 = 0x%08" PRIx32 \
      "  R2 = 0x%08" PRIx32 "  R3 = 0x%08" PRIx32 "\n", \
      0, \
      _frame->r1, \
      _frame->r2, \
      _frame->r3 \
    ); \
    _out( \
      _prefix "  R4 = 0x%08" PRIx32 "  R5 = 0x%08" PRIx32 \
      "  R6 = 0x%08" PRIx32 "  R7 = 0x%08" PRIx32 "\n", \
      _frame->r4, \
      _frame->r5, \
      _frame->r6, \
      _frame->r7 \
    ); \
    _out( \
      _prefix "  R8 = 0x%08" PRIx32 "  R9 = 0x%08" PRIx32 \
      " R10 = 0x%08" PRIx32 " R11 = 0x%08" PRIx32 "\n", \
      _frame->r8, \
      _frame->r9, \
      _frame->r10, \
      _frame->r11 \
    ); \
    _out( \
      _prefix " R12 = 0x%08" PRIx32 " R13 = 0x%08" PRIx32 \
      " R14 = 0x%08" PRIxPTR " R15 = 0x%08" PRIxPTR "\n", \
      _frame->r12, \
      _frame->r13, \
      (uintptr_t) _frame->r14, \
      (uintptr_t) _frame->r15 \
    ); \
    _out( \
      _prefix " R16 = 0x%08" PRIxPTR " R17 = 0x%08" PRIxPTR \
      " R18 = 0x%08" PRIx32 " R19 = 0x%08" PRIx32 "\n", \
      (uintptr_t) _frame->r16, \
      (uintptr_t) _frame->r17, \
      _frame->r18, \
      _frame->r19 \
    ); \
    _out( \
      _prefix " R20 = 0x%08" PRIx32 " R21 = 0x%08" PRIx32 \
      " R22 = 0x%08" PRIx32 " R23 = 0x%08" PRIx32 "\n", \
      _frame->r20, \
      _frame->r21, \
      _frame->r22, \
      _frame->r23 \
    ); \
    _out( \
      _prefix " R24 = 0x%08" PRIx32 " R25 = 0x%08" PRIx32 \
      " R26 = 0x%08" PRIx32 " R27 = 0x%08" PRIx32 "\n", \
      _frame->r24, \
      _frame->r25, \
      _frame->r26, \
      _frame->r27 \
    ); \
    _out( \
      _prefix " R28 = 0x%08" PRIx32 " R29 = 0x%08" PRIx32 \
      " R30 = 0x%08" PRIxPTR " R31 = 0x%08" PRIxPTR "\n", \
      _frame->r28, \
      _frame->r29, \
      _frame->r30, \
      _frame->r31 \
    ); \
    _out( \
      _prefix " EAR = %p ESR = 0x%08" PRIx32 "\n", \
      _frame->ear, \
      _frame->esr \
    ); \
    _out( \
      _prefix "  PC = %p\n", \
      _frame->r16 \
    ); \
    _out( \
      _prefix " MSR = 0x%08" PRIx32 " En:%c%c%c%c Prog:%c%c%c Mode:%c%c Arith:%c%c\n", \
      _frame->msr, \
      ( _frame->msr & MICROBLAZE_MSR_IE ) != 0 ? 'I' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_ICE ) != 0 ? 'C' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_DCE ) != 0 ? 'D' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_EE ) != 0 ? 'E' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_BIP ) != 0 ? 'B' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_FSL ) != 0 ? 'F' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_EIP ) != 0 ? 'E' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_UM ) != 0 ? 'U' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_VM ) != 0 ? 'V' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_C ) != 0 ? 'C' : '-', \
      ( _frame->msr & MICROBLAZE_MSR_DZO ) != 0 ? 'Z' : '-' \
    ); \
  } while ( 0 )

/**
 * The breakpoint instruction can be intercepted on hardware by an active JTAG
 * connection. This instead uses an illegal opcode (0xdeadbeef) to trigger an
 * exception as a mechanism to call into the debugger.
 */
static const uint8_t breakpoint[ 4 ] = { 0xef, 0xbe, 0xad, 0xde };

/**
 * Target lock.
 */
RTEMS_INTERRUPT_LOCK_DEFINE( static, target_lock, "target_lock" )

/**
 * Is a session active?
 */
static bool debug_session_active;

/*
 * MicroBlaze debug hardware.
 */
static uint8_t hw_breakpoints;
static uint8_t hw_read_watchpoints;
static uint8_t hw_write_watchpoints;

/* Software breakpoints for single stepping */
typedef struct {
  uint32_t *address;
} microblaze_soft_step;

microblaze_soft_step next_soft_break = { 0 };
microblaze_soft_step target_soft_break = { 0 };

static void set_soft_break(
  microblaze_soft_step *soft_break,
  uint32_t             *next_ins
)
{
  soft_break->address = next_ins;
  rtems_debugger_target_swbreak_control(
    true,
    (uintptr_t) soft_break->address,
    4
  );
}

static void restore_soft_step( microblaze_soft_step *bp )
{
  /*
   * Only restore if the breakpoint is active and the instruction at the address
   * is the breakpoint instruction.
   */
  if ( bp->address != NULL ) {
    rtems_debugger_target_swbreak_control( false, (uintptr_t) bp->address, 4 );
  }

  bp->address = NULL;
}

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

static int microblaze_debug_probe( rtems_debugger_target *target )
{
  uint32_t    msr;
  uint32_t    pvr0;
  uint32_t    pvr3;
  const char *version = NULL;

  rtems_debugger_printf(
    "rtems-db: MicroBlaze\n"
  );

  _CPU_MSR_GET( msr );

  if ( ( msr & MICROBLAZE_MSR_PVR ) == 0 ) {
    rtems_debugger_printf(
      "rtems-db: Processor Version Registers not supported\n"
    );
    return 0;
  }

  _CPU_PVR0_GET( pvr0 );

  switch ( MICROBLAZE_PVR0_VERSION_GET( pvr0 ) ) {
   case 0x1:
     version = "v5.00.a";
     break;
   case 0x2:
     version = "v5.00.b";
     break;
   case 0x3:
     version = "v5.00.c";
     break;
   case 0x4:
     version = "v6.00.a";
     break;
   case 0x5:
     version = "v7.00.a";
     break;
   case 0x6:
     version = "v6.00.b";
     break;
   case 0x7:
     version = "v7.00.b";
     break;
   case 0x8:
     version = "v7.10.a";
     break;
  }

  rtems_debugger_printf(
    "rtems-db: Version: %s (%d)\n",
    version,
    MICROBLAZE_PVR0_VERSION_GET( pvr0 )
  );

  /* further PVR supported? */
  if ( ( pvr0 >> 31 ) == 0 ) {
    rtems_debugger_printf(
      "rtems-db: Further Processor Version Registers not supported\n"
    );
    return 0;
  }

  _CPU_PVR3_GET( pvr3 );

  hw_breakpoints = MICROBLAZE_PVR3_BP_GET( pvr3 );
  hw_read_watchpoints = MICROBLAZE_PVR3_RWP_GET( pvr3 );
  hw_write_watchpoints = MICROBLAZE_PVR3_WWP_GET( pvr3 );

  rtems_debugger_printf(
    "rtems-db: breakpoints:%" PRIu32
    " read watchpoints:%" PRIu32 " write watchpoints:%" PRIu32 "\n",
    hw_breakpoints,
    hw_read_watchpoints,
    hw_write_watchpoints
  );

  return 0;
}

int rtems_debugger_target_configure( rtems_debugger_target *target )
{
  target->capabilities = ( RTEMS_DEBUGGER_TARGET_CAP_SWBREAK
    | RTEMS_DEBUGGER_TARGET_CAP_PURE_SWBREAK );
  target->reg_num = RTEMS_DEBUGGER_NUMREGS;
  target->reg_offset = microblaze_reg_offsets;
  target->breakpoint = &breakpoint[ 0 ];
  target->breakpoint_size = sizeof( breakpoint );
  return microblaze_debug_probe( target );
}

static void target_print_frame( CPU_Exception_frame *frame )
{
  EXC_FRAME_PRINT( target_printk, "[} ", frame );
}

/* returns true if cascade is required */
static bool target_exception( CPU_Exception_frame *frame )
{
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
  }

  target_printk(
    "[} < resuming frame = %016" PRIxPTR "\n",
    (uintptr_t) frame
  );
  target_print_frame( frame );

  return false;
}

static void target_exception_handler( CPU_Exception_frame *ef )
{
  if ( debug_session_active == false ) {
    /* Falls into fatal error handler */
    return;
  }

  /*
   * Blindly roll back R17 in the exception frame due to exceptions resuming at
   * the next instruction and not the instruction that caused the exception.
   * TODO(kmoore): This does not apply in all cases for MicroBlaze cores that
   *               have a MMU and can generate MMU exceptions.
   */
  ef->r17 = &ef->r17[ -1 ];

  /*
   * Remove single step software breakpoints since they will need to be
   * recalculated for the current instruction.
   */
  restore_soft_step( &next_soft_break );
  restore_soft_step( &target_soft_break );

  /* disable all software breakpoints */
  rtems_debugger_target_swbreak_remove();

  if ( target_exception( ef ) == true ) {
    /* Roll R17 forward for an accurate frame in the fatal error handler */
    ef->r17 = &ef->r17[ 1 ];

    /* Falls into fatal error handler */
    return;
  }

  /* Enable all software breakpoints including added single-step breakpoints */
  rtems_debugger_target_swbreak_insert();

  /* does not return */
  _CPU_Exception_resume( ef );
}

static void rtems_debugger_set_int_reg(
  rtems_debugger_thread *thread,
  size_t                 reg,
  const uint32_t         value
)
{
  const size_t offset = microblaze_reg_offsets[ reg ];

  memcpy( &thread->registers[ offset ], &value, sizeof( uint32_t ) );
}

static const uint32_t rtems_debugger_get_int_reg(
  rtems_debugger_thread *thread,
  size_t                 reg
)
{
  const size_t offset = microblaze_reg_offsets[ reg ];
  uint32_t     value;

  memcpy( &value, &thread->registers[ offset ], sizeof( uint32_t ) );
  return value;
}

static bool tid_is_excluded( const rtems_id tid )
{
  rtems_debugger_threads *threads = rtems_debugger->threads;
  rtems_id               *excludes;
  size_t                  i;

  excludes = rtems_debugger_thread_excludes( threads );

  for ( i = 0; i < threads->excludes.level; ++i ) {
    if ( tid == excludes[ i ] ) {
      return true;
    }
  }

  /* DBSe is dynamically created and destroyed, so might not actually be in the excludes list */
  char name[ RTEMS_DEBUGGER_THREAD_NAME_SIZE ];

  rtems_object_get_name( tid, sizeof( name ), (char *) &name[ 0 ] );

  if ( strcmp( "DBSe", name ) == 0 ) {
    return true;
  }

  return false;
}

static void mb_thread_switch( Thread_Control *executing, Thread_Control *heir )
{
  if ( tid_is_excluded( heir->Object.id ) == true ) {
    rtems_debugger_target_swbreak_remove();
    return;
  }

  /* Insert all software breaks */
  rtems_debugger_target_swbreak_insert();
}

User_extensions_Control mb_ext = {
  .Callouts = { .thread_switch = mb_thread_switch }
};

int rtems_debugger_target_enable( void )
{
  debug_session_active = true;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );

  _MicroBlaze_Debug_install_handler( target_exception_handler, NULL );
  _MicroBlaze_Exception_install_handler( target_exception_handler, NULL );
  _User_extensions_Add_set( &mb_ext );

  rtems_interrupt_lock_release( &target_lock, &lock_context );
  return RTEMS_SUCCESSFUL;
}

int rtems_debugger_target_disable( void )
{
  debug_session_active = false;
  rtems_interrupt_lock_context lock_context;

  rtems_interrupt_lock_acquire( &target_lock, &lock_context );

  _User_extensions_Remove_set( &mb_ext );

  rtems_interrupt_lock_release( &target_lock, &lock_context );
  return RTEMS_SUCCESSFUL;
}

int rtems_debugger_target_read_regs( rtems_debugger_thread *thread )
{
  if (
    rtems_debugger_thread_flag(
      thread,
      RTEMS_DEBUGGER_THREAD_FLAG_REG_VALID
    ) == 0
  ) {
    static const uintptr_t good_address = (uintptr_t) &good_address;
    int                    i;

    memset( &thread->registers[ 0 ], 0, RTEMS_DEBUGGER_NUMREGBYTES );

    /* set all integer register to a known valid address */
    for ( i = 0; i < RTEMS_DEBUGGER_NUMREGS; ++i ) {
      rtems_debugger_set_int_reg( thread, i, (uintptr_t) &good_address );
    }

    if ( thread->frame != NULL ) {
      CPU_Exception_frame *frame = thread->frame;

      *( (CPU_Exception_frame *) thread->registers ) = *frame;
      rtems_debugger_set_int_reg( thread, REG_R0, 0 );
      rtems_debugger_set_int_reg( thread, REG_R1, frame->r1 );
      rtems_debugger_set_int_reg( thread, REG_R2, frame->r2 );
      rtems_debugger_set_int_reg( thread, REG_R3, frame->r3 );
      rtems_debugger_set_int_reg( thread, REG_R4, frame->r4 );
      rtems_debugger_set_int_reg( thread, REG_R5, frame->r5 );
      rtems_debugger_set_int_reg( thread, REG_R6, frame->r6 );
      rtems_debugger_set_int_reg( thread, REG_R7, frame->r7 );
      rtems_debugger_set_int_reg( thread, REG_R8, frame->r8 );
      rtems_debugger_set_int_reg( thread, REG_R9, frame->r9 );
      rtems_debugger_set_int_reg( thread, REG_R10, frame->r10 );
      rtems_debugger_set_int_reg( thread, REG_R11, frame->r11 );
      rtems_debugger_set_int_reg( thread, REG_R12, frame->r12 );
      rtems_debugger_set_int_reg( thread, REG_R13, frame->r13 );
      rtems_debugger_set_int_reg( thread, REG_R14, (uintptr_t) frame->r14 );
      rtems_debugger_set_int_reg( thread, REG_R15, (uintptr_t) frame->r15 );
      rtems_debugger_set_int_reg( thread, REG_R16, (uintptr_t) frame->r16 );
      rtems_debugger_set_int_reg( thread, REG_R17, (uintptr_t) frame->r17 );
      rtems_debugger_set_int_reg( thread, REG_R18, frame->r18 );
      rtems_debugger_set_int_reg( thread, REG_R19, frame->r19 );
      rtems_debugger_set_int_reg( thread, REG_R20, frame->r20 );
      rtems_debugger_set_int_reg( thread, REG_R21, frame->r21 );
      rtems_debugger_set_int_reg( thread, REG_R22, frame->r22 );
      rtems_debugger_set_int_reg( thread, REG_R23, frame->r23 );
      rtems_debugger_set_int_reg( thread, REG_R24, frame->r24 );
      rtems_debugger_set_int_reg( thread, REG_R25, frame->r25 );
      rtems_debugger_set_int_reg( thread, REG_R26, frame->r26 );
      rtems_debugger_set_int_reg( thread, REG_R27, frame->r27 );
      rtems_debugger_set_int_reg( thread, REG_R28, frame->r28 );
      rtems_debugger_set_int_reg( thread, REG_R29, frame->r29 );
      rtems_debugger_set_int_reg( thread, REG_R30, frame->r30 );
      rtems_debugger_set_int_reg( thread, REG_R31, frame->r31 );
      rtems_debugger_set_int_reg(
        thread,
        REG_PC,
        rtems_debugger_target_frame_pc( frame )
      );
      rtems_debugger_set_int_reg( thread, REG_MS, frame->msr );
      rtems_debugger_set_int_reg( thread, REG_EA, (uintptr_t) frame->ear );
      rtems_debugger_set_int_reg( thread, REG_ES, frame->esr );
      rtems_debugger_set_int_reg( thread, REG_BT, (uintptr_t) frame->btr );
      /*
       * Get the signal from the frame.
       */
      thread->signal = rtems_debugger_target_exception_to_signal( frame );
    } else {
      rtems_debugger_set_int_reg(
        thread,
        REG_R1,
        thread->tcb->Registers.r1
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R13,
        thread->tcb->Registers.r13
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R14,
        thread->tcb->Registers.r14
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R15,
        thread->tcb->Registers.r15
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R16,
        thread->tcb->Registers.r16
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R17,
        thread->tcb->Registers.r17
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R18,
        thread->tcb->Registers.r18
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R19,
        thread->tcb->Registers.r19
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R20,
        thread->tcb->Registers.r20
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R21,
        thread->tcb->Registers.r21
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R22,
        thread->tcb->Registers.r22
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R23,
        thread->tcb->Registers.r23
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R24,
        thread->tcb->Registers.r24
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R25,
        thread->tcb->Registers.r25
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R26,
        thread->tcb->Registers.r26
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R27,
        thread->tcb->Registers.r27
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R28,
        thread->tcb->Registers.r28
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R29,
        thread->tcb->Registers.r29
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R30,
        thread->tcb->Registers.r30
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_R31,
        thread->tcb->Registers.r31
      );
      rtems_debugger_set_int_reg(
        thread,
        REG_MS,
        (intptr_t) thread->tcb->Registers.rmsr
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
    ) != 0
  ) {
    /*
     * Only write to debugger controlled exception threads. Do not touch the
     * registers for threads blocked in the context switcher.
     */
    if (
      rtems_debugger_thread_flag(
        thread,
        RTEMS_DEBUGGER_THREAD_FLAG_EXCEPTION
      ) != 0
    ) {
      CPU_Exception_frame *frame = thread->frame;
      frame->r1 = rtems_debugger_get_int_reg( thread, REG_R1 );
      frame->r2 = rtems_debugger_get_int_reg( thread, REG_R2 );
      frame->r3 = rtems_debugger_get_int_reg( thread, REG_R3 );
      frame->r4 = rtems_debugger_get_int_reg( thread, REG_R4 );
      frame->r5 = rtems_debugger_get_int_reg( thread, REG_R5 );
      frame->r6 = rtems_debugger_get_int_reg( thread, REG_R6 );
      frame->r7 = rtems_debugger_get_int_reg( thread, REG_R7 );
      frame->r8 = rtems_debugger_get_int_reg( thread, REG_R8 );
      frame->r9 = rtems_debugger_get_int_reg( thread, REG_R9 );
      frame->r10 = rtems_debugger_get_int_reg( thread, REG_R10 );
      frame->r11 = rtems_debugger_get_int_reg( thread, REG_R11 );
      frame->r12 = rtems_debugger_get_int_reg( thread, REG_R12 );
      frame->r13 = rtems_debugger_get_int_reg( thread, REG_R13 );
      frame->r14 = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_R14 );
      frame->r15 = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_R15 );
      frame->r16 = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_R16 );
      frame->r17 = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_R17 );
      frame->r18 = rtems_debugger_get_int_reg( thread, REG_R18 );
      frame->r19 = rtems_debugger_get_int_reg( thread, REG_R19 );
      frame->r20 = rtems_debugger_get_int_reg( thread, REG_R20 );
      frame->r21 = rtems_debugger_get_int_reg( thread, REG_R21 );
      frame->r22 = rtems_debugger_get_int_reg( thread, REG_R22 );
      frame->r23 = rtems_debugger_get_int_reg( thread, REG_R23 );
      frame->r24 = rtems_debugger_get_int_reg( thread, REG_R24 );
      frame->r25 = rtems_debugger_get_int_reg( thread, REG_R25 );
      frame->r26 = rtems_debugger_get_int_reg( thread, REG_R26 );
      frame->r27 = rtems_debugger_get_int_reg( thread, REG_R27 );
      frame->r28 = rtems_debugger_get_int_reg( thread, REG_R28 );
      frame->r29 = rtems_debugger_get_int_reg( thread, REG_R29 );
      frame->r30 = rtems_debugger_get_int_reg( thread, REG_R30 );
      frame->r31 = rtems_debugger_get_int_reg( thread, REG_R31 );
      frame->msr = rtems_debugger_get_int_reg( thread, REG_MS );
      frame->ear = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_EA );
      frame->esr = rtems_debugger_get_int_reg( thread, REG_ES );
      frame->btr = (uint32_t *) rtems_debugger_get_int_reg( thread, REG_BT );
    }

    thread->flags &= ~RTEMS_DEBUGGER_THREAD_FLAG_REG_DIRTY;
  }

  return 0;
}

uintptr_t rtems_debugger_target_reg_pc( rtems_debugger_thread *thread )
{
  return thread->tcb->Registers.r15;
}

uintptr_t rtems_debugger_target_frame_pc( CPU_Exception_frame *frame )
{
  return (uintptr_t) _MicroBlaze_Get_return_address( frame );
}

uintptr_t rtems_debugger_target_reg_sp( rtems_debugger_thread *thread )
{
  int r;

  r = rtems_debugger_target_read_regs( thread );

  if ( r >= 0 ) {
    return rtems_debugger_get_int_reg( thread, REG_R1 );
  }

  return 0;
}

uintptr_t rtems_debugger_target_tcb_sp( rtems_debugger_thread *thread )
{
  return (uintptr_t) thread->tcb->Registers.r1;
}

#define IGROUP_MASK 0x3f

static uint32_t get_igroup( uint32_t ins )
{
  return ( ins >> 26 ) & IGROUP_MASK;
}

#define REGISTER_MASK 0x1f

static uint32_t get_Ra( uint32_t ins )
{
  return ( ins >> 16 ) & REGISTER_MASK;
}

static uint32_t get_Rb( uint32_t ins )
{
  return ( ins >> 11 ) & REGISTER_MASK;
}

static uint32_t get_Rd( uint32_t ins )
{
  return ( ins >> 21 ) & REGISTER_MASK;
}

#define IMM16_MASK 0xffff

static int32_t get_Imm16( uint32_t ins )
{
  int16_t base = (int16_t) ins & IMM16_MASK;

  return base;
}

#define IMM24_MASK 0xffffff

static int32_t get_Imm24( uint32_t ins )
{
  int32_t base = ins & IMM24_MASK;

  /* Sign-extend manually if necessary */
  if ( ( base & 0x800000 ) != 0 ) {
    base &= 0xFF000000;
  }

  return base;
}

static int64_t get_Imm( uint32_t ins )
{
  if ( ( get_Rd( ins ) & 0x10 ) != 0 ) {
    return get_Imm24( ins );
  }

  return get_Imm16( ins );
}

#define IMM_GROUP 0x2c

static bool is_imm( uint32_t ins )
{
  return get_igroup( ins ) == IMM_GROUP;
}

#define RETURN_GROUP 0x2d

static bool is_return( uint32_t ins )
{
  return get_igroup( ins ) == RETURN_GROUP;
}

/* Unconditional branch */
#define UBRANCH_GROUP 0x26

static bool is_ubranch( uint32_t ins )
{
  return get_igroup( ins ) == UBRANCH_GROUP;
}

/* Comparison branch */
#define CBRANCH_GROUP 0x27

static bool is_cbranch( uint32_t ins )
{
  return get_igroup( ins ) == CBRANCH_GROUP;
}

/* Unconditional Immediate branch */
#define UIBRANCH_GROUP 0x2e

static bool is_uibranch( uint32_t ins )
{
  /* Ra == 0x2 is a memory barrier which continues at the next instruction */
  return get_igroup( ins ) == UIBRANCH_GROUP && get_Ra( ins ) != 0x2;
}

/* Comparison Immediate branch */
#define CIBRANCH_GROUP 0x2f

static bool is_cibranch( uint32_t ins )
{
  return get_igroup( ins ) == CIBRANCH_GROUP;
}

static bool branch_has_delay_slot( uint32_t ins )
{
  if ( is_ubranch( ins ) == true && ( get_Ra( ins ) & 0x10 ) != 0 ) {
    return true;
  }

  if ( is_cbranch( ins ) == true && ( get_Ra( ins ) & 0x10 ) != 0 ) {
    return true;
  }

  if ( is_uibranch( ins ) == true && ( get_Ra( ins ) & 0x10 ) != 0 ) {
    return true;
  }

  if ( is_cibranch( ins ) == true && ( get_Rd( ins ) & 0x10 ) != 0 ) {
    return true;
  }

  return false;
}

/* All return instructions have a delay slot */

static bool branch_is_absolute( uint32_t ins )
{
  return ( is_ubranch( ins ) == true || is_uibranch( ins ) == true ) &&
         ( get_Ra( ins ) & 0x8 ) != 0;
}

/* All returns are absolute */

static bool target_is_absolute( uint32_t ins )
{
  return branch_is_absolute( ins ) == true || is_return( ins ) == true;
}

static bool is_branch( uint32_t ins )
{
  if ( is_ubranch( ins ) == true ) {
    return true;
  }

  if ( is_cbranch( ins ) == true ) {
    return true;
  }

  if ( is_uibranch( ins ) == true ) {
    return true;
  }

  if ( is_cibranch( ins ) == true ) {
    return true;
  }

  return false;
}

#define BRK_RA 0xC

static bool is_brk( uint32_t ins )
{
  return ( is_ubranch( ins ) == true || is_uibranch( ins ) == true ) &&
         get_Ra( ins ) == BRK_RA;
}

static uint32_t get_register_value(
  CPU_Exception_frame *frame,
  uint32_t             target_register
)
{
  if ( target_register == 0 ) {
    return 0;
  }

  /* Assumes all registers are contiguous and accounted for */
  return ( &( frame->r1 ) )[ target_register - 1 ];
}

static void set_frame_pc( CPU_Exception_frame *frame, uint32_t *new_pc )
{
  Per_CPU_Control *cpu_self = _Per_CPU_Get();

  /* Break in progress */
  if ( ( frame->msr & MICROBLAZE_MSR_BIP ) != 0 ) {
    frame->r16 = (uint32_t *) new_pc;
    return;
  }

  /* Exception in progress */
  if ( ( frame->msr & MICROBLAZE_MSR_EIP ) != 0 ) {
    frame->r17 = (uint32_t *) new_pc;
    return;
  }

  /* Interrupt in progress must be determined by stack pointer location */
  if (
    frame->r1 >= (uint32_t) cpu_self->interrupt_stack_low
    && frame->r1 < (uint32_t) cpu_self->interrupt_stack_high
  ) {
    frame->r14 = (uint32_t *) new_pc;
    return;
  }

  /* Default to normal link register */
  frame->r15 = (uint32_t *) new_pc;
}

static uint32_t bypass_swbreaks( uint32_t *addr )
{
  rtems_debugger_target *target = rtems_debugger->target;

  if ( target != NULL && target->swbreaks.block != NULL ) {
    rtems_debugger_target_swbreak *swbreaks = target->swbreaks.block;
    size_t                         i;

    for ( i = 0; i < target->swbreaks.level; ++i ) {
      if ( swbreaks[ i ].address == addr ) {
        return *( (uint32_t *) &( swbreaks[ i ].contents[ 0 ] ) );
      }
    }
  }

  return *addr;
}

static int setup_single_step_breakpoints( CPU_Exception_frame *frame )
{
  /*
   * It may be necessary to evaluate the current instruction and next immediate
   * instruction to determine the address of the "next" instruction and possible
   * branch target instructions
   */
  uint32_t *pc = (uint32_t *) rtems_debugger_target_frame_pc( frame );
  int64_t   imm = 0;
  uint32_t *resume_pc;

  /*
   * Normalize PC address to the real instruction and not any IMM. This deals
   * with a possible cascade of IMM.
   */
  while ( is_imm( bypass_swbreaks( pc ) ) == true ) {
    pc = &pc[ 1 ];
  }

  resume_pc = pc;

  /*
   * If execution ends up on a branch instruction that is preceeded by IMM, bad
   * things can happen since it's not possible to know if the IMM was actually
   * executed or something jumped to the branch directly. Exceptions treat IMM
   * as part of the following instruction, so the RTEMS debugger will do so as
   * well.
   */
  uint32_t bypass_ins = bypass_swbreaks( &pc[ -1 ] );

  if ( is_imm( bypass_ins ) == true ) {
    imm = get_Imm( bypass_ins );
    imm <<= 16;
    resume_pc = &pc[ -1 ];
  }

  uint32_t ins = bypass_swbreaks( pc );
  bool     needs_target_break = false;
  bool     needs_next_break = true;

  if ( is_brk( ins ) == true ) {
    /*
     * If the instruction being stepped is brk or brki, something bad has
     * happened. If this instruction is stepped, the target of the branch (the
     * debug vector) has a brki placed in it which results in an tight infinite
     * recursive call. Under normal circumstances, this shouldn't happen.
     */
    rtems_debugger_printf(
      "rtems-db: Unable to set single-step breakpoints for brk/brki instructions\n"
    );

    return -1;
  }

  if ( is_branch( ins ) == true ) {
    needs_target_break = true;

    /*
     * Unconditional branches (including returns) do not need to break on the
     * next instruction.
     */
    if (
      is_ubranch( ins ) == true
      || is_uibranch( ins ) == true
      || is_return( ins ) == true
    ) {
      needs_next_break = false;
    }
  }

  if ( is_return( ins ) == true ) {
    needs_target_break = true;
    needs_next_break = false;
  }

  if ( needs_next_break == true ) {
    uint32_t *next_ins = &pc[ 1 ];

    if ( branch_has_delay_slot( ins ) == true ) {
      next_ins = &pc[ 2 ];
    }

    if ( is_brk( *next_ins ) == false ) {
      /* setup next instruction software break */
      set_soft_break( &next_soft_break, next_ins );
    }
  }

  if ( imm != 0 ) {
    imm |= ( get_Imm16( ins ) & 0xFFFF );
  } else {
    imm = get_Imm16( ins );
  }

  if ( needs_target_break == true ) {
    /* Calculate target address */
    uintptr_t target_ins = 0;

    if ( target_is_absolute( ins ) == false ) {
      target_ins += (uintptr_t) pc;
    }

    if (
      is_uibranch( ins ) == true || is_cibranch( ins ) == true ||
      is_return( ins ) == true
    ) {
      target_ins += imm;
    }

    if ( is_return( ins ) == true ) {
      uint32_t target_register = get_Ra( ins );
      target_ins += get_register_value( frame, target_register );
    }

    if ( is_ubranch( ins ) == true || is_cbranch( ins ) == true ) {
      uint32_t target_register = get_Rb( ins );
      target_ins += get_register_value( frame, target_register );
    }

    if ( is_brk( *( (uint32_t *) target_ins ) ) == false ) {
      /* setup target instruction software break */
      set_soft_break( &target_soft_break, (uint32_t *) target_ins );
    }
  }

  /* Alter resume address */
  set_frame_pc( frame, resume_pc );

  return 0;
}

int rtems_debugger_target_thread_stepping( rtems_debugger_thread *thread )
{
  CPU_Exception_frame *frame = thread->frame;
  int                  ret = 0;

  if (
    rtems_debugger_thread_flag(
      thread,
      RTEMS_DEBUGGER_THREAD_FLAG_STEP_INSTR
    ) != 0
  ) {
    /* Especially on first startup, frame isn't guaranteed to be non-NULL */
    if ( frame == NULL ) {
      return -1;
    }

    /* set software breakpoint(s) here */
    ret = setup_single_step_breakpoints( frame );
  }

  return ret;
}

int rtems_debugger_target_exception_to_signal( CPU_Exception_frame *frame )
{
  uint32_t BiP = frame->msr & MICROBLAZE_MSR_BIP;
  uint32_t EiP = frame->msr & MICROBLAZE_MSR_EIP;

  if ( BiP != 0 ) {
    return RTEMS_DEBUGGER_SIGNAL_TRAP;
  }

  if ( EiP != 0 ) {
    uint32_t EC = frame->esr & 0x1f;

    switch ( EC ) {
     case 0x0: /* FSL */
     case 0x1: /* Unaligned data access */
     case 0x3: /* instruction fetch */
     case 0x4: /* data bus error */
     case 0x10: /* MMU data storage */
     case 0x11: /* MMU instruction storage */
     case 0x12: /* MMU data TLB miss */
     case 0x13: /* MMU instruction TLB miss */
       return RTEMS_DEBUGGER_SIGNAL_SEGV;

     case 0x7: /* priveleged */
       return RTEMS_DEBUGGER_SIGNAL_TRAP;

     case 0x5: /* div/0 */
     case 0x6: /* FPU */
       return RTEMS_DEBUGGER_SIGNAL_FPE;

     case 0x2: /* illegal opcode (unknown instruction) */

       /* Check for the illegal opcode being used in place of brki */
       if ( rtems_debugger_target_swbreak_is_configured( (uintptr_t) frame->r17 ) ) {
         return RTEMS_DEBUGGER_SIGNAL_TRAP;
       }

     default:
       return RTEMS_DEBUGGER_SIGNAL_ILL;
    }
  }

  /* Default to SIGILL */
  return RTEMS_DEBUGGER_SIGNAL_ILL;
}

void rtems_debugger_target_exception_print( CPU_Exception_frame *frame )
{
  EXC_FRAME_PRINT( rtems_debugger_printf, "", frame );
}

/*
 * Debug hardware is inaccessible to the CPU, so hardware breaks and watchpoints
 * are not supported.
 */
int rtems_debugger_target_hwbreak_insert( void )
{
  return 0;
}

int rtems_debugger_target_hwbreak_remove( void )
{
  return 0;
}

int rtems_debugger_target_hwbreak_control(
  rtems_debugger_target_watchpoint wp,
  bool                             insert,
  uintptr_t                        addr,
  DB_UINT                          kind
)
{
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
