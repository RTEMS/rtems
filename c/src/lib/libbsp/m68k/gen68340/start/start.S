/*
 *  This file contains the entry point for the application.
 *  The name of this entry point is compiler dependent.
 *  It jumps to the BSP which is responsible for performing
 *  all initialization.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may in
 *  the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 * Based on the `gen68360' board support package, and covered by the
 * original distribution terms.
 *
 * Geoffroy Montel
 * France Telecom - CNET/DSM/TAM/CAT
 * 4, rue du Clos Courtel
 * 35512 CESSON-SEVIGNE
 * FRANCE
 *
 * e-mail: g_montel@yahoo.com
 */

#include <rtems/asm.h>
#include <m68349.inc>

#define	_OLD_ASTECC 1 /* old addresses for AST68340 only, undefine for AST68349 */

BEGIN_CODE
	/*
	 * Step 1: Decide on Reset Stack Pointer and Initial Program Counter
	 */
Entry:
	.long	SYM(m340)+1024		|   0: Initial SSP
	.long	start			|   1: Initial PC
	.long	SYM(_uhoh)		|   2: Bus error
	.long	SYM(_uhoh)		|   3: Address error
	.long	SYM(_uhoh)		|   4: Illegal instruction
	.long	SYM(_uhoh)		|   5: Zero division
	.long	SYM(_uhoh)		|   6: CHK, CHK2 instruction
	.long	SYM(_uhoh)		|   7: TRAPcc, TRAPV instructions
	.long	SYM(_uhoh)		|   8: Privilege violation
	.long	SYM(_uhoh)		|   9: Trace
	.long	SYM(_uhoh)		|  10: Line 1010 emulator
	.long	SYM(_uhoh)		|  11: Line 1111 emulator
	.long	SYM(_uhoh)		|  12: Hardware breakpoint
	.long	SYM(_uhoh)		|  13: Reserved for coprocessor violation
	.long	SYM(_uhoh)		|  14: Format error
	.long	SYM(_uhoh)		|  15: Uninitialized interrupt
	.long	SYM(_uhoh)		|  16: Unassigned, reserved
	.long	SYM(_uhoh)		|  17:
	.long	SYM(_uhoh)		|  18:
	.long	SYM(_uhoh)		|  19:
	.long	SYM(_uhoh)		|  20:
	.long	SYM(_uhoh)		|  21:
	.long	SYM(_uhoh)		|  22:
	.long	SYM(_uhoh)		|  23:
	.long	SYM(_spuriousInterrupt)	|  24: Spurious interrupt
	.long	SYM(_uhoh)		|  25: Level 1 interrupt autovector
	.long	SYM(_uhoh)		|  26: Level 2 interrupt autovector
	.long	SYM(_uhoh)		|  27: Level 3 interrupt autovector
	.long	SYM(_uhoh)		|  28: Level 4 interrupt autovector
	.long	SYM(_uhoh)		|  29: Level 5 interrupt autovector
	.long	SYM(_uhoh)		|  30: Level 6 interrupt autovector
	.long	SYM(_uhoh)		|  31: Level 7 interrupt autovector
	.long	SYM(_uhoh)		|  32: Trap instruction (0-15)
	.long	SYM(_uhoh)		|  33:
	.long	SYM(_uhoh)		|  34:
	.long	SYM(_uhoh)		|  35:
	.long	SYM(_uhoh)		|  36:
	.long	SYM(_uhoh)		|  37:
	.long	SYM(_uhoh)		|  38:
	.long	SYM(_uhoh)		|  39:
	.long	SYM(_uhoh)		|  40:
	.long	SYM(_uhoh)		|  41:
	.long	SYM(_uhoh)		|  42:
	.long	SYM(_uhoh)		|  43:
	.long	SYM(_uhoh)		|  44:
	.long	SYM(_uhoh)		|  45:
	.long	SYM(_uhoh)		|  46:
	.long	SYM(_uhoh)		|  47:
	.long	SYM(_uhoh)		|  48: Reserved for coprocessor
	.long	SYM(_uhoh)		|  49:
	.long	SYM(_uhoh)		|  50:
	.long	SYM(_uhoh)		|  51:
	.long	SYM(_uhoh)		|  52:
	.long	SYM(_uhoh)		|  53:
	.long	SYM(_uhoh)		|  54:
	.long	SYM(_uhoh)		|  55:
	.long	SYM(_uhoh)		|  56:
	.long	SYM(_uhoh)		|  57:
	.long	SYM(_uhoh)		|  58:
	.long	SYM(_uhoh)		|  59: Unassigned, reserved
	.long	SYM(_uhoh)		|  60:
	.long	SYM(_uhoh)		|  61:
	.long	SYM(_uhoh)		|  62:
	.long	SYM(_uhoh)		|  63:
	.long	SYM(_uhoh)		|  64: User defined vectors (192)
	.long	SYM(_uhoh)		|  65:
	.long	SYM(_uhoh)		|  66:
	.long	SYM(_uhoh)		|  67:
	.long	SYM(_uhoh)		|  68:
	.long	SYM(_uhoh)		|  69:
	.long	SYM(_uhoh)		|  70:
	.long	SYM(_uhoh)		|  71:
	.long	SYM(_uhoh)		|  72:
	.long	SYM(_uhoh)		|  73:
	.long	SYM(_uhoh)		|  74:
	.long	SYM(_uhoh)		|  75:
	.long	SYM(_uhoh)		|  76:
	.long	SYM(_uhoh)		|  77:
	.long	SYM(_uhoh)		|  78:
	.long	SYM(_uhoh)		|  79:
	.long	SYM(_uhoh)		|  80:
	.long	SYM(_uhoh)		|  81:
	.long	SYM(_uhoh)		|  82:
	.long	SYM(_uhoh)		|  83:
	.long	SYM(_uhoh)		|  84:
	.long	SYM(_uhoh)		|  85:
	.long	SYM(_uhoh)		|  86:
	.long	SYM(_uhoh)		|  87:
	.long	SYM(_uhoh)		|  88:
	.long	SYM(_uhoh)		|  89:
	.long	SYM(_uhoh)		|  90:
	.long	SYM(_uhoh)		|  91:
	.long	SYM(_uhoh)		|  92:
	.long	SYM(_uhoh)		|  93:
	.long	SYM(_uhoh)		|  94:
	.long	SYM(_uhoh)		|  95:
	.long	SYM(_uhoh)		|  96:
	.long	SYM(_uhoh)		|  97:
	.long	SYM(_uhoh)		|  98:
	.long	SYM(_uhoh)		|  99:
	.long	SYM(_uhoh)		| 100:
	.long	SYM(_uhoh)		| 101:
	.long	SYM(_uhoh)		| 102:
	.long	SYM(_uhoh)		| 103:
	.long	SYM(_uhoh)		| 104:
	.long	SYM(_uhoh)		| 105:
	.long	SYM(_uhoh)		| 106:
	.long	SYM(_uhoh)		| 107:
	.long	SYM(_uhoh)		| 108:
	.long	SYM(_uhoh)		| 109:
	.long	SYM(_uhoh)		| 110:
	.long	SYM(_uhoh)		| 111:
	.long	SYM(_uhoh)		| 112:
	.long	SYM(_uhoh)		| 113:
	.long	SYM(_uhoh)		| 114:
	.long	SYM(_uhoh)		| 115:
	.long	SYM(_uhoh)		| 116:
	.long	SYM(_uhoh)		| 117:
	.long	SYM(_uhoh)		| 118:
	.long	SYM(_uhoh)		| 119:
	.long	SYM(_uhoh)		| 120:
	.long	SYM(_uhoh)		| 121:
	.long	SYM(_uhoh)		| 122:
	.long	SYM(_uhoh)		| 123:
	.long	SYM(_uhoh)		| 124:
	.long	SYM(_uhoh)		| 125:
	.long	SYM(_uhoh)		| 126:
	.long	SYM(_uhoh)		| 127:
	.long	SYM(_uhoh)		| 128:
	.long	SYM(_uhoh)		| 129:
	.long	SYM(_uhoh)		| 130:
	.long	SYM(_uhoh)		| 131:
	.long	SYM(_uhoh)		| 132:
	.long	SYM(_uhoh)		| 133:
	.long	SYM(_uhoh)		| 134:
	.long	SYM(_uhoh)		| 135:
	.long	SYM(_uhoh)		| 136:
	.long	SYM(_uhoh)		| 137:
	.long	SYM(_uhoh)		| 138:
	.long	SYM(_uhoh)		| 139:
	.long	SYM(_uhoh)		| 140:
	.long	SYM(_uhoh)		| 141:
	.long	SYM(_uhoh)		| 142:
	.long	SYM(_uhoh)		| 143:
	.long	SYM(_uhoh)		| 144:
	.long	SYM(_uhoh)		| 145:
	.long	SYM(_uhoh)		| 146:
	.long	SYM(_uhoh)		| 147:
	.long	SYM(_uhoh)		| 148:
	.long	SYM(_uhoh)		| 149:
	.long	SYM(_uhoh)		| 150:
	.long	SYM(_uhoh)		| 151:
	.long	SYM(_uhoh)		| 152:
	.long	SYM(_uhoh)		| 153:
	.long	SYM(_uhoh)		| 154:
	.long	SYM(_uhoh)		| 155:
	.long	SYM(_uhoh)		| 156:
	.long	SYM(_uhoh)		| 157:
	.long	SYM(_uhoh)		| 158:
	.long	SYM(_uhoh)		| 159:
	.long	SYM(_uhoh)		| 160:
	.long	SYM(_uhoh)		| 161:
	.long	SYM(_uhoh)		| 162:
	.long	SYM(_uhoh)		| 163:
	.long	SYM(_uhoh)		| 164:
	.long	SYM(_uhoh)		| 165:
	.long	SYM(_uhoh)		| 166:
	.long	SYM(_uhoh)		| 167:
	.long	SYM(_uhoh)		| 168:
	.long	SYM(_uhoh)		| 169:
	.long	SYM(_uhoh)		| 170:
	.long	SYM(_uhoh)		| 171:
	.long	SYM(_uhoh)		| 172:
	.long	SYM(_uhoh)		| 173:
	.long	SYM(_uhoh)		| 174:
	.long	SYM(_uhoh)		| 175:
	.long	SYM(_uhoh)		| 176:
	.long	SYM(_uhoh)		| 177:
	.long	SYM(_uhoh)		| 178:
	.long	SYM(_uhoh)		| 179:
	.long	SYM(_uhoh)		| 180:
	.long	SYM(_uhoh)		| 181:
	.long	SYM(_uhoh)		| 182:
	.long	SYM(_uhoh)		| 183:
	.long	SYM(_uhoh)		| 184:
	.long	SYM(_uhoh)		| 185:
	.long	SYM(_uhoh)		| 186:
	.long	SYM(_uhoh)		| 187:
	.long	SYM(_uhoh)		| 188:
	.long	SYM(_uhoh)		| 189:
	.long	SYM(_uhoh)		| 190:
	.long	SYM(_uhoh)		| 191:
	.long	SYM(_uhoh)		| 192:
	.long	SYM(_uhoh)		| 193:
	.long	SYM(_uhoh)		| 194:
	.long	SYM(_uhoh)		| 195:
	.long	SYM(_uhoh)		| 196:
	.long	SYM(_uhoh)		| 197:
	.long	SYM(_uhoh)		| 198:
	.long	SYM(_uhoh)		| 199:
	.long	SYM(_uhoh)		| 200:
	.long	SYM(_uhoh)		| 201:
	.long	SYM(_uhoh)		| 202:
	.long	SYM(_uhoh)		| 203:
	.long	SYM(_uhoh)		| 204:
	.long	SYM(_uhoh)		| 205:
	.long	SYM(_uhoh)		| 206:
	.long	SYM(_uhoh)		| 207:
	.long	SYM(_uhoh)		| 208:
	.long	SYM(_uhoh)		| 209:
	.long	SYM(_uhoh)		| 210:
	.long	SYM(_uhoh)		| 211:
	.long	SYM(_uhoh)		| 212:
	.long	SYM(_uhoh)		| 213:
	.long	SYM(_uhoh)		| 214:
	.long	SYM(_uhoh)		| 215:
	.long	SYM(_uhoh)		| 216:
	.long	SYM(_uhoh)		| 217:
	.long	SYM(_uhoh)		| 218:
	.long	SYM(_uhoh)		| 219:
	.long	SYM(_uhoh)		| 220:
	.long	SYM(_uhoh)		| 221:
	.long	SYM(_uhoh)		| 222:
	.long	SYM(_uhoh)		| 223:
	.long	SYM(_uhoh)		| 224:
	.long	SYM(_uhoh)		| 225:
	.long	SYM(_uhoh)		| 226:
	.long	SYM(_uhoh)		| 227:
	.long	SYM(_uhoh)		| 228:
	.long	SYM(_uhoh)		| 229:
	.long	SYM(_uhoh)		| 230:
	.long	SYM(_uhoh)		| 231:
	.long	SYM(_uhoh)		| 232:
	.long	SYM(_uhoh)		| 233:
	.long	SYM(_uhoh)		| 234:
	.long	SYM(_uhoh)		| 235:
	.long	SYM(_uhoh)		| 236:
	.long	SYM(_uhoh)		| 237:
	.long	SYM(_uhoh)		| 238:
	.long	SYM(_uhoh)		| 239:
	.long	SYM(_uhoh)		| 240:
	.long	SYM(_uhoh)		| 241:
	.long	SYM(_uhoh)		| 242:
	.long	SYM(_uhoh)		| 243:
	.long	SYM(_uhoh)		| 244:
	.long	SYM(_uhoh)		| 245:
	.long	SYM(_uhoh)		| 246:
	.long	SYM(_uhoh)		| 247:
	.long	SYM(_uhoh)		| 248:
	.long	SYM(_uhoh)		| 249:
	.long	SYM(_uhoh)		| 250:
	.long	SYM(_uhoh)		| 251:
	.long	SYM(_uhoh)		| 252:
	.long	SYM(_uhoh)		| 253:
	.long	SYM(_uhoh)		| 254:
	.long	SYM(_uhoh)		| 255:

