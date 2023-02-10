/* SPDX-License-Identifier: BSD-2-Clause */

/**
 *  Common start code for SPARC.
 *
 *  This is based on the file srt0.s provided with the binary
 *  distribution of the SPARC Instruction Simulator (SIS) found
 *  at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 */

/*
 *  COPYRIGHT (c) 1989-2011.
 *  On-Line Applications Research Corporation (OAR).
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
#include <rtems/score/percpu.h>
#include <bspopts.h>

#if defined(RTEMS_SMP) && defined(BSP_LEON3_SMP)
  #define START_LEON3_ENABLE_SMP
#endif

#define TRAP_SYM(_vector) SYM(trap_##_vector)

/*
 *  Unexpected trap will lead to an RTEMS fatal error.
 */
#define BAD_TRAP(_vector) \
  TRAP_SYM(_vector):; \
  TRAP(_vector, _SPARC_Bad_trap)

/*
 * External interrupt trap
 */
#define ISR_TRAP(_vector) \
  TRAP_SYM(_vector):; \
  TRAP(_vector - 0x10, _SPARC_Interrupt_trap)

/*
 * System call optimized trap table entry
 */
#define FPDIS_TRAP(_handler)  \
  mov   %psr, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
   sethi %hi(SPARC_PSR_EF_MASK), %l3

/*
 * System call optimized trap table entry
 */
#define IRQDIS_TRAP(_handler)  \
  mov   %psr, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
   or   %l0, 0x0f00, %l3; ! Set PIL=0xf to disable IRQ

/*
 * System call optimized trap table entry
 */
#define IRQEN_TRAP(_handler)  \
  mov   %psr, %l0 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
   andn %l0, 0xf00, %l3; ! Set PIL=0 to Enable IRQ

/*
 * Window Overflow optimized trap table entry
 */
#define WOTRAP(_vector, _handler)  \
  sethi %hi(_handler), %l4; \
  jmp   %l4+%lo(_handler); \
    save; \
  nop

/*
 * Window Underflow optimized trap table entry
 */
#define WUTRAP(_vector, _handler)  \
  mov   %wim, %l3 ; \
  sethi %hi(_handler), %l4 ; \
  jmp   %l4+%lo(_handler); \
   sll  %l3, 1, %l4 ! l4 = WIM << 1

/*
 *  Software trap. Treat as BAD_TRAP for the time being...
 */

#define SOFT_TRAP(_vector) BAD_TRAP(_vector)

  .section    ".text"
  PUBLIC(start)
  .global start

SYM(start):
#if SYM(start) != start
start:
#endif 

/*
 *  The trap table has to be the first code in a boot PROM.  But because
 *  the Memory Configuration comes up thinking we only have 4K of PROM, we
 *  cannot have a full trap table and still have room left over to
 *  reprogram the Memory Configuration register correctly.  This file
 *  uses an abbreviated trap which has every entry which might be used
 *  before RTEMS installs its own trap table.
 */

  PUBLIC(trap_table)
SYM(trap_table):

  RTRAP( 0, SYM(hard_reset) );                  ! 00 reset trap
  BAD_TRAP(0x01)                                ! 01 instruction access
                                                !    exception
  BAD_TRAP(0x02)                                ! 02 illegal instruction
  BAD_TRAP(0x03)                                ! 03 privileged instruction
#if defined(SPARC_USE_LAZY_FP_SWITCH)
TRAP_SYM(0x04):
  FPDIS_TRAP(SYM(syscall_lazy_fp_switch));      ! 04 fp disabled
#else
  BAD_TRAP(0x04)                                ! 04 fp disabled
#endif
TRAP_SYM(0x05):
  WOTRAP(5, SYM(window_overflow_trap_handler)); ! 05 window overflow
