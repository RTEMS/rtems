/*
 *  em86real.S
 *
 *  Copyright (C) 1998, 1999 Gabriel Paubert, paubert@iram.es
 *
 *  Modified to compile in RTEMS development environment
 *  by Eric Valette
 *
 *  Copyright (C) 1999 Eric Valette. valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

/* If the symbol __BOOT__ is defined, a slightly different version is
 * generated to be compiled with the -m relocatable option
 */

#ifdef __BOOT__
#include "bootldr.h"
/* It is impossible to gather statistics in the boot version */
#undef EIP_STATS
#endif

/*
 *
 * Given the size of this code, it deserves a few comments on how it works,
 * and why it was implemented the way it is.
 *
 * The goal is to have a real mode i486SX emulator to initialize hardware,
 * mostly graphics boards, by interpreting ROM BIOSes. The choice of a 486SX
 * is logical since this is the lowest processor that PCI ROM BIOSes must run
 * on.
 *
 * The goal of this emulator is not performance, but a small enough memory
 * footprint to include it in a bootloader.
 *
 * It is actually likely to be comparable to a 25MHz 386DX on a 200MHz 603e !
 * This is not as serious as it seems since most of the BIOS code performs
 * a lot of accesses to I/O and non-cacheable memory spaces. For such
 * instructions, the execution time is often dominated by bus accesses.
 * Statistics of the code also shows that it spends a large function of
 * the time in loops waiting for vertical retrace or programs one of the
 * timers and waits for the count to go down to zero. This type of loop
 * runs emulated at the same speed as on 5 GHz Pentium IV++ ;)
 *
 */

/*
 * Known bugs or differences with a real 486SX (real mode):
 * - segment limits are not enforced (too costly)
 * - xchg instructions with memory are not locked
 * - lock prefixes are not implemented at all
 * - long divides implemented but perhaps still buggy
 * - miscellaneous system instructions not implemented
 *   (some probably cannot be implemented)
 * - neither control nor debug registers are implemented for the time being
 *   (debug registers are impossible to implement at a reasonable cost)
 */

/* Code options,  put them on the compiler command line */
/* #define EIP_STATS */	/* EIP based profiling */
/* #undef EIP_STATS */

/*
 * Implementation notes:
 *
 * A) flags emulation.
 *
 * The most important decisions when it comes to obtain a reasonable speed
 * are related to how the EFLAGS register is emulated.
 *
 *   Note: the code to set up flags is complex, but it is only seldom
 * executed since cmp and test instructions use much faster flag evaluation
 * paths. For example the overflow flag is almost only needed for pushf and
 * int. Comparison results only involve (SF^OF) or (SF^OF)+ZF and the
 * implementation is fast in this case.
 *
 * Rarely used flags: AC, NT and IOPL are kept in a memory EFLAGS image.
 * All other flags are either kept explicitly in PPC cr (DF, IF, and TF) or
 * lazily evaluated from the state of 4 registers called flags, result, op1,
 * op2, and sometimes the cr itself. The emulation has been designed for
 * minimal overhead for the common case where the flags are never used. With
 * few exceptions, all instructions that set flags leave the result of the
 * computation in a register called result, and operands are taken from op1
 * and op2 registers. However a few instructions like cmp, test and bit tests
 * (bt/btc/btr/bts/bsf/bsr) explicitly set cr bits to short circuit
 * condition code evaluation of conditional instructions.
 *
 * As a very brief summary:
 *
 * - the result of the last flag setting operation is often either in the
 *   result register or in op2 after increment or decrement instructions
 *   because result and op1 may be needed to compute the carry.
 *
 * - compare instruction leave the result of the unsigned comparison
 *   in cr4 and of signed comparison in cr6. This means that:
 *   - cr4[0]=CF            (short circuit for jc/jnc)
 *   - cr4[1]=~(CF+ZF)      (short circuit for ja/jna)
 *   - cr6[0]=(OF^SF)       (short circuit for jl/jnl)
 *   - cr6[1]=~((SF^OF)+ZF) (short circuit for jg/jng)
 *   - cr6[2]=ZF            (short circuit for jz/jnz)
 *
 * - test instruction set flags in cr6 and clear overflow. This means that:
 *   - cr6[0]=SF=(SF^OF)    (short circuit for jl/jnl/js/jns)
 *   - cr6[1]=~((SF^OF)+ZF) (short circuit for jg/jng)
 *   - cr6[2]=ZF            (short circuit for jz/jnz)
 *
 * All flags may be lazily evaluated from several values kept in registers:
 *
 *	Flag:	Depends upon:
 *	OF      result, op1, op2, flags[INCDEC_FIELD,SUBTRACTING,OF_STATE_MASK]
 *	SF	result, op2, flags[INCDEC_FIELD,RES_SIZE]
 *	ZF	result, op2, cr6[2], flags[INCDEC_FIELD,RES_SIZE,ZF_PROTECT]
 *	AF	op1, op2, flags[INCDEC_FIELD,SUBTRACTING,CF_IN]
 *	PF	result, op2, flags[INCDEC_FIELD]
 *	CF	result, op1, flags[CF_STATE_MASK, CF_IN]
 *
 * The order of the fields in the flags register has been chosen so that a
 * single rlwimi is necessary for common instruction that do not affect all
 * flags. (See the code for inc/dec emulation).
 *
 *
 * B) opcodes and prefixes.
 *
 * The register called opcode holds in its low order 8 bits the opcode
 * (second byte if the first byte is 0x0f). More precisely it holds the
 * last byte fetched before the modrm byte or the immediate operand(s)
 * of the instruction, if any. High order 24 bits are zero unless the
 * instruction has prefixes. These higher order bits have the following
 * meaning:
 * 0x80000000	segment override prefix
 * 0x00001000   repnz prefix (0xf2)
 * 0x00000800   repz prefix (0xf3)
 * 0x00000400   address size prefix (0x67)
 * 0x00000200   operand size prefix (0x66)
 * (bit 0x1000 and 0x800 cannot be set simultaneously)
 *
 * Therefore if there is a segment override the value will be between very
 * negative (between 0x80000000 and 0x800016ff), if there is no segment
 * override, the value will be between 0 and 0x16ff. The reason for
 * this choice will be understood in the next part.
 *
 * C) addresing mode description tables.
 *
 * the encoding of the modrm bytes (especially in 16 bit mode) is quite
 * complex. Hence a table, indexed by the five useful bits of the modrm
 * byte is used to simplify decoding. Here is a description:
 *
 *  bit mask	meaning
 *  0x80000000	use ss as default segment register
 *  0x00004000	means that this addressing mode needs a base register
 *              (set for all entries except sib and displacement-only)
 *  0x00002000	set if preceding is not set
 *  0x00001000  set if an sib follows
 *  0x00000700	base register to use (16 and 32 bit)
 *  0x00000080	set in 32 bit addressing mode table, cleared in 16 bit
 *		(so extsb mask,entry; ori mask,mask,0xffff gives a mask)
 *  0x00000070	kludge field, possible values are
 *		0: 16 bit addressing mode without index
 *		10: 32 bit addressing mode
 *		60: 16 bit addressing mode with %si as index
 *		70: 16 bit addressing mode with %di as index
 *
 *  This convention leads to the following special values used to check for
 * sib present and displacement-only, which happen to the three lowest
 * values in the table (unsigned):
 * 0x00003090	sib follows (implies it is a 32 bit mode)
 * 0x00002090	32 bit displacement-only
 * 0x00002000	16 bit displacement-only
 *
 * This means that all entries are either very negative in the 0x80002000
 * range if the segment defaults to ss or higher than 0x2000 if it defaults
 * to ds. Combined with the value in opcode this gives the following table:
 * opcode	entry		entry>opcode ?	segment to use
 * positive	positive	yes		ds (default)
 * negative	positive	yes		overriden by prefix
 * positive	negative	no		ss
 * negative	negative	yes		overridden by prefix
 *
 * Hence a simple comparison allows to check for the need to override
 * the current base with ss, i.e., when ss is the default base and the
 * instruction has no override prefix.
 *
 * D) BUGS
 *
 * This software is obviously bug-free :-). Nevertheless, if you encounter
 * an interesting feature. Mail me a note, if possible with a detailed
 * instruction example showing where and how it fails.
 *
 */

/* Now the details of flag evaluation with the necessary macros */

/* Alignment check is toggable so the system believes it is a 486, but
CPUID is not to avoid unnecessary complexities. However, alignment
is actually never checked (real mode is CPL 0 anyway). */
#define AC86    13		/* Can only be toggled */
#define VM86	14		/* Not used for now */
#define RF86    15		/* Not emulated precisely */
/* Actually NT and IOPL are kept in memory */
#define NT86    17
#define IOPL86  18		/* Actually 18 and 19 */
#define OF86	20
#define DF86	21
#define IF86	22
#define TF86	23
#define SF86	24
#define ZF86	25
#define AF86	27
#define PF86	29
#define CF86	31

/* Where the less important flags are placed in PPC cr */
#define RF	20		/* Suppress trap flag: cr5[0] */
#define	DF	21		/* Direction flag: cr5[1] */
#define IF	22		/* Interrupt flag: cr5[2] */
#define TF      23		/* Single step flag: cr5[3] */

/* Now the flags which are frequently used */
/*
 * CF_IN is a copy of the input carry with PPC polarity,
 * it is cleared for add, set for sub and cmp,
 * equal to the x86 carry for adc and to its complement for sbb.
 * it is used to evaluate AF and CF.
 */
#define CF_IN		0x80000000

/* #define GET_CF_IN(dst)	rlwinm dst,flags,1,0x01 */

/* CF_IN_CR set in flags means that cr4[0] is a copy of carry bit */
#define CF_IN_CR	0x40000000

#define EVAL_CF		andis. r3,flags,(CF_IN_CR)>>16; beql- _eval_cf

/*
 * CF_STATE tells how to compute the carry bit.
 * NOTRESULT16 and NOTRESULT8 are never set explicitly,
 * but they may happen after a cmc instruction.
 */
#define CF		16		/* cr4[0] */
#define CF_LOCATION	0x30000000
#define CF_ZERO		0x00000000
#define CF_EXPLICIT	0x00000000
#define CF_COMPLEMENT	0x08000000	/* Indeed a polarity bit */
#define CF_STATE_MASK	(CF_LOCATION|CF_COMPLEMENT)
#define CF_VALUE	0x08000000
#define CF_SET		0x08000000
#define CF_RES32	0x10000000
#define CF_NOTRES32	0x18000000
#define CF_RES16	0x20000000
#define CF_NOTRES16	0x28000000
#define CF_RES8		0x30000000
#define CF_NOTRES8	0x38000000

#define CF_ADDL		CF_RES32
#define CF_SUBL		CF_NOTRES32
#define CF_ADDW		CF_RES16
#define CF_SUBW		CF_RES16
#define CF_ADDB		CF_RES8
#define CF_SUBB		CF_RES8

#define CF_ROTCNT(dst)	rlwinm dst,flags,7,0x18
#define CF_POL(dst,pos)	rlwinm dst,flags,(36-pos)%32,pos,pos
#define CF_POL_INSERT(dst,pos)	\
			rlwimi dst,flags,(36-pos)%32,pos,pos
#define RES2CF(dst)	rlwinm dst,result,8,7,15

/*
 * OF_STATE tells how to compute the overflow bit. When the low order bit
 * is set (OF_EXPLICIT), it means that OF is the exclusive or of the
 * two other bits. For the reason of this choice, see rotate instructions.
 */
#define	OF		1		/* Only after EVAL_OF */
#define OF_STATE_MASK	0x07000000
#define OF_INCDEC	0x00000000
#define OF_EXPLICIT	0x01000000
#define OF_ZERO		0x01000000
#define OF_VALUE        0x04000000
#define OF_SET          0x04000000
#define OF_ONE		0x05000000
#define OF_XOR		0x06000000
#define OF_ARITHL	0x06000000
#define OF_ARITHW	0x02000000
#define OF_ARITHB	0x04000000

#define EVAL_OF		rlwinm. r3,flags,6,0,1; bngl+ _eval_of; andis. r3,flags,OF_VALUE>>16

/* See _eval_of to see how this can be used */
#define OF_ROTCNT(dst)	rlwinm dst,flags,10,0x1c

/*
 * SIGNED_IN_CR means that cr6 is set as after a signed compare:
 * - cr6[0] is SF^OF for jl/jnl/setl/setnl...
 * - cr6[1] is ~((SF^OF)+ZF) for jg/jng/setg/setng...
 * - cr6[2] is ZF (ZF_IN_CR is always set if this bit is set)
 */
#define SLT		24		/* cr6[0], signed less than */
#define SGT		25		/* cr6[1], signed greater than */
#define SIGNED_IN_CR	0x00800000

#define EVAL_SIGNED	andis. r3,flags,SIGNED_IN_CR>>16; beql- _eval_signed

/*
 * Above in CR means that cr4 is set as after an unsigned compare:
 * - cr4[0] is CF (CF_IN_CR is also set)
 * - cr4[1] is ~(CF+ZF) (ZF_IN_CR is also set)
 */
#define ABOVE		17		/* cr4[1] */
#define ABOVE_IN_CR	0x00400000

#define EVAL_ABOVE	andis. r3,flags,ABOVE_IN_CR>>16; beql- _eval_above

/* SF_IN_CR means cr6[0] is a copy of SF. It implies ZF_IN_CR is also set */
#define SF		24		/* cr6[0] */
#define SF_IN_CR	0x00200000

#define EVAL_SF		andis. r3,flags,SF_IN_CR>>16; beql- _eval_sf_zf

/* ZF_IN_CR means cr6[2] is a copy of ZF. */
#define ZF	26
#define ZF_IN_CR	0x00100000

#define EVAL_ZF		andis. r3,flags,ZF_IN_CR>>16; beql- _eval_sf_zf
#define ZF2ZF86(s,d)	rlwimi d,s,ZF-ZF86,ZF86,ZF86
#define ZF862ZF(reg)	rlwimi reg,reg,32+ZF86-ZF,ZF,ZF

/*
 * ZF_PROTECT means cr6[2] is the only valid value for ZF. This is necessary
 * because some infrequent instructions may leave SF and ZF in an apparently
 * inconsistent state (both set): sahf, popf and the few (not implemented)
 * instructions that only affect ZF.
 */
#define ZF_PROTECT	0x00080000

/* The parity is always evaluated when it is needed */
#define PF		0		/* Only after EVAL_PF */
#define EVAL_PF		bl _eval_pf

/* This field gives the shift amount to use to evaluate SF
   and ZF when ZF_PROTECT is not set */
#define RES_SIZE_MASK	0x00060000
#define RESL		0x00000000
#define RESW		0x00040000
#define RESB		0x00060000

#define RES_SHIFT(dst)	rlwinm dst,flags,18,0x18

/* SUBTRACTING is set if the last flag setting instruction was sub/sbb/cmp,
   used to evaluate OF and AF */
#define SUBTRACTING	0x00010000

#define GET_ADDSUB(dst)	rlwinm dst,flags,16,0x01

/* rotate (rcl/rcr/rol/ror) affect CF and OF but not other flags */
#define ROTATE_MASK	(CF_IN_CR|CF_STATE_MASK|ABOVE_IN_CR|OF_STATE_MASK|SIGNED_IN_CR)
#define ROTATE_FLAGS	rlwimi flags,one,24,ROTATE_MASK

/*
 * INCDEC_FIELD has at most one bit set when the last flag setting instruction
 * was either inc or dec (which do not affect the carry). When one of these
 * bits is set, it affects the way OF, SF, ZF, AF, and PF are evaluated.
 */
#define INCDEC_FIELD	0x0000ff00

#define DECB_SHIFT	8
#define INCB_SHIFT	9
#define DECW_SHIFT	10
#define INCW_SHIFT	11
#define DECL_SHIFT	14
#define INCL_SHIFT	15

#define INCDEC_MASK	(OF_STATE_MASK|SIGNED_IN_CR|ABOVE_IN_CR|SF_IN_CR|\
			ZF_IN_CR|ZF_PROTECT|RES_SIZE_MASK|SUBTRACTING|\
			INCDEC_FIELD)
/* Operations to perform to tell where the flags are after inc or dec */
#define INC_FLAGS(BWL)	rlwimi flags,one,INC##BWL##_SHIFT,INCDEC_MASK
#define DEC_FLAGS(BWL)	rlwimi flags,one,DEC##BWL##_SHIFT,INCDEC_MASK

/* How the flags are set after arithmetic operations */
#define FLAGS_ADD(BWL)	(CF_ADD##BWL|OF_ARITH##BWL|RES##BWL)
#define FLAGS_SBB(BWL)	(CF_SUB##BWL|OF_ARITH##BWL|RES##BWL|SUBTRACTING)
#define FLAGS_SUB(BWL)	FLAGS_SBB(BWL)|CF_IN
#define FLAGS_CMP(BWL)	FLAGS_SUB(BWL)|ZF_IN_CR|CF_IN_CR|SIGNED_IN_CR|ABOVE_IN_CR

/* How the flags are set after logical operations */
#define FLAGS_LOG(BWL)	(CF_ZERO|OF_ZERO|RES##BWL)
#define FLAGS_TEST(BWL)	FLAGS_LOG(BWL)|ZF_IN_CR|SIGNED_IN_CR|SF_IN_CR

/*  How the flags are set after bt/btc/btr/bts. */
#define FLAGS_BTEST	CF_IN_CR|CF_ADDL|OF_ZERO|RESL

/*  How the flags are set after bsf/bsr. */
#define FLAGS_BSRCH(WL)	CF_ZERO|OF_ZERO|RES##WL|ZF_IN_CR

/* How the flags are set after logical right shifts */
#define FLAGS_SHR(BWL)	(CF_EXPLICIT|OF_ARITH##BWL|RES##BWL)

/* How the flags are set after double length shifts */
#define FLAGS_DBLSH(WL)	(CF_EXPLICIT|OF_ARITH##WL|RES##WL)

/* How the flags are set after multiplies */
#define FLAGS_MUL	(CF_EXPLICIT|OF_EXPLICIT)

#define SET_FLAGS(fl)	lis flags,(fl)>>16
#define ADD_FLAGS(fl)	addis flags,flags,(fl)>>16

/*
 * We are always off by one when compared with Intel's eip, this shortens
 * code by allowing to load next byte with lbzu x,1(eip). The register
 * called eip actually contains csbase+eip, and thus should be called lip
 * for linear ip.
 */

/*
 * Reason codes passed to the C part of the emulator, this includes all
 * instructions which may change the current code segment. These definitions
 * will soon go into a separate include file. Codes 0 to 255 correspond
 * directly to the interrupt/trap that has to be generated.
 */

#define code_divide_err	0
#define code_trap	1
#define code_int3	3
#define code_into	4
#define code_bound	5
#define code_ud		6
#define code_dna	7	/* FPU not available */

#define code_iretw	256	/* Interrupt returns */
#define code_iretl	257
#define code_lcallw	258	/* Far calls and jumps */
#define code_lcalll	259
#define code_ljmpw	260
#define code_ljmpl	261
#define code_lretw	262	/* Far returns */
#define code_lretl	263
#define code_softint	264	/* int $xx */
#define code_lock	265	/* Lock prefix */
/* Codes 1024 to 2047 are used for I/O port access instructions:
 - The three LSB define the port size (1, 2 or 4)
 - bit of weight 512 means out if set, in if clear
 - bit of weight 256 means ins/outs if set, in/out if clear
 - bit of weight 128 means use 32 bit addresses if set, 16 bit if clear
   (only used for ins/outs instructions, always clear for in/out)
 */
#define code_inb	1024+1
#define code_inw	1024+2
#define code_inl	1024+4
#define code_outb	1024+512+1
#define code_outw	1024+512+2
#define code_outl	1024+512+4
#define code_insb_a16	1024+256+1
#define code_insw_a16	1024+256+2
#define code_insl_a16	1024+256+4
#define code_outsb_a16	1024+512+256+1
#define code_outsw_a16	1024+512+256+2
#define code_outsl_a16	1024+512+256+4
#define code_insb_a32	1024+256+128+1
#define code_insw_a32	1024+256+128+2
#define code_insl_a32	1024+256+128+4
#define code_outsb_a32	1024+512+256+128+1
#define code_outsw_a32	1024+512+256+128+2
#define code_outsl_a32	1024+512+256+128+4