/*
 * Default trap handler
 * With an oscilloscope you can see AS* stop
 */
	PUBLIC (_uhoh)
SYM(_uhoh):	nop				| Leave spot for breakpoint
/*	stop	#0x2700				| Stop with interrupts disabled */
	move.w	#0x2700,sr
	move.w  (a7),_boot_panic_registers+4	| SR
	move.l  2(a7),_boot_panic_registers	| PC
	move.w  6(a7),_boot_panic_registers+6	| format & vector
	movem.l d0-d7/a0-a7, _boot_panic_registers+8
	movec	sfc, d0
	movem.l	d0, _boot_panic_registers+72
	movec	dfc, d0
	movem.l	d0, _boot_panic_registers+76
	movec	vbr, d0
	movem.l	d0, _boot_panic_registers+80
	jmp	SYM(_dbug_dumpanic)
	bra.s	_crt0_cold_start

/*
 * Log, but otherwise ignore, spurious interrupts
 */
	PUBLIC (_spuriousInterrupt)
SYM(_spuriousInterrupt):
	addql	#1,SYM(_M68kSpuriousInterruptCount)
	rte

/*
 * Place the low-order 3 octets of the board's ethernet address at
 * a `well-known' fixed location relative to the startup location.
 */
	.align 2
	.word	0			| Padding