TRAP_SYM(0x06):
  WUTRAP(6, SYM(window_underflow_trap_handler));! 06 window underflow
  BAD_TRAP(0x07)                                ! 07 memory address not aligned
  BAD_TRAP(0x08)                                ! 08 fp exception
  BAD_TRAP(0x09)                                ! 09 data access exception
  BAD_TRAP(0x0a)                                ! 0A tag overflow
  BAD_TRAP(0x0b)                                ! 0B undefined
  BAD_TRAP(0x0c)                                ! 0C undefined
  BAD_TRAP(0x0d)                                ! 0D undefined
  BAD_TRAP(0x0e)                                ! 0E undefined
  BAD_TRAP(0x0f)                                ! 0F undefined

  /*
   *  External interrupt traps
   */

  ISR_TRAP(0x10)                                ! 10 undefined
  ISR_TRAP(0x11)                                ! 11 external interrupt 1
  ISR_TRAP(0x12)                                ! 12 external interrupt 2
  ISR_TRAP(0x13)                                ! 13 external interrupt 3
  ISR_TRAP(0x14)                                ! 14 external interrupt 4
  ISR_TRAP(0x15)                                ! 15 external interrupt 5
  ISR_TRAP(0x16)                                ! 16 external interrupt 6
  ISR_TRAP(0x17)                                ! 17 external interrupt 7
  ISR_TRAP(0x18)                                ! 18 external interrupt 8
  ISR_TRAP(0x19)                                ! 19 external interrupt 9
  ISR_TRAP(0x1a)                                ! 1A external interrupt 10
  ISR_TRAP(0x1b)                                ! 1B external interrupt 11
  ISR_TRAP(0x1c)                                ! 1C external interrupt 12
  ISR_TRAP(0x1d)                                ! 1D external interrupt 13
  ISR_TRAP(0x1e)                                ! 1E external interrupt 14
  ISR_TRAP(0x1f)                                ! 1F external interrupt 15

  BAD_TRAP(0x20)                                ! 20 undefined
  BAD_TRAP(0x21)                                ! 21 undefined
  BAD_TRAP(0x22)                                ! 22 undefined
  BAD_TRAP(0x23)                                ! 23 undefined
  BAD_TRAP(0x24)                                ! 24 cp_disabled
  BAD_TRAP(0x25)                                ! 25 undefined
  BAD_TRAP(0x26)                                ! 26 undefined
  BAD_TRAP(0x27)                                ! 27 undefined
  BAD_TRAP(0x28)                                ! 28 cp_exception
  BAD_TRAP(0x29)                                ! 29 undefined
  BAD_TRAP(0x2a)                                ! 2A undefined
  BAD_TRAP(0x2b)                                ! 2B undefined
  BAD_TRAP(0x2c)                                ! 2C undefined
  BAD_TRAP(0x2d)                                ! 2D undefined
  BAD_TRAP(0x2e)                                ! 2E undefined
  BAD_TRAP(0x2f)                                ! 2F undefined
  BAD_TRAP(0x30)                                ! 30 undefined
  BAD_TRAP(0x31)                                ! 31 undefined
  BAD_TRAP(0x32)                                ! 32 undefined
  BAD_TRAP(0x33)                                ! 33 undefined
  BAD_TRAP(0x34)                                ! 34 undefined
  BAD_TRAP(0x35)                                ! 35 undefined
  BAD_TRAP(0x36)                                ! 36 undefined
  BAD_TRAP(0x37)                                ! 37 undefined
  BAD_TRAP(0x38)                                ! 38 undefined
  BAD_TRAP(0x39)                                ! 39 undefined
  BAD_TRAP(0x3a)                                ! 3A undefined
  BAD_TRAP(0x3b)                                ! 3B undefined
  BAD_TRAP(0x3c)                                ! 3C undefined
  BAD_TRAP(0x3d)                                ! 3D undefined
  BAD_TRAP(0x3e)                                ! 3E undefined
  BAD_TRAP(0x3f)                                ! 3F undefined
  BAD_TRAP(0x40)                                ! 40 undefined
  BAD_TRAP(0x41)                                ! 41 undefined
  BAD_TRAP(0x42)                                ! 42 undefined
  BAD_TRAP(0x43)                                ! 43 undefined
  BAD_TRAP(0x44)                                ! 44 undefined
  BAD_TRAP(0x45)                                ! 45 undefined
  BAD_TRAP(0x46)                                ! 46 undefined
  BAD_TRAP(0x47)                                ! 47 undefined
  BAD_TRAP(0x48)                                ! 48 undefined
  BAD_TRAP(0x49)                                ! 49 undefined
  BAD_TRAP(0x4a)                                ! 4A undefined
  BAD_TRAP(0x4b)                                ! 4B undefined
  BAD_TRAP(0x4c)                                ! 4C undefined
  BAD_TRAP(0x4d)                                ! 4D undefined
  BAD_TRAP(0x4e)                                ! 4E undefined
  BAD_TRAP(0x4f)                                ! 4F undefined
  BAD_TRAP(0x50)                                ! 50 undefined
  BAD_TRAP(0x51)                                ! 51 undefined
  BAD_TRAP(0x52)                                ! 52 undefined
  BAD_TRAP(0x53)                                ! 53 undefined
  BAD_TRAP(0x54)                                ! 54 undefined
  BAD_TRAP(0x55)                                ! 55 undefined
  BAD_TRAP(0x56)                                ! 56 undefined
  BAD_TRAP(0x57)                                ! 57 undefined
  BAD_TRAP(0x58)                                ! 58 undefined
  BAD_TRAP(0x59)                                ! 59 undefined
  BAD_TRAP(0x5a)                                ! 5A undefined
  BAD_TRAP(0x5b)                                ! 5B undefined
  BAD_TRAP(0x5c)                                ! 5C undefined
  BAD_TRAP(0x5d)                                ! 5D undefined
  BAD_TRAP(0x5e)                                ! 5E undefined
  BAD_TRAP(0x5f)                                ! 5F undefined
  BAD_TRAP(0x60)                                ! 60 undefined
  BAD_TRAP(0x61)                                ! 61 undefined
  BAD_TRAP(0x62)                                ! 62 undefined
  BAD_TRAP(0x63)                                ! 63 undefined
  BAD_TRAP(0x64)                                ! 64 undefined
  BAD_TRAP(0x65)                                ! 65 undefined
  BAD_TRAP(0x66)                                ! 66 undefined
  BAD_TRAP(0x67)                                ! 67 undefined
  BAD_TRAP(0x68)                                ! 68 undefined
  BAD_TRAP(0x69)                                ! 69 undefined
  BAD_TRAP(0x6a)                                ! 6A undefined
  BAD_TRAP(0x6b)                                ! 6B undefined
  BAD_TRAP(0x6c)                                ! 6C undefined
  BAD_TRAP(0x6d)                                ! 6D undefined
  BAD_TRAP(0x6e)                                ! 6E undefined
  BAD_TRAP(0x6f)                                ! 6F undefined
  BAD_TRAP(0x70)                                ! 70 undefined
  BAD_TRAP(0x71)                                ! 71 undefined
  BAD_TRAP(0x72)                                ! 72 undefined
  BAD_TRAP(0x73)                                ! 73 undefined
  BAD_TRAP(0x74)                                ! 74 undefined
  BAD_TRAP(0x75)                                ! 75 undefined
  BAD_TRAP(0x76)                                ! 76 undefined
  BAD_TRAP(0x77)                                ! 77 undefined
  BAD_TRAP(0x78)                                ! 78 undefined
  BAD_TRAP(0x79)                                ! 79 undefined
  BAD_TRAP(0x7a)                                ! 7A undefined
  BAD_TRAP(0x7b)                                ! 7B undefined

