/*
 * crt0.S -- startup file for hppa on RTEMS
 *
 *  $Id$
 */

	.COPYRIGHT "crt0.S for hppa"

	.DATA

_progname  	.STRINGZ "simhppa"
_crt0_argv      .WORD    _progname, 0

/* 
 * Set up the standard spaces (sections) These definitions come
 * from /lib/pcc_prefix.s.
 */
	.TEXT
	
/*
 * stuff we need that is defined elsewhere.
 */
	.IMPORT boot_card, CODE
	.IMPORT _bss_start, DATA
	.IMPORT _bss_end, DATA
	.IMPORT environ, DATA

/*
 * start -- set things up so the application will run.
 *
 */
        .PROC
        .CALLINFO SAVE_SP, FRAME=48
        .EXPORT $START$,ENTRY
        .EXPORT start,ENTRY
$START$
start:

/*
 * Get a stack
 */
        
	ldil 	L%crt_stack+48,%r30
        ldo 	R%crt_stack+48(%r30),%r30

/*
 * we need to set %r27 (global data pointer) here
 */

	ldil 	L%$global$,%r27
        ldo 	R%$global$(%r27),%r27

/*
 * zerobss -- zero out the bss section
 * XXX We don't do this since simulator and boot rom will do this for us.
 */
#if 0
	; load the start of bss
	ldil 	L%_bss_start,%r4
        ldo 	R%_bss_start(%r4),%r4

	;  load the end of bss
	ldil 	L%_bss_end,%r5
        ldo 	R%_bss_end(%r5),%r5


bssloop
	addi	-1,%r5,%r5			; decrement _bss_end
	stb	%r0,0(0,%r5)			; we do this by bytes for now even
						; though it is slower, it is safer
	combf,=	%r4,%r5, bssloop	
	nop
#endif
    
	ldi	1,%ret0

/*
 * Call the "boot_card" routine from the application to get it going.
 * We call it as boot_card(1, argv, 0)
 */

        copy    %r0, %r24

	ldil 	L%_crt0_argv,%r25
        ldo 	R%_crt0_argv(%r25),%r25

	bl	boot_card,%r2
        ldo     1(%r0), %r26

        .PROCEND
/*
 * _exit -- Exit from the application. Normally we cause a user trap
 * 	    to return to the ROM monitor for another run, but with
 *	    this monitor we can not. Still, "C" wants this symbol, it
 *	    should be here. Jumping to 0xF0000004 jumps back into the
 *	    firmware, while writing a 5 to 0xFFFE0030 causes a reset.
 */
_exit_fallthru
	.PROC
	.CALLINFO
	.ENTRY
	
	;; This just causes a breakpoint exception
	break	0x0,0x0
        bv,n    (%rp)
	nop
	.EXIT
	.PROCEND

/*
 * _sr4export -- support for called functions. (mostly for GDB)
 */
	.EXPORT _sr4export, ENTRY
_sr4export:
	.PROC
	.CALLINFO
	.ENTRY

	ble	0(%sr4,%r22)
	copy	%r31,%rp
	ldw	-18(%sr0,%sp),%rp
	ldsid	(%sr0,%rp),%r1
	mtsp	%r1,%sr0
	be,n	0(%sr0,%rp)
	nop
	.EXIT
	.PROCEND


#if 0
        .subspa $UNWIND_START$,QUAD=0,ALIGN=8,ACCESS=0x2c,SORT=56
        .export $UNWIND_START
$UNWIND_START
        .subspa $UNWIND$,QUAD=0,ALIGN=8,ACCESS=0x2c,SORT=64
        .subspa $UNWIND_END$,QUAD=0,ALIGN=8,ACCESS=0x2c,SORT=72
        .export $UNWIND_END
$UNWIND_END
        .subspa $RECOVER_START$,QUAD=0,ALIGN=4,ACCESS=0x2c,SORT=73
        .export $RECOVER_START
$RECOVER_START
        .subspa $RECOVER$,QUAD=0,ALIGN=4,ACCESS=0x2c,SORT=80
        .subspa $RECOVER_END$,QUAD=0,ALIGN=4,ACCESS=0x2c,SORT=88
        .export $RECOVER_END
$RECOVER_END
#endif

/*
 * Here we set up the standard date sub spaces.
 *
 * Set up some room for a stack. We just grab a chunk of memory.
 * We also setup some space for the global variable space, which
 * must be done using the reserved name "$global$" so "C" code
 * can find it. The stack grows towards the higher addresses.
 */

#if 0
        .subspa $DATA$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=16
        .subspa $SHORTDATA$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=24
        .subspa $GLOBAL$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=40
        .export $global$
$global$
        .subspa $SHORTBSS$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=80,ZERO
        .subspa $BSS$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=82,ZERO

        .subspa $STACK$,QUAD=1,ALIGN=8,ACCESS=0x1f,SORT=88,ZERO
#endif

        .export crt_stack
crt_stack
        .comm          0x1000

        .end