ethernet_address_buffer:
	.word	0x08F3			| Default address
	.word	0xDEAD
	.word	0xCAFE

BEGIN_DATA

/* equates */

.equ	_CPU340, 0x0
.equ	_CPU349, 0x31

#ifdef _OLD_ASTECC				/* old addresses for AST68340 only */
.equ	_EPLD_CS_BASE, 0x1
.equ	_PROM_Start, 0x01000000    		/* CS0 */
.equ	_FLEX_Start, 0x08000000			/* CS2 */
.equ	_I2C_Start, 0x0c000000    		/* CS3 */

.equ	_BCCram_Start, 0x00000000    		/* CS1	64 Kbytes */
.equ	_BCCram_Size, 0x00010000    		/* CS1	64 Kbytes */

.equ	_ExtRam_Start, 0x10000000  		/* SRAM */
.equ 	_ExtRam_Size, 0x00400000		/* 4 Mbytes */

.equ 	_FastRam_Start, 0x00000000		/* overlap /CS1 for the first 4 Kbytes */
.equ	_FastRam_Size, 0x00001000		/* 4 Kbytes */

#else 	/* new addresses for AST68349 and 68340 */

.equ	_EPLD_CS_BASE, 0x5
.equ	_PROM_Start, 0x50000000    		/* CS0 */
.equ	_FLEX_Start, 0x08000000    		/* CS2 */
.equ	_I2C_Start, 0x0c000000    		/* CS3 */