#define state 31
/* r31 (state) is a pointer to a structure describing the emulated x86
processor, its layout is the following:

first the general purpose registers, they are in little endian byte order

offset	name

   0	eax/ax/al
   1	ah
   4	ecx/cx/cl
   5	ch
   8	edx/dx/dl
   9	dh
  12	ebx/bx/bl
  13	bh
  16	esp/sp
  20	ebp/bp
  24	esi/si
  28	edi/di
*/

#define AL	0
#define AX	0
#define EAX	0
#define AH	1
#define CL	4
#define CX	4
#define ECX	4
#define DX	8
#define EDX	8
#define BX	12
#define EBX	12
#define SP	16
#define ESP	16
#define BP	20
#define EBP	20
#define SI	24
#define ESI	24
#define DI	28
#define EDI	28

/*
than the rest of the machine state, big endian !

offset     name

  32       essel     segment register selectors (values)
  36	   cssel
  40       sssel
  44       dssel
  48       fssel
  52       gssel
  56       eipimg    true eip (register named eip is csbase+eip)
  60       eflags    eip and eflags only valid when C code running !
  64       esbase    segment registers bases
  68       csbase
  72       ssbase
  76       dsbase
  80       fsbase
  84       gsbase
  88       iobase    For I/O instructions, I/O space virtual base
  92       ioperm    I/O permission bitmap pointer
  96       reason    Reason code when calling external emulator
 100       nexteip   eip past instruction for external emulator
 104       parm1     parameter for external emulator
 108       parm2     parameter for external emulator
 112       _opcode   current opcode register for external emulator
 116       _base     segment register base for external emulator
 120       _offset   intruction operand offset
 More internal state was dumped here for debugging in first versions

 128       vbase     where the 1Mb memory is mapped
 132       cntimg    instruction counter
 136                 scratch
 192       eipstat   array of 32k unsigned long pairs for eip stats
*/

#define essel	32
#define cssel	36
#define sssel	40
#define dssel	44
#define fssel	48
#define gssel	52
#define eipimg	56
#define eflags	60
#define esbase	64
#define csbase	68
#define ssbase	72
#define dsbase	76
#define fsbase	80
#define gsbase	84
#define iobase	88
#define ioperm	92
#define reason	96
#define nexteip	100
#define parm1	104
#define parm2	108
#define _opcode	112
#define _base	116
#define _offset 120
#define vbase	128
#define cntimg	132
#ifdef EIP_STATS
#define eipstat	192
#endif
/* Global registers */

/* Some segment register bases are permanently kept in registers since they
are often used: these are csb, esb and ssb because they are
required for jumps, string instructions, and pushes/pops/calls/rets.
dsbase is not kept in a register but loaded from memory to allow somewhat
more parallelism in the main emulation loop.
*/

#define one	30		/* Constant one, so pervasive */
#define ssb	29
#define csb	28
#define esb	27
#define eip	26		/* That one is indeed csbase+(e)ip-1 */
#define result	25		/* For the use of result, op1, op2 */
#define op1	24		/* see the section on flag emulation */
#define op2	23
#define opbase	22		/* default opcode table */
#define flags	21		/* See earlier description */
#define opcode	20		/* Opcode */
#define opreg	19		/* Opcode extension/register number */
/* base is reloaded with the base of the ds segment at the beginning of
every instruction, it is modified by segment override prefixes, when
the default base segment is ss, or when the modrm byte specifies a
register operand */
#define base	18		/* Instruction's operand segment base */
#define offset	17		/* Instruction's memory operand offset */
/* used to address a table telling how to decode the addressing mode
specified by the modrm byte */
#define adbase	16		/* addressing mode table */
/* Following registers are used only as dedicated temporaries during decoding,
they are free for use during emulation */
/*
 * ceip (current eip) is only in use when we call the external emulator for
 * instructions that fault. Note that it is forbidden to change flags before
 * the check for the fault happens (divide by zero...) ! ceip is also used
 * when measuring timing.
 */
#define ceip 15

/* A register used to measure timing information (when enabled) */
#ifdef EIP_STATS
#define tstamp 14
#endif

#define count 12		/* Instruction counter. */

#define r0 0
#define r1 1			/* PPC Stack pointer. */
#define r3 3
#define r4 4
#define r5 5
#define r6 6
#define r7 7

/* Macros to read code stream */
#define NEXTBYTE(dest) lbzu dest,1(eip)
#define NEXTWORD(dest) lhbrx dest,eip,one; la eip,2(eip)
#define NEXTDWORD(dest) lwbrx dest,eip,one; la eip,4(eip)
#define NEXT	b nop
#define GOTNEXT b gotopcode

#ifdef __BOOT__
		START_GOT
		GOT_ENTRY(_jtables)
		GOT_ENTRY(jtab_www)
		GOT_ENTRY(adtable)
		END_GOT
#else
		.text
#endif
		.align 2
		.global em86_enter
		.type em86_enter,@function
em86_enter:	stwu r1,-96(r1)			# allocate stack
		mflr r0
		stmw 14,24(r1)
		mfcr r4
		stw r0,100(r1)
		mr state,r3
		stw r4,20(r1)
#ifdef __BOOT__
/* We need this since r30 is the default GOT pointer */
#define	r30 30
		GET_GOT
/* The relocation of these tables is explicit, this could be done
 * automatically with fixups but would add more than 8kb in the fixup tables.
 */
		lwz r3,GOT(_jtables)
		lwz r4,_endjtables-_jtables(r3)
		sub. r4,r3,r4
		beq+ 1f
		li r0,((_endjtables-_jtables)>>2)+1
		addi r3,r3,-4
		mtctr r0
0:		lwzu r5,4(r3)
		add r5,r5,r4
		stw r5,0(r3)
		bdnz 0b
1:		lwz adbase,GOT(adtable)
		lwz opbase,GOT(jtab_www)
/* Now r30 is only used as constant 1 */
#undef r30
		li one,1			# pervasive constant
#else
		lis opbase,jtab_www@ha
		lis adbase,adtable@ha
		li one,1			# pervasive constant
		addi opbase,opbase,jtab_www@l
		addi adbase,adbase,adtable@l
#ifdef EIP_STATS
		li ceip,0
		mftb tstamp
#endif
#endif
/* We branch back here when calling an external function tells us to resume */
restart:	lwz r3,eflags(state)
		lis flags,(OF_EXPLICIT|ZF_IN_CR|ZF_PROTECT|SF_IN_CR)>>16
		lwz csb,csbase(state)
		extsb result,r3			# SF/PF
		rlwinm op1,r3,31,0x08		# AF
		lwz eip,eipimg(state)
		ZF862ZF(r3)			# cr6
		addi op2,op1,0			# AF
		lwz ssb,ssbase(state)
		rlwimi flags,r3,15,OF_VALUE	# OF
		rlwimi r3,r3,32+RF86-RF,RF,RF	# RF
		lwz esb,esbase(state)
		ori result,result,0xfb		# PF
		mtcrf 0x06,r3			# RF/DF/IF/TF/SF/ZF
		lbzux opcode,eip,csb
		rlwimi flags,r3,27,CF_VALUE	# CF
		xori result,result,0xff		# PF
		lwz count,cntimg(state)
		GOTNEXT				# start the emulator

/* Now return */
exit:		lwz r0,100(r1)
		lwz r4,20(r1)
		mtlr r0
		lmw 14,24(r1)
		mtcr r4
		addi r1,r1,96
		blr

trap:		crmove 0,RF
		crclr RF
		bt- 0,resume
		sub ceip,eip,csb
		li r3,code_trap
complex:	addi eip,eip,1
		stw r3,reason(state)
		sub eip,eip,csb
		stw op1,240(state)
		stw op2,244(state)
		stw result,248(state)
		stw flags,252(state)
		stw r4,parm1(state)
		stw r5,parm2(state)
		stw opcode,_opcode(state)
		bl _eval_flags
		stw base,_base(state)
		stw eip,nexteip(state)
		stw r3,eflags(state)
		mr r3,state
		stw offset,_offset(state)
		stw ceip,eipimg(state)
		stw count,cntimg(state)
		bl em86_trap
		cmpwi r3,0
		bne exit
		b restart

/* Main loop */
/*
 * The two LSB of each entry in the main table mean the following:
 * 00: indirect opcode: modrm follows and the three middle bits are an
 *     opcode extension. The entry points to another jump table.
 * 01: direct instruction, branch directly to the routine.
 * 10: modrm specifies byte size memory and register operands.
 * 11: modrm specifies word/long memory and register operands.
 *
 *  The modrm byte, if present, is always loaded in r7.
 *
 * Note: most "mr x,y" instructions have been replaced by "addi x,y,0" since
 * the latter can be executed in the second integer unit on 603e.
 */

/*
 * This code is very good example of absolutely unmaintainable code.
 * It was actually much easier to write than it is to understand !
 * If my computations are right, the maximum path length from fetching
 * the opcode to exiting to the actual instruction execution is
 * 46 instructions (for non-prefixed, single byte opcode instructions).
 *
 */
		.align 5
#ifdef EIP_STATS
nop:		NEXTBYTE(opcode)
gotopcode:	slwi r3,opcode,2
		bt- TF,trap
resume:		lwzx r4,opbase,r3
		addi r5,state,eipstat+4
		clrlslwi r6,ceip,17,3
		mtctr r4
		lwzux r7,r5,r6
		slwi. r0,r4,30		# two lsb of table entry
		sub r7,r7,tstamp
		lwz r6,-4(r5)
		mftb tstamp
		addi r6,r6,1
		sub ceip,eip,csb
		stw r6,-4(r5)
		add r7,r7,tstamp
		lwz base,dsbase(state)
		stw r7,0(r5)
#else
nop:		NEXTBYTE(opcode)
gotopcode:	slwi r3,opcode,2
		bt- TF,trap
resume:		lwzx r4,opbase,r3
		sub ceip,eip,csb
		mtctr r4
		slwi. r0,r4,30		# two lsb of table entry
		lwz base,dsbase(state)
		addi count,count,1
#endif
		bgtctr-			# for instructions without modrm

/* modrm byte present */
		NEXTBYTE(r7)		# modrm byte
		cmplwi cr1,r7,192
		rlwinm opreg,r7,31,0x1c
		beq- cr0,8f		# extended opcode
/* modrm with middle 3 bits specifying a register (non prefixed) */
		rlwinm r0,r4,3,0x8
		li r4,0x1c0d
		rlwimi opreg,r7,27,0x01
		srw r4,r4,r0
		and opreg,opreg,r4
		blt cr1,9f
/* modrm with 2 register operands */
1:		rlwinm offset,r7,2,0x1c
		addi base,state,0
		rlwimi offset,r7,30,0x01
		and offset,offset,r4
		bctr

/* Prefixes: first segment overrides */
		.align 4
_es:		NEXTBYTE(r7); addi base,esb,0
		oris opcode,opcode,0x8000; b 2f
_cs:		NEXTBYTE(r7); addi base,csb,0
		oris opcode,opcode,0x8000; b 2f
_fs:		NEXTBYTE(r7); lwz base,fsbase(state)
		oris opcode,opcode,0x8000; b 2f
_gs:		NEXTBYTE(r7); lwz base,gsbase(state)
		oris opcode,opcode,0x8000; b 2f
_ss:		NEXTBYTE(r7); addi base,ssb,0
		oris opcode,opcode,0x8000; b 2f
_ds:		NEXTBYTE(r7)
		oris opcode,opcode,0x8000; b 2f

/* Lock (unimplemented) and repeat prefixes */
_lock:		li r3,code_lock; b complex
_repnz:		NEXTBYTE(r7); rlwimi opcode,one,12,0x1800; b 2f
_repz:		NEXTBYTE(r7); rlwimi opcode,one,11,0x1800; b 2f

/* Operand and address size prefixes */
		.align 4
_opsize:	NEXTBYTE(r7); ori opcode,opcode,0x200
		rlwinm r3,opcode,2,0x1ffc; b 2f
_adsize:	NEXTBYTE(r7); ori opcode,opcode,0x400
		rlwinm r3,opcode,2,0x1ffc; b 2f

_twobytes:	NEXTBYTE(r7); addi r3,r3,0x400
2:		rlwimi r3,r7,2,0x3fc
		lwzx r4,opbase,r3
		rlwimi opcode,r7,0,0xff
		mtctr r4
		slwi. r0,r4,30
		bgtctr-				# direct instruction
/* modrm byte in a prefixed instruction */
		NEXTBYTE(r7)			# modrm byte
		cmpwi cr1,r7,192
		rlwinm opreg,r7,31,0x1c
		beq- 6f
/* modrm with middle 3 bits specifying a register (prefixed) */
		rlwinm r0,r4,3,0x8
		li r4,0x1c0d
		rlwimi opreg,r7,27,0x01
		srw r4,r4,r0
		and opreg,opreg,r4
		bnl cr1,1b			# 2 register operands
/* modrm specifying memory with prefix */
3:		rlwinm r3,r3,27,0xff80
		rlwimi adbase,r7,2,0x1c
		extsh r3,r3
		rlwimi r3,r7,31,0x60
		lwzx r4,r3,adbase
		cmpwi cr1,r4,0x3090
		bnl+ cr1,10f
/* displacement only addressing modes */
4:		cmpwi r4,0x2000
		bne 5f
		NEXTWORD(offset)
		bctr
5:		NEXTDWORD(offset)
		bctr
/* modrm with opcode extension (prefixed) */
6:		lwzx r4,r4,opreg
		mtctr r4
		blt cr1,3b
/* modrm with opcode extension and register operand */
7:		rlwinm offset,r7,2,0x1c
		addi base,state,0
		rlwinm r0,r4,3,0x8
		li r4,0x1c0d
		rlwimi offset,r7,30,0x01
		srw r4,r4,r0
		and offset,offset,r4
		bctr
/* modrm with opcode extension (non prefixed) */
8:		lwzx r4,r4,opreg
		mtctr r4
/* FIXME ? We continue fetching even if the opcode extension is undefined.
 * It shouldn't do any harm on real mode emulation anyway, and for ROM
 * BIOS emulation, we are supposed to read valid code.
 */
		bnl cr1,7b
/* modrm specifying memory without prefix */
9:		rlwimi adbase,r7,2,0x1c	# memory addressing mode computation
		rlwinm r3,r7,31,0x60
		lwzx r4,r3,adbase
		cmplwi cr1,r4,0x3090
		blt- cr1,4b		# displacement only addressing mode
10:		rlwinm. r0,r7,24,0,1	# three cases distinguished
		beq- cr1,15f		# an sib follows
		rlwinm r3,r4,30,0x1c	# 16bit/32bit/%si index/%di index
		cmpwi cr1,r3,8		# set cr1 as early as possible
		rlwinm r6,r4,26,0x1c	# base register
		lwbrx offset,state,r6	# load the base register
		beq cr0,14f		# no displacement
		cmpw cr2,r4,opcode	# check for ss as default base
		bgt cr0,12f		# byte offset
		beq cr1,11f		# 32 bit displacement
		NEXTWORD(r5)		# 16 bit displacement
		bgt cr1,13f		# d16(base,index)
/* d16(base) */
		add offset,offset,r5
		clrlwi offset,offset,16
		bgtctr cr2
		addi base,ssb,0
		bctr
/* d32(base) */
11:		NEXTDWORD(r5)
		add offset,offset,r5
		bgtctr cr2
		addi base,ssb,0
		bctr
/* 8 bit displacement */
12:		NEXTBYTE(r5)
		extsb r5,r5
		bgt cr1,13f
/* d8(base) */
		extsb r6,r4
		add offset,offset,r5
		ori r6,r6,0xffff
		and offset,offset,r6
		bgtctr cr2
		addi base,ssb,0
		bctr
/* d8(base,index) and d16(base,index) share this code ! */
13:		lhbrx r3,state,r3
		add offset,offset,r5
		add offset,offset,r3
		clrlwi offset,offset,16
		bgtctr cr2
		addi base,ssb,0
		bctr
/* no displacement: only indexed modes may use ss as default base */
14:		beqctr cr1		# 32 bit register indirect
		clrlwi offset,offset,16
		bltctr cr1		# 16 bit register indirect
/* (base,index) */
		lhbrx r3,state,r3	# 16 bit [{bp,bx}+{si,di}]
		cmpw cr2,r4,opcode	# check for ss as default base
		add offset,offset,r3
		clrlwi offset,offset,r3
		bgtctr+ cr2
		addi base,ssb,0
		bctr
/* sib modes, note that the size of the offset can be known from cr0 */
15:		NEXTBYTE(r7)			# get sib
		rlwinm r3,r7,31,0x1c		# index
		rlwinm offset,r7,2,0x1c		# base
		cmpwi cr1,r3,ESP		# has index ?
		bne cr0,18f			# base+d8/d32
		cmpwi offset,EBP
		beq 17f				# d32(,index,scale)
		xori r4,one,0xcc01		# build 0x0000cc00
		rlwnm r4,r4,offset,0,1		# 0 or 0xc0000000
		lwbrx offset,state,offset
		cmpw cr2,r4,opcode		# use ss ?
		beq- cr1,16f			# no index
/* (base,index,scale) */
		lwbrx r3,state,r3
		srwi r6,r7,6
		slw r3,r3,r6
		add offset,offset,r3
		bgtctr cr2
		addi base,ssb,0
		bctr
/* (base), in practice only (%esp) is coded this way */
16:		bgtctr cr2
		addi base,ssb,0
		bctr
/* d32(,index,scale) */
17:		NEXTDWORD(offset)
		beqctr- cr1			# no index: very unlikely
		lwbrx r3,state,r3
		srwi r6,r7,6
		slw r3,r3,r6
		add offset,offset,r3
		bctr
/* 8 or 32 bit displacement */
18:		xori r4,one,0xcc01		# build 0x0000cc00
		rlwnm r4,r4,offset,0,1		# 0 or 0xc0000000
		lwbrx offset,state,offset
		cmpw cr2,r4,opcode		# use ss ?
		bgt cr0,20f			# 8 bit offset
/* 32 bit displacement */
		NEXTDWORD(r5)
		beq- cr1,21f
/* d(base,index,scale) */
19:		lwbrx r3,state,r3
		add offset,offset,r5
		add offset,offset,r3
		bgtctr cr2
		addi base,ssb,0
		bctr
/* 8 bit displacement */
20:		NEXTBYTE(r5)
		extsb r5,r5
		bne+ cr1,19b
/* d(base), in practice base is %esp  */
21:		add offset,offset,r5
		bgtctr- cr2
		addi base,ssb,0
		bctr

/*
 *  Flag evaluation subroutines: they have not been written for performance
 * since they are not often used in practice. The rule of the game was to
 * write them with as few branches as possible.
 * The first routines eveluate either one or 2 (ZF and SF simultaneously)
 * flags and do not use r0 and r7.
 * The more complex routines (_eval_above, _eval_signed and _eval_flags)
 * call the former ones, using r0 as a return address save register and
 * r7 as a safe temporary.
 */

/*
 * _eval_sf_zf evaluates simultaneously SF and ZF unless ZF is already valid
 * and protected because it is possible, although it is exceptional, to have
 * SF and ZF set at the same time after a few instructions which may leave the
 * flags in this apparently inconsistent state: sahf, popf, iret and the few
 * (for now unimplemented) instructions which only affect ZF (lar, lsl, arpl,
 * cmpxchg8b). This also solves the obscure case of ZF set and PF clear.
 * On return: SF=cr6[0], ZF=cr6[2].
 */

_eval_sf_zf:	andis. r5,flags,ZF_PROTECT>>16
		rlwinm r3,flags,0,INCDEC_FIELD
		RES_SHIFT(r4)
		cntlzw r3,r3
		slw r4,result,r4
		srwi r5,r3,5			# ? use result : use op1
		rlwinm r3,r3,2,0x18
		oris flags,flags,(SF_IN_CR|SIGNED_IN_CR|ZF_IN_CR)>>16
		neg r5,r5			# mux result/op2
		slw r3,op2,r3
		and r4,r4,r5
		andc r3,r3,r5
		xoris flags,flags,(SIGNED_IN_CR)>>16
		bne- 1f				# 12 instructions between set
		or r3,r3,r4			# and test, good for folding
		cmpwi cr6,r3,0
		blr
