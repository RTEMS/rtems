/*
 *  start.s
 *
 *  Common start code for SPARC.
 *
 *  This is based on the file srt0.s provided with the binary
 *  distribution of the SPARC Instruction Simulator (SIS) found
 *  at ftp://ftp.estec.esa.nl/pub/ws/wsd/erc32.
 *
 *  $Id$
 */

#include <asm.h>

/*
 *  Unexpected trap will halt the processor by forcing it to error state
 */

#define BAD_TRAP \
  ta 0; \
  nop; \
  nop; \
  nop;

/*
 *  Software trap. Treat as BAD_TRAP for the time being...
 */

#define SOFT_TRAP BAD_TRAP


  .seg    "text"
  PUBLIC(start)
  .global start, __bsp_mem_init

SYM(start):
start:

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

  RTRAP( 0, SYM(hard_reset) );                   ! 00 reset trap 
  BAD_TRAP;                                     ! 01 instruction access 
                                                !    exception
  BAD_TRAP;                                     ! 02 illegal instruction
  BAD_TRAP;                                     ! 03 privileged instruction
  BAD_TRAP;                                     ! 04 fp disabled
  TRAP( 5, SYM(window_overflow_trap_handler) ); ! 05 window overflow
  TRAP( 6, SYM(window_underflow_trap_handler) );! 06 window underflow
  BAD_TRAP;                                     ! 07 memory address not aligned
  BAD_TRAP;                                     ! 08 fp exception
  BAD_TRAP;                                     ! 09 data access exception
  BAD_TRAP;                                     ! 0A tag overflow
  BAD_TRAP;                                     ! 0B undefined
  BAD_TRAP;                                     ! 0C undefined
  BAD_TRAP;                                     ! 0D undefined
  BAD_TRAP;                                     ! 0E undefined
  BAD_TRAP;                                     ! 0F undefined
  BAD_TRAP;                                     ! 10 undefined

  /* 
   *  ERC32 defined traps
   */

  BAD_TRAP;                                     ! 11 masked errors
  BAD_TRAP;                                     ! 12 external 1
  BAD_TRAP;                                     ! 13 external 2
  BAD_TRAP;                                     ! 14 UART A RX/TX
  BAD_TRAP;                                     ! 15 UART B RX/TX
  BAD_TRAP;                                     ! 16 correctable memory error
  BAD_TRAP;                                     ! 17 UART error
  BAD_TRAP;                                     ! 18 DMA access error
  BAD_TRAP;                                     ! 19 DMA timeout
  BAD_TRAP;                                     ! 1A external 3
  BAD_TRAP;                                     ! 1B external 4
  BAD_TRAP;                                     ! 1C general purpose timer
  BAD_TRAP;                                     ! 1D real time clock
  BAD_TRAP;                                     ! 1E external 5
  BAD_TRAP;                                     ! 1F watchdog timeout


  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 20 - 23 undefined
  BAD_TRAP;                                     ! 24 cp_disabled
            BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 25 - 27 undefined
  BAD_TRAP;                                     ! 28 cp_exception
            BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 29 - 2B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 2C - 2F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 30 - 33 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 34 - 37 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 38 - 3B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 3C - 3F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 40 - 43 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 44 - 47 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 48 - 4B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 4C - 4F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 50 - 53 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 54 - 57 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 58 - 5B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 5C - 5F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 60 - 63 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 64 - 67 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 68 - 6B undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 6C - 6F undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 70 - 73 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 74 - 77 undefined
  BAD_TRAP; BAD_TRAP; BAD_TRAP; BAD_TRAP;       ! 78 - 7B undefined

/* 
   This is a sad patch to make sure that we know where the
   MEC timer control register mirror is so we can stop the timers
   from an external debugger. It is needed because the control
   register is write-only. Trap 0x7C cannot occure in ERC32...

   We also use this location to store the last location of the
   usable RAM in order not to overwrite the remote debugger with
   the RTEMS work-space area.

*/

	.global SYM(_ERC32_MEC_Timer_Control_Mirror), SYM(rdb_start), SYM(CLOCK_SPEED)
	.global SYM(Configuration)