.equ	_BCCram_Start, 0x00000000    		/* CS1	64 Kbytes */
.equ	_BCCram_Size, 0x00010000    		/* CS1	64 Kbytes */

.equ	_ExtRam_Start, 0x80000000  		/* DRAM */
.equ	_ExtRam_Size, 0x00400000		/* 4 Mbytes */

.equ	_FastRam_Start, 0x00000000		/* overlap /CS1 for the first 4 Kbytes */
.equ	_FastRam_Size, 0x00001000		/* 4 Kbytes */
#endif

.equ	_SPEED349, 0xD680			/* 24 Mhz */
.equ	_SPEED340, 0xD700			/* 25 Mhz */
/* .equ	_SPEED340, 0xCE00			 16 Mhz */

#define	crt0_boot_type	d0			/* cold/warm start (must be D0) */
#define	crt0_temp	d1
#define	crt0_cpu_type	d2
#define	crt0_csswitch	d3
#define	crt0_buswidth	d4
#define	crt0_pdcs	d5
#define	crt0_spare6	d6
#define	crt0_spare7	d7
#define	crt0_sim_base	a0
#define	crt0_glue	a1
#define	crt0_dram	a2
#define	crt0_ptr3	a3
#define	crt0_ptr4	a4
#define	crt0_ptr5	a5
#define	crt0_ptr6	a6

/* -- PDCS buffer equates -- */
.equ	pdcs_mask, 0x1F				/* DRAM configuration */
.equ	pdcs_sw12, 7				/* switch 12 */
.equ	pdcs_sw11, 6				/* switch 11 */
.equ	pdcs_sw14, 5				/* switch 14 */

.equ	bit_cache, pdcs_sw12			/* enable cache if on */
.equ	bit_meminit, pdcs_sw11			/* init memory if on */

/* -- Initialization stack and vars -- */

/* When using DWARF, everything must be a multiple of 16-bits. */
#if 1
_AsteccBusWidth:	ds.w 0x0101
_AsteccCsSwitch:	ds.w 0x0101
#else
_AsteccBusWidth:	ds.b 1
_AsteccCsSwitch:	ds.b 1
#endif
_AsteccCpuName:		ds.l 1