/*
   This is a sad patch to make sure that we know where the
   MEC timer control register mirror is so we can stop the timers
   from an external debugger. It is needed because the control
   register is write-only. Trap 0x7C cannot occure in ERC32...
*/

	.global SYM(_ERC32_MEC_Timer_Control_Mirror), SYM(CLOCK_SPEED)

SYM(_ERC32_MEC_Timer_Control_Mirror):

  BAD_TRAP(0x7C)                                ! 7C undefined
  BAD_TRAP(0x7D)                                ! 7D undefined

SYM(CLOCK_SPEED):
TRAP_SYM(0x7e):

  .word	0x0a, 0, 0, 0				! 7E (10 MHz default)

  BAD_TRAP(0x7F)                                ! 7F undefined

  /*
   *  Software traps
   *
   *  NOTE: At the risk of being redundant... this is not a full
   *        table.  The setjmp on the SPARC requires a window flush trap
   *        handler and RTEMS will preserve the entries that were
   *        installed before.
   */

TRAP_SYM(0x80):
  TRAP( 0x80, SYM(syscall) );                   ! 80 halt syscall SW trap
  SOFT_TRAP(0x81)                               ! 81
  SOFT_TRAP(0x82)                               ! 82
TRAP_SYM(0x83):
  TRAP( 0x83, SYM(window_flush_trap_handler) ); ! 83 flush windows SW trap
  SOFT_TRAP(0x84)                               ! 84
  SOFT_TRAP(0x85)                               ! 85
  SOFT_TRAP(0x86)                               ! 86
  SOFT_TRAP(0x87)                               ! 87
  SOFT_TRAP(0x88)                               ! 88

  /*
   *  SW Trap 9-15 Reserved for Operating System
   *  
   *  SPARC_SWTRAP_IRQDIS
   *  SPARC_SWTRAP_IRQEN
   */