1:		or. r3,r3,r4
		crmove SF,0
		blr

/*
 * _eval_cf may be called at any time, no other flag is affected.
 * On return: CF=cr4[0], r3= CF ? 0x100:0 = CF<<8.
 */
_eval_cf:	addc r3,flags,flags		# CF_IN to xer[ca]
		RES2CF(r4)			# get 8 or 16 bit carry
		subfe r3,result,op1		# generate PPC carry for
		CF_ROTCNT(r5)			# preceding operation
		addze r3,r4			# put carry into LSB
		CF_POL(r4,23)			# polarity & 0x100
		oris flags,flags,(CF_IN_CR|ABOVE_IN_CR)>>16
		rlwnm r3,r3,r5,23,23		# shift carry there
		xor r3,r3,r4			# CF <<8
		xoris flags,flags,(ABOVE_IN_CR)>>16
		cmplw cr4,one,r3		# sets cr4[0]
		blr

/*
 * eval_of returns the overflow flag in OF_STATE field, which will be
 * either 001 (OF clear) or 101 (OF set), is is only called when the two
 * low order bits of OF_STATE are not 01 (otherwise it will work but
 * it is an elaborate variant of a nop with a few registers destroyed)
 * The code multiplexes several sources in a branchless way, was fun to write.
 */
_eval_of:	GET_ADDSUB(r4)			# 0(add)/1(sub)
		rlwinm r3,flags,0,INCDEC_FIELD
		neg r4,r4			# 0(add)/-1(sub)
		eqv r5,result,op1		# result[]==op1[] (bit by bit)
		cntlzw r3,r3			# inc/dec
		xor r4,r4,op2			# true sign of op2
		oris r5,r5,0x0808		# bits to clear
		clrlwi r6,r3,31			# 0(inc)/1(dec)
		eqv r4,r4,op1			# op1[]==op2[] (bit by bit)
		add r6,op2,r6			# add 1 if dec
		rlwinm r3,r3,2,0x18		# incdec_shift
		andc r4,r4,r5			# arithmetic overflow
		slw r3,r6,r3			# shifted inc/dec result
		addis r3,r3,0x8000		# compare with 0x80000000
		ori r4,r4,0x0808		# bits to set
		cntlzw r3,r3			# 32 if inc/dec overflow
		OF_ROTCNT(r6)
		rlwimi r4,r3,18,0x00800000	# insert inc/dec overflow
		rlwimi flags,one,24,OF_STATE_MASK
		rlwnm r3,r4,r6,8,8		# get field
		rlwimi flags,r3,3,OF_VALUE	# insert OF
		blr

/*
 * _eval_pf will always be called when needed (complex but infrequent),
 * there are a few quirks for a branchless solution.
 * On return: PF=cr0[0], PF=MSB(r3)
 */
_eval_pf:	rlwinm r3,flags,0,INCDEC_FIELD
		rotrwi r4,op2,4			# from inc/dec
		rotrwi r5,result,4		# from result
		cntlzw r3,r3			# use result if 32
		xor r4,r4,op2
		xor r5,r5,result
		rlwinm r3,r3,26,0,0		# 32 becomes 0x80000000
		clrlwi r4,r4,28
		lis r6,0x9669			# constant to shift
		clrlwi r5,r5,28
		rlwnm r4,r6,r4,0,0		# parity from inc/dec
		rlwnm r5,r6,r5,0,0		# parity from result
		andc r4,r4,r3			# select which one
		and r5,r5,r3
		add. r3,r4,r5			# and test to simplify
		blr				# returns in r3 and cr0 set.

/*
 * _eval_af will always be called when needed (complex but infrequent):
 * - if after inc, af is set when 4 low order bits of op1 are 0
 * - if after dec, af is set when 4 low order bits of op1 are 1
 *   (or 0 after adding 1 as implemented here)
 * - if after add/sub/adc/sbb/cmp af is set from sum of 4 LSB of op1
 *    and 4 LSB of op2 (eventually complemented) plus carry in.
 * - other instructions leave AF undefined so the returned value is irrelevant.
 * Returned value must be masked with 0x10, since all other bits are undefined.
 * There branchless code is perhaps not the most efficient, but quite parallel.
 */
_eval_af:	rlwinm r3,flags,0,INCDEC_FIELD
		clrlwi r5,op2,28		# 4 LSB of op2
		addc r4,flags,flags		# carry_in
		GET_ADDSUB(r6)
		cntlzw r3,r3			# if inc/dec 16..23 else 32
		neg r6,r6			# add/sub
		clrlwi r4,r3,31			# if dec 1 else 0
		xor r5,r5,r6			# conditionally complement
		clrlwi r6,op1,28		# 4 LSB of op1
		add r4,op2,r4			# op2+(dec ? 1 : 0)
		clrlwi r4,r4,28			# 4 LSB of op2+(dec ? 1 : 0)
		adde r5,r6,r5			# op1+cy_in+(op2/~op2)
		cntlzw r4,r4			# 28..31 if not AF, 32 if set
		andc r5,r5,r3			# masked AF from add/sub...
		andc r4,r3,r4			# masked AF from inc/dec
		or r3,r4,r5
		blr

/*
 * _eval_above will only be called if ABOVE_IN_CR is not set.
 * On return: ZF=cr6[2], CF=cr4[0], ABOVE=cr4[1]
 */
_eval_above:	andis. r3,flags,ZF_IN_CR>>16
		mflr r0
		beql+ _eval_sf_zf
		andis. r3,flags,CF_IN_CR>>16
		beql+ _eval_cf
		mtlr r0
		oris flags,flags,ABOVE_IN_CR>>16
		crnor ABOVE,ZF,CF
		blr

/* _eval_signed may only be called when signed_in_cr is clear ! */
_eval_signed:	andis. r3,flags,SF_IN_CR>>16
		mflr r0
		beql+ _eval_sf_zf
/* SF_IN_CR and ZF_IN_CR are set, SIGNED_IN_CR is clear */
		rlwinm. r3,flags,5,0,1
		xoris flags,flags,(SIGNED_IN_CR|SF_IN_CR)>>16
		bngl+ _eval_of
		andis. r3,flags,OF_VALUE>>16
		mtlr r0
		crxor SLT,SF,OF
		crnor SGT,SLT,ZF
		blr

_eval_flags:	mflr r0
		bl _eval_cf
		li r7,2
		rlwimi r7,r3,24,CF86,CF86	# 2 if CF clear, 3 if set
		bl _eval_pf
		andis. r4,flags,SF_IN_CR>>16
		rlwimi r7,r3,32+PF-PF86,PF86,PF86
		bl _eval_af
		rlwimi r7,r3,0,AF86,AF86
		beql+ _eval_sf_zf
		mfcr r3
		rlwinm. r4,flags,5,0,1
		rlwimi r7,r3,0,DF86,SF86
		ZF2ZF86(r3,r7)
		bngl+ _eval_of
		mtlr r0
		lis r4,0x0004
		lwz r3,eflags(state)
		addi r4,r4,0x7000
		rlwimi r7,flags,17,OF86,OF86
		and r3,r3,r4
		or r3,r3,r7
		blr

/* Quite simple for real mode, input in r4, returns in r3. */
_segment_load:	lwz r5,vbase(state)
		rlwinm r3,r4,4,0xffff0		# segment selector * 16
		add r3,r3,r5
		blr

/* To allow I/O port virtualization if necessary, code for exception in r3,
port number in r4 */
_check_port:	lwz r5,ioperm(state)
		rlwinm r6,r4,29,0x1fff		# 0 to 8kB
		lis r0,0xffff
		lhbrx r5,r5,r6
		clrlwi r6,r4,29			# modulo 8
		rlwnm r0,r0,r3,0x0f		# 1, 3, or 0xf
		slw r0,r0,r6
		and. r0,r0,r5
		bne- complex
		blr
/*
 * Instructions are in approximate functional order:
 * 1) move, exchange, lea, push/pop, pusha/popa
 * 2) cbw/cwde/cwd/cdq, zero/sign extending moves, in/out
 * 3) arithmetic: add/sub/adc/sbb/cmp/inc/dec/neg
 * 4) logical: and/or/xor/test/not/bt/btc/btr/bts/bsf/bsr
 * 5) jump, call, ret
 * 6) string instructions and xlat
 * 7) rotate/shift/mul/div
 * 8) segment register, far jumps, calls and rets, interrupts
 * 9) miscellenaous (flags, bcd,...)
 */

#define MEM offset,base
#define REG opreg,state
#define SELECTORS 32
#define SELBASES 64

/* Immediate moves */
movb_imm_reg:	rlwinm opreg,opcode,2,28,29; lbz r3,1(eip)
		rlwimi opreg,opcode,30,31,31; lbzu opcode,2(eip)
		stbx r3,REG; GOTNEXT

movw_imm_reg:	lhz r3,1(eip); clrlslwi opreg,opcode,29,2; lbzu opcode,3(eip)
		sthx r3,REG; GOTNEXT

movl_imm_reg:	lwz r3,1(eip); clrlslwi opreg,opcode,29,2; lbzu opcode,5(eip)
		stwx r3,REG; GOTNEXT

movb_imm_mem:	lbz r0,1(eip); cmpwi opreg,0
		lbzu opcode,2(eip); bne- ud
		stbx r0,MEM; GOTNEXT

movw_imm_mem:	lhz r0,1(eip); cmpwi opreg,0
		lbzu opcode,3(eip); bne- ud
		sthx r0,MEM; GOTNEXT

movl_imm_mem:	lwz r0,1(eip); cmpwi opreg,0
		lbzu opcode,5(eip); bne- ud
		stwx r0,MEM; GOTNEXT

/* The special short form moves between memory and al/ax/eax */
movb_al_a32:	lwbrx offset,eip,one; lbz r0,AL(state); lbzu opcode,5(eip)
		stbx r0,MEM; GOTNEXT

movb_al_a16:	lhbrx offset,eip,one; lbz r0,AL(state); lbzu opcode,3(eip)
		stbx r0,MEM; GOTNEXT

movw_ax_a32:	lwbrx offset,eip,one; lhz r0,AX(state); lbzu opcode,5(eip)
		sthx r0,MEM; GOTNEXT

movw_ax_a16:	lhbrx offset,eip,one; lhz r0,AX(state); lbzu opcode,3(eip)
		sthx r0,MEM; GOTNEXT

movl_eax_a32:	lwbrx offset,eip,one; lwz r0,EAX(state); lbzu opcode,5(eip)
		stwx r0,MEM; GOTNEXT

movl_eax_a16:	lhbrx offset,eip,one; lwz r0,EAX(state); lbzu opcode,3(eip)
		stwx r0,MEM; GOTNEXT

movb_a32_al:	lwbrx offset,eip,one; lbzu opcode,5(eip); lbzx r0,MEM
		stb r0,AL(state); GOTNEXT

movb_a16_al:	lhbrx offset,eip,one; lbzu opcode,3(eip); lbzx r0,MEM
		stb r0,AL(state); GOTNEXT

movw_a32_ax:	lwbrx offset,eip,one; lbzu opcode,5(eip); lhzx r0,MEM
		sth r0,AX(state); GOTNEXT

movw_a16_ax:	lhbrx offset,eip,one; lbzu opcode,3(eip); lhzx r0,MEM
		sth r0,AX(state); GOTNEXT

movl_a32_eax:	lwbrx offset,eip,one; lbzu opcode,5(eip); lwzx r0,MEM
		stw r0,EAX(state); GOTNEXT

movl_a16_eax:	lhbrx offset,eip,one; lbzu opcode,3(eip); lwzx r0,MEM
		stw r0,EAX(state); GOTNEXT

/* General purpose move (all are exactly 4 instructions long) */
		.align 4
movb_reg_mem:	lbzx r0,REG
		NEXTBYTE(opcode)
		stbx r0,MEM
		GOTNEXT

movw_reg_mem:	lhzx r0,REG
		NEXTBYTE(opcode)
		sthx r0,MEM
		GOTNEXT

movl_reg_mem:	lwzx r0,REG
		NEXTBYTE(opcode)
		stwx r0,MEM
		GOTNEXT

movb_mem_reg:	lbzx r0,MEM
		NEXTBYTE(opcode)
		stbx r0,REG
		GOTNEXT

movw_mem_reg:	lhzx r0,MEM
		NEXTBYTE(opcode)
		sthx r0,REG
		GOTNEXT

movl_mem_reg:	lwzx r0,MEM
		NEXTBYTE(opcode)
		stwx r0,REG
		GOTNEXT

/* short form exchange ax/eax with register */
xchgw_ax_reg:	clrlslwi opreg,opcode,29,2
		lhz r3,AX(state)
		lhzx r4,REG
		sthx r3,REG
		sth r4,AX(state)
		NEXT

xchgl_eax_reg:	clrlslwi opreg,opcode,29,2
		lwz r3,EAX(state)
		lwzx r4,REG
		stwx r3,REG
		stw r4,EAX(state)
		NEXT

/* General exchange (unlocked!) */
xchgb_reg_mem:	lbzx r3,MEM
		lbzx r4,REG
		NEXTBYTE(opcode)
		stbx r3,REG
		stbx r4,MEM
		GOTNEXT

xchgw_reg_mem:	lhzx r3,MEM
		lhzx r4,REG
		sthx r3,REG
		sthx r4,MEM
		NEXT

xchgl_reg_mem:	lwzx r3,MEM
		lwzx r4,REG
		stwx r3,REG
		stwx r4,MEM
		NEXT

/* lea, one of the simplest instructions */
leaw:		cmpw base,state
		beq- ud
		sthbrx offset,REG
		NEXT

leal:		cmpw base,state
		beq- ud
		stwbrx offset,REG
		NEXT

/* Short form pushes and pops */
pushw_sp_reg:	li r3,SP
		lhbrx r4,state,r3
		clrlslwi opreg,opcode,29,2
		lhzx r0,REG
		addi r4,r4,-2
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		sthx r0,ssb,r4
		NEXT

pushl_sp_reg:	li r3,SP
		lhbrx r4,state,r3
		clrlslwi opreg,opcode,29,2
		lwzx r0,REG
		addi r4,r4,-4
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		stwx r0,ssb,r4
		NEXT

popw_sp_reg:	li r3,SP
		lhbrx r4,state,r3
		clrlslwi opreg,opcode,29,2
		lhzx r0,ssb,r4
		addi r4,r4,2		# order is important in case of pop sp
		sthbrx r4,state,r3
		sthx r0,REG
		NEXT

popl_sp_reg:	li r3,SP
		lhbrx r4,state,r3
		clrlslwi opreg,opcode,29,2
		lwzx r0,ssb,r4
		addi r4,r4,4
		sthbrx r4,state,r3
		stwx r0,REG
		NEXT

/* Push immediate */
pushw_sp_imm:	li r3,SP
		lhbrx r4,state,r3
		lhz r0,1(eip)
		addi r4,r4,-2
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		lbzu opcode,3(eip)
		sthx r0,ssb,r4
		GOTNEXT

pushl_sp_imm:	li r3,SP
		lhbrx r4,state,r3
		lwz r0,1(eip)
		addi r4,r4,-4
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		lbzu opcode,5(eip)
		stwx r0,ssb,r4
		GOTNEXT

pushw_sp_imm8:	li r3,SP
		lhbrx r4,state,r3
		lhz r0,1(eip)
		addi r4,r4,-2
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		lbzu opcode,2(eip)
		extsb r0,r0
		sthx r0,ssb,r4
		GOTNEXT

pushl_sp_imm8:	li r3,SP
		lhbrx r4,state,r3
		lhz r0,1(eip)
		addi r4,r4,-4
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		lbzu opcode,2(eip)
		extsb r0,r0
		stwx r0,ssb,r4
		GOTNEXT

/* General push/pop */
pushw_sp:	lhbrx r0,MEM
		li r3,SP
		lhbrx r4,state,r3
		addi r4,r4,-2
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		sthbrx r0,r4,ssb
		NEXT

pushl_sp:	lwbrx r0,MEM
		li r3,SP
		lhbrx r4,state,r3
		addi r4,r4,-4
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		stwbrx r0,r4,ssb
		NEXT

/* pop is an exception with 32 bit addressing modes, it is possible
to calculate wrongly the address when esp is used as base. But 16 bit
addressing modes are safe */

popw_sp_a16:	cmpw cr1,opreg,0	# first check the opcode
		li r3,SP
		lhbrx r4,state,r3
		bne- cr1,ud
		lhzx r0,ssb,r4
		addi r4,r4,2
		sthx r0,MEM
		sthbrx r4,state,r3
		NEXT

popl_sp_a16:	cmpw cr1,opreg,0
		li r3,SP
		lhbrx r4,state,r3
		bne- cr1,ud
		lwzx r0,ssb,r4
		addi r4,r4,2
		stwx r0,MEM
		sthbrx r4,state,r3
		NEXT

/* 32 bit addressing modes for pop not implemented for now. */
		.equ popw_sp_a32,unimpl
		.equ popl_sp_a32,unimpl

/* pusha/popa */
pushaw_sp:	li r3,SP
		li r0,8
		lhbrx r4,r3,state
		mtctr r0
		addi r5,state,-4
1:		addi r4,r4,-2
		lhzu r6,4(r5)
		clrlwi r4,r4,16
		sthx r6,ssb,r4
		bdnz 1b
		sthbrx r4,r3,state	# new sp
		NEXT

pushal_sp:	li r3,SP
		li r0,8
		lhbrx r4,r3,state
		mtctr r0
		addi r5,state,-4
1:		addi r4,r4,-4
		lwzu r6,4(r5)
		clrlwi r4,r4,16
		stwx r6,ssb,r4
		bdnz 1b
		sthbrx r4,r3,state	# new sp
		NEXT

popaw_sp:	li r3,SP
		li r0,8
		lhbrx r4,state,r3
		mtctr r0
		addi r5,state,32
1:		lhzx r6,ssb,r4
		addi r4,r4,2
		sthu r6,-4(r5)
		clrlwi r4,r4,16
		bdnz 1b
		sthbrx r4,r3,state	# updated sp
		NEXT

popal_sp:	li r3,SP
		lis r0,0xef00		# mask to skip esp
		lhbrx r4,state,r3
		addi r5,state,32
1:		add. r0,r0,r0
		lwzx r6,ssb,r4
		addi r4,r4,4
		stwu r6,-4(r5)
		clrlwi r4,r4,16
		blt 1b
		addi r6,r6,-4
		beq 2f
		addi r4,r4,4
		clrlwi r4,r4,16
		b 1b
2:		sthbrx r4,state,r3	# updated sp
		NEXT

/* Moves with zero or sign extension: first the special cases */
cbw:		lbz r3,AL(state)
		extsb r3,r3
		sthbrx r3,AX,state
		NEXT

cwde:		lhbrx r3,AX,state
		extsh r3,r3
		stwbrx r3,EAX,state
		NEXT

cwd:		lbz r3,AH(state)
		extsb r3,r3
		srwi r3,r3,8			# get sign bits
		sth r3,DX(state)
		NEXT

cdq:		lwbrx r3,EAX,state
		srawi r3,r3,31
		stw r3,EDX(state)		# byte order unimportant !
		NEXT

/* The move with zero or sign extension are special since the source
and destination are not the same size. The register describing the destination
is modified to take this into account. */

movsbw:		lbzx r3,MEM
		rlwimi opreg,opreg,4,0x10
		extsb r3,r3
		rlwinm opreg,opreg,0,0x1c
		sthbrx r3,REG
		NEXT

movsbl:		lbzx r3,MEM
		rlwimi opreg,opreg,4,0x10
		extsb r3,r3
		rlwinm opreg,opreg,0,0x1c
		stwbrx r3,REG
		NEXT

		.equ movsww, movw_mem_reg

movswl:		lhbrx r3,MEM
		extsh r3,r3
		stwbrx r3,REG
		NEXT

movzbw:		lbzx r3,MEM
		rlwimi opreg,opreg,4,0x10
		rlwinm opreg,opreg,0,0x1c
		sthbrx r3,REG
		NEXT