.align 4

_crt0_init_stack:
		ds.l	500
_crt0_init_stktop:

/* -- Initialization code -- */
BEGIN_CODE

.align 4
		dc.l    _crt0_init_stktop        /* reset SP */
		dc.l    _crt0_cold_start         /* reset PC */
		dc.l    _crt0_warm_start

/* When using DWARF, everything must be a multiple of 16-bits. */
		.ascii	"BOOT XHM68K/Spectra for ASTECC 68349 and 68340 boards "
		.text
		dc.w	0
.align 4

.globl start
start:

_crt0_cold_start:
	moveq.l #0,crt0_boot_type			| signal cold reset
	bra.s   _crt0_common_start

_crt0_warm_start:
	moveq.l #1,crt0_boot_type		 	| signal warm reset

_crt0_common_start:
	move.w	#0x2700,sr               		| disable interrupts and switch to interrupt mode
	movea.l	#_crt0_init_stktop,sp			| set up initialization stack

	move.l	#Entry,crt0_temp			| VBR initialization
	movec.l crt0_temp,vbr				|
	moveq.l #0x07,crt0_temp
	movec.l crt0_temp,dfc				| prepare access in CPU space
	move.l  #(BASE_SIM+0x111),crt0_temp 		| mask CPU, RESERVED USER SPACES
	moves.l crt0_temp,BASE_REG			| base initialization (must be MOVES, PCC-130795)

	movea.l #BASE_SIM,crt0_sim_base

	/* -- disable Bus Monitor -- */
	move.b  #0,SIM_SYPCR(crt0_sim_base)   		| system protection control register

	/* -- enable A31-A24 -- */
	clr.b   SIM_PPRA1(crt0_sim_base)

	/* -- show cycles, user acces to SIM, 4 /CS & 4 /IT -- */
	move.w  #0x427F,SIM_MCR(crt0_sim_base)

	/* -- enable /IRQ3, 5, 6, 7 -- */
	move.b   #0xE8,SIM_PPRB(crt0_sim_base)

	/* -- enable autovector on /IRQ7 -- */
	move.b   #0x80,SIM_AVR(crt0_sim_base)

	/* -- test CPU type -- */
	cmp.b	  #_CPU349,SIM_IDR(crt0_sim_base)
	bne	  cpu_is_68340

/*-------------------------------------------------------------------------------------------*/
cpu_is_68349:

 	/* -- set cpu clock -- */
	move.w  #_SPEED349,SIM_SYNCR(crt0_sim_base)    		| clock

sync_wait349:
	btst.b	#3,(SIM_SYNCR+1)(crt0_sim_base)
	beq	sync_wait349

	/* to allow access to the EPLD internal registers, it is necessary
	to disable the global chip-select /CS0 (which decodes every external
	cycles). To do that, we initialize the 68349 internal RAM,
	copy a part of the initialization code in it, and jump there.
	from that moment, /CS0 is not used, therefore it can be initialized
	with its default value. Its width may be incorrect, but it will be
	adjusted later. The goal is to avoid any conflict with
	the accesses to the EPLD registers.
	When this is done, we read the RESET parameters (boot prom width
	and chip-select switch) and proceed with the initialization
	when all is done, we jump back to the boot prom now
	decoded with a properly configured /CS0 */

	/*-------------------------------------*/
	/* -- configure internal SRAM banks -- */

	move.l	#0x00000000,QDMM_MCR(crt0_sim_base)
	move.l	#_FastRam_Start+0x0005,QDMM_QBAR0(crt0_sim_base)
	move.l	#_FastRam_Start+0x0405,QDMM_QBAR1(crt0_sim_base)
	move.l	#_FastRam_Start+0x0805,QDMM_QBAR2(crt0_sim_base)
	move.l	#_FastRam_Start+0x0c05,QDMM_QBAR3(crt0_sim_base)

	/*--------------------------------------------------------*/
	/* -- copy to address of the 68349 initialization code -- */

	lea.l		_copy_start_code(%pc),crt0_ptr3
	lea.l		_copy_end_code(%pc),crt0_ptr4
	move.l		crt0_ptr4,crt0_temp
	sub.l		crt0_ptr3,crt0_temp
	add.l		#3,crt0_temp				| adjust to next long word
	lsr.l		#2,crt0_temp

	move.l		#_FastRam_Start,crt0_ptr4
_copy_loop:
	move.l		(crt0_ptr3)+,(crt0_ptr4)+
	subq.l		#1,crt0_temp
	bne.s		_copy_loop
	bra.l		_FastRam_Start				| jump to code in internal RAM

	/*------------------------------------*/
	/* -- start of initialization code -- */

_copy_start_code:
	bra.l		_begin_68349_init

	/*----------------------------------------------------------*/
	/*  Astecc  68349 board : chip-select initialization values */

