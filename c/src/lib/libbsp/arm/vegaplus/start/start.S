/*
 *  start.S :	  RTEMS entry point
 *
 *  Copyright (C) 2000 Canon Research Centre France SA.
 *  Emmanuel Raguet, mailto:raguet@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 */
	
/* Register definition */

.equ CNTL_BASE_ADR,	 0xF3000 /* Base address of registers */
.equ PORTCNTL,		 0x0C60
.equ CSCNTL0_0,		 0x0C00  /*  Offset of CS0CNTL */
.equ CSCNTL0_1,		 0x0C04  /*  Offset of CS0CNTL */
.equ CSCNTL0_2,		 0x0C08  /*  Offset of CS0CNTL */
.equ CSCNTL1_0,		 0x0C20  /*  Offset of CS0CNTL */
.equ CSCNTL1_1,		 0x0C24  /*  Offset of CS0CNTL */
.equ CSCNTL1_2,		 0x0C28  /*  Offset of CS0CNTL */
.equ CNTL_CLK_ADR, 	 0xF2000 /*  Base address of registers */
.equ CLKCNTL,		 0x08F4  /*  Offset of CS0CNTL */
.equ INTHPAI,		 0x0800		
.equ INTEOI,		 0x0808
.equ EOI,		 0x80

/* Some standard definitions...*/

.equ Mode_USR,        	     0x10
.equ Mode_FIQ,        	     0x11
.equ Mode_IRQ,        	     0x12
.equ Mode_SVC,        	     0x13
.equ Mode_ABT,        	     0x17
.equ Mode_ABORT,             0x17
.equ Mode_UNDEF,      	     0x1B
.equ Mode_SYS,        	     0x1F /*only available on ARM Arch. v4*/

.equ I_Bit,           	     0x80
.equ F_Bit,           	     0x40


.equ Mode_SVC_MIRQ,	Mode_SVC | I_Bit | F_Bit
.equ Mode_SVC_UIRQ,	Mode_SVC
.equ Mode_IRQ_MIRQ,	Mode_SVC | I_Bit | F_Bit


.equ MARK_STACK,  0    /*Fill every stack with a pattern for debug (0 or 1)*/
 
/*-----------------------------------------------------------------------------
 * Definitions
 ----------------------------------------------------------------------------*/
.equ PID_RAM_Limit,        0x1800

/* stack size definition */
.equ FIQ_StackSize,     0x400		/* FIQ stack size */                   
.equ IRQ_StackSize,     0xE00		/* IRQ stack size */         
.equ SVC_StackSize,     0x200		/* SVC stack size */         
.equ ABORT_StackSize,   0x100		/* ABORT stack size */ 
.equ UNDEF_StackSize,   0x100        	/* UNDEF stack size */        

/* sack size address */
.equ Stack_Limit,     PID_RAM_Limit           
.equ SVC_Stack,       Stack_Limit
.equ ABORT_Stack,     Stack_Limit - SVC_StackSize     
.equ UNDEF_Stack,     ABORT_Stack - ABORT_StackSize	
.equ IRQ_Stack,       UNDEF_Stack - UNDEF_StackSize   
.equ FIQ_Stack,       IRQ_Stack - IRQ_StackSize  	
.equ END_FIQ,	      FIQ_Stack - FIQ_StackSize

	.text
	.globl	_start

/* 
 * This "strange" code is used to switch the memory access
 * from 8 bits to 16 bits, because the vega plus accesses 
 * the memory via 8 bits at reset time
 */
	 
_start:
	.long 	 0x00300010		/*LDR  r3,0x18*/
	.long 	 0x00E5009F

	.long 	 0x00400010
	.long 	 0x00E5009F

	.long 	 0x004600B0
	.long 	 0x00E100C3

	 .long     0x00400002		/* CS0 = 16 bits*/
	 .long 	 0x00E300A0

	 .long 	 0x004200B0
	 .long 	 0x00E100C3

	 .long 	 0x00000009
	 .long 	 0x00EA0000

	 .long	 0x003C0000
	 .long     0x0000000F

	 .long 	 0x00A60087
	 .long 	 0x00000000

	.code 32

/* --- Initialise external bus*/
Real_start:	
    MOV	    r0,#CNTL_BASE_ADR