movzbl:		lbzx r3,MEM
		rlwimi opreg,opreg,4,0x10
		rlwinm opreg,opreg,0,0x1c
		stwbrx r3,REG
		NEXT

		.equ movzww, movw_mem_reg

movzwl:		lhbrx r3,MEM
		stwbrx r3,REG
		NEXT

/* Byte swapping */
bswap:		clrlslwi opreg,opcode,29,2	# extract reg from opcode
		lwbrx r0,REG
		stwx r0,REG
		NEXT

/* Input/output */
inb_port_al:	NEXTBYTE(r4)
		b 1f
inb_dx_al:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_inb
		bl _check_port
		lwz r3,iobase(state)
		lbzx r5,r4,r3
		eieio
		stb r5,AL(state)
		NEXT

inw_port_ax:	NEXTBYTE(r4)
		b 1f
inw_dx_ax:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_inw
		bl _check_port
		lwz r3,iobase(state)
		lhzx r5,r4,r3
		eieio
		sth r5,AX(state)
		NEXT

inl_port_eax:	NEXTBYTE(r4)
		b 1f
inl_dx_eax:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_inl
		bl _check_port
		lwz r3,iobase(state)
		lwzx r5,r4,r3
		eieio
		stw r5,EAX(state)
		NEXT

outb_al_port:	NEXTBYTE(r4)
		b 1f
outb_al_dx:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_outb
		bl _check_port
		lwz r3,iobase(state)
		lbz r5,AL(state)
		stbx r5,r4,r3
		eieio
		NEXT

outw_ax_port:	NEXTBYTE(r4)
		b 1f
outw_ax_dx:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_outw
		bl _check_port
		lwz r3,iobase(state)
		lhz r5,AX(state)
		sthx r5,r4,r3
		eieio
		NEXT

outl_eax_port:	NEXTBYTE(r4)
		b 1f
outl_eax_dx:	li r4,DX
		lhbrx r4,r4,state
1:		li r3,code_outl
		bl _check_port
		lwz r4,iobase(state)
		lwz r5,EAX(state)
		stwx r5,r4,r3
		eieio
		NEXT

/* Macro used for add and sub */
#define ARITH(op,fl) \
op##b_reg_mem:	lbzx op1,MEM; SET_FLAGS(fl(B)); lbzx op2,REG; \
		op result,op1,op2; \
		stbx result,MEM; NEXT; \
op##w_reg_mem:	lhbrx op1,MEM; SET_FLAGS(fl(W)); lhbrx op2,REG; \
		op result,op1,op2; \
		sthbrx result,MEM; NEXT; \
op##l_reg_mem:	lwbrx op1,MEM; SET_FLAGS(fl(L)); lwbrx op2,REG; \
		op result,op1,op2; \
		stwbrx result,MEM; NEXT; \
op##b_mem_reg:	lbzx op2,MEM; SET_FLAGS(fl(B)); lbzx op1,REG; \
		op result,op1,op2; \
		stbx result,REG; NEXT; \
op##w_mem_reg:	lhbrx op2,MEM; SET_FLAGS(fl(W)); lhbrx op1,REG; \
		op result,op1,op2; \
		sthbrx result,REG; NEXT; \
op##l_mem_reg:	lwbrx op2,MEM; SET_FLAGS(fl(L)); lwbrx op1,REG; \
		op result,op1,op2; \
		stwbrx result,REG; NEXT; \
op##b_imm_al:	addi base,state,0; li offset,AL; \
op##b_imm:	lbzx op1,MEM; SET_FLAGS(fl(B)); lbz op2,1(eip); \
		op result,op1,op2; \
		lbzu opcode,2(eip); \
		stbx result,MEM; GOTNEXT; \
op##w_imm_ax:	addi base,state,0; li offset,AX; \
op##w_imm:	lhbrx op1,MEM; SET_FLAGS(fl(W)); lhbrx op2,eip,one; \
		op result,op1,op2; \
		lbzu opcode,3(eip); \
		sthbrx result,MEM; GOTNEXT; \
op##w_imm8:	lbz op2,1(eip); SET_FLAGS(fl(W)); lhbrx op1,MEM; \
		extsb op2,op2; clrlwi op2,op2,16; \
		op result,op1,op2; \
		lbzu opcode,2(eip); \
		sthbrx result,MEM; GOTNEXT; \
op##l_imm_eax:	addi base,state,0; li offset,EAX; \
op##l_imm:	lwbrx op1,MEM; SET_FLAGS(fl(L)); lwbrx op2,eip,one; \
		op result,op1,op2; lbzu opcode,5(eip); \
		stwbrx result,MEM; GOTNEXT; \
op##l_imm8:	lbz op2,1(eip); SET_FLAGS(fl(L)); lwbrx op1,MEM; \
		extsb op2,op2; lbzu opcode,2(eip); \
		op result,op1,op2; \
		stwbrx result,MEM; GOTNEXT

		ARITH(add, FLAGS_ADD)
		ARITH(sub, FLAGS_SUB)

#define adc(result, op1, op2) adde result,op1,op2
#define sbb(result, op1, op2) subfe result,op2,op1

#define ARITH_WITH_CARRY(op, fl) \
op##b_reg_mem: 	lbzx op1,MEM; bl carryfor##op; lbzx op2,REG; \
		ADD_FLAGS(fl(B)); op(result, op1, op2); \
		stbx result,MEM; NEXT; \
op##w_reg_mem:	lhbrx op1,MEM; bl carryfor##op; lhbrx op2,REG; \
		ADD_FLAGS(fl(W)); op(result, op1, op2); \
		sthbrx result,MEM; NEXT; \
op##l_reg_mem:	lwbrx op1,MEM; bl carryfor##op; lwbrx op2,REG; \
		ADD_FLAGS(fl(L)); op(result, op1, op2); \
		stwbrx result,MEM; NEXT; \
op##b_mem_reg:	lbzx op1,MEM; bl carryfor##op; lbzx op2,REG; \
		ADD_FLAGS(fl(B)); op(result, op1, op2); \
		stbx result,REG; NEXT; \
op##w_mem_reg:	lhbrx op1,MEM; bl carryfor##op; lhbrx op2,REG; \
		ADD_FLAGS(fl(W)); op(result, op1, op2); \
		sthbrx result,REG; NEXT; \
op##l_mem_reg:	lwbrx op1,MEM; bl carryfor##op; lwbrx op2,REG; \
		ADD_FLAGS(fl(L)); op(result, op1, op2); \
		stwbrx result,REG; NEXT; \
op##b_imm_al:	addi base,state,0; li offset,AL; \
op##b_imm:	lbzx op1,MEM; bl carryfor##op; lbz op2,1(eip); \
		ADD_FLAGS(fl(B)); lbzu opcode,2(eip); op(result, op1, op2); \
		stbx result,MEM; GOTNEXT; \
op##w_imm_ax:	addi base,state,0; li offset,AX; \
op##w_imm:	lhbrx op1,MEM; bl carryfor##op; lhbrx op2,eip,one; \
		ADD_FLAGS(fl(W)); lbzu opcode,3(eip); op(result, op1, op2); \
		sthbrx result,MEM; GOTNEXT; \
op##w_imm8:	lbz op2,1(eip); bl carryfor##op; lhbrx op1,MEM; \
		extsb op2,op2; ADD_FLAGS(fl(W)); clrlwi op2,op2,16; \
		lbzu opcode,2(eip); op(result, op1, op2); \
		sthbrx result,MEM; GOTNEXT; \
op##l_imm_eax:	addi base,state,0; li offset,EAX; \
op##l_imm:	lwbrx op1,MEM; bl carryfor##op; lwbrx op2,eip,one; \
		ADD_FLAGS(fl(L)); lbzu opcode,5(eip); op(result, op1, op2); \
		stwbrx result,MEM; GOTNEXT; \
op##l_imm8:	lbz op2,1(eip); SET_FLAGS(fl(L)); lwbrx op1,MEM; \
		extsb op2,op2; lbzu opcode,2(eip); \
		op(result, op1, op2); \
		stwbrx result,MEM; GOTNEXT

carryforadc:	addc r3,flags,flags		# CF_IN to xer[ca]
		RES2CF(r4)			# get 8 or 16 bit carry
		subfe r3,result,op1		# generate PPC carry for
		CF_ROTCNT(r5)			# preceding operation
		addze r3,r4			# 32 bit carry in LSB
		CF_POL(r4,23)			# polarity
		rlwnm r3,r3,r5,0x100		# shift carry there
		xor flags,r4,r3			# CF86 ? 0x100 : 0
		addic r4,r3,0xffffff00		# set xer[ca]
		rlwinm flags,r3,23,CF_IN
		blr

		ARITH_WITH_CARRY(adc, FLAGS_ADD)

/* for sbb the input carry must be the complement of the x86 carry */
carryforsbb:	addc r3,flags,flags		# CF_IN to xer[ca]
		RES2CF(r4)			# 8/16 bit carry from result
		subfe r3,result,op1
		CF_ROTCNT(r5)
		addze r3,r4
		CF_POL(r4,23)
		rlwnm r3,r3,r5,0x100
		eqv flags,r4,r3			# CF86 ? 0xfffffeff:0xffffffff
		addic r4,r3,1			# set xer[ca]
		rlwinm flags,r3,23,CF_IN	# keep only the carry
		blr

		ARITH_WITH_CARRY(sbb, FLAGS_SBB)

cmpb_reg_mem:	lbzx op1,MEM
		SET_FLAGS(FLAGS_CMP(B))
		lbzx op2,REG
		extsb r3,op1
		cmplw cr4,op1,op2
		extsb r4,op2
		sub result,op1,op2
		cmpw cr6,r3,r4
		NEXT

cmpw_reg_mem:	lhbrx op1,MEM
		SET_FLAGS(FLAGS_CMP(W))
		lhbrx op2,REG
		extsh r3,op1
		cmplw cr4,op1,op2
		extsh r4,op2
		sub result,op1,op2
		cmpw cr6,r3,r4
		NEXT

cmpl_reg_mem:	lwbrx op1,MEM
		SET_FLAGS(FLAGS_CMP(L))
		lwbrx op2,REG
		cmplw cr4,op1,op2
		sub result,op1,op2
		cmpw cr6,op1,op2
		NEXT

cmpb_mem_reg:	lbzx op2,MEM
		SET_FLAGS(FLAGS_CMP(B))
		lbzx op1,REG
		extsb r4,op2
		cmplw cr4,op1,op2
		extsb r3,op1
		sub result,op1,op2
		cmpw cr6,r3,r4
		NEXT

cmpw_mem_reg:	lhbrx op2,MEM
		SET_FLAGS(FLAGS_CMP(W))
		lhbrx op1,REG
		extsh r4,op2
		cmplw cr4,op1,op2
		extsh r3,op1
		sub result,op1,op2
		cmpw cr6,r3,r4
		NEXT

cmpl_mem_reg:	lwbrx op2,MEM
		SET_FLAGS(FLAGS_CMP(L))
		lwbrx op1,REG
		cmpw cr6,op1,op2
		sub result,op1,op2
		cmplw cr4,op1,op2
		NEXT

cmpb_imm_al:	addi base,state,0
		li offset,AL
cmpb_imm:	lbzx op1,MEM
		SET_FLAGS(FLAGS_CMP(B))
		lbz op2,1(eip)
		extsb r3,op1
		cmplw cr4,op1,op2
		lbzu opcode,2(eip)
		extsb r4,op2
		sub result,op1,op2
		cmpw cr6,r3,r4
		GOTNEXT

cmpw_imm_ax:	addi base,state,0
		li offset,AX
cmpw_imm:	lhbrx op1,MEM
		SET_FLAGS(FLAGS_CMP(W))
		lhbrx op2,eip,one
		extsh r3,op1
		cmplw cr4,op1,op2
		lbzu opcode,3(eip)
		extsh r4,op2
		sub result,op1,op2
		cmpw cr6,r3,r4
		GOTNEXT

cmpw_imm8:	lbz op2,1(eip)
		SET_FLAGS(FLAGS_CMP(W))
		lhbrx op1,MEM
		extsb r4,op2
		extsh r3,op1
		lbzu opcode,2(eip)
		clrlwi op2,r4,16
		cmpw cr6,r3,r4
		sub result,op1,op2
		cmplw cr4,op1,op2
		GOTNEXT

cmpl_imm_eax:	addi base,state,0
		li offset,EAX
cmpl_imm:	lwbrx op1,MEM
		SET_FLAGS(FLAGS_CMP(L))
		lwbrx op2,eip,one
		cmpw cr6,op1,op2
		lbzu opcode,5(eip)
		sub result,op1,op2
		cmplw cr4,op1,op2
		GOTNEXT

cmpl_imm8:	lbz op2,1(eip)
		SET_FLAGS(FLAGS_CMP(L))
		lwbrx op1,MEM
		extsb op2,op2
		lbzu opcode,2(eip)
		cmpw cr6,op1,op2
		sub result,op1,op2
		cmplw cr4,op1,op2
		GOTNEXT

/* Increment and decrement */
incb:		lbzx op2,MEM
		INC_FLAGS(B)
		addi op2,op2,1
		stbx op2,MEM
		NEXT

incw_reg:	clrlslwi opreg,opcode,29,2	# extract reg from opcode
		lhbrx op2,REG
		INC_FLAGS(W)
		addi op2,op2,1
		sthbrx op2,REG
		NEXT

incw:		lhbrx op2,MEM
		INC_FLAGS(W)
		addi op2,op2,1
		sthbrx op2,MEM
		NEXT

incl_reg:	clrlslwi opreg,opcode,29,2
		lwbrx op2,REG
		INC_FLAGS(L)
		addi op2,op2,1
		sthbrx op2,REG
		NEXT

incl:		lwbrx op2,MEM
		INC_FLAGS(L)
		addi op2,op2,1
		stwbrx op2,MEM
		NEXT

decb:		lbzx op2,MEM
		DEC_FLAGS(B)
		addi op2,op2,-1
		stbx op2,MEM
		NEXT

decw_reg:	clrlslwi opreg,opcode,29,2	# extract reg from opcode
		lhbrx op2,REG
		DEC_FLAGS(W)
		addi op2,op2,-1
		sthbrx op2,REG
		NEXT

decw:		lhbrx op2,MEM
		DEC_FLAGS(W)
		addi op2,op2,-1
		sthbrx op2,MEM
		NEXT

decl_reg:	clrlslwi opreg,opcode,29,2
		lwbrx op2,REG
		DEC_FLAGS(L)
		addi op2,op2,-1
		sthbrx op2,REG
		NEXT

decl:		lwbrx op2,MEM
		DEC_FLAGS(L)
		addi op2,op2,-1
		stwbrx op2,MEM
		NEXT

negb:		lbzx op2,MEM
		SET_FLAGS(FLAGS_SUB(B))
		neg result,op2
		li op1,0
		stbx result,MEM
		NEXT

negw:		lhbrx op2,MEM
		SET_FLAGS(FLAGS_SUB(W))
		neg result,op2
		li op1,0
		sthbrx r0,MEM
		NEXT

negl:		lwbrx op2,MEM
		SET_FLAGS(FLAGS_SUB(L))
		subfic result,op2,0
		li op1,0
		stwbrx result,MEM
		NEXT

/* Macro used to generate code for OR/AND/XOR */
#define LOGICAL(op) \
op##b_reg_mem:	lbzx op1,MEM; SET_FLAGS(FLAGS_LOG(B)); lbzx op2,REG; \
		op result,op1,op2; \
		stbx result,MEM; NEXT; \
op##w_reg_mem:	lhbrx op1,MEM; SET_FLAGS(FLAGS_LOG(W)); lhbrx op2,REG; \
		op result,op1,op2; \
		sthbrx result,MEM; NEXT; \
op##l_reg_mem:	lwbrx op1,MEM; SET_FLAGS(FLAGS_LOG(L)); lwbrx op2,REG; \
		op result,op1,op2; \
		stwbrx result,MEM; NEXT; \
op##b_mem_reg:	lbzx op1,MEM; SET_FLAGS(FLAGS_LOG(B)); lbzx op2,REG; \
		op result,op1,op2; \
		stbx result,REG; NEXT; \
op##w_mem_reg:	lhbrx op2,MEM; SET_FLAGS(FLAGS_LOG(W)); lhbrx op1,REG; \
		op result,op1,op2; \
		sthbrx result,REG; NEXT; \
op##l_mem_reg:	lwbrx op2,MEM; SET_FLAGS(FLAGS_LOG(L)); lwbrx op1,REG; \
		op result,op1,op2; \
		stwbrx result,REG; NEXT; \
op##b_imm_al:	addi base,state,0; li offset,AL; \
op##b_imm:	lbzx op1,MEM; SET_FLAGS(FLAGS_LOG(B)); lbz op2,1(eip); \
		op result,op1,op2; lbzu opcode,2(eip); \
		stbx result,MEM; GOTNEXT; \
op##w_imm_ax:	addi base,state,0; li offset,AX; \
op##w_imm:	lhbrx op1,MEM; SET_FLAGS(FLAGS_LOG(W)); lhbrx op2,eip,one; \
		op result,op1,op2; lbzu opcode,3(eip); \
		sthbrx result,MEM; GOTNEXT; \
op##w_imm8:	lbz op2,1(eip); SET_FLAGS(FLAGS_LOG(W)); lhbrx op1,MEM; \
		extsb op2,op2; lbzu opcode,2(eip); \
		op result,op1,op2; \
		sthbrx result,MEM; GOTNEXT; \
op##l_imm_eax:	addi base,state,0; li offset,EAX; \
op##l_imm:	lwbrx op1,MEM; SET_FLAGS(FLAGS_LOG(L)); lwbrx op2,eip,one; \
		op result,op1,op2; lbzu opcode,5(eip); \
		stwbrx result,MEM; GOTNEXT; \
op##l_imm8:	lbz op2,1(eip); SET_FLAGS(FLAGS_LOG(L)); lwbrx op1,MEM; \
		extsb op2,op2; lbzu opcode,2(eip); \
		op result,op1,op2; \
		stwbrx result,MEM; GOTNEXT

		LOGICAL(or)

		LOGICAL(and)

		LOGICAL(xor)

testb_reg_mem:	lbzx op1,MEM
		SET_FLAGS(FLAGS_TEST(B))
		lbzx op2,REG
		and result,op1,op2
		extsb r3,result
		cmpwi cr6,r3,0
		NEXT

testw_reg_mem:	lhbrx op1,MEM
		SET_FLAGS(FLAGS_TEST(W))
		lhbrx op2,REG
		and result,op1,op2
		extsh r3,result
		cmpwi cr6,r3,0
		NEXT

testl_reg_mem:	lwbrx r3,MEM
		SET_FLAGS(FLAGS_TEST(L))
		lwbrx r4,REG
		and result,op1,op2
		cmpwi cr6,result,0
		NEXT

testb_imm_al:	addi base,state,0
		li offset,AL
testb_imm:	lbzx op1,MEM
		SET_FLAGS(FLAGS_TEST(B))
		lbz op2,1(eip)
		and result,op1,op2
		lbzu opcode,2(eip)
		extsb r3,result
		cmpwi cr6,r3,0
		GOTNEXT

testw_imm_ax:	addi base,state,0
		li offset,AX
testw_imm:	lhbrx op1,MEM
		SET_FLAGS(FLAGS_TEST(W))
		lhbrx op2,eip,one
		and result,op1,op2
		lbzu opcode,3(eip)
		extsh r3,result
		cmpwi cr6,r3,0
		GOTNEXT

testl_imm_eax:	addi base,state,0
		li offset,EAX
testl_imm:	lwbrx op1,MEM
		SET_FLAGS(FLAGS_TEST(L))
		lwbrx op2,eip,one
		and result,r3,r4
		lbzu opcode,5(eip)
		cmpwi cr6,result,0
		GOTNEXT

/* Not does not affect flags */
notb:		lbzx r3,MEM
		xori r3,r3,255
		stbx r3,MEM
		NEXT

notw:		lhzx r3,MEM
		xori r3,r3,65535
		sthx r3,MEM
		NEXT

notl:		lwzx r3,MEM
		not r3,r3
		stwx r3,MEM
		NEXT