TRAP_SYM(0x89):
  IRQDIS_TRAP(SYM(syscall_irqdis));             ! 89 IRQ Disable syscall trap
TRAP_SYM(0x8a):
  IRQEN_TRAP(SYM(syscall_irqen));               ! 8A IRQ Enable syscall trap
#if defined(SPARC_USE_SYNCHRONOUS_FP_SWITCH)
TRAP_SYM(0x8b):
  IRQDIS_TRAP(SYM(syscall_irqdis_fp));          ! 8B IRQ disable
                                                ! and set PSR[EF] syscall trap
#else
  SOFT_TRAP(0x8b)                               ! 8B
#endif
  SOFT_TRAP(0x8c)                               ! 8C
  SOFT_TRAP(0x8d)                               ! 8D
  SOFT_TRAP(0x8e)                               ! 8E
  SOFT_TRAP(0x8f)                               ! 8F
  SOFT_TRAP(0x90)                               ! 90
  SOFT_TRAP(0x91)                               ! 91
  SOFT_TRAP(0x92)                               ! 92
  SOFT_TRAP(0x93)                               ! 93
  SOFT_TRAP(0x94)                               ! 94
  SOFT_TRAP(0x95)                               ! 95
  SOFT_TRAP(0x96)                               ! 96
  SOFT_TRAP(0x97)                               ! 97
  SOFT_TRAP(0x98)                               ! 98
  SOFT_TRAP(0x99)                               ! 99
  SOFT_TRAP(0x9a)                               ! 9A
  SOFT_TRAP(0x9b)                               ! 9B
  SOFT_TRAP(0x9c)                               ! 9C
  SOFT_TRAP(0x9d)                               ! 9D
  SOFT_TRAP(0x9e)                               ! 9E
  SOFT_TRAP(0x9f)                               ! 9F
  SOFT_TRAP(0xa0)                               ! a0
  SOFT_TRAP(0xa1)                               ! a1
  SOFT_TRAP(0xa2)                               ! a2
  SOFT_TRAP(0xa3)                               ! a3
  SOFT_TRAP(0xa4)                               ! a4
  SOFT_TRAP(0xa5)                               ! a5
  SOFT_TRAP(0xa6)                               ! a6
  SOFT_TRAP(0xa7)                               ! a7
  SOFT_TRAP(0xa8)                               ! a8
  SOFT_TRAP(0xa9)                               ! a9
  SOFT_TRAP(0xaa)                               ! aA
  SOFT_TRAP(0xab)                               ! aB
  SOFT_TRAP(0xac)                               ! aC
  SOFT_TRAP(0xad)                               ! aD
  SOFT_TRAP(0xae)                               ! aE
  SOFT_TRAP(0xaf)                               ! aF
  SOFT_TRAP(0xb0)                               ! b0
  SOFT_TRAP(0xb1)                               ! b1
  SOFT_TRAP(0xb2)                               ! b2
  SOFT_TRAP(0xb3)                               ! b3
  SOFT_TRAP(0xb4)                               ! b4
  SOFT_TRAP(0xb5)                               ! b5
  SOFT_TRAP(0xb6)                               ! b6
  SOFT_TRAP(0xb7)                               ! b7
  SOFT_TRAP(0xb8)                               ! b8
  SOFT_TRAP(0xb9)                               ! b9
  SOFT_TRAP(0xba)                               ! bA
  SOFT_TRAP(0xbb)                               ! bB
  SOFT_TRAP(0xbc)                               ! bC
  SOFT_TRAP(0xbd)                               ! bD
  SOFT_TRAP(0xbe)                               ! bE
  SOFT_TRAP(0xbf)                               ! bF
  SOFT_TRAP(0xc0)                               ! c0
  SOFT_TRAP(0xc1)                               ! c1
  SOFT_TRAP(0xc2)                               ! c2
  SOFT_TRAP(0xc3)                               ! c3
  SOFT_TRAP(0xc4)                               ! c4
  SOFT_TRAP(0xc5)                               ! c5
  SOFT_TRAP(0xc6)                               ! c6
  SOFT_TRAP(0xc7)                               ! c7
  SOFT_TRAP(0xc8)                               ! c8
  SOFT_TRAP(0xc9)                               ! c9
  SOFT_TRAP(0xca)                               ! cA
  SOFT_TRAP(0xcb)                               ! cB
  SOFT_TRAP(0xcc)                               ! cC
  SOFT_TRAP(0xcd)                               ! cD
  SOFT_TRAP(0xce)                               ! cE
  SOFT_TRAP(0xcf)                               ! cF
  SOFT_TRAP(0xd0)                               ! d0
  SOFT_TRAP(0xd1)                               ! d1
  SOFT_TRAP(0xd2)                               ! d2
  SOFT_TRAP(0xd3)                               ! d3
  SOFT_TRAP(0xd4)                               ! d4
  SOFT_TRAP(0xd5)                               ! d5
  SOFT_TRAP(0xd6)                               ! d6
  SOFT_TRAP(0xd7)                               ! d7
  SOFT_TRAP(0xd8)                               ! d8
  SOFT_TRAP(0xd9)                               ! d9
  SOFT_TRAP(0xda)                               ! dA
  SOFT_TRAP(0xdb)                               ! dB
  SOFT_TRAP(0xdc)                               ! dC
  SOFT_TRAP(0xdd)                               ! dD
  SOFT_TRAP(0xde)                               ! dE
  SOFT_TRAP(0xdf)                               ! dF
  SOFT_TRAP(0xe0)                               ! e0
  SOFT_TRAP(0xe1)                               ! e1
  SOFT_TRAP(0xe2)                               ! e2
  SOFT_TRAP(0xe3)                               ! e3
  SOFT_TRAP(0xe4)                               ! e4
  SOFT_TRAP(0xe5)                               ! e5
  SOFT_TRAP(0xe6)                               ! e6
  SOFT_TRAP(0xe7)                               ! e7
  SOFT_TRAP(0xe8)                               ! e8
  SOFT_TRAP(0xe9)                               ! e9
  SOFT_TRAP(0xea)                               ! eA
  SOFT_TRAP(0xeb)                               ! eB
  SOFT_TRAP(0xec)                               ! eC
  SOFT_TRAP(0xed)                               ! eD
  SOFT_TRAP(0xee)                               ! eE
  SOFT_TRAP(0xef)                               ! eF
  SOFT_TRAP(0xf0)                               ! f0
  SOFT_TRAP(0xf1)                               ! f1
  SOFT_TRAP(0xf2)                               ! f2
  SOFT_TRAP(0xf3)                               ! f3
  SOFT_TRAP(0xf4)                               ! f4
  SOFT_TRAP(0xf5)                               ! f5
  SOFT_TRAP(0xf6)                               ! f6
  SOFT_TRAP(0xf7)                               ! f7
  SOFT_TRAP(0xf8)                               ! f8
  SOFT_TRAP(0xf9)                               ! f9
  SOFT_TRAP(0xfa)                               ! fA
  SOFT_TRAP(0xfb)                               ! fB
  SOFT_TRAP(0xfc)                               ! fC
  SOFT_TRAP(0xfd)                               ! fD
  SOFT_TRAP(0xfe)                               ! fE
  SOFT_TRAP(0xff)                               ! fF

