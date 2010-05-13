/*
** start.S -- startup file for Mongoose V BSP based upon crt0.S from
** newlib-1.8.2/libgloss/mips and adapted for RTEMS.
**
** crt0.S -- startup file for MIPS.
**
** Copyright (c) 1995, 1996, 1997 Cygnus Support
**
** The authors hereby grant permission to use, copy, modify, distribute,
** and license this software and its documentation for any purpose, provided
** that existing copyright notices are retained in all copies and that this
** notice is included verbatim in any distributions. No written agreement,
** license, or royalty fee is required for any of the authorized uses.
** Modifications to this software may be copyrighted by their authors
** and need not follow the licensing terms described here, provided that
** the new terms are clearly indicated on the first page of each file where
** they apply.
**
**
** Modification History:
**        01/XX/01  Joel Sherrill, OAR Corp,
**           Modified for Mongoose V BSP for NASA/GSFC Code 582.
**
**        06/XX/01  Greg Menke, Raytheon, Code 582
**           Debug modifications. Removed R4000 dependencies.
**           Added HACKED_PMON defines to facilitate startup.
**           Added DEFAULT_EXIT_RETURN_TO_MONITOR option.
**
**        11/14/01  A.Ferrer, NASA/GSFC, Code 582
**           Cleanup for ST5 mission.
**
**        11/27/01  A.Ferrer, NASA/GSFC, Code 582
**           Added cache flush routines.
*/

#ifndef LANGUAGE_ASSEMBLY
#define LANGUAGE_ASSEMBLY
#endif

#include <rtems/asm.h>
#include <bsp/regs.h>
#include "mg5.h"

#ifdef __mips16
   /* This file contains 32 bit assembly code.  */
   .set nomips16
#endif

/*
** defined by linkcmds, pointing to the start of the relocation target
** memory, referenced in this way so we can avoid defining it
** multiply
*/
   .bss
   .word 0
   .text
   .align 2

/**********************************************************************
**
** Function: _start
*/

   /* Without the following nop, GDB thinks _start is a data variable.
   ** This is probably a bug in GDB in handling a symbol that is at the
   ** start of the .text section.
   */
    nop
   .globl  _start
   .ent    _start

   .globl putch_rom
_start:
   .set    noreorder
   $LF1 = . + 8

   /*
   ** Get the address of start into $5 in a position independent fashion.
   ** This lets us know whether we have been relocated or not.
   */

   bal     $LF1
   nop
_branch:
   move    a1, ra      /* save return address from the jump above */

   /* ensure we're sane before doing anything */

   li      t0, SR_CU0|SR_PE
   mtc0    t0, C0_SR
   nop
   li      t0, 0
   mtc0    t0, C0_DCIC
   nop
   mtc0    t0, C0_CAUSE
   nop

   /*
   ** Call cpuinit. Masking used to call EEPROM address of _cpuinit.  Label is RAM label.
   */
   move   t2,a1
   and    t2,0xffff0000
   la     t0,_cpuinit
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   /*
   ** Configure UART
   */
   move   t2,a1
   and    t2,0xffff0000
   la     t0,config_uart
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   /*
   ** Print 'b'.  Show that we started.
   */
   move   t2,a1
   and    t2,0xffff0000
   li     a0,'b'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   li	k0,0
   li   k1,0

   move t1,a1
   nop
   li	t2,0xa0000000	/* lower limit of kseg1 */
   li   t3,0xbfffffff	/* upper limit of kseg1 */

   subu	t0,t1,t2
   srl  t0,31		/* shift high bit down to bit 0 */
   bnez t0,1f		/* booting from below kseg1 */

   subu t0,t3,t1
   srl  t0,31		/* shift high bit down to bit 0 */
   bnez t0,1f		/* booting from above kseg1 */

   /*
   ** Call IcacheFlush. Masking used to call EEPROM address of IcacheFlush.  Label is RAM label.
   */
   move   t2,a1
   and    t2,0xffff0000
   la     t0,IcacheFlush
   and    t0,0x0000ffff
   or     t0,t2
   move	  k0,t0		/* save cache flush in-prom address */
   jal    t0
   nop

   /*
   ** Print 'I'.  Show that we flushed I cache.
   */
   move   t2,a1
   and    t2,0xffff0000
   li     a0,'I'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   /*
   ** Call DcacheFlush. Masking used to call EEPROM address of DcacheFlush.  Label is RAM label.
   */
   move   t2,a1
   and    t2,0xffff0000
   la     t0,DcacheFlush
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   /*
   ** Print 'D'.  Show that we flushed D cache.
   */
   move   t2,a1
   and    t2,0xffff0000
   li     a0,'D'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   move	  k1,t0		/* save cache flush in-prom address */
   jal    t0
   nop