boundw:		lhbrx r4,REG
		li r3,code_bound
		lhbrx r5,MEM
		addi offset,offset,2
		extsh r4,r4
		lhbrx r6,MEM
		extsh r5,r5
		cmpw r4,r5
		extsh r6,r6
		blt- complex
		cmpw r4,r6
		ble+ nop
		b complex

boundl:		lwbrx r4,REG
		li r3,code_bound
		lwbrx r5,MEM
		addi offset,offset,4
		lwbrx r6,MEM
		cmpw r4,r5
		blt- complex
		cmpw r4,r6
		ble+ nop
		b complex

/* Bit test and modify instructions */

/* Common routine: bit index in op2, returns memory value in r3, mask in op2,
and of mask and value in op1. CF flag is set as with 32 bit add when bit is
non zero since result (which is cleared) will be less than op1, and in cr4,
all other flags are undefined from Intel doc. Here OF and SF are cleared
and ZF is set as a side effect of result being cleared.  */
_setup_bitw:	cmpw base,state
		SET_FLAGS(FLAGS_BTEST)
		extsh op2,op2
		beq- 1f
		srawi r4,op2,4
		add offset,offset,r4
1:		clrlwi op2,op2,28		# true bit index
		lhbrx r3,MEM
		slw op2,one,op2			# build mask
		li result,0			# implicitly sets CF
		and op1,r3,op2			# if result<op1
		cmplw cr4,result,op1		# sets CF in cr4
		blr

_setup_bitl:	cmpw base,state
		SET_FLAGS(FLAGS_BTEST)
		beq- 1f
		srawi r4,op2,5
		add offset,offset,r4
1:		lwbrx r3,MEM
		rotlw op2,one,op2		# build mask
		li result,0
		and op1,r3,op2
		cmplw cr4,result,op1
		blr

/* Immediate forms bit tests are not frequent since logical are often faster */
btw_imm:	NEXTBYTE(op2)
		b 1f
btw_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitw
		NEXT

btl_imm:	NEXTBYTE(op2)
		b 1f
btl_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitl
		NEXT

btcw_imm:	NEXTBYTE(op2)
		b 1f
btcw_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitw
		xor r3,r3,op2
		sthbrx r3,MEM
		NEXT

btcl_imm:	NEXTBYTE(op2)
		b 1f
btcl_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitl
		xor r3,r3,op2
		stwbrx result,MEM
		NEXT

btrw_imm:	NEXTBYTE(op2)
		b 1f
btrw_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitw
		andc r3,r3,op2
		sthbrx r3,MEM
		NEXT

btrl_imm:	NEXTBYTE(op2)
		b 1f
btrl_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitl
		andc r3,r3,op2
		stwbrx r3,MEM
		NEXT

btsw_imm:	NEXTBYTE(op2)
		b 1f
btsw_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitw
		or r3,r3,op2
		sthbrx r3,MEM
		NEXT

btsl_imm:	NEXTBYTE(op2)
		b 1f
btsl_reg_mem:	lhbrx op2,REG
1:		bl _setup_bitl
		or r3,r3,op2
		stwbrx r3,MEM
		NEXT

/* Bit string search instructions, only ZF is defined after these, and the
result value is not defined when the bit field is zero. */
bsfw:		lhbrx result,MEM
		SET_FLAGS(FLAGS_BSRCH(W))
		neg r3,result
		cmpwi cr6,result,0		# sets ZF
		and r3,r3,result		# keep only LSB
		cntlzw r3,r3
		subfic r3,r3,31
		sthbrx r3,REG
		NEXT

bsfl:		lwbrx result,MEM
		SET_FLAGS(FLAGS_BSRCH(L))
		neg r3,result
		cmpwi cr6,result,0		# sets ZF
		and r3,r3,result		# keep only LSB
		cntlzw r3,r3
		subfic r3,r3,31
		stwbrx r3,REG
		NEXT

bsrw:		lhbrx result,MEM
		SET_FLAGS(FLAGS_BSRCH(W))
		cntlzw r3,result
		cmpwi cr6,result,0
		subfic r3,r3,31
		sthbrx r3,REG
		NEXT

bsrl:		lwbrx result,MEM
		SET_FLAGS(FLAGS_BSRCH(L))
		cntlzw r3,result
		cmpwi cr6,result,0
		subfic r3,r3,31
		stwbrx r3,REG
		NEXT

/* Unconditional jumps, first the indirect than relative */
jmpw:		lhbrx eip,MEM
		lbzux opcode,eip,csb
		GOTNEXT

jmpl:		lwbrx eip,MEM
		lbzux opcode,eip,csb
		GOTNEXT

sjmp_w:		lbz r3,1(eip)
		sub eip,eip,csb
		addi eip,eip,2			# EIP after instruction
		extsb r3,r3
		add eip,eip,r3
		clrlwi eip,eip,16		# module 64k
		lbzux opcode,eip,csb
		GOTNEXT

jmp_w:		lhbrx r3,eip,one		# eip now off by 3
		sub eip,eip,csb
		addi r3,r3,3			# compensate
		add eip,eip,r3
		clrlwi eip,eip,16
		lbzux opcode,eip,csb
		GOTNEXT

sjmp_l:		lbz r3,1(eip)
		addi eip,eip,2
		extsb r3,r3
		lbzux opcode,eip,r3
		GOTNEXT

jmp_l:		lwbrx r3,eip,one		# Simple
		addi eip,eip,5
		lbzux opcode,eip,r3
		GOTNEXT

/*  The conditional jumps: although it should not happen,
byte relative jumps (sjmp) may wrap around in 16 bit mode */

#define NOTTAKEN_S lbzu opcode,2(eip); GOTNEXT
#define NOTTAKEN_W lbzu opcode,3(eip); GOTNEXT
#define NOTTAKEN_L lbzu opcode,5(eip); GOTNEXT

#define CONDJMP(cond, eval, flag) \
sj##cond##_w:	EVAL_##eval; bt flag,sjmp_w; NOTTAKEN_S; \
j##cond##_w:	EVAL_##eval; bt flag,jmp_w; NOTTAKEN_W; \
sj##cond##_l:	EVAL_##eval; bt flag,sjmp_l; NOTTAKEN_S; \
j##cond##_l:	EVAL_##eval; bt flag,jmp_l; NOTTAKEN_L; \
sjn##cond##_w:	EVAL_##eval; bf flag,sjmp_w; NOTTAKEN_S; \
jn##cond##_w:	EVAL_##eval; bf flag,jmp_w; NOTTAKEN_W; \
sjn##cond##_l:	EVAL_##eval; bf flag,sjmp_l; NOTTAKEN_S; \
jn##cond##_l:	EVAL_##eval; bf flag,jmp_l; NOTTAKEN_L

		CONDJMP(o, OF, OF)
		CONDJMP(c, CF, CF)
		CONDJMP(z, ZF, ZF)
		CONDJMP(a, ABOVE, ABOVE)
		CONDJMP(s, SF, SF)
		CONDJMP(p, PF, PF)
		CONDJMP(g, SIGNED, SGT)
		CONDJMP(l, SIGNED, SLT)

jcxz_w:		lhz r3,CX(state); cmpwi r3,0; beq- sjmp_w; NOTTAKEN_S
jcxz_l:		lhz r3,CX(state); cmpwi r3,0; beq- sjmp_l; NOTTAKEN_S
jecxz_w:	lwz r3,ECX(state); cmpwi r3,0; beq- sjmp_w; NOTTAKEN_S
jecxz_l:	lwz r3,ECX(state); cmpwi r3,0; beq- sjmp_l; NOTTAKEN_S

/* Note that loop is somewhat strange, the data size attribute gives
the size of eip, and the address size whether the counter is cx or ecx.
This is the same for jcxz/jecxz. */

loopw_w:	li opreg,CX
		lhbrx r0,REG
		sub. r0,r0,one
		sthbrx r0,REG
		bne+ sjmp_w
		NOTTAKEN_S

loopl_w:	li opreg,ECX
		lwbrx r0,REG
		sub. r0,r0,one
		stwbrx r0,REG
		bne+ sjmp_w
		NOTTAKEN_S

loopw_l:	li opreg,CX
		lhbrx r0,REG
		sub. r0,r0,one
		sthbrx r0,REG
		bne+ sjmp_l
		NOTTAKEN_S

loopl_l:	li opreg,ECX
		lwbrx r0,REG
		sub. r0,r0,one
		stwbrx r0,REG
		bne+ sjmp_l
		NOTTAKEN_S

loopzw_w:	li opreg,CX
		lhbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		sthbrx r0,REG
		bf ZF,1f
		bne+ sjmp_w
1:		NOTTAKEN_S

loopzl_w:	li opreg,ECX
		lwbrx r0,REG
		EVAL_ZF
		sub. r3,r3,one
		stwbrx r3,REG
		bf ZF,1f
		bne+ sjmp_w
1:		NOTTAKEN_S

loopzw_l:	li opreg,CX
		lhbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		sthbrx r0,REG
		bf ZF,1f
		bne+ sjmp_l
1:		NOTTAKEN_S

loopzl_l:	li opreg,ECX
		lwbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		stwbrx r0,REG
		bf ZF,1f
		bne+ sjmp_l
1:		NOTTAKEN_S

loopnzw_w:	li opreg,CX
		lhbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		sthbrx r0,REG
		bt ZF,1f
		bne+ sjmp_w
1:		NOTTAKEN_S

loopnzl_w:	li opreg,ECX
		lwbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		stwbrx r0,REG
		bt ZF,1f
		bne+ sjmp_w
1:		NOTTAKEN_S

loopnzw_l:	li opreg,CX
		lhbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		sthbrx r0,REG
		bt ZF,1f
		bne+ sjmp_l
1:		NOTTAKEN_S

loopnzl_l:	li opreg,ECX
		lwbrx r0,REG
		EVAL_ZF
		sub. r0,r0,one
		stwbrx r0,REG
		bt ZF,1f
		bne+ sjmp_l
1:		NOTTAKEN_S

/* Memory indirect calls are rare enough to limit code duplication */
callw_sp_mem:	lhbrx r3,MEM
		sub r4,eip,csb
		addi r4,r4,1			# r4 is now return address
		b 1f
		.equ calll_sp_mem, unimpl

callw_sp:	lhbrx r3,eip,one
		sub r4,eip,csb
		addi r4,r4,3			# r4 is return address
		add r3,r4,r3
1:		clrlwi eip,r3,16
		li r5,SP
		lhbrx r6,state,r5		# get sp
		addi r6,r6,-2
		lbzux opcode,eip,csb
		sthbrx r6,state,r5		# update sp
		clrlwi r6,r6,16
		sthbrx r4,ssb,r6		# push return address
		GOTNEXT
		.equ calll_sp, unimpl

retw_sp_imm:	li opreg,SP
		lhbrx r4,REG
		lhbrx r6,eip,one
		addi r5,r4,2
		lhbrx eip,ssb,r4
		lbzux opcode,eip,csb
		add r5,r5,r6
		sthbrx r5,REG
		GOTNEXT

		.equ retl_sp_imm, unimpl

retw_sp:	li opreg,SP
		lhbrx r4,REG
		addi r5,r4,2
		lhbrx eip,ssb,r4
		lbzux opcode,eip,csb
		sthbrx r5,REG
		GOTNEXT

		.equ retl_sp, unimpl

/* Enter is a mess, and the description in Intel documents is actually wrong
 * in most revisions (all PPro/PII I have but the old Pentium is Ok) !
 */

enterw_sp:	lhbrx r0,eip,one		# Stack space to allocate
		li opreg,SP
		lhbrx r3,REG			# SP
		li r7,BP
		lbzu r4,3(eip)			# nesting level
		addi r3,r3,-2
		lhbrx r5,state,r7		# Original BP
		clrlwi r3,r3,16
		sthbrx r5,ssb,r3		# Push BP
		andi. r4,r4,31			# modulo 32 and test
		mr r6,r3			# Save frame pointer to temp
		beq 3f
		mtctr r4			# iterate level-1 times
		b 2f
1:		addi r5,r5,-2			# copy list of frame pointers
		clrlwi r5,r5,16
		lhzx r4,ssb,r5
		addi r3,r3,-2
		clrlwi r3,r3,16
		sthx r4,ssb,r3
2:		bdnz 1b
		addi r3,r3,-2			# save current frame pointer
		clrlwi r3,r3,16
		sthbrx r6,ssb,r3
3:		sthbrx r6,state,r7		# New BP
		sub r3,r3,r0
		sthbrx r3,REG			# Save new stack pointer
		NEXT

		.equ enterl_sp, unimpl

leavew_sp:	li opreg,BP
		lhbrx r3,REG			# Stack = BP
		addi r4,r3,2			#
		lhzx r3,ssb,r3
		li opreg,SP
		sthbrx r4,REG			# New Stack
		sth r3,BP(state)		# Popped BP
		NEXT

		.equ leavel_sp, unimpl

/* String instructions:	first a generic setup routine, which exits early
if there is a repeat prefix with a count of 0 */
#define STRINGSRC base,offset
#define STRINGDST esb,opreg

_setup_stringw:	li offset,SI			#
		rlwinm. r3,opcode,19,0,1	# lt=repnz, gt= repz, eq none
		li opreg,DI
		lhbrx offset,state,offset	# load si
		li r3,1				# no repeat
		lhbrx opreg,state,opreg		# load di
		beq 1f				# no repeat
		li r3,CX
		lhbrx r3,state,r3		# load CX
		cmpwi r3,0
		beq nop				# early exit here !
1:		mtctr r3			# ctr=CX or 1
		li r7,1				# stride
		bflr+ DF
		li r7,-1			# change stride sign
		blr

/* Ending routine to update all changed registers (goes directly to NEXT) */
_finish_strw:	li r4,SI
		sthbrx offset,state,r4		# update si
		li r4,DI
		sthbrx opreg,state,r4		# update di
		beq nop
		mfctr r3
		li r4,CX
		sthbrx r3,state,r4		# update cx
		NEXT

lodsb_a16:	bl _setup_stringw
1:		lbzx r0,STRINGSRC		# [rep] lodsb
		add offset,offset,r7
		clrlwi offset,offset,16
		bdnz 1b
		stb r0,AL(state)
		b _finish_strw

lodsw_a16:	bl _setup_stringw
		slwi r7,r7,1
1:		lhzx r0,STRINGSRC		# [rep] lodsw
		add offset,offset,r7
		clrlwi offset,offset,16
		bdnz 1b
		sth r0,AX(state)
		b _finish_strw

lodsl_a16:	bl _setup_stringw
		slwi r7,r7,2
1:		lwzx r0,STRINGSRC		# [rep] lodsl
		add offset,offset,r7
		clrlwi offset,offset,16
		bdnz 1b
		stw r0,EAX(state)
		b _finish_strw

stosb_a16:	bl _setup_stringw
		lbz r0,AL(state)
1:		stbx r0,STRINGDST		# [rep] stosb
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

stosw_a16:	bl _setup_stringw
		lhz r0,AX(state)
		slwi r7,r7,1
1:		sthx r0,STRINGDST		# [rep] stosw
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

stosl_a16:	bl _setup_stringw
		lwz r0,EAX(state)
		slwi r7,r7,2
1:		stwx r0,STRINGDST		# [rep] stosl
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

movsb_a16:	bl _setup_stringw
1:		lbzx r0,STRINGSRC		# [rep] movsb
		add offset,offset,r7
		stbx r0,STRINGDST
		clrlwi offset,offset,16
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

movsw_a16:	bl _setup_stringw
		slwi r7,r7,1
1:		lhzx r0,STRINGSRC		# [rep] movsw
		add offset,offset,r7
		sthx r0,STRINGDST
		clrlwi offset,offset,16
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

movsl_a16:	bl _setup_stringw
		slwi r7,r7,2
1:		lwzx r0,STRINGSRC		# [rep] movsl
		add offset,offset,r7
		stwx r0,STRINGDST
		clrlwi offset,offset,16
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

/* At least on a Pentium, repeated string I/O instructions check for
access port permission even if count is 0 ! So the order of the check is not
important. */
insb_a16:	li r4,DX
		li r3,code_insb_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		lwz base,iobase(state)
1:		lbzx r0,base,r4			# [rep] insb
		eieio
		stbx r0,STRINGDST
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

insw_a16:	li r4,DX
		li r3,code_insw_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		lwz base,iobase(state)
		slwi r7,r7,1
1:		lhzx r0,base,r4			# [rep] insw
		eieio
		sthx r0,STRINGDST
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

insl_a16:	li r4,DX
		li r3,code_insl_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		lwz base,iobase(state)
		slwi r7,r7,2
1:		lwzx r0,base,r4			# [rep] insl
		eieio
		stwx r0,STRINGDST
		add opreg,opreg,r7
		clrlwi opreg,opreg,16
		bdnz 1b
		b _finish_strw

outsb_a16:	li r4,DX
		li r3,code_outsb_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		lwz r6,iobase(state)
1:		lbzx r0,STRINGSRC		# [rep] outsb
		add offset,offset,r7
		stbx r0,r6,r4
		clrlwi offset,offset,16
		eieio
		bdnz 1b
		b _finish_strw

outsw_a16:	li r4,DX
		li r3,code_outsw_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		li r5,DX
		lwz r6,iobase(state)
		slwi r7,r7,1
1:		lhzx r0,STRINGSRC		# [rep] outsw
		add offset,offset,r7
		sthx r0,r6,r4
		clrlwi offset,offset,16
		eieio
		bdnz 1b
		b _finish_strw

outsl_a16:	li r4,DX
		li r3,code_outsl_a16
		lhbrx r4,state,r4
		bl _check_port
		bl _setup_stringw
		lwz r6,iobase(state)
		slwi r7,r7,2
1:		lwzx r0,STRINGSRC		# [rep] outsl
		add offset,offset,r7
		stwx r0,r6,r4
		clrlwi offset,offset,16
		eieio
		bdnz 1b
		b _finish_strw

cmpsb_a16:	bl _setup_stringw
		SET_FLAGS(FLAGS_CMP(B))
		blt 3f				# repnz prefix
1:		lbzx op1,STRINGSRC		# [repz] cmpsb
		add offset,offset,r7
		lbzx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzt CF+2,1b
2:		extsb r3,op1
		extsb r4,op2
		cmpw cr6,r3,r4
		sub result,op1,op2
		b _finish_strw

3:		lbzx op1,STRINGSRC		# repnz cmpsb
		add offset,offset,r7
		lbzx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzf CF+2,3b
		b 2b

cmpsw_a16:	bl _setup_stringw
		SET_FLAGS(FLAGS_CMP(W))
		slwi r7,r7,1
		blt 3f				# repnz prefix
1:		lhbrx op1,STRINGSRC		# [repz] cmpsb
		add offset,offset,r7
		lhbrx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzt CF+2,1b
2:		extsh r3,op1
		extsh r4,op2
		cmpw cr6,r3,r4
		sub result,op1,op2
		b _finish_strw

3:		lhbrx op1,STRINGSRC		# repnz cmpsw
		add offset,offset,r7
		lhbrx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzf CF+2,3b
		b 2b

cmpsl_a16:	bl _setup_stringw
		SET_FLAGS(FLAGS_CMP(L))
		slwi r7,r7,2
		blt 3f				# repnz prefix
1:		lwbrx op1,STRINGSRC		# [repz] cmpsl
		add offset,offset,r7
		lwbrx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzt CF+2,1b
2:		cmpw cr6,op1,op2
		sub result,op1,op2
		b _finish_strw

3:		lwbrx op1,STRINGSRC		# repnz cmpsl
		add offset,offset,r7
		lwbrx op2,STRINGDST
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi offset,offset,16
		clrlwi opreg,opreg,16
		bdnzf CF+2,3b
		b 2b

scasb_a16:	bl _setup_stringw
		lbzx op1,AL,state		# AL
		SET_FLAGS(FLAGS_CMP(B))
		bgt 3f				# repz prefix
1:		lbzx op2,STRINGDST		# [repnz] scasb
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzf CF+2,1b
2:		extsb r3,op1
		extsb r4,op2
		cmpw cr6,r3,r4
		sub result,op1,op2
		b _finish_strw

3:		lbzx op2,STRINGDST		# repz scasb
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzt CF+2,3b
		b 2b

scasw_a16:	bl _setup_stringw
		lhbrx op1,AX,state
		SET_FLAGS(FLAGS_CMP(W))
		slwi r7,r7,1
		bgt 3f				# repz prefix
