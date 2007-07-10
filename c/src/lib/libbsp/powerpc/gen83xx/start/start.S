/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the startup assembly code                    |
\*===============================================================*/
/* $Id$ */   

#include <rtems/asm.h> 
#include <rtems/powerpc/cache.h>
#include <rtems/powerpc/registers.h>
#include <mpc83xx/mpc83xx.h>
#include <bsp.h>

/* Macro definitions to load a register with a 32-bit address.
   Both functions identically.  Sometimes one mnemonic is more
   appropriate than the other.
   reg 		-> register to load
   value 	-> value to be loaded
   LA  reg,value    ("Load Address")
   LWI reg,value    ("Load Word Immediate") */

.macro LA reg, value
	lis \reg , \value@h
	ori \reg , \reg, \value@l
.endm

.macro LWI reg, value
	lis \reg , (\value)@h
	ori \reg , \reg, (\value)@l
.endm 

.macro SET_IMM_REGW base, reg2, offset, value
	LA \reg2, \value
	stw \reg2,\offset(\base)
.endm 

/* Macro definitions to test, set or clear a single 
   bit or bit pattern in a given 32bit GPR. 
   reg1		-> register content to be tested
   reg2		-> 2nd register only needed for computation
   mask		-> any bit pattern */

.macro	TSTBITS	reg1, reg2, reg3, mask	/* Match is indicated by EQ=0 (CR) */ 
	LWI 	\reg3, \mask		/* Unmatch is indicated by EQ=1 (CR) */
	and	\reg1, \reg1, \reg3
	and	\reg2, \reg2, \reg3
	cmplw	\reg1, \reg2
	sync
.endm	
	
.macro 	SETBITS reg1, reg2, mask
	LWI 	\reg2, \mask
	or	\reg1, \reg1, \reg2 
	sync
.endm

.macro 	CLRBITS reg1, reg2, mask
	LWI 	\reg2, \mask
	andc	\reg1, \reg1, \reg2
	sync
.endm

.extern	_bss_start
.extern	_bss_size
.extern	_data_start
.extern	_data_size
.extern	_text_start
.extern	_text_size
/*.extern _s_got*/
.extern boot_card
.extern MBAR   

.section ".vectors"  
PUBLIC_VAR (reset_vec)
reset_vec:	
	bl start
.section ".entry"  
PUBLIC_VAR (start)
start:
	/*
	 * FIXME: basic CPU setup:	
	 *        init MSR
	 */
	mfmsr	r30
	SETBITS r30, r29, MSR_ME|MSR_RI
	CLRBITS r30, r29, MSR_IP|MSR_EE
	mtmsr	r30 				/* Set RI/ME, Clr EE in MSR */
	/*
	 * check, wether we are starting from ROM 
	 * detect this using the absolute code address:
	 * when the upper 4 bits are 0xF, then we are in ROM
	 */
	bl 1f
1:	mflr r28
	LWI  r29,0xF0000000
	TSTBITS r28,r29,r30,0xF0000000
	bne start_rom_skip
	/*
	 * ROM startup: remap IMMR to 0xE0000000
	 * use special sequence from MPC8349EA RM Rev 1, 5.2.4.1.1 "Updating IMMRBAR"
	 */
	LWI  r30,IMMRBAR_DEFAULT
	LWI  r31,IMMRBAR
	lwz  r29,0(r30)
	stw  r31,0(r30)
	lwz  r29,0(r28) /* read from ROM... */
	isync
	lwz  r29,0(r31) /* read from IMMRBAR... */
	isync
	/*
	 * NOTE:	 now r31 points to onchip registers
	
	/*
	 * ROM startup: init local access windows
	 */
#ifdef LBLAWBAR0_VAL
	SET_IMM_REGW r31,r30,LBLAWBAR0_OFF,LBLAWBAR0_VAL
#endif
#ifdef LBLAWAR0_VAL
	SET_IMM_REGW r31,r30,LBLAWAR0_OFF,LBLAWAR0_VAL
#endif
#ifdef LBLAWBAR1_VAL
	SET_IMM_REGW r31,r30,LBLAWBAR1_OFF,LBLAWBAR1_VAL
#endif
#ifdef LBLAWAR1_VAL
	SET_IMM_REGW r31,r30,LBLAWAR1_OFF,LBLAWAR1_VAL
#endif
#ifdef LBLAWBAR2_VAL
	SET_IMM_REGW r31,r30,LBLAWBAR2_OFF,LBLAWBAR2_VAL
#endif
#ifdef LBLAWAR2_VAL
	SET_IMM_REGW r31,r30,LBLAWAR2_OFF,LBLAWAR2_VAL
#endif
#ifdef LBLAWBAR3_VAL
	SET_IMM_REGW r31,r30,LBLAWBAR3_OFF,LBLAWBAR3_VAL