1:
   /*
   ** Print ' RTEMS  b'.  Show that we are booting.
   */
   move   t2,a1
   and    t2,0xffff0000
   li     a0,' '
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'R'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'T'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'E'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'M'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'S'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,' '
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   move   t2,a1
   and    t2,0xffff0000
   li     a0,'b'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   /*
   ** get the address of the _branch label above as it would appear in
   ** the relocated code
   */

   la  a2, _branch                 /* relocation destination */
   beq a1, a2, _start_in_ram       /* skip relocating if we're already there */
   nop

   /* relocate the code from EEPROM to RAM */

   /*
   ** Print 'r'
   */
   move   t2,a1
   and    t2,0xffff0000
   li     a0,'r'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal	  t0
   nop

   la		a3, _edata
relocate:
   lw		t0, (a1)            /* load from EEPROM */
   addu		a1, 4
   sw		t0, (a2)            /* store to RAM */
   addu		a2, 4
   bne		a2, a3, relocate    /* copied all the way to edata? */
   nop

   /*
   ** Print 'R'
   */
   li     a0,'R'
   la     t0,putch_rom
   and    t0,0x0000ffff
   or     t0,t2
   jal    t0
   nop

   la  a2, _start_in_ram
   jr  a2
   nop
   .end _start

/**********************************************************************
**
** Function: _start_in_ram
*/

   .globl  _start_in_ram
   .ent _start_in_ram
_start_in_ram:

   /*
   ** Print 'S'.  Already in RAM no need to reference EEPROM address.
   */
   li	a0,'S'
   jal	putch_rom
   nop

   la	gp, _gp			/* set the global data pointer */
   .end _start_in_ram

/**********************************************************************
**
** Function: zerobss
*/
   .globl  __memsize
   .globl  zerobss
   .ent    zerobss
zerobss:

   /*
   ** Print 'z'.  Starting to zero out bss.
   */
   li a0,'z'
   jal putch_rom
   nop

   la		v0, _fbss
   la		v1, _end
3:
   sw		zero,0(v0)
   bltu		v0,v1,3b
   addiu	v0,v0,4               /* executed in delay slot */

   la  t0, _stack_init         /* initialize stack so we */

   /*
   ** We must subtract 24 bytes for the 3 8 byte arguments to main, in
   ** case main wants to write them back to the stack.  The caller is
   ** supposed to allocate stack space for parameters in registers in
   ** the old MIPS ABIs.  We must do this even though we aren't passing
   ** arguments, because main might be declared to have them.
   **
   ** Some ports need a larger alignment for the stack, so we subtract
   ** 32, which satisifes the stack for the arguments and keeps the
   ** stack pointer better aligned.
   */
   subu    t0,t0,32
   move    sp,t0               /* set stack pointer */
   nop

   /*
   ** Print 'Z'.  Finished zeroing bss.
   */
   li	a0,'Z'
   jal	putch_rom
   nop

   .end    zerobss

/**********************************************************************
**
** Function: _init
*/
   .globl  exit .text
   .globl  _initialize_rtems
   .ent    _initialize_rtems
_initialize_rtems:

   /*
   ** Print 'i'.  Starting to initialize RTEMS.
   */
   li		a0, 'i'
   jal		putch_rom
   nop

   /*
   ** Save the boot-time addresses of the I & D cache flush routines.
   ** Note, if we're running from RAM, we cannot manipulate the cache
   ** so we just disable the cache flush functions.
   */
   la		a0,_promIcache
   sw		k0,0(a0)
   nop

   la		a0,_promDcache
   sw		k1,0(a0)
   nop

   move		a0,zero         /* set command line to 0 */
   jal		boot_card           /* call the program start function */
   nop

   /*
   ** fall through to the "exit" routine
   */
   jal _sys_exit
   nop
   .end _initialize_rtems

/**********************************************************************
**
** Function: _sys_exit
**
** Exit from the application by jumping to PMON address in EEPROM.
*/
   .globl  _sys_exit
   .ent _sys_exit
_sys_exit:
   la  t0, PMON_ADDRESS
   jal t0
   .end _sys_exit

/**********************************************************************
**
** function: putch
** input   : ASCII character in A0
** registers used: ra, a0, t0, t1
**
*/
   .globl putch_rom
   .ent   putch_rom
putch_rom:

   /*
   ** Delay for UART
   */
   li   t0, 1000
   move t1, zero
2:
   beq  t0, t1, 3f
   addu t1, 1
   b    2b
   nop