_table_csepld:
/* When using DWARF, everything must be a multiple of 16-bits. */
#if 1
	dc.w (((_EPLD_CS_BASE&0x0F)+0x80) << 8)	| 0x80 | 16 bits, 0ws
	dc.w 0x9090				| 16 bits, ext /dsack

#else
	dc.b	(_EPLD_CS_BASE&0x0F)+0x80	| 16 bits, 0ws
	dc.b	0x80				| 16 bits, 0 ws
	dc.b	0x90				| 16 bits, ext /dsack
	dc.b	0x90				| 16 bits, ext /dsack
#endif

_table_cs349:
	dc.l	0x003FFFF4               			| Mask CS0 (4Mbytes PROM, 32bits, 1WS)
	dc.l	(_PROM_Start&0xFFFFFF00)+0x00000003  		| Base CS0
	dc.l    0x003FFFF1               			| MASK CS1 (4Mbytes RAM, 16bits, 0WS)
	dc.l	(_BCCram_Start&0xFFFFFF00)+0x00000003  		| Base CS1
	dc.l    0x000000FF               			| MASK CS2 (FLEX, ext DTACK, 256 bytes)
	dc.l	(_FLEX_Start&0xFFFFFF00)+0x00000003  		| Base CS2
	dc.l	0x000000FF               			| Mask CS3 (I2C, ext DTACK, 256 bytes)
	dc.l	(_I2C_Start&0xFFFFFF00)+0x00000003  		| Base CS3

	/*-------------------------------------------------*/
_begin_68349_init:

	/*-------------------------------------------------*/
	/* 68349 chip select initialization

	at this stage, the width of /CS0 may be incorrect
	it will be corrected later
	*/

_cs68349_init:
	lea.l   	SIM_MASKH0(crt0_sim_base),crt0_ptr4
	lea.l   	_table_cs349(%pc),crt0_ptr3

	moveq.l 	#0x07,crt0_temp
_cs349_init2:
	move.l  	(crt0_ptr3)+,(crt0_ptr4)+
	dbra    	crt0_temp,_cs349_init2

	/*-----------------------------------------------*/
	/* -- prepare access to the internal registers --*/
	moveq.l 	#EPLD_SPACE,crt0_temp
	movec.l 	crt0_temp,dfc
	movec.l 	crt0_temp,sfc
	move.l 		#GLUE_EPLD,crt0_glue
	move.l 		#DRAM_EPLD,crt0_dram

	/*-------------------------------------------*/
	/* EPLD generated /CS[3..0] must be disabled */

_csepld_clear:
	move.l		crt0_glue,crt0_ptr4
	move.w		#3,crt0_spare6
	clr.b		crt0_temp

_csepld_clear1:
	moves.b		crt0_temp,(crt0_ptr4)+
	dbra    	crt0_spare6,_csepld_clear1

	/*---------------------------------------------------------*/
	/* -- get width of boot PROM, and active chip-select set --*/
	moves.b  	REG_BUSWIDTH(crt0_dram),crt0_csswitch
	move.b		crt0_csswitch,crt0_buswidth

	/* state of CS_SWITCH  : sel == 0 => CPU chip_selects  (/CS[3..0])
	                       : sel == 1 => EPLD chip_selects (/CS[3..0]) */
	and.b		#1,crt0_csswitch

	/* bus width of /CS0 during reset		bw[1..0]  : 0   1   2       3
	                                   		bus width : 32  16  8   ext./dsackx */
	rol.b		#2,crt0_buswidth
	and.b		#3,crt0_buswidth

	/*----------------------------------------------------*/
	/* -- configure chip select 0 with boot prom width -- */
	lea.l   	SIM_MASKH0(crt0_sim_base),crt0_ptr4
	lea.l   	_table_cs349(%pc),crt0_ptr3
	move.l  	(crt0_ptr3)+,crt0_temp
	and.b		#0xFC,crt0_temp				| clear PS0 & PS1
	or.b		crt0_buswidth,crt0_temp			| set boot PROM bus width
	move.l		crt0_temp,(crt0_ptr4)+

	/*------------------------*/
	/* -- read PDCS buffer -- */
	moves.b		REG_PDCS(crt0_glue),crt0_pdcs
/*	move.b		#0x3F,crt0_pdcs 					pour test */

	/*---------------------------------------*/
	/* -- EPLD chip-select initialization -- */
	/*---------------------------------------*/
	btst.b		#0,crt0_csswitch
	beq		_cs_init_end

	/*--------------------------------------------*/
	/* 68349 generated /CS[3..0] must be disabled */
	lea.l   	SIM_MASKH0(crt0_sim_base),crt0_ptr4
	lea.l   	_table_cs349(%pc),crt0_ptr3
	moveq.l 	#0x03,crt0_temp