#endif
#ifdef LBLAWAR3_VAL
	SET_IMM_REGW r31,r30,LBLAWAR3_OFF,LBLAWAR3_VAL
#endif
	/*
	 * ROM startup: init bus system
	 */
#ifdef BR0_VAL
	SET_IMM_REGW r31,r30,BR0_OFF,BR0_VAL
#endif
#ifdef OR0_VAL
	SET_IMM_REGW r31,r30,OR0_OFF,OR0_VAL
#endif
#ifdef BR1_VAL
	SET_IMM_REGW r31,r30,BR1_OFF,BR1_VAL
#endif
#ifdef OR1_VAL
	SET_IMM_REGW r31,r30,OR1_OFF,OR1_VAL
#endif
#ifdef BR2_VAL
	SET_IMM_REGW r31,r30,BR2_OFF,BR2_VAL
#endif
#ifdef OR2_VAL
	SET_IMM_REGW r31,r30,OR2_OFF,OR2_VAL
#endif
#ifdef BR3_VAL
	SET_IMM_REGW r31,r30,BR3_OFF,BR3_VAL
#endif
#ifdef OR3_VAL
	SET_IMM_REGW r31,r30,OR3_OFF,OR3_VAL
#endif
	/*
	 * ROM startup: init SDRAM access window
	 */
#ifdef DDRLAWBAR0_VAL
	SET_IMM_REGW r31,r30,DDRLAWBAR0_OFF,DDRLAWBAR0_VAL
#endif
#ifdef DDRLAWAR0_VAL
	SET_IMM_REGW r31,r30,DDRLAWAR0_OFF,DDRLAWAR0_VAL
#endif
#ifdef DDRLAWBAR1_VAL
	SET_IMM_REGW r31,r30,DDRLAWBAR1_OFF,DDRLAWBAR1_VAL
#endif
#ifdef DDRLAWAR1_VAL
	SET_IMM_REGW r31,r30,DDRLAWAR1_OFF,DDRLAWAR1_VAL
#endif
	/*
	 * ROM startup: init SDRAM
	 */
#ifdef CS0_BNDS_VAL
	SET_IMM_REGW r31,r30,CS0_BNDS_OFF,CS0_BNDS_VAL
#endif
#ifdef CS1_BNDS_VAL
	SET_IMM_REGW r31,r30,CS1_BNDS_OFF,CS1_BNDS_VAL
#endif
#ifdef CS2_BNDS_VAL
	SET_IMM_REGW r31,r30,CS2_BNDS_OFF,CS2_BNDS_VAL
#endif
#ifdef CS3_BNDS_VAL
	SET_IMM_REGW r31,r30,CS3_BNDS_OFF,CS3_BNDS_VAL
#endif
#ifdef CS0_CONFIG_VAL
	SET_IMM_REGW r31,r30,CS0_CONFIG_OFF,CS0_CONFIG_VAL
#endif
#ifdef CS1_CONFIG_VAL
	SET_IMM_REGW r31,r30,CS1_CONFIG_OFF,CS1_CONFIG_VAL
#endif
#ifdef CS2_CONFIG_VAL
	SET_IMM_REGW r31,r30,CS2_CONFIG_OFF,CS2_CONFIG_VAL
#endif
#ifdef CS3_CONFIG_VAL
	SET_IMM_REGW r31,r30,CS3_CONFIG_OFF,CS3_CONFIG_VAL
#endif
#ifdef TIMING_CFG_3_VAL
	SET_IMM_REGW r31,r30,TIMING_CFG_3_OFF,TIMING_CFG_3_VAL
#endif
#ifdef TIMING_CFG_0_VAL
	SET_IMM_REGW r31,r30,TIMING_CFG_0_OFF,TIMING_CFG_0_VAL
#endif
#ifdef TIMING_CFG_1_VAL
	SET_IMM_REGW r31,r30,TIMING_CFG_1_OFF,TIMING_CFG_1_VAL
#endif
#ifdef TIMING_CFG_2_VAL
	SET_IMM_REGW r31,r30,TIMING_CFG_2_OFF,TIMING_CFG_2_VAL
#endif
#ifdef DDR_SDRAM_CFG_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_CFG_OFF,DDR_SDRAM_CFG_VAL
#endif
#ifdef DDR_SDRAM_CFG_2_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_CFG_2_OFF,DDR_SDRAM_CFG_2_VAL
#endif
#ifdef DDR_SDRAM_MODE_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_MODE_OFF,DDR_SDRAM_MODE_VAL
#endif
#ifdef DDR_SDRAM_MODE_2_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_MODE_2_OFF,DDR_SDRAM_MODE_2_VAL
#endif
#ifdef DDR_SDRAM_MD_CNTL_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_MD_CNTL_OFF,DDR_SDRAM_MD_CNTL_VAL
#endif
#ifdef DDR_SDRAM_MD_ITVL_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_MD_ITVL_OFF,DDR_SDRAM_MD_ITVL_VAL
#endif
#ifdef DDR_SDRAM_CLK_CNTL_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_CLK_CNTL_OFF,DDR_SDRAM_CLK_CNTL_VAL
#endif
#ifdef DDR_SDRAM_INIT_ADDR_VAL
	SET_IMM_REGW r31,r30,DDR_SDRAM_INIT_ADDR_OFF,DDR_SDRAM_INIT_ADDR_VAL