3:
   /*
   ** Print a character out from a0
   */

   li   t0, MG5_INT_STATUS_REG      /* load uart register base address */
   lw   t1, 0(t0)                   /* Read status */
   nop
   and  t1, t1, UART_0_TX_READY_BIT /* see if the transmitter is ready */
   beq  t1 , zero , 1f              /* skip uart output if not ready */
   nop
   la   t0, MG5_UART_0_TX_REG
   sw   a0, 0(t0)
   nop

1: /*
   ** if jumped to here, UART was not ready...forget it
   */
   j    ra
  .end putch_rom

/**********************************************************************
**
** function: config_uart
** registers used: ra, t0, t1
**
*/

   .globl config_uart
   .ent   config_uart
config_uart:

   /*
   **  Configure UART 0
   */

   /* First, reset the uart */
   la   t0, MG5_COMMAND_REG
   li   t1, UART_RESET_BIT
   sw   t1, 0(t0)

   /* Next, set the baud rate register for 19200 with a clock speed of 12 Mhz*/
   la   t0, MG5_UART_0_BAUD_REG
   li   t1, 0x02700270
   sw   t1, 0(t0)

   /* Now, clear the reset bit & set the tx enable bit */
   la   t0, MG5_COMMAND_REG
   li   t1, UART_0_TX_ENABLE_BIT
   sw   t1, 0(t0)

   /*
   ** return
   */
   j    ra
.end config_uart

/*************************************************************
* CpuInit:
*   Perform CPU-specific initialization
*   This routine is only callable from assembly because it
*   clobbers s7. It should be called from your ROM-based startup
*   code. It returns:
*       s0 = address of cache flush routine
*/

   .globl  _cpuinit
   .ent _cpuinit
_cpuinit:

   /*
   ** BIU/Cache config register setup
   **
   ** RES    = 0: 31 -> 18 : Reserved
   ** RES    = 1: 17       : Reserved must be set to 1 (Synova Manual)
   ** RES    = 0: 16       : Reserved must be set to 0 (Synova Manual)
   ** BGNT   = 0: 15       : Disable Bus Grant (set to 0)
   ** NOPAD  = 1: 14       : No padding of waitstates between transactions
   ** RDPRI  = 1: 13       : Loads have priority over stores
   ** INTP   = 1: 12       : Interrupts are active high
   ** IS1    = 1: 11       : Enable I-Cache
   ** IS0    = 0: 10       : Hardwired to zero
   ** IBLKSZ =10:  9 ->  8 : I-Cache refill size = 8 words
   ** DS     = 1:  7       : Enable D-Cache
   ** RES    = 0:  6       : Hardwared to zero
   ** DBLKSZ =10:  5 ->  4 : D-Cache refill block size 8 words
   ** RAM    = 0:  3       : No Scratchpad RAM
   ** TAG    = 0:  2       : Disable tag test
   ** INV    = 0:  1       : Disable invalidate mode
   ** LOCK   = 0:  0       : Disable cache lock
   **
   ** 0x00027AA0 caches on
   ** 0x00027220 caches off
   */
   li  t0,0x00027aa0
   sw  t0,M_BIU

   /*
   ** Refresh register setup
   **
   ** set 94 clock cycles at 12Mhz
   */
   li  t1,M_RTIC
   li  t0,0x5E
   sw  t0,(t1)

   /*
   ** DRAM register setup
   **
   **
   ** RESERVED=0: 31 -> 29 : Reserved
   ** SYNC  = 0 : 27       : No Syncronous DRAM
   ** SCFG  = 0 : 26       : No Syncronous DRAM
   ** DMARDY =1 : 25       : Internal DRDY for DMA
   ** DMABLK =0 : 24 -> 22 : 2 word blk size for DMA transfers
   ** DPTH = 0  : 21 -> 20 : No interleaved or syncronous memory
   ** RDYW = 0  : 19       : No interleaved or syncronous memory
   ** PGSZ = 110: 18 -> 16 : Page size = 1K
   ** PGMW = 0  : 15       : Disable page mode write
   ** RFWE = 0  : 14 -> 13 : Allow BIU to do non-DRAM work during refresh
   ** RFEN = 1  : 12       : Enable Refresh generator
   ** RDYEN = 1 : 11       : Internal DRDY
   ** BFD =   1 : 10       : Block fetch disable
   ** PE =    0 : 9        : No parity checking
   ** RPC =   0 : 8 -> 7   : RAS Precharge = 2 SYSCLK cycles
   ** RCD =   1 : 6 -> 5   : RAS-to-CAS delay = 3 cycles
   ** CS  =   0 : 4        : CAS shortened by 1/2 cycle
   ** CL  =   1 : 3 -> 1   : 2.5 cycle CAS pulse width
   ** DCE =   1 : 0        : Enable DRAM controller
   */
   li  s0,0x02061C23
   sw  s0,M_DRAM

   /*
   ** SRAM setup
   ** Dont Care about this, we are not using SRAM
   ** Power on default of 0x0 is ok
   */
   li  t0,0
   sw  t0,M_SRAM

   /*
   ** SPEC0 setup
   **
   ** SPEC0 contains the BCRT registers, BCRT Shared RAM and EEPROM
   ** This area is configured to use an external waitstate generator
   ** and Data Ready signal.
   ** Also, I see no need to cache this data. It could confuse the
   ** BCRT.
   **
   ** - 9/29/99 - APC - set NOSNOOP to 1 and EXTGNT to 1
   **  Bit 23 = 1 : EXTGNT External data ready = 1
   **  Bit 19 = 1 : NOSNOOP No Snoop = 1
   */
   li  t0,0x00880000         # use external waitstates
   sw  t0,M_SPEC0

   /*
   ** SPEC1 setup
   **
   ** This is where most of the SDB I/O is.
   **
   **  Important fields:
   **
   **  Bit 19 =1 : NOSNOOP = 1
   **  Bit 6 = 1 : Enable DAWG
   **  Bit 5 -> 0  = 1 : 1 Wait state
   */
   li  t0,0x00880000      /* Bit23 EXTGNT set to 1, Bit19 NOSNOOP set to 1 */
   sw  t0,M_SPEC1

   /*
   ** SPEC2 setup
   **
   ** SPEC2 is not currently used on the SDB.
   ** Bit 19 = 1 : NOSNOOP = 1
   **
   **li t0, 0x00080000
   **sw t0,M_SPEC2
   */
   li  t0, 0x0
   sw  t0,M_SPEC2

   /*
   ** SPEC3 Setup
   ** SPEC3 will be used for the SONIC ethernet controller.
   ** Use the same ** of waitstates that the turborocket board uses.
   ** Bit 19 = 1 : NOSNOOP = 1
   **
   **li t0, (SPC_CACHED | SPC_WAITENA | (16<<SPC_WAITSHFT))
   **sw t0,M_SPEC3
   */
   li  t0, 0x0
   sw  t0,M_SPEC3

   /*
   ** Finally, delay to allow RAM to stabilize
   */
   li  t0,2000