1:		lhbrx op2,STRINGDST		# [repnz] scasw
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzf CF+2,1b
2:		extsh r3,op1
		extsh r4,op2
		cmpw cr6,r3,r4
		sub result,op1,op2
		b _finish_strw

3:		lhbrx op2,STRINGDST		# repz scasw
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzt CF+2,3b
		b 2b

scasl_a16:	bl _setup_stringw
		lwbrx op1,EAX,state
		SET_FLAGS(FLAGS_CMP(L))
		slwi r7,r7,2
		bgt 3f				# repz prefix
1:		lwbrx op2,STRINGDST		# [repnz] scasl
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzf CF+2,1b
2:		cmpw cr6,op1,op2
		sub result,op1,op2
		b _finish_strw

3:		lwbrx op2,STRINGDST		# repz scasl
		add opreg,opreg,r7
		cmplw cr4,op1,op2
		clrlwi opreg,opreg,16
		bdnzt CF+2,3b
		b 2b

		.equ lodsb_a32, unimpl
		.equ lodsw_a32, unimpl
		.equ lodsl_a32, unimpl
		.equ stosb_a32, unimpl
		.equ stosw_a32, unimpl
		.equ stosl_a32, unimpl
		.equ movsb_a32, unimpl
		.equ movsw_a32, unimpl
		.equ movsl_a32, unimpl
		.equ insb_a32, unimpl
		.equ insw_a32, unimpl
		.equ insl_a32, unimpl
		.equ outsb_a32, unimpl
		.equ outsw_a32, unimpl
		.equ outsl_a32, unimpl
		.equ cmpsb_a32, unimpl
		.equ cmpsw_a32, unimpl
		.equ cmpsl_a32, unimpl
		.equ scasb_a32, unimpl
		.equ scasw_a32, unimpl
		.equ scasl_a32, unimpl

xlatb_a16:	li offset,BX
		lbz r3,AL(state)
		lhbrx offset,offset,state
		add r3,r3,base
		lbzx r3,r3,offset
		stb r3,AL(state)
		NEXT

		.equ xlatb_a32, unimpl

/*
 * Shift and rotates: note the oddity that rotates do not affect SF/ZF/AF/PF
 * but shifts do. Also testing has indicated that rotates with a count of zero
 * do not affect any flag. The documentation specifies this for shifts but
 * is more obscure for rotates. The overflow flag setting is only specified
 * when count is 1, otherwise OF is undefined which simplifies emulation.
 */

/*
 * The rotates through carry are among the most difficult instructions,
 * they are implemented as a shift of 2*n+some bits depending on case.
 * First the left rotates through carry.
 */

/* Byte rcl is performed on 18 bits (17 actually used) in a single register */
rclb_imm:	NEXTBYTE(r3)
		b 1f
rclb_cl:	lbz r3,CL(state)
		b 1f
rclb_1:		li r3,1
1:		lbzx r0,MEM
		andi. r3,r3,31			# count%32
		addc r4,flags,flags		# CF_IN->xer[ca]
		RES2CF(r6)
		subfe r4,result,op1
		mulli r5,r3,29			# 29=ceil(256/9)
		CF_ROTCNT(r7)
		addze r6,r6
		CF_POL_INSERT(r0,23)
		srwi r5,r5,8			# count/9
		rlwnm r6,r6,r7,0x100
		xor r0,r0,r6			# (23)0:CF:data8
		rlwimi r5,r5,3,26,28		# 9*(count/9)
		rlwimi r0,r0,23,0,7		# CF:(data8):(14)0:CF:data8
		sub r3,r3,r5			# count%9
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		rlwnm r0,r0,r3,0x000001ff	# (23)0:NewCF:Result8
		rlwimi flags,r0,19,CF_VALUE
		stbx r0,MEM
		rlwimi flags,r0,18,OF_XOR
		NEXT

/* Word rcl is performed on 33 bits (CF:data16:CF:(15 MSB of data16) */
rclw_imm:	NEXTBYTE(r3)
		b 1f
rclw_cl:	lbz r3,CL(state)
		b 1f
rclw_1:		li r3,1
1:		lhbrx r0,MEM
		andi. r3,r3,31			# count=count%32
		addc r4,flags,flags
		RES2CF(r6)
		subfe r4,result,op1
		addi r5,r3,15			# modulo 17: >=32 if >=17
		CF_ROTCNT(r7)
		addze r6,r6
		addi r7,r7,8
		CF_POL_INSERT(r0,15)
		srwi r5,r5,5			# count/17
		rlwnm r6,r6,r7,0x10000
		rlwimi r5,r5,4,27,27		# 17*(count/17)
		xor r0,r0,r6			# (15)0:CF:data16
		sub r3,r3,r5			# count%17
		rlwinm r4,r0,15,0xffff0000	# CF:(15 MSB of data16):(16)0
		slw r0,r0,r3			# New carry and MSBs
		rlwnm r4,r4,r3,16,31		# New LSBs
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		add r0,r0,r4			# result
		rlwimi flags,r0,11,CF_VALUE
		sthbrx r0,MEM
		rlwimi flags,r0,10,OF_XOR
		NEXT

/* Longword rcl only needs 64 bits because the maximum rotate count is 31 ! */
rcll_imm:	NEXTBYTE(r3)
		b 1f
rcll_cl:	lbz r3,CL(state)
		b 1f
rcll_1:		li r3,1
1:		lwbrx r0,MEM
		andi. r3,r3,31			# count=count%32
		addc r4,r4,flags		# ~XER[CA]
		RES2CF(r6)
		subfe r4,result,op1
		CF_ROTCNT(r7)
		addze r6,r6
		srwi r4,r0,1			# 0:(31 MSB of data32)
		addi r7,r7,23
		CF_POL_INSERT(r4,0)
		rlwnm r6,r6,r7,0,0
		beq- nop			# no flags changed if count 0
		subfic r5,r3,32
		xor r4,r4,r6
		ROTATE_FLAGS
		slw r0,r0,r3			# New MSBs
		srw r5,r4,r5			# New LSBs
		rlwnm r4,r4,r3,0,0		# New Carry
		add r0,r0,r5			# result
		rlwimi flags,r4,28,CF_VALUE
		rlwimi flags,r0,27,OF_XOR
		stwbrx r0,MEM
		NEXT

/* right rotates through carry are even worse because PPC only has a left
rotate instruction. Somewhat tough when combined with modulo 9, 17, or
33 operation and the rules of OF and CF flag settings. */
/* Byte rcr is performed on 17 bits */
rcrb_imm:	NEXTBYTE(r3)
		b 1f
rcrb_cl:	lbz r3,CL(state)
		b 1f
rcrb_1:		li r3,1
1:		lbzx r0,MEM
		andi. r3,r3,31			# count%32
		addc r4,flags,flags		# cf_in->xer[ca]
		RES2CF(r6)
		mulli r5,r3,29			# 29=ceil(256/9)
		subfe r4,result,op1
		CF_ROTCNT(r7)
		addze r6,r6
		CF_POL_INSERT(r0,23)
		srwi r5,r5,8			# count/9
		rlwimi r0,r0,9,0x0001fe00	# (15)0:data8:0:data8
		rlwnm r6,r6,r7,0x100
		rlwimi r5,r5,3,26,28		# 9*(count/9)
		xor r0,r0,r6			# (15)0:data8:CF:data8
		sub r3,r3,r5			# count%9
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		srw r0,r0,r3			# (23)junk:NewCF:Result8
		rlwimi flags,r0,19,CF_VALUE|OF_XOR
		stbx r0,MEM
		NEXT

/* Word rcr is a 33 bit right shift with a quirk, because the 33rd bit
is only needed when the rotate count is 16 and rotating left or right
by 16 a 32 bit quantity is the same ! */
rcrw_imm:	NEXTBYTE(r3)
		b 1f
rcrw_cl:	lbz r3,CL(state)
		b 1f
rcrw_1:		li r3,1
1:		lhbrx r0,MEM
		andi. r3,r3,31			# count%32
		addc r4,flags,flags		# cf_in->xer[ca]
		RES2CF(r6)
		subfe r4,result,op1
		addi r5,r3,15			# >=32 if >=17
		CF_ROTCNT(r7)
		addze r6,r6
		addi r7,r7,8
		CF_POL_INSERT(r0,15)
		srwi r5,r5,5			# count/17
		rlwnm r6,r6,r7,0x10000
		rlwinm r7,r0,16,0x01		# MSB of data16
		rlwimi r0,r0,17,0xfffe0000	# (15 MSB of data16):0:data16
		rlwimi r5,r5,4,27,27		# 17*(count/17)
		xor r0,r0,r6			# (15 MSB of data16):CF:data16
		sub r3,r3,r5			# count%17
		beq- nop			# no flags changed if count 0
		srw r0,r0,r3			# shift right
		rlwnm r7,r7,r3,0x10000		# just in case count=16
		ROTATE_FLAGS
		add r0,r0,r7			# junk15:NewCF:result16
		rlwimi flags,r0,11,CF_VALUE|OF_XOR
		sthbrx r0,MEM
		NEXT

/* Longword rcr need only 64 bits since the rotate count is limited to 31 */
rcrl_imm:	NEXTBYTE(r3)
		b 1f
rcrl_cl:	lbz r3,CL(state)
		b 1f
rcrl_1:		li r3,1
1:		lwbrx r0,MEM
		andi. r3,r3,31			# count%32
		addc r4,flags,flags
		RES2CF(r6)
		subfe r4,result,op1
		CF_ROTCNT(r7)
		slwi r4,r0,1			# (31MSB of data32):0
		addze r6,r6
		addi r7,r7,24
		CF_POL_INSERT(r4,31)
		rlwnm r6,r6,r7,0x01
		beq- nop			# no flags changed if count 0
		subfic r7,r3,32
		xor r4,r4,r6
		srw r0,r0,r3			# Result LSB
		slw r5,r4,r7			# Result MSB
		srw r4,r4,r3			# NewCF in LSB
		add r0,r0,r5			# result
		rlwimi flags,r4,27,CF_VALUE
		stwbrx r0,MEM
		rlwimi flags,r0,27,OF_XOR
		NEXT

/* After the rotates through carry, normal rotates are so simple ! */
rolb_imm:	NEXTBYTE(r3)
		b 1f
rolb_cl:	lbz r3,CL(state)
		b 1f
rolb_1:		li r3,1
1:		lbzx r0,MEM
		andi. r4,r3,31			# count%32 == 0 ?
		clrlwi r3,r3,29			# count%8
		rlwimi r0,r0,24,0xff000000	# replicate for shift in
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		rotlw r0,r0,r3
		rlwimi flags,r0,27,CF_VALUE	# New CF
		stbx r0,MEM
		rlwimi flags,r0,26,OF_XOR	# New OF (CF xor MSB)
		NEXT

rolw_imm:	NEXTBYTE(r3)
		b 1f
rolw_cl:	lbz r3,CL(state)
		b 1f
rolw_1:		li r3,1
1:		lhbrx r0,MEM
		andi. r3,r3,31
		rlwimi r0,r0,16,0,15		# duplicate
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		rotlw r0,r0,r3			# result word duplicated
		rlwimi flags,r0,27,CF_VALUE	# New CF
		sthbrx r0,MEM
		rlwimi flags,r0,26,OF_XOR	# New OF (CF xor MSB)
		NEXT

roll_imm:	NEXTBYTE(r3)
		b 1f
roll_cl:	lbz r3,CL(state)
		b 1f
roll_1:		li r3,1
1:		lwbrx r0,MEM
		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		rotlw r0,r0,r3			# result
		rlwimi flags,r0,27,CF_VALUE	# New CF
		stwbrx r0,MEM
		rlwimi flags,r0,26,OF_XOR	# New OF (CF xor MSB)
		NEXT

rorb_imm:	NEXTBYTE(r3)
		b 1f
rorb_cl:	lbz r3,CL(state)
		b 1f
rorb_1:		li r3,1
1:		lbzx r0,MEM
		andi. r4,r3,31			# count%32 == 0 ?
		clrlwi r3,r3,29			# count%8
		rlwimi r0,r0,8,0x0000ff00	# replicate for shift in
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		srw r0,r0,r3
		rlwimi flags,r0,20,CF_VALUE
		stbx r0,MEM
		rlwimi flags,r0,19,OF_XOR
		NEXT

rorw_imm:	NEXTBYTE(r3)
		b 1f
rorw_cl:	lbz r3,CL(state)
		b 1f
rorw_1:		li r3,1
1:		lhbrx r0,MEM
		andi. r4,r3,31
		clrlwi r3,r3,28			# count %16
		rlwimi r0,r0,16,0xffff0000	# duplicate
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		srw r0,r0,r3			# junk16:result16
		rlwimi flags,r0,12,CF_VALUE
		sthbrx r0,MEM
		rlwimi flags,r0,11,OF_XOR
		NEXT

rorl_imm:	NEXTBYTE(r3)
		b 1f
rorl_cl:	lbz r3,CL(state)
		b 1f
rorl_1:		li r3,1
1:		lwbrx r0,MEM
		andi. r4,r3,31
		neg r3,r3
		beq- nop			# no flags changed if count 0
		ROTATE_FLAGS
		rotlw r0,r0,r3			# result
		rlwimi flags,r0,28,CF_VALUE
		stwbrx r0,MEM
		rlwimi flags,r0,27,OF_XOR
		NEXT

/* Right arithmetic shifts: they clear OF whenever count!=0 */
#define SAR_FLAGS       CF_ZERO|OF_ZERO|RESL
sarb_imm:	NEXTBYTE(r3)
		b 1f
sarb_cl:	lbz r3,CL(state)
		b 1f
sarb_1:		li r3,1
1:		lbzx r4,MEM
		andi. r3,r3,31
		addi r5,r3,-1
		extsb r4,r4
		beq- nop			# no flags changed if count 0
		SET_FLAGS(SAR_FLAGS)
		sraw result,r4,r3
		srw r5,r4,r5
		stbx result,MEM
		rlwimi flags,r5,27,CF_VALUE
		NEXT

sarw_imm:	NEXTBYTE(r3)
		b 1f
sarw_cl:	lbz r3,CL(state)
		b 1f
sarw_1:		li r3,1
1:		lhbrx r4,MEM
		andi. r3,r3,31
		addi r5,r3,-1
		extsh r4,r4
		beq- nop			# no flags changed if count 0
		SET_FLAGS(SAR_FLAGS)
		sraw result,r4,r3
		srw r5,r4,r5
		sthbrx result,MEM
		rlwimi flags,r5,27,CF_VALUE
		NEXT

sarl_imm:	NEXTBYTE(r3)
		b 1f
sarl_cl:	lbz r3,CL(state)
		b 1f
sarl_1:		li r3,1
1:		lwbrx r4,MEM
		andi. r3,r3,31
		addi r5,r3,-1
		beq- nop			# no flags changed if count 0
		SET_FLAGS(SAR_FLAGS)
		sraw result,r4,r3
		srw r5,r4,r5
		stwbrx result,MEM
		rlwimi flags,r5,27,CF_VALUE
		NEXT

/* Left shifts are quite easy: they use the flag mechanism of add */
shlb_imm:	NEXTBYTE(r3)
		b 1f
shlb_cl:	lbz r3,CL(state)
		b 1f
shlb_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lbzx op1,MEM
		SET_FLAGS(FLAGS_ADD(B))
		slw result,op1,r3
		addi op2,op1,0			# for OF computation only !
		stbx result,MEM
		NEXT

shlw_imm:	NEXTBYTE(r3)
		b 1f
shlw_cl:	lbz r3,CL(state)
		b 1f
shlw_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lhbrx op1,MEM
		SET_FLAGS(FLAGS_ADD(W))
		slw result,op1,r3
		addi op2,op1,0			# for OF computation only !
		sthbrx result,MEM
		NEXT

/* That one may be wrong */
shll_imm:	NEXTBYTE(r3)
		b 1f
shll_cl:	lbz r3,CL(state)
		b 1f
shll_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lwbrx op1,MEM
		addi r4,r3,-1
		SET_FLAGS(FLAGS_ADD(L))
		slw result,op1,r3
		addi op2,op1,0			# for OF computation only !
		slw op1,op1,r4			# for CF computation
		stwbrx result,MEM
		NEXT

/* Right shifts are quite complex, because of funny flag rules ! */
shrb_imm:	NEXTBYTE(r3)
		b 1f
shrb_cl:	lbz r3,CL(state)
		b 1f
shrb_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lbzx op1,MEM
		addi r4,r3,-1
		SET_FLAGS(FLAGS_SHR(B))
		srw result,op1,r3
		srw r4,op1,r4
		li op2,-1			# for OF computation only !
		stbx result,MEM
		rlwimi flags,r4,27,CF_VALUE	# Set CF
		NEXT

shrw_imm:	NEXTBYTE(r3)
		b 1f
shrw_cl:	lbz r3,CL(state)
		b 1f
shrw_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lhbrx op1,MEM
		addi r4,r3,-1
		SET_FLAGS(FLAGS_SHR(W))
		srw result,op1,r3
		srw r4,op1,r4
		li op2,-1			# for OF computation only !
		sthbrx result,MEM
		rlwimi flags,r4,27,CF_VALUE	# Set CF
		NEXT

shrl_imm:	NEXTBYTE(r3)
		b 1f
shrl_cl:	lbz r3,CL(state)
		b 1f
shrl_1:		li r3,1
1:		andi. r3,r3,31
		beq- nop			# no flags changed if count 0
		lwbrx op1,MEM
		addi r4,r3,-1
		SET_FLAGS(FLAGS_SHR(L))
		srw result,op1,r3
		srw r4,op1,r4
		li op2,-1			# for OF computation only !
		stwbrx result,MEM
		rlwimi flags,r4,27,CF_VALUE	# Set CF
		NEXT

/* Double length shifts, shldw uses FLAGS_ADD for simplicity */
shldw_imm:	NEXTBYTE(r3)
		b 1f
shldw_cl:	lbz r3,CL(state)
1:		andi. r3,r3,31
		beq- nop
		lhbrx op1,MEM
		SET_FLAGS(FLAGS_ADD(W))
		lhbrx op2,REG
		rlwimi op1,op2,16,0,15		# op2:op1
		addi op2,op1,0
		rotlw result,op1,r3
		sthbrx result,MEM
		NEXT

shldl_imm:	NEXTBYTE(r3)
		b 1f
shldl_cl:	lbz r3,CL(state)
1:		andi. r3,r3,31
		beq- nop
		lwbrx op1,MEM
		SET_FLAGS(FLAGS_DBLSH(L))
		lwbrx op2,REG
		subfic r4,r3,32
		slw result,op1,r3
		srw r4,op2,r4
		rotlw r3,op1,r3
		or result,result,r4
		addi op2,op1,0
		rlwimi flags,r3,27,CF_VALUE
		stwbrx result,MEM
		NEXT

shrdw_imm:	NEXTBYTE(r3)
		b 1f
shrdw_cl:	lbz r3,CL(state)
1:		andi. r3,r3,31
		beq- nop
		lhbrx op1,MEM
		SET_FLAGS(FLAGS_DBLSH(W))
		lhbrx op2,REG
		addi r4,r3,-1
		rlwimi op1,op2,16,0,15		# op2:op1
		addi op2,op1,0
		srw result,op1,r3
		srw r4,op1,r4
		sthbrx result,MEM
		rlwimi flags,r4,27,CF_VALUE
		NEXT

shrdl_imm:	NEXTBYTE(r3)
		b 1f
shrdl_cl:	lbz r3,CL(state)
1:		andi. r3,r3,31
		beq- nop
		lwbrx op1,MEM
		SET_FLAGS(FLAGS_DBLSH(L))
		lwbrx op2,REG
		subfic r4,r3,32
		srw result,op1,r3
		addi r3,r3,-1
		slw r4,op2,r4
		srw r3,op1,r3
		or result,result,r4
		addi op2,op1,0
		rlwimi flags,r3,27,CF_VALUE
		stwbrx result,MEM
		NEXT

/* One operand multiplies: with result double the operand size, unsigned */
mulb:		lbzx op2,MEM
		lbz op1,AL(state)
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		subfic r3,result,255
		sthbrx result,AX,state
		rlwimi flags,r3,0,CF_VALUE|OF_VALUE
		NEXT