_cs349_clear:
	move.l  	(crt0_ptr3)+,(crt0_ptr4)+
	move.l  	(crt0_ptr3)+,crt0_spare6
	and.b		#0xFE,crt0_spare6			| disable chip-select
	move.l		crt0_spare6,(crt0_ptr4)+
	dbra    	crt0_temp,_cs349_clear

	/*---------------------------------------------*/
	/* EPLD generated /CS[3..0] must be configured */
_csepld_init:
	move.l		crt0_glue,crt0_ptr4
	lea.l   	_table_csepld(%pc),crt0_ptr3

	move.b  	(crt0_ptr3)+,crt0_temp
	or.b		#0x20,crt0_temp				| default width is 32 bits
	tst.b		crt0_buswidth				| is boot PROM bus width 32 bits ?
	beq		_csepld1				| if not
	and.b		#0xDF,crt0_temp				| set width to 16 bits
_csepld1:
	moves.b		crt0_temp,(crt0_ptr4)+

	moveq.l 	#0x02,crt0_spare6
_csepld2:
	move.b  	(crt0_ptr3)+,crt0_temp
	moves.b		crt0_temp,(crt0_ptr4)+
	dbra    	crt0_spare6,_csepld2

_cs_init_end:

	/*--------------------------------------*/
	/* -- DRAM controller initialization -- */
_dram_init:
	move.w 		#15,crt0_temp
	move.l		#_ExtRam_Start,crt0_ptr3

_dram_init1:
	clr.l		(crt0_ptr3)+				| must access DRAM
	dbra		crt0_temp,_dram_init1			| prior to init refresh

_dram_init2:
	move.b 		#3,crt0_temp
	moves.b		crt0_temp,REG_WS(crt0_dram) 		| set 3 wait-states

	move.b 		#0x81,crt0_temp
	moves.b		crt0_temp,REG_REFRESH(crt0_dram) 	| refresh every 10µs

	move.b 		#0,crt0_temp
	moves.b		crt0_temp,REG_CONFIG(crt0_dram)		| default size = 4Mbytes

	/*-----------------------*/
	/* -- configure cache -- */
_init_cache:
	move.l		#0x000001E0,CACHE_MCR(crt0_sim_base)
	btst.b		#bit_cache,crt0_pdcs
	bne		_init_cache_end
	or.l		#0x00000001,CACHE_MCR(crt0_sim_base)

_init_cache_end:

	/*-----------------------------*/
	/* -- timers initialization -- */

	clr.b 		crt0_temp
	moves.b		crt0_temp,REG_TIMER1(crt0_glue)		| disable timer 1
	moves.b		crt0_temp,REG_TIMER2(crt0_glue)		| disable timer 2

	/*--------------------------*/
	/* -- I2C initialization -- */
	move.b 		#3,crt0_temp
	moves.b		crt0_temp,REG_I2C(crt0_glue)		| tri-states I2C ports

	/*-----------------------------------------*/
	/* -- baudrate generator initialization -- */
	move.b 		#2,crt0_temp
	moves.b		crt0_temp,REG_BAUDRATE(crt0_glue) 	| baudrate = 38400

	/*-------------------------------*/
	/* --  IO port initialization -- */
	clr.b 		crt0_temp
	moves.b		crt0_temp,REG_IO(crt0_glue)		| set port as input

	/* -- */

	move.l 		#68349,crt0_cpu_type

	/* -- jump back to PROM -- */

	jmp.l		(_fill_test)				| must be absolute long

_copy_end_code:

/*-------------------------------------------------
  initialization code for the 68340 board
  -------------------------------------------------*/

	/* Astecc  68340 board : chip-select initialization values */
_table_cs340:
	dc.l	0x003FFFF0               		/* Mask CS0 (4Mbytes PROM, 32bits, 0WS)	*/
	dc.l	((_PROM_Start&0xFFFFFF00)+0x00000003)  	/* Base CS0 */
	dc.l    0x0000FFFD               		/* MASK CS1 (RAMBCC340, 0WS, FTE) */
	dc.l	((_BCCram_Start&0xFFFFFF00)+0x00000003)	/* Base CS1 */
	dc.l    0x000000FF               		/* MASK CS2 (FLEX, ext DTACK, 256 bytes) */
	dc.l	((_FLEX_Start&0xFFFFFF00)+0x00000003)  	/* Base CS2 */
	dc.l	0x000000FF               		/* Mask CS3 (I2C, ext DTACK, 256 bytes)	*/
	dc.l	((_I2C_Start&0xFFFFFF00)+0x00000003)  	/* Base CS3 */

cpu_is_68340:

	/* -- set cpu clock -- */
	move.w  #_SPEED340,SIM_SYNCR(crt0_sim_base) 		| clock
sync_wait340:
	btst.b	#3,(SIM_SYNCR+1)(crt0_sim_base)
	beq	sync_wait340

	/* -- chip select initialization -- */
	lea.l   SIM_MASKH0(crt0_sim_base),crt0_ptr4
	lea.l   _table_cs340(%pc),crt0_ptr3
	moveq.l #0x07,crt0_temp