1: subu    t0,1
   bne t0,zero,1b
   nop

   /*
   ** Init Mongoose V registers.
   */

   /*
   ** Mongoose V Control Register Setup
   ** For now just setup UART defaults, turn edac off.
   ** May not even need to put anything in here...
   */
   li  t0,0
   sw  t0,MG5_COMMAND_REG

   /*
   ** Setup Mongoose V extended interrupt mask
   */
   li  t0,0
   sw  t0,MG5_INT_MASK_REG

   /*
   ** Clear Mongoose V extended interrupts
   ** Clear all of the pulse interrupts that may be pending.
   */
   li  t0,( EDAC_SERR_BIT | EDAC_MERR_BIT | UART_0_RX_OVERRUN_BIT | UART_0_FRAME_ERR_BIT | UART_1_RX_OVERRUN_BIT | UART_1_FRAME_ERR_BIT | MAVN_WRITE_ACC_BIT | MAVN_READ_ACC_BIT )
   sw  t0,MG5_INT_STATUS_REG

   /*
   ** Setup MAVN Access Priv Register
   */
   li  t0,0x7FFFFFFF  /* Default reset value */
   sw  t0,MG5_MAVN_PRIVLEGE_REG

   /*
   ** Mavn Range Register 0 -- 0 and 1 cover EEPROM
   ** 0xbfc00000 -> 0xbfe00000
   */
   li  t0,( 0xBFC00000 | 0x15 )
   sw  t0,MG5_MAVN_RANGE_0_REG

   /*
   ** Mavn Range Register 1
   ** 0xbfe00000 -> 0xc0000000
   */
   li  t0,( 0xBFE00000 | 0x15 )
   sw  t0,MG5_MAVN_RANGE_1_REG

   /*
   ** Mavn Range Register 2 -- 2 and 3 cover the first RAM
   ** 0x80000000 -> 0x80200000
   */
   li  t0,( 0x80000000 | 0x15 )
   sw  t0,MG5_MAVN_RANGE_2_REG

   /*
   ** Mavn Range Register 3
   ** 0x80200000 -> 0x80400000
   */
   li  t0, ( 0x80200000 | 0x15 )
   sw  t0, MG5_MAVN_RANGE_3_REG

   /*
   ** Mavn Range Register 4 -- IO Space 1
   ** 0xBE00000 -> 0xBe0000200
   */
   li  t0, ( 0xBe000000 | 0x09 )
   sw  t0, MG5_MAVN_RANGE_4_REG

   /*
   ** Mavn Range Register 5 -- IO Space 2
   ** 0xBe200000 -> 0xbe400000
   */
   li  t0, ( 0xBE200000 | 0x15 )
   sw  t0, MG5_MAVN_RANGE_5_REG

   /*
   ** MAVN Error Address Register ( Unstick )
   */
   la      t0, MG5_MAVN_VIOLATION_REG
   lw      t1, 0(t0)

   /*
   ** Read EDAC Error Register to unstick it
   */
   la      t0, MG5_EDAC_ADDR_REG
   lw      t1, 0(t0)

   /*
   ** Enable Mongoose V EDAC
   */
   la      t0, MG5_COMMAND_REG
   li      t1, EDAC_ENABLE_BIT
   sw      t1, 0(t0)
   nop

   /*
   ** Program Watchdog to 10 seconds - If PMON will
   ** run, it will be set to MAX later.
   */
   la      t0, 0xBE000000
   li      t1, 0xA0
   sw      t1, 0(t0)