mulw:		lhbrx op2,MEM
		lhbrx op1,AX,state
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		li r4,DX
		srwi r3,result,16
		sthbrx result,AX,state
		neg r5,r3
		sthbrx r3,r4,state		# DX
		rlwimi flags,r5,0,CF_VALUE|OF_VALUE
		NEXT

mull:		lwbrx op2,MEM
		lwbrx op1,EAX,state
		mullw result,op1,op2
		mulhwu. r3,op1,op2
		SET_FLAGS(FLAGS_MUL)
		stwbrx result,EAX,state
		li r4,EDX
		stwbrx r3,r4,state
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT

/* One operand multiplies: with result double the operand size, signed */
imulb:		lbzx op2,MEM
		extsb op2,op2
		lbz op1,AL(state)
		extsb op1,op1
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		extsb r3,result
		sthbrx result,AX,state
		cmpw r3,result
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT

imulw:		lhbrx op2,MEM
		extsh op2,op2
		lhbrx op1,AX,state
		extsh op1,op1
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		li r3,DX
		extsh r4,result
		srwi r5,result,16
		sthbrx result,AX,state
		cmpw r4,result
		sthbrx r5,r3,state
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT

imull:		lwbrx op2,MEM
		SET_FLAGS(FLAGS_MUL)
		lwbrx op1,EAX,state
		li r3,EDX
		mulhw r4,op1,op2
		mullw result,op1,op2
		stwbrx r4,r3,state
		srawi r3,result,31
		cmpw r3,r4
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT

/* Other multiplies */
imulw_mem_reg:	lhbrx op2,REG
		extsh op2,op2
		b 1f

imulw_imm:	NEXTWORD(op2)
		extsh op2,op2
		b 1f

imulw_imm8:	NEXTBYTE(op2)
		extsb op2,op2
1:		lhbrx op1,MEM
		extsh op1,op1
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		extsh r3,result
		sthbrx result,REG
		cmpw r3,result
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT			# SF/ZF/AF/PF undefined !

imull_mem_reg:	lwbrx op2,REG
		b 1f

imull_imm:	NEXTDWORD(op2)
		b 1f

imull_imm8:	NEXTBYTE(op2)
		extsb op2,op2
1:		lwbrx op1,MEM
		mullw result,op1,op2
		SET_FLAGS(FLAGS_MUL)
		mulhw r3,op1,op2
		srawi r4,result,31
		stwbrx result,REG
		cmpw r3,r4
		beq+ nop
		oris flags,flags,(CF_SET|OF_SET)>>16
		NEXT			# SF/ZF/AF/PF undefined !

/* aad is indeed a multiply */
aad:		NEXTBYTE(r3)
		lbz op1,AH(state)
		lbz op2,AL(state)
		mullw result,op1,r3		# AH*imm
		SET_FLAGS(FLAGS_LOG(B))		# SF/ZF/PF from result
		add result,result,op2		# AH*imm+AL
		slwi r3,result,8
		sth r3,AX(state)			# AH=0
		NEXT				# OF/AF/CF undefined

/* Unsigned divides: we may destroy all flags */
divb:		lhbrx r4,AX,state
		lbzx r3,MEM
		srwi r5,r4,8
		cmplw r5,r3
		bnl- _divide_error
		divwu r5,r4,r3
		mullw r3,r5,r3
		sub r3,r4,r3
		stb r5,AL(state)
		stb r3,AH(state)
		NEXT

divw:		li opreg,DX
		lhbrx r4,AX,state
		lhbrx r5,REG
		lhbrx r3,MEM
		insrwi r4,r5,16,0
		cmplw r5,r3
		bnl- _divide_error
		divwu r5,r4,r3
		mullw r3,r5,r3
		sub r3,r4,r3
		sthbrx r5,AX,state
		sthbrx r3,REG
		NEXT

divl:		li opreg,EDX			# Not yet fully implemented
		lwbrx r3,MEM
		lwbrx r4,REG
		lwbrx r5,EAX,state
		cmplw r4,r3
		bnl- _divide_error
		cmplwi r4,0
		bne- 1f
		divwu r4,r5,r3
		mullw r3,r4,r3
		stwbrx r4,EAX,state
		sub r3,r5,r3
		stwbrx r3,REG
		NEXT
/* full implementation of 64:32 unsigned divide, slow but rarely used */
1:		bl _div_64_32
		stwbrx r5,EAX,state
		stwbrx r4,REG
		NEXT
/*
 * Divide r4:r5 by r3, quotient in r5, remainder in r4.
 * The algorithm is stupid because it won't be used very often.
 */
_div_64_32:	li r7,32
		mtctr r7
1:		cmpwi r4,0			# always subtract in case
		addc r5,r5,r5			# MSB is set
		adde r4,r4,r4
		blt 2f
		cmplw r4,r3
		blt 3f
2:		sub r4,r4,r3
		addi r5,r5,1
3:		bdnz 1b

/* Signed divides: we may destroy all flags */
idivb:		lbzx r3,MEM
		lhbrx r4,AX,state
		cmpwi r3,0
		beq- _divide_error
		divw r5,r4,r3
		extsb r7,r5
		mullw r3,r5,r3
		cmpw r5,r7
		sub r3,r4,r3
		bne- _divide_error
		stb r5,AL(state)
		stb r3,AH(state)
		NEXT

idivw:		li opreg,DX
		lhbrx r4,AX,state
		lhbrx r5,REG
		lhbrx r3,MEM
		insrwi r4,r5,16,0
		cmpwi r3,0
		beq- _divide_error
		divw r5,r4,r3
		extsh r7,r5
		mullw r3,r5,r3
		cmpw r5,r7
		sub r3,r4,r3
		bne- _divide_error
		sthbrx r5,AX,state
		sthbrx r3,REG
		NEXT

idivl:		li opreg,EDX			# Not yet fully implemented
		lwbrx r3,MEM
		lwbrx r5,EAX,state
		cmpwi cr1,r3,0
		lwbrx r4,REG
		srwi  r7,r5,31
		beq- _divide_error
		add. r7,r7,r4
		bne- 1f				# EDX not sign extension of EAX
		divw r4,r5,r3
		xoris r7,r5,0x8000		# only overflow case is
		orc. r7,r7,r3			# 0x80000000 divided by -1
		mullw r3,r4,r3
		beq- _divide_error
		stwbrx r4,EAX,state
		sub r3,r5,r3
		stwbrx r3,REG
		NEXT

/* full 64 by 32 signed divide, checks for overflow might be right now */
1:		srawi r6,r4,31			# absolute value of r4:r5
		srawi r0,r3,31			# absolute value of r3
		xor r5,r5,r6
		xor r3,r3,r0
		subfc r5,r6,r5
		xor r4,r4,r6
		sub r3,r3,r0
		subfe r4,r6,r4
		xor r0,r0,r6			# sign of result
		cmplw r4,r3			# coarse overflow detection
		bnl- _divide_error		# (probably not necessary)
		bl _div_64_32
		xor r5,r5,r0			# apply sign to result
		sub r5,r5,r0
		xor. r7,r0,r5			# wrong sign: overflow
		xor r4,r4,r6			# apply sign to remainder
		blt- _divide_error
		stwbrx r5,EAX,state
		sub r4,r4,r6
		stwbrx r4,REG
		NEXT

/* aam is indeed a divide */
aam:		NEXTBYTE(r3)
		lbz r4,AL(state)
		cmpwi r3,0
		beq- _divide_error		# zero divide
		divwu op2,r4,r3			# AL/imm8
		SET_FLAGS(FLAGS_LOG(B))		# SF/ZF/PF from AL
		mullw r3,op2,r3			# (AL/imm8)*imm8
		stb op2,AH(state)
		sub result,r4,r3		# AL-imm8*(AL/imm8)
		stb result,AL(state)
		NEXT				# OF/AF/CF undefined

_divide_error:	li r3,code_divide_err
		b complex

/* Instructions dealing with segment registers */
pushw_sp_sr:	li r3,SP
		rlwinm opreg,opcode,31,27,29
		addi r5,state,SELECTORS+2
		lhbrx r4,state,r3
		lhzx r0,r5,opreg
		addi r4,r4,-2
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		sthbrx r0,r4,ssb
		NEXT

pushl_sp_sr:	li r3,SP
		rlwinm opreg,opcode,31,27,29
		addi r5,state,SELECTORS+2
		lhbrx r4,state,r3
		lhzx r0,r5,opreg
		addi r4,r4,-4
		sthbrx r4,state,r3
		clrlwi r4,r4,16
		stwbrx r0,r4,ssb
		NEXT

movl_sr_mem:	cmpwi opreg,20
		addi opreg,opreg,SELECTORS+2
		cmpw cr1,base,state		# Only registers are sensitive
		bgt- ud				# to word/longword difference
		lhzx r0,REG
		bne cr1,1f
		stwbrx r0,MEM			# Actually a register
		NEXT

movw_sr_mem:	cmpwi opreg,20			# SREG 0 to 5 only
		addi opreg,opreg,SELECTORS+2
		bgt- ud
		lhzx r0,REG
1:		sthbrx r0,MEM
		NEXT

/* Now the instructions that modify the segment registers, note that
move/pop to ss disable interrupts and traps for one instruction ! */
popl_sp_sr:	li r6,4
		b 1f
popw_sp_sr:	li r6,2
1:		li r7,SP
		rlwinm opreg,opcode,31,27,29
		lhbrx offset,state,r7
		addi opreg,opreg,SELBASES
		lhbrx r4,ssb,offset		# new selector
		add offset,offset,r6
		bl _segment_load
		sthbrx offset,state,r7		# update sp
		cmpwi opreg,8			# is ss ?
		stwux r3,REG
		stw r4,SELECTORS-SELBASES(opreg)
		lwz esb,esbase(state)
		bne+ nop
		lwz ssb,ssbase(state)		# pop ss
		crmove RF,TF			# prevent traps
		NEXT

movw_mem_sr:	cmpwi opreg,20
		addi r7,state,SELBASES
		bgt- ud
		cmpwi opreg,4			# CS illegal
		beq- ud
		lhbrx r4,MEM
		bl _segment_load
		stwux r3,r7,opreg
		cmpwi opreg,8
		stw r4,SELECTORS-SELBASES(r7)
		lwz esb,esbase(state)
		bne+ nop
		lwz ssb,ssbase(state)
		crmove RF,TF			# prevent traps
		NEXT

		.equ movl_mem_sr, movw_mem_sr

/* The encoding of les/lss/lds/lfs/lgs is strange, opcode is c4/b2/c5/b4/b5
for es/ss/ds/fs/gs which are sreg 0/2/3/4/5. And obviously there is
no lcs instruction, it's called a far jump. */

ldlptrl:	lwzux r7,MEM
		li r4,4
		bl 1f
		stwx r7,REG
		NEXT
ldlptrw:	lhzux r7,MEM
		li r4,2
		bl 1f
		sthx r7,REG
		NEXT

1:		cmpw base,state
		lis r3,0xc011			# es/ss/ds/fs/gs
		rlwinm r5,opcode,2,0x0c		# 00/08/04/00/04
		mflr r0
		addi r3,r3,0x4800		# r4=0xc0114800
		rlwimi r5,opcode,0,0x10		# 00/18/04/10/14
		lhbrx r4,r4,offset
		rlwnm opcode,r3,r5,0x1c		# 00/08/0c/10/14 = sreg*4 !
		beq- ud				# Only mem operands allowed !
		bl _segment_load
		addi r5,opcode,SELBASES
		stwux r3,r5,state
		mtlr r0
		stw r4,SELECTORS-SELBASES(r5)
		lwz esb,esbase(state)		# keep shadow state in sync
		lwz ssb,ssbase(state)
		blr

/* Intructions that may modify the current code segment: the next optimization
 * might be to avoid calling C code when the code segment does not change. But
 * it's probably not worth the effort.
 */
/* Far calls, jumps and returns */
lcall_w:	NEXTWORD(r4)
		NEXTWORD(r5)
		li r3,code_lcallw
		b complex

lcall_l:	NEXTDWORD(r4)
		NEXTWORD(r5)
		li r3,code_lcalll
		b complex

lcallw:		lhbrx r4,MEM
		addi offset,offset,2
		lhbrx r5,MEM
		li r3,code_lcallw
		b complex

lcalll:		lwbrx r4,MEM
		addi offset,offset,4
		lhbrx r5,MEM
		li r3,code_lcalll
		b complex

ljmp_w:		NEXTWORD(r4)
		NEXTWORD(r5)
		li r3,code_ljmpw
		b complex

ljmp_l:		NEXTDWORD(r4)
		NEXTWORD(r5)
		li r3,code_ljmpl
		b complex

ljmpw:		lhbrx r4,MEM
		addi offset,offset,2
		lhbrx r5,MEM
		li r3,code_ljmpw
		b complex

ljmpl:		lwbrx r4,MEM
		addi offset,offset,4
		lhbrx r5,MEM
		li r3,code_ljmpl
		b complex

lretw_imm:	NEXTWORD(r4)
		b 1f
lretw:		li r4,0
1:		li r3,code_lretw
		b complex

lretl_imm:	NEXTWORD(r4)
		b 1f
lretl:		li r4,0
1:		li r3,code_lretl
		b complex

/* Interrupts */
int:		li r3,code_softint		# handled by C code
		NEXTBYTE(r4)
		b complex

int3:		li r3,code_int3			# handled by C code
		b complex

into:		EVAL_OF
		bf+ OF,nop
		li r3,code_into
		b complex			# handled by C code

iretw:		li r3,code_iretw		# handled by C code
		b complex

iretl:		li r3,code_iretl
		b complex

/* Miscellaneous flag control instructions */
clc:		oris flags,flags,(CF_IN_CR|CF_STATE_MASK|ABOVE_IN_CR)>>16
		xoris flags,flags,(CF_IN_CR|CF_STATE_MASK|ABOVE_IN_CR)>>16
		NEXT

cmc:		oris flags,flags,(CF_IN_CR|ABOVE_IN_CR)>>16
		xoris flags,flags,(CF_IN_CR|CF_COMPLEMENT|ABOVE_IN_CR)>>16
		NEXT

stc:		oris flags,flags,\
			(CF_IN_CR|CF_LOCATION|CF_COMPLEMENT|ABOVE_IN_CR)>>16
		xoris flags,flags,(CF_IN_CR|CF_LOCATION|ABOVE_IN_CR)>>16
		NEXT

cld:		crclr DF
		NEXT

std:		crset DF
		NEXT

cli:		crclr IF
		NEXT

sti:		crset IF
		NEXT

lahf:		bl _eval_flags
		stb r3,AH(state)
		NEXT

sahf:		andis. r3,flags,OF_EXPLICIT>>16
		lbz r0,AH(state)
		beql+ _eval_of			# save OF just in case
		rlwinm op1,r0,31,0x08		# AF
		rlwinm flags,flags,0,OF_STATE_MASK
		extsb result,r0			# SF/PF
		ZF862ZF(r0)
		oris flags,flags,(ZF_PROTECT|ZF_IN_CR|SF_IN_CR)>>16
		addi op2,op1,0			# AF
		ori result,result,0x00fb	# set all except PF
		mtcrf 0x02,r0			# SF/ZF
		rlwimi flags,r0,27,CF_VALUE	# CF
		xori result,result,0x00ff	# 00 if PF set, 04 if clear
		NEXT

pushfw_sp:	bl _eval_flags
		li r4,SP
		lhbrx r5,r4,state
		addi r5,r5,-2
		sthbrx r5,r4,state
		clrlwi r5,r5,16
		sthbrx r3,ssb,r5
		NEXT

pushfl_sp:	bl _eval_flags
		li r4,SP
		lhbrx r5,r4,state
		addi r5,r5,-4
		sthbrx r5,r4,state
		clrlwi r5,r5,16
		stwbrx r3,ssb,r5
		NEXT

popfl_sp:	li r4,SP
		lhbrx r5,r4,state
		lwbrx r3,ssb,r5
		addi r5,r5,4
		stw r3,eflags(state)
		sthbrx r5,r4,state
		b 1f

popfw_sp:	li r4,SP
		lhbrx r5,r4,state
		lhbrx r3,ssb,r5
		addi r5,r5,2
		sth r3,eflags+2(state)
		sthbrx r5,r4,state
1:		rlwinm op1,r3,31,0x08			# AF
		xori result,r3,4			# PF
		ZF862ZF(r3)				# cr6
		lis flags,(OF_EXPLICIT|ZF_PROTECT|ZF_IN_CR|SF_IN_CR)>>16
		addi op2,op1,0				# AF
		rlwinm result,result,0,0x04		# PF
		rlwimi flags,r3,27,CF_VALUE		# CF
		mtcrf 0x6,r3				# IF,DF,TF,SF,ZF
		rlwimi result,r3,24,0,0			# SF
		rlwimi flags,r3,15,OF_VALUE		# OF
		NEXT

/* SETcc is slightly faster for setz/setnz */
setz:		EVAL_ZF
		bt ZF,1f
0:		cmpwi opreg,0
		bne- ud
		stbx opreg,MEM
		NEXT

setnz:		EVAL_ZF
		bt ZF,0b
1:		cmpwi opreg,0
		bne- ud
		stbx one,MEM
		NEXT

#define SETCC(cond, eval, flag) \
set##cond:	EVAL_##eval; bt flag,1b; b 0b; \
setn##cond:	EVAL_##eval; bt flag,0b; b 1b

		SETCC(c, CF, CF)
		SETCC(a, ABOVE, ABOVE)
		SETCC(s, SF, SF)
		SETCC(g, SIGNED, SGT)
		SETCC(l, SIGNED, SLT)
		SETCC(o, OF, OF)
		SETCC(p, PF, PF)

/* No wait for a 486SX */
		.equ wait, nop

/* ARPL is not recognized in real mode */
		.equ arpl, ud

/* clts and in general control and debug registers are not implemented */
		.equ clts, unimpl

aaa:		lhbrx r0,AX,state
		bl _eval_af
		rlwinm r3,r3,0,0x10
		SET_FLAGS(FLAGS_ADD(W))
		rlwimi r3,r0,0,0x0f
		li r4,0x106
		addi r3,r3,-10
		srwi r3,r3,16			# carry ? 0 : 0xffff
		andc op1,r4,r3			# carry ? 0x106 : 0
		add result,r0,op1
		rlwinm result,result,0,28,23	# clear high half of AL
		li op2,10			# sets AF indirectly
		sthbrx r3,AX,state		# OF/SF/ZF/PF undefined !
		rlwimi result,op1,8,0x10000	# insert CF
		NEXT

aas:		lhbrx r0,AX,state
		bl _eval_af
		rlwinm r3,r3,0,0x10
		SET_FLAGS(FLAGS_ADD(W))
		rlwimi r3,r0,0,0x0f		# AF:AL&0x0f
		li r4,0x106
		addi r3,r3,-10
		srwi r3,r3,16			# carry ? 0 : 0xffff
		andc op1,r4,r3			# carry ? 0x106 : 0
		sub result,r0,op1
		rlwinm result,result,0,28,23	# clear high half of AL
		li op2,10			# sets AF indirectly
		sthbrx r3,AX,state		# OF/SF/ZF/PF undefined !
		rlwimi result,op1,8,0x10000	# insert CF
		NEXT

daa:		lbz r0,AL(state)
		bl _eval_af
		rlwinm r7,r3,0,0x10
		bl _eval_cf			# r3=CF<<8
		rlwimi r7,r0,0,0x0f
		SET_FLAGS(FLAGS_ADD(B))
		addi r4,r7,-10
		rlwinm r4,r4,3,0x06		# 6 if AF or >9, 0 otherwise
		srwi op1,r7,1			# 0..4, no AF, 5..f AF set
		add r0,r0,r4			# conditional add
		li op2,11			# sets AF depnding on op1
		or r0,r0,r3
		subfic r3,r0,159
		rlwinm r3,r3,7,0x60		# mask value to add
		add result,r0,r3		# final result for SF/ZF/PF
		stb result,AL(state)
		rlwimi result,r3,2,0x100	# set CF if added
		NEXT