_b_cs340:
	move.l  (crt0_ptr3)+,crt0_ptr5
	move.l 	crt0_ptr5,(crt0_ptr4)+				| pour test
	dbra    crt0_temp,_b_cs340

	move.l	#68340,crt0_cpu_type
	move.b	#0,crt0_csswitch				| CPU
	move.b	#1,crt0_buswidth				| 16 bits

	/*-------------------------------------------------
	  fill RAM if COLDSTART
	  -------------------------------------------------*/
_fill_test:

  	tst.l 	crt0_boot_type
  	bne 	_dont_fill

	cmp.b	#_CPU349,SIM_IDR(crt0_sim_base)
	bne	_fill
	btst.b	#bit_meminit,crt0_pdcs
	bne	_dont_fill

	/* fill main memory */
_fill:
	move.l  #_crt0_init_stack,crt0_ptr3			| skip Astecc vars
  	move.l	#_ExtRam_Start,crt0_temp
	sub.l  	#_crt0_init_stack,crt0_temp
	add.l   #_ExtRam_Size,crt0_temp 			| get size
	lsr.l   #2,crt0_temp            			| ajust for long word
_fill_loop:
	clr.l 	(crt0_ptr3)+
	subq.l 	#1,crt0_temp
	bne 	_fill_loop

	cmp.b	#_CPU349,SIM_IDR(crt0_sim_base)
	bne	_fill_bccram

	/* fill QDMM memory */
	movea.l #_FastRam_Start,crt0_ptr3			| get start
	move.l  #_FastRam_Size,crt0_temp 			| get size
	lsr.l   #2,crt0_temp            			| ajust for long word

_QDMMfill_loop:
	clr.l 	(crt0_ptr3)+
	subq.l	#1,crt0_temp
	bne	_QDMMfill_loop
	bra	_dont_fill

	/* fill BCC memory */
_fill_bccram:
	movea.l #_BCCram_Start,crt0_ptr3			| get start
	move.l  #_BCCram_Size,crt0_temp 			| get size
	lsr.l   #2,crt0_temp            			| ajust for long word
_BCCfill_loop:
	clr.l 	(crt0_ptr3)+
	subq.l 	#1,crt0_temp
	bne 	_BCCfill_loop

	/*-------------------------------------------------*/
_dont_fill:
	move.b	crt0_csswitch,_AsteccCsSwitch
	move.b	crt0_buswidth,_AsteccBusWidth
	move.l	crt0_cpu_type,_AsteccCpuName

	jmp	SYM(_Init68340)					| Start C code (which never returns)

/*
 * Copy DATA segment, clear BSS segment, set up real stack,
 * initialize heap, start C program.
 * Assume that DATA and BSS sizes are multiples of 4.
 */
	PUBLIC (_CopyDataClearBSSAndStart)
SYM(_CopyDataClearBSSAndStart):
	lea	SYM(_copy_start),a0				| Get start of DATA in RAM
	lea	SYM(_etext),a2					| Get start of DATA in ROM
	cmpl	a0,a2						| Are they the same?
	beq.s	NOCOPY						| Yes, no copy necessary
	lea	SYM(_copy_end),a1				| Get end of DATA in RAM
	bra.s	COPYLOOPTEST					| Branch into copy loop
COPYLOOP:
	movel	a2@+,a0@+					| Copy word from ROM to RAM
COPYLOOPTEST:
	cmpl	a1,a0						| Done?
	bcs.s	COPYLOOP					| No, skip
NOCOPY:

	lea	_clear_start,a0					| Get start of BSS
	lea	_clear_end,a1					| Get end of BSS
	clrl	d0						| Value to set
	bra.s	ZEROLOOPTEST					| Branch into clear loop
ZEROLOOP:
	movel	d0,a0@+						| Clear a word
ZEROLOOPTEST:
	cmpl	a1,a0						| Done?
	bcs.s	ZEROLOOP					| No, skip

	movel	#_stack_init,a7					| set master stack pointer
	movel	d0,a7@-						| command line
	jsr	SYM(boot_card)					| Call C main

	PUBLIC (_mainDone)
SYM(_mainDone):
	nop							| Leave spot for breakpoint
	movew	#1,a7						| Force a double bus error
	movel	d0,a7@-						| This should cause a RESET
/*	stop	#0x2700						| Stop with interrupts disabled */
	move.w	#0x2700,sr
	bra.l	SYM(_mainDone)					| Stuck forever

        .align 2
BEGIN_DATA_DCL
        .align 2
	PUBLIC (environ)
SYM (environ):
	.long	0
	PUBLIC (_M68kSpuriousInterruptCount)
SYM (_M68kSpuriousInterruptCount):
	.long	0
END_DATA_DCL

END