3: nop

   j ra
   .end _cpuinit

/**********************************************************************
**
** Keep the boot-time address of the I & D cache reset code for
** later on.  If we need to clear the I/D caches, we <must> run from
** non-cached memory.  This means the relocated versions are useless,
** thankfully they are quite small.
*/

_promIcache:	.word 0
_promDcache:	.word 0

	.globl promCopyIcacheFlush
	.ent promCopyIcacheFlush
	.set noreorder
promCopyIcacheFlush:
	move	a0,ra

	la	t1,_promIcache
	lw	t0,0(t1)
	nop
	beqz	t0,1f

	jal	t0
	nop

1:	j	a0
	nop
	.set reorder
	.end promCopyIcacheFlush

	.globl promCopyDcacheFlush
	.ent promCopyDcacheFlush
	.set noreorder
promCopyDcacheFlush:
	move	a0,ra

	la	t1,_promDcache
	lw	t0,0(t1)
	nop
	beqz	t0,1f

	jal	t0
	nop

1:	j	a0
	nop
	.set reorder
	.end promCopyDcacheFlush

/*******************************************************************************
** Function Name:   IcacheFlush
** Description:     This functions flushes the on chip icache.
*/

     .ent IcacheFlush
     .set noreorder
IcacheFlush:

1:
     /*
     ** Assume I cache is already enabled in BIU/Cache setup
     ** Get contents of M_BIU register and save in t1
     */
     li        t0, M_BIU
     lw        t1, 0(t0)

     /*
     ** Isolate I cache
     */
     mfc0      t3, C0_SR        /* Read Status Register */
     nop
     or        t0, t3, SR_ISC   /* Isolate Cache so we don't propagate operations */
     mtc0      t0, C0_SR        /* Write it back to Status Register */
     nop

     /*
     ** Setup for cache flush
     */
     li        t8, 0            /* Store zero */
     li        t9, LR33300_IC_SIZE

icache_write:
     sw        zero, 0(t8)          /* Store zero to memory addres in t8 */
     addu      t8, 4                /* Increment t8 address by 4 */
     bltu      t8, t9, icache_write /* check to see if we are done */
     nop

     /*
     ** De-isolate I cache
     */
     mtc0      t3, C0_SR        /* Load unchanged t3 to Status Register */
     nop

     jal       ra
     nop
     .set reorder
     .end IcacheFlush

/********************************************************
** Function Name:   DcacheFlush
** Description:     This functions flushes the on chip dcache.
*/

     .ent DcacheFlush
     .set noreorder
DcacheFlush:

     /*
     ** isolate icache
     */
     mfc0      t3,C0_SR
     nop
     or        t0, t3, SR_ISC
     mtc0      t0, C0_SR
     nop

     /*
     ** Setup up for cache flush
     */
     li        t8, 0
     li        t9, LR33300_DC_SIZE

dcache_write:
     sw        zero, 0(t8)
     addu      t8, 4
     bltu      t8, t9, dcache_write /* check to see if we are done */
     nop

     /*
     ** De-isolate cache
     */
     mtc0      t3, C0_SR
     nop

     jal       ra
     nop
     .set reorder
     .end DcacheFlush

/* EOF start.S */