/*
 *  This is the hard reset code.
 */

#define PSR_INIT   0x10c0       /* Disable traps, set s and ps */
#define WIM_INIT   2

        PUBLIC(hard_reset)
SYM(hard_reset):

/* Common initialisation */

        set     SYM(trap_table), %g1    ! Initialize TBR
        mov     %g1, %tbr

        mov	%psr, %g1         	! Initialize WIM
	add	%g1, 1, %g2
	and	%g2, 0x7, %g2
	set	1, %g3
	sll	%g3, %g2, %g3
        mov     %g3, %wim

	or	%g1, 0xf20, %g1
        wr      %g1, %psr		! enable traps and disable ints

        nop
        nop
        nop

        mov     %sp, %o0 ! save end of usable RAM provided by the boot loader
        set     SYM(_Per_CPU_Information), %g6 ! set per-CPU control
        set     SYM(_ISR_Stack_size), %l0 ! get ISR stack size

#if defined(START_LEON3_ENABLE_SMP)
        rd      %asr17, %l7 ! get CPU identifier
        srl     %l7, LEON3_ASR17_PROCESSOR_INDEX_SHIFT, %l7
        add     %l7, 1, %l1 ! get ISR stack offset for this CPU
        smul    %l1, %l0, %l0
        sll     %l7, PER_CPU_CONTROL_SIZE_LOG2, %l2 ! per-CPU for this CPU
        add     %g6, %l2, %g6