SYM(rdb_start):
SYM(_ERC32_MEC_Timer_Control_Mirror):

  BAD_TRAP; BAD_TRAP;                           ! 7C - 7D undefined

SYM(CLOCK_SPEED):

  .word	0x0a, 0, 0, 0				! 7E (10 MHz default)
  
  BAD_TRAP; 					! 7F undefined

  /* 
   *  Software traps
   *
   *  NOTE: At the risk of being redundant... this is not a full
   *        table.  The setjmp on the SPARC requires a window flush trap
   *        handler and RTEMS will preserve the entries that were
   *        installed before.
   */

  TRAP( 0x80, SYM(syscall) ); 			! 80 syscall SW trap
  SOFT_TRAP; SOFT_TRAP;  	            	! 81 - 82
  TRAP( 0x83, SYM(window_flush_trap_handler) ); ! 83 flush windows SW trap

  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 84 - 87
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 88 - 8B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 8C - 8F
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 90 - 93
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 94 - 97
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 98 - 9B
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! 9C - 9F
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A0 - A3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A4 - A7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! A8 - AB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! AC - AF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B0 - B3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B4 - B7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! B8 - BB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! BC - BF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C0 - C3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C4 - C7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! C8 - CB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! CC - CF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D0 - D3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D4 - D7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! D8 - DB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! DC - DF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E0 - E3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E4 - E7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! E8 - EB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! EC - EF
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F0 - F3
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F4 - F7
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! F8 - FB
  SOFT_TRAP; SOFT_TRAP; SOFT_TRAP; SOFT_TRAP;   ! FC - FF

/*
 *  This is the hard reset code.
 */

#define PSR_INIT   0x10c0       /* Disable traps, set s and ps */
#define WIM_INIT   2
#define STACK_SIZE 16 * 1024

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

	or	%g1, 0x20, %g1
        wr      %g1, %psr         ! enable traps

        nop
        nop
        nop

	call	__bsp_board_init
	nop


	set	(SYM(rdb_start)), %g6	! End of work-space area
	st	%sp, [%g6]
	set	(SYM(Configuration)), %l1
	ld	[%l1+4], %l3		! work_space_size
	sub	%sp, %l3, %sp		! set %sp to area below work_space
	andn	%sp, 0x0f, %sp		! align stack on 16-byte boundary

        mov     %sp, %fp                ! Set frame pointer
        nop

        /*
         *  Copy the initialized data to RAM
         *
         *  FROM:   _endtext 
         *  TO:     _data_start 
         *  LENGTH: (__bss_start - _data_start) bytes
         */
 

        sethi %hi(_endtext),%g2
        or    %g2,%lo(_endtext),%g2     ! g2 = start of initialized data in ROM

        sethi %hi(_data_start),%g3
        or    %g3,%lo(_data_start),%g3  ! g3 = start of initialized data in RAM

        sethi %hi(__bss_start),%g4
        or    %g4,%lo(__bss_start),%g4  ! g4 = end of initialized data in RAM

	cmp   %g2, %g3
	be    1f
	nop

copy_data:
        ldd   [ %g2 ], %g6
        std   %g6 , [ %g3 ]             ! copy this double word
        add   %g3, 8, %g3               ! bump the destination pointer
        add   %g2, 8, %g2               ! bump the source pointer
        cmp   %g3, %g4                  ! Is the pointer past the end of dest?
        bl    copy_data
        nop

        /* clear the bss */
1:

        sethi %hi(_edata),%g2
        or    %g2,%lo(_edata),%g2       ! g2 = start of bss
        sethi %hi(_end),%g3
        or    %g3,%lo(_end),%g3         ! g3 = end of bss
        mov   %g0,%g1                   ! so std has two zeros
zerobss:
        std    %g0,[%g2]
        add    %g2,8,%g2
        cmp    %g2,%g3
        bleu,a zerobss
        nop

        mov     %0, %o2                 ! environ
        mov     %0, %o1                 ! argv
        mov     %0, %o0                 ! argc
        call    SYM(boot_card)
        sub     %sp, 0x60, %sp          ! room for boot_card to save args
        nop

        PUBLIC(BSP_fatal_return)
SYM(BSP_fatal_return):
	mov	1, %g1
        ta      0                       ! Halt if _main returns ...
        nop

/* end of file */
