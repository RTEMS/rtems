#include "asm.h" 

	BEGIN_CODE

	.globl _romStart
	.globl _start
	.globl _romFaultStart
	.globl _led_array
	.text   
SYM(romStart ): 
SYM(_romStart ): 
          # This line is to make compiler happy.
	mov	0, g14
        ldconst 0x120f,r10   # BIST register
/*
	 Setup code for in memory loaded image
*/
/*
	  # Interrupt stack is used by default.
	  # Copy all code (except Fault Table and Fault Handler)
	  # from EPROM into DRAM.
*/
        ldconst 0x120f,r10   //# BIST register
	ldconst 0x2,r3
	stob	r3,0(r10)	//# 2->LED
/*
	  # Zero all uninitialized data
*/
	callx	_zeroBss
        ldconst 0x120f,r10   //# BIST register
	ldconst 0x3,r3
	stob	r3,0(r10)	//# 3->LED
/*
	  # And reinitialize processor.
	  # _start is a few lines below.
	  # _prcb is RAM-based struct PRCB.
*/
	ldconst	0x300, r4
	ldconst	_pmc_start, r5
	ldconst	_ram_prcb, r6
	sysctl	r4, r5, r6
/*
	  # This point will never be reached.
*/

SYM(_pmc_start) :  
SYM(pmc_start) :  
        ldconst 0x120f,r10   # BIST register
	ldconst 0x4,r3
	stob	r3,0(r10)	//# 4->LED
	mov	0, g14
/*
	  # To get ready to invoke procedures.
	  # I'll use supervisor stack.
*/

	ldconst	_svrStackPtr, fp
	lda	16*4(fp), sp 
	ldconst 0x5,r3
	stob	r3,0(r10)	//# 5->LED

	ldconst	0x1F0000, r4
	ldconst	0x1F2000, r3
	ldconst	0, r5
	modpc	r4, r3, r5

	ldconst 7,r3
	stob	r3,0(r10)	//# 7->LED

	callx	_rx_boot_card
/*	  # if _romMain ever returns ...	*/
	b	_romExit

SYM(romFaultStart) :  
SYM(_romFaultStart) :  

	mov	0, g14

	ldconst	0, sf0
/*
	  # To get ready to invoke procedures.
	  # I'll use supervisor stack.
	  # _svrStackPtr is defined directly in rom.ld.
*/
	ldconst	_svrStackPtr, fp
	lda	16*4(fp), sp 
/*
	  # Set processor priority to zero.
*/
	ldconst	0x1F0000, r4
	ldconst	0, r5
	modpc	r4, r4, r5
//	  # Now to real code
// Fix this up	callx	_romFaultMain
	callx	_rx_boot_card
//	  # if _romMain ever returns ...	
	b	_romExit

_romExit :
//	  # if _romMain ever returns ...
//	fmark
	b	_romExit



SYM(led_array):
	.byte 99
	.byte 1
	.byte 2
	.byte 3
	.byte 4
	.byte 5
	.byte 6
	.byte 7
	.byte 8
	.byte 9
	.byte 0xa
	.byte 0xb
	.byte 0xc