/*Load timing configuration of CS0*/
    LDR     r1, =0x0804
    STR     r1, [r0,#CSCNTL0_0]
    LDR     r1, =0xC432
    STR     r1, [r0,#CSCNTL1_0]

/* Load timing configuration and access mode of CS1
   NOTE : Important for macro REGION_INIT of Region_init.s 
   if initialisation of data in external RAM */
   	LDR     r1, =0x2200
	STR     r1, [r0,#CSCNTL0_1]
   	LDR     r1, =0x8022
	STR     r1, [r0,#CSCNTL1_1]

/* Load timing configuration and access mode of CS2 */
	LDR     r1, =0x342
	STR     r1, [r0,#CSCNTL0_2]
	LDR     r1, =0xA2
	STR     r1, [r0,#CSCNTL1_2]
 

	MOV		r0,#CNTL_CLK_ADR
/* Load clock mode 55 MHz */
   	LDR     r1, =0x0010
        STR     r1, [r0,#CLKCNTL]

/* Copy data from FLASH to RAM */
        LDR     r0, =_initdata        /* load address of region */
	LDR     r1, =0x400000         /* execution address of region */
        LDR     r2, =_edata           /* copy execution address into r2 */

copy:	
        CMP     r1, r2		      /* loop whilst r1 < r2 */
        LDRLO   r3, [r0], #4
        STRLO   r3, [r1], #4
        BLO     copy

/* zero the bss */
        LDR     r1, =__bss_end__       /* get end of ZI region */
        LDR     r0, =__bss_start__     /* load base address of ZI region */
zi_init:	
        MOV     r2, #0
        CMP     r0, r1		       /* loop whilst r0 < r1 */
        STRLOT   r2, [r0], #4
        BLO     zi_init 

	
/* Load basic ARM7 interrupt table */
VectorInit:	
	MOV	R8, #0
	ADR	R9, Vector_Init_Block
	LDMIA	R9!, {R0-R7}	/* Copy the Vectors (8 words) */
	STMIA	R8!, {R0-R7}
	LDMIA	R9!, {R0-R7}	/* Copy the .long'ed addresses (8 words) */
	STMIA	R8!, {R0-R7}

	B	init2

/*******************************************************
 standard exception vectors table
 *** Must be located at address 0 
********************************************************/	 

Vector_Init_Block:	
	LDR	PC, Reset_Addr
	LDR	PC, Undefined_Addr
	LDR	PC, SWI_Addr
	LDR	PC, Prefetch_Addr
	LDR	PC, Abort_Addr
	NOP
	LDR	PC, IRQ_Addr
	LDR	PC, FIQ_Addr

	.globl Reset_Addr
Reset_Addr:	.long   _start 
Undefined_Addr:	.long	Undefined_Handler
SWI_Addr:	.long	SWI_Handler
Prefetch_Addr:	.long	Prefetch_Handler
Abort_Addr:	.long	Abort_Handler
		.long	0	
IRQ_Addr:	.long	IRQ_Handler
FIQ_Addr:	.long	FIQ_Handler
	
/* The following handlers do not do anything useful */
	.globl Undefined_Handler
Undefined_Handler:	
        B       Undefined_Handler
	.globl SWI_Handler
SWI_Handler:	
        B       SWI_Handler 
	.globl Prefetch_Handler
Prefetch_Handler:	
        B       Prefetch_Handler
	.globl Abort_Handler
Abort_Handler:	
        B       Abort_Handler
	.globl IRQ_Handler
IRQ_Handler:	
        B       IRQ_Handler
	.globl FIQ_Handler
FIQ_Handler:	
        B       FIQ_Handler

init2 :	
/* --- Initialise stack pointer registers
   Set up the ABORT stack pointer last and stay in SVC mode */
    MOV     r0, #(Mode_ABORT | I_Bit | F_Bit)   /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =ABORT_Stack

/* Enter IRQ mode and set up the IRQ stack pointer */
    MOV     r0, #Mode_IRQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =IRQ_Stack

/* Enter FIQ mode and set up the FIQ stack pointer */
    MOV     r0, #Mode_FIQ | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =FIQ_Stack

/* Set up the SVC stack pointer last and stay in SVC mode */
    MOV     r0, #Mode_SVC | I_Bit | F_Bit     /* No interrupts */
    MSR     cpsr, r0
    LDR     sp, =SVC_Stack

/* --- Now we enter the C code */

    B	boot_card