#endif
        set     SYM(_ISR_Stack_area_begin), %sp ! set stack pointer
        add     %sp, %l0, %sp
        andn    %sp, CPU_STACK_ALIGNMENT - 1, %sp
        sub     %sp, SPARC_MINIMUM_STACK_FRAME_SIZE, %sp
        mov     %sp, %fp ! set frame pointer

#if defined(START_LEON3_ENABLE_SMP)
        /* If .Lbootcpuindex < 0 then assign us as boot CPU and continue. */
        set     SYM(.Lbootcpuindex), %l0
        mov     -1, %l1
        casa    [%l0] (10), %l1, %l7
        cmp     %l1, %l7
        bne     .Lbootsecondarycpu
         nop
#endif

        /* clear the bss */

        sethi %hi(__bss_start), %g2     ! g2 = start of bss
        or    %g2,%lo(__bss_start),%g2

        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
.Lzerobss:
        std    %g0,[%g2]
        add    %g2,8,%g2
        cmp    %g2,%g3
        bleu,a .Lzerobss
        nop

        call    SYM(_SPARC_Memory_initialize)
         nop ! o0 = end of usable RAM provided by the boot loader

        call    SYM(boot_card)          ! does not return
         mov    %g0, %o0                ! command line

#if defined(START_LEON3_ENABLE_SMP)
.Lbootsecondarycpu:
        call    SYM(bsp_start_on_secondary_processor) ! does not return
         mov    %g6, %o0

        /*
         * This is the index of the boot CPU.  Set by the first CPU at boot to
         * its CPU index.
         */
        .section .data, "aw"
        .align  4
        .type   .Lbootcpuindex, #object
        .size   .Lbootcpuindex, 4
.Lbootcpuindex:
        .long   -1
#endif

/* end of file */