das:		lbz r0,AL(state)
		bl _eval_af
		rlwinm r7,r3,0,0x10
		bl _eval_cf
		rlwimi r7,r0,0,0x0f
		SET_FLAGS(FLAGS_ADD(B))
		addi r4,r7,-10
		rlwinm r4,r4,3,0x06
		srwi op1,r7,1			# 0..4, no AF, 5..f AF set
		sub r0,r0,r4			# conditional add
		li op2,11			# sets AF depending on op1
		or r4,r0,r3			# insert CF
		addi r3,r4,-160
		rlwinm r3,r3,7,0x60		# mask value to add
		sub result,r4,r3		# final result for SF/ZF/PF
		stb result,AL(state)
		rlwimi result,r3,2,0x100	# set CF
		NEXT

/* 486 specific instructions */

/* For cmpxchg, only the zero flag is important */

cmpxchgb:	lbz op1,AL(state)
		SET_FLAGS(FLAGS_SUB(B)|ZF_IN_CR)
		lbzx op2,MEM
		cmpw cr6,op1,op2
		sub result,op1,op2
		bne cr6,1f
		lbzx r3,REG			# success: swap
		stbx r3,MEM
		NEXT
1:		stb op2,AL(state)
		NEXT

cmpxchgw:	lhbrx op1,AX,state
		SET_FLAGS(FLAGS_SUB(W)|ZF_IN_CR)
		lhbrx op2,MEM
		cmpw cr6,op1,op2
		sub result,op1,op2
		bne cr6,1f
		lhzx r3,REG			# success: swap
		sthx r3,MEM
		NEXT
1:		sthbrx op2,AX,state
		NEXT

cmpxchgl:	lwbrx op1,EAX,state
		SET_FLAGS(FLAGS_SUB(L)|ZF_IN_CR|SIGNED_IN_CR)
		lwbrx op2,MEM
		cmpw cr6,op1,op2
		sub result,op1,op2
		bne cr6,1f
		lwzx r3,REG			# success: swap
		stwx r3,MEM
		NEXT
1:		stwbrx op2,EAX,state
		NEXT

xaddb:		lbzx op2,MEM
		SET_FLAGS(FLAGS_ADD(B))
		lbzx op1,REG
		add result,op1,op2
		stbx result,MEM
		stbx op2,REG
		NEXT

xaddw:		lhbrx op2,MEM
		SET_FLAGS(FLAGS_ADD(W))
		lhbrx op1,REG
		add result,op1,op2
		sthbrx result,MEM
		sthbrx op2,REG
		NEXT

xaddl:		lwbrx op2,MEM
		SET_FLAGS(FLAGS_ADD(L))
		lwbrx op1,REG
		add result,op1,op2
		stwbrx result,MEM
		stwbrx op2,REG
		NEXT

/* All FPU instructions skipped. This is a 486 SX ! */
esc:		li r3,code_dna			# DNA interrupt
		b complex

		.equ hlt, unimpl		# Cannot stop

		.equ invd, unimpl

/* Undefined in real address mode */
		.equ lar, ud

		.equ lgdt, unimpl
		.equ lidt, unimpl
		.equ lldt, ud
		.equ lmsw, unimpl

/* protected mode only */
		.equ lsl, ud
		.equ ltr, ud

		.equ movl_cr_reg, unimpl
		.equ movl_reg_cr, unimpl
		.equ movl_dr_reg, unimpl
		.equ movl_reg_dr, unimpl

		.equ sgdt, unimpl

		.equ sidt, unimpl
		.equ sldt, ud
		.equ smsw, unimpl

		.equ str, ud

ud:		li r3,code_ud
		li r4,0
		b complex

unimpl:		li r3,code_ud
		li r4,1
		b complex

		.equ verr, ud
		.equ verw, ud
		.equ wbinvd, unimpl

em86_end:
		.size em86_enter,em86_end-em86_enter
#ifdef	__BOOT__
		.data
#define ENTRY(x,t) .long x+t-_jtables
#else
		.section .rodata
#define ENTRY(x,t) .long x+t
#endif

#define	BOP(x)	ENTRY(x,2)	/* Byte operation with mod/rm byte */
#define WLOP(x) ENTRY(x,3)	/* 16 or 32 bit operation with mod/rm byte */
#define EXTOP(x) ENTRY(x,0)	/* Opcode with extension in mod/rm byte */
#define OP(x)	ENTRY(x,1)	/* Direct one byte opcode/prefix */

/* A few macros for the main table */
#define gen6(op, wl, axeax) \
		BOP(op##b##_reg_mem); WLOP(op##wl##_reg_mem); \
		BOP(op##b##_mem_reg); WLOP(op##wl##_mem_reg); \
		OP(op##b##_imm_al); OP(op##wl##_imm_##axeax)

#define rep7(l,t) \
		ENTRY(l,t); ENTRY(l,t); ENTRY(l,t); ENTRY(l,t); \
		ENTRY(l,t); ENTRY(l,t); ENTRY(l,t)

#define rep8(l) l ; l; l; l; l; l; l; l;

#define allcond(pfx, sfx, t) \
		ENTRY(pfx##o##sfx, t); ENTRY(pfx##no##sfx, t); \
		ENTRY(pfx##c##sfx, t); ENTRY(pfx##nc##sfx, t); \
		ENTRY(pfx##z##sfx, t); ENTRY(pfx##nz##sfx, t); \
		ENTRY(pfx##na##sfx, t); ENTRY(pfx##a##sfx, t); \
		ENTRY(pfx##s##sfx, t); ENTRY(pfx##ns##sfx, t); \
		ENTRY(pfx##p##sfx, t); ENTRY(pfx##np##sfx, t); \
		ENTRY(pfx##l##sfx, t); ENTRY(pfx##nl##sfx, t); \
		ENTRY(pfx##ng##sfx, t); ENTRY(pfx##g##sfx, t)

/* single/double register sign extensions and other oddities */
#define h2sextw cbw		/* Half to Single sign extension */
#define s2dextw cwd		/* Single to Double sign extension */
#define h2sextl cwde
#define s2dextl cdq
#define j_a16_cxz_w jcxz_w
#define j_a32_cxz_w jecxz_w
#define j_a16_cxz_l jcxz_l
#define j_a32_cxz_l jecxz_l
#define loopa16_w loopw_w
#define loopa16_l loopw_l
#define loopa32_w loopl_w
#define loopa32_l loopl_l
#define loopnza16_w loopnzw_w
#define loopnza16_l loopnzw_l
#define loopnza32_w loopnzl_w
#define loopnza32_l loopnzl_l
#define loopza16_w loopzw_w
#define loopza16_l loopzw_l
#define loopza32_w loopzl_w
#define loopza32_l loopzl_l
/* No FP support */

/* Addressing mode table */
		.align 5
#		      (%bx,%si),    (%bx,%di),    (%bp,%si),    (%bp,%di)
adtable:	.long 0x00004360,   0x00004370,   0x80004560,   0x80004570
#		      (%si),        (%di),        o16,          (%bx)
		.long 0x00004600,   0x00004700,   0x00002000,   0x00004300
#		      o8(%bx,%si),  o8(%bx,%di),  o8(%bp,%si),  o8(%bp,%di)
		.long 0x00004360,   0x00004370,   0x80004560,   0x80004570
#		      o8(%si),      o8(%di),      o8(%bp),      o8(%bx)
		.long 0x00004600,   0x00004700,   0x80004500,   0x00004300
#		      o16(%bx,%si), o16(%bx,%di), o16(%bp,%si), o16(%bp,%di)
		.long 0x00004360,   0x00004370,   0x80004560,   0x80004570
#		      o16(%si),     o16(%di),     o16(%bp),     o16(%bx)
		.long 0x00004600,   0x00004700,   0x80004500,   0x00004300
#		register addressing modes do not use the table
		.long 0, 0, 0, 0, 0, 0, 0, 0
#now 32 bit modes
#		      (%eax),	    (%ecx),       (%edx),       (%ebx)
		.long 0x00004090,   0x00004190,   0x00004290,   0x00004390
#		      sib,	    o32,	  (%esi),       (%edi)
		.long 0x00003090,   0x00002090,   0x00004690,   0x00004790
#		      o8(%eax),     o8(%ecx),     o8(%edx),     o8(%ebx)
		.long 0x00004090,   0x00004190,   0x00004290,   0x00004390
#		      sib,	    o8(%ebp),	  o8(%esi),     o8(%edi)
		.long 0x00003090,   0x80004590,   0x00004690,   0x00004790
#		      o32(%eax),    o32(%ecx),    o32(%edx),    o32(%ebx)
		.long 0x00004090,   0x00004190,   0x00004290,   0x00004390
#		      sib,	    o32(%ebp),	  o32(%esi),    o32(%edi)
		.long 0x00003090,   0x80004590,   0x00004690,   0x00004790
#		register addressing modes do not use the table
		.long 0, 0, 0, 0, 0, 0, 0, 0

#define jtable(wl, awl, spesp, axeax, name ) \
		.align	5; \
jtab_##name:	gen6(add, wl, axeax); \
		OP(push##wl##_##spesp##_sr); \
		OP(pop##wl##_##spesp##_sr); \
		gen6(or, wl, axeax); \
		OP(push##wl##_##spesp##_sr); \
		OP(_twobytes); \
		gen6(adc, wl, axeax); \
		OP(push##wl##_##spesp##_sr); \
		OP(pop##wl##_##spesp##_sr); \
		gen6(sbb, wl, axeax); \
		OP(push##wl##_##spesp##_sr); \
		OP(pop##wl##_##spesp##_sr); \
		gen6(and, wl, axeax); OP(_es); OP(daa); \
		gen6(sub, wl, axeax); OP(_cs); OP(das); \
		gen6(xor, wl, axeax); OP(_ss); OP(aaa); \
		gen6(cmp, wl, axeax); OP(_ds); OP(aas); \
		rep8(OP(inc##wl##_reg)); \
		rep8(OP(dec##wl##_reg)); \
		rep8(OP(push##wl##_##spesp##_reg)); \
		rep8(OP(pop##wl##_##spesp##_reg)); \
		OP(pusha##wl##_##spesp); OP(popa##wl##_##spesp); \
		WLOP(bound##wl); WLOP(arpl); \
		OP(_fs); OP(_gs); OP(_opsize); OP(_adsize); \
		OP(push##wl##_##spesp##_imm); WLOP(imul##wl##_imm); \
		OP(push##wl##_##spesp##_imm8); WLOP(imul##wl##_imm8); \
		OP(insb_##awl); OP(ins##wl##_##awl); \
		OP(outsb_##awl); OP(outs##wl##_##awl); \
		allcond(sj,_##wl,1); \
		EXTOP(grp1b_imm); EXTOP(grp1##wl##_imm); \
		EXTOP(grp1b_imm); EXTOP(grp1##wl##_imm8); \
		BOP(testb_reg_mem); WLOP(test##wl##_reg_mem); \
		BOP(xchgb_reg_mem); WLOP(xchg##wl##_reg_mem); \
		BOP(movb_reg_mem); WLOP(mov##wl##_reg_mem); \
		BOP(movb_mem_reg); WLOP(mov##wl##_mem_reg); \
		WLOP(mov##wl##_sr_mem); WLOP(lea##wl); \
		WLOP(mov##wl##_mem_sr); WLOP(pop##wl##_##spesp##_##awl); \
		OP(nop); rep7(xchg##wl##_##axeax##_reg,1); \
		OP(h2sext##wl); OP(s2dext##wl); \
		OP(lcall_##wl); OP(wait); \
		OP(pushf##wl##_##spesp); OP(popf##wl##_##spesp); \
		OP(sahf); OP(lahf); \
		OP(movb_##awl##_al); OP(mov##wl##_##awl##_##axeax); \
		OP(movb_al_##awl); OP(mov##wl##_##axeax##_##awl); \
		OP(movsb_##awl); OP(movs##wl##_##awl); \
		OP(cmpsb_##awl); OP(cmps##wl##_##awl); \
		OP(testb_imm_al); OP(test##wl##_imm_##axeax); \
		OP(stosb_##awl); OP(stos##wl##_##awl); \
		OP(lodsb_##awl); OP(lods##wl##_##awl); \
		OP(scasb_##awl); OP(scas##wl##_##awl); \
		rep8(OP(movb_imm_reg)); \
		rep8(OP(mov##wl##_imm_reg)); \
		EXTOP(shiftb_imm); EXTOP(shift##wl##_imm); \
		OP(ret##wl##_##spesp##_imm); OP(ret##wl##_##spesp); \
		WLOP(ldlptr##wl); WLOP(ldlptr##wl); \
		BOP(movb_imm_mem); WLOP(mov##wl##_imm_mem); \
		OP(enter##wl##_##spesp); OP(leave##wl##_##spesp); \
		OP(lret##wl##_imm); OP(lret##wl); \
		OP(int3); OP(int); OP(into); OP(iret##wl); \
		EXTOP(shiftb_1); EXTOP(shift##wl##_1); \
		EXTOP(shiftb_cl); EXTOP(shift##wl##_cl); \
		OP(aam); OP(aad); OP(ud); OP(xlatb_##awl); \
		rep8(OP(esc)); \
		OP(loopnz##awl##_##wl); OP(loopz##awl##_##wl); \
		OP(loop##awl##_##wl); OP(j_##awl##_cxz_##wl); \
		OP(inb_port_al); OP(in##wl##_port_##axeax); \
		OP(outb_al_port); OP(out##wl##_##axeax##_port); \
		OP(call##wl##_##spesp); OP(jmp_##wl); \
		OP(ljmp_##wl); OP(sjmp_##wl); \
		OP(inb_dx_al); OP(in##wl##_dx_##axeax); \
		OP(outb_al_dx); OP(out##wl##_##axeax##_dx); \
		OP(_lock); OP(ud); OP(_repnz); OP(_repz); \
		OP(hlt); OP(cmc); \
		EXTOP(grp3b); EXTOP(grp3##wl); \
		OP(clc); OP(stc); OP(cli); OP(sti); \
		OP(cld); OP(std); \
		EXTOP(grp4b); EXTOP(grp5##wl##_##spesp); \
		/* Here we start the table for twobyte instructions */ \
		OP(ud); OP(ud); WLOP(lar); WLOP(lsl); \
		OP(ud); OP(ud); OP(clts); OP(ud); \
		OP(invd); OP(wbinvd); OP(ud); OP(ud); \
		OP(ud); OP(ud); OP(ud); OP(ud); \
		rep8(OP(ud)); \
		rep8(OP(ud)); \
		OP(movl_cr_reg); OP(movl_reg_cr); \
		OP(movl_dr_reg); OP(movl_reg_dr); \
		OP(ud); OP(ud); OP(ud); OP(ud); \
		rep8(OP(ud)); \
		/* .long	wrmsr, rdtsc, rdmsr, rdpmc; */\
		rep8(OP(ud)); \
		rep8(OP(ud)); \
		/* allcond(cmov, wl); */  \
		rep8(OP(ud)); rep8(OP(ud)); \
		rep8(OP(ud)); rep8(OP(ud)); \
		/* MMX Start */ \
		rep8(OP(ud)); rep8(OP(ud)); \
		rep8(OP(ud)); rep8(OP(ud)); \
		/* MMX End */ \
		allcond(j,_##wl, 1); \
		allcond(set,,2); \
		OP(push##wl##_##spesp##_sr); OP(pop##wl##_##spesp##_sr); \
		OP(ud) /* cpuid */; WLOP(bt##wl##_reg_mem); \
		WLOP(shld##wl##_imm); WLOP(shld##wl##_cl); \
		OP(ud); OP(ud); \
		OP(push##wl##_##spesp##_sr); OP(pop##wl##_##spesp##_sr); \
		OP(ud) /* rsm */; WLOP(bts##wl##_reg_mem); \
		WLOP(shrd##wl##_imm); WLOP(shrd##wl##_cl); \
		OP(ud); WLOP(imul##wl##_mem_reg); \
		BOP(cmpxchgb); WLOP(cmpxchg##wl); \
		WLOP(ldlptr##wl); WLOP(btr##wl##_reg_mem); \
		WLOP(ldlptr##wl); WLOP(ldlptr##wl); \
		WLOP(movzb##wl); WLOP(movzw##wl); \
		OP(ud); OP(ud); \
		EXTOP(grp8##wl); WLOP(btc##wl##_reg_mem); \
		WLOP(bsf##wl); WLOP(bsr##wl); \
		WLOP(movsb##wl); WLOP(movsw##wl); \
		BOP(xaddb); WLOP(xadd##wl); \
		OP(ud); OP(ud); \
		OP(ud); OP(ud); OP(ud); OP(ud); \
		rep8(OP(bswap)); \
		/* MMX Start */ \
		rep8(OP(ud)); rep8(OP(ud)); \
		rep8(OP(ud)); rep8(OP(ud)); \
		rep8(OP(ud)); rep8(OP(ud)); \
		/* MMX End */
		.align 5		/* 8kb of tables, 32 byte aligned */
_jtables:	jtable(w, a16, sp, ax, www)	/* data16, addr16 */
		jtable(l, a16, sp, eax, lww)	/* data32, addr16 */
		jtable(w, a32, sp, ax, wlw)	/* data16, addr32 */
		jtable(l, a32, sp, eax, llw)	/* data32, addr32 */
/* The other possible combinations are only required by protected mode
code using a big stack segment */
/* Here are the auxiliary tables for opcode extensions, note that
all entries get 2 or 3 added. */
#define grp1table(bwl,t,s8) \
grp1##bwl##_imm##s8:; \
		ENTRY(add##bwl##_imm##s8,t); ENTRY(or##bwl##_imm##s8,t); \
		ENTRY(adc##bwl##_imm##s8,t); ENTRY(sbb##bwl##_imm##s8,t); \
		ENTRY(and##bwl##_imm##s8,t); ENTRY(sub##bwl##_imm##s8,t); \
		ENTRY(xor##bwl##_imm##s8,t); ENTRY(cmp##bwl##_imm##s8,t)

		grp1table(b,2,)
		grp1table(w,3,)
		grp1table(w,3,8)
		grp1table(l,3,)
		grp1table(l,3,8)

#define shifttable(bwl,t,c) \
shift##bwl##_##c:; \
		ENTRY(rol##bwl##_##c,t); ENTRY(ror##bwl##_##c,t); \
		ENTRY(rcl##bwl##_##c,t); ENTRY(rcr##bwl##_##c,t); \
		ENTRY(shl##bwl##_##c,t); ENTRY(shr##bwl##_##c,t); \
		OP(ud); ENTRY(sar##bwl##_##c,t)

		shifttable(b,2,1)
		shifttable(w,3,1)
		shifttable(l,3,1)

		shifttable(b,2,cl)
		shifttable(w,3,cl)
		shifttable(l,3,cl)

		shifttable(b,2,imm)
		shifttable(w,3,imm)
		shifttable(l,3,imm)

#define grp3table(bwl,t) \
grp3##bwl:	ENTRY(test##bwl##_imm,t); OP(ud); \
		ENTRY(not##bwl,t); ENTRY(neg##bwl,t); \
		ENTRY(mul##bwl,t); ENTRY(imul##bwl,t); \
		ENTRY(div##bwl,t); ENTRY(idiv##bwl,t)

		grp3table(b,2)
		grp3table(w,3)
		grp3table(l,3)

grp4b:		BOP(incb); BOP(decb); \
		OP(ud); OP(ud); \
		OP(ud); OP(ud); \
		OP(ud); OP(ud)

#define grp5table(wl,spesp) \
grp5##wl##_##spesp: \
		WLOP(inc##wl); WLOP(dec##wl); \
		WLOP(call##wl##_##spesp##_mem); WLOP(lcall##wl##); \
		WLOP(jmp##wl); WLOP(ljmp##wl); \
		WLOP(push##wl##_##spesp); OP(ud)

		grp5table(w,sp)
		grp5table(l,sp)

#define grp8table(wl) \
grp8##wl:	OP(ud); OP(ud); OP(ud); OP(ud); \
		WLOP(bt##wl##_imm); WLOP(bts##wl##_imm); \
		WLOP(btr##wl##_imm); WLOP(btc##wl##_imm)

		grp8table(w)
		grp8table(l)
#ifdef __BOOT__
_endjtables:	.long	0	/* Points to _jtables after relocation */
#endif