#endif
	/*
	 * FIXME: ROM startup: perform mode set commands etc for SDRAM
	 */
	/*
	 * ROM startup: copy code to SDRAM
	 */
	LA	r30, _text_start  /* get start address of text section in RAM */
	add	r30, r20, r30    /* get start address of text section in ROM (add reloc offset) */
	LA	r29, _text_start /* get start address of text section in RAM */
	LA	r28, _text_size  /* get size of RAM image */
	bl	copy_image      /* copy text section from ROM to RAM location */
	
	/*
	 * FIXME: ROM startup: copy data to SDRAM
	 */
	LA	r30, _data_start /* get start address of data section in RAM */
	add	r30, r20, r30    /* get start address of data section in ROM (add reloc offset) */
	LA	r29, _data_start /* get start address of data section in RAM */
	LA	r28, _data_size  /* get size of RAM image */
	bl	copy_image       /* copy initialized data section from ROM to RAM location */
start_rom_skip:	
	/*
	 * ROM startup: clear bss in SDRAM
	 */
	LWI	r30, _bss_start  /* get start address of bss section */
        LWI	r29, _bss_size   /* get size of bss section */
        bl      clr_mem          /* Clear the bss section */
	/*
	 * ROM startup: jump to code copy in  SDRAM
	 */
	LA	r29, start_code_in_ram /* get compile time address of label */
	mtlr	r29
	blr                     /* now further execution RAM */
start_code_in_ram:	
	/*
	 * call boot_card
	 */
/* set stack pointer (common for RAM/ROM startup) */
	LA	r1, _text_start  
        addi    r1, r1, -0x10 /* Set up stack pointer = beginning of text section - 0x10 */     
/* clear arguments and do further init. in C (common for RAM/ROM startup) */
        xor     r3, r3, r3
        xor     r4, r4, r4       /* Clear argc and argv */        
        bl      SYM (boot_card)  /* Call the first C routine */
	
twiddle:
	/* We don't expect to return from boot_card but if we do */
	/* wait here for watchdog to kick us into hard reset     */
	b	twiddle          
						
copy_image:
	mr	r27, r28
	srwi	r28, r28, 2
	mtctr	r28

	slwi	r28, r28, 2
	sub     r27, r27, r28			/* maybe some residual bytes */
copy_image_word:
	lswi	r28, r30, 0x04
	
	stswi   r28, r29, 0x04			/* do word copy ROM -> RAM */
	

	addi    r30, r30, 0x04			/* increment source pointer */
	addi    r29, r29, 0x04			/* increment destination pointer */
	
	bdnz	copy_image_word			/* decrement ctr and branch if not 0 */

	cmpwi	r27, 0x00			/* copy image finished ? */
	beq	copy_image_end;
	mtctr   r27				/* reload counter for residual bytes */
copy_image_byte:
	lswi	r28, r30, 0x01
	
	stswi   r28, r29, 0x01			/* do byte copy ROM -> RAM */
	
	
	addi    r30, r30, 0x01			/* increment source pointer */
	addi    r29, r29, 0x01			/* increment destination pointer */
	
	bdnz	copy_image_byte			/* decrement ctr and branch if not 0 */
	
copy_image_end:
	blr

clr_mem:
	mr 	r28, r29		
	srwi	r29, r29, 2
        mtctr   r29				/* set ctr reg */
        
        
        slwi	r29, r29, 2
        sub     r28, r28, r29			/* maybe some residual bytes */
        xor     r29, r29, r29		
	
	
clr_mem_word:
        stswi   r29, r30, 0x04			/* store r29 (word) to r30 memory location */
        addi    r30, r30, 0x04			/* increment r30 */
        
        bdnz    clr_mem_word			/* dec counter and loop */
	
	
	cmpwi	r28, 0x00			/* clear mem. finished ? */
	beq	clr_mem_end;
        mtctr   r28				/* reload counter for residual bytes */
clr_mem_byte:
	stswi   r29, r30, 0x01			/* store r29 (byte) to r30 memory location  */
	addi    r30, r30, 0x01			/* update r30 */
        
        bdnz    clr_mem_byte			/* dec counter and loop */
        
clr_mem_end:
        blr					/* return */
