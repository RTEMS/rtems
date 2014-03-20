/*  cpuModel.S
 *
 *  This file contains all assembly code for the Intel Cpu identification.
 *  It is based on linux cpu detection code.
 *
 *  Intel also provides public similar code in the book
 *  called :
 *
 *	Pentium Processor Family
 *		Developer Family
 *	Volume  3 :	Architecture and Programming Manual
 *
 * At the following place :
 *
 *	Chapter 5 :	Feature determination
 *	Chapter 25:	CPUID instruction
 *
 *  COPYRIGHT (c) 1998 valette@crf.canon.fr
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems/asm.h>
#include <rtems/score/registers.h>

BEGIN_CODE
	PUBLIC(checkCPUtypeSetCr0);
/*
 * check Processor type: 386, 486, 6x86(L) or CPUID capable processor
 */

SYM (checkCPUtypeSetCr0):
	/*
	 *  Assume 386 for now
	 */
	movl $3, SYM (x86)
	/*
	 * Start using the EFLAGS AC bit determination method described in
	 * the book mentioned above page 5.1. If this bit can be set we
	 * have a 486 or above.
	 */
	pushfl				/* save EFLAGS			*/

	pushfl				/* Get EFLAGS in EAX		*/
	popl eax

	movl eax,ecx			/* save original EFLAGS in ECX	*/
	xorl $EFLAGS_ALIGN_CHECK,eax	/* flip AC bit in EAX		*/
	pushl eax			/* set EAX as EFLAGS		*/
	popfl
	pushfl				/* Get new EFLAGS in EAX	*/
	popl eax

	xorl ecx,eax			/* check if AC bit changed	*/
	andl $EFLAGS_ALIGN_CHECK,eax
	je is386			/* If not : we have a 386	*/
	/*
	 *  Assume 486 for now
	 */
	movl $4,SYM (x86)
	movl ecx,eax			/* Restore orig EFLAGS in EAX	*/
	xorl $EFLAGS_ID,eax		/* flip  ID flag		*/
	pushl eax			/* set EAX as EFLAGS		*/
	popfl
	pushfl				/* Get new EFLAGS in EAX	*/
	popl eax

	xorl ecx,eax			/* check if ID bit changed	*/
	andl $EFLAGS_ID,eax

	/*
	 * if we are on a straight 486DX,
	 * SX, or 487SX we can't change it
	 * OTOH 6x86MXs and MIIs check OK
	 * Also if we are on a Cyrix 6x86(L)
	 */
	je is486x

isnew:
	/*
	 * restore original EFLAGS
	 */
	popfl
	incl SYM(have_cpuid)	/* we have CPUID instruction */

	/*
	 * Addressable Processor Ids
	 *
	 * CPUID.(EAX=4, ECX=0):EAX[31:26] + 1 = Y)
	 */
	movl $4, eax
	movl $0, ecx
	cpuid
	movl eax,SYM(x86_capability_cores)

	/* use it to get :
	 *	processor type,
	 *	processor model,
	 *	processor mask,
	 * by using it with EAX = 1
	 */
	movl $1, eax
	cpuid
	movl ebx,SYM(x86_capability_ebx) /* store ebx feature info */
	movl ecx,SYM(x86_capability_x) /* store ecx feature flags */

	movb al, cl		/* save reg for future use */

	andb $0x0f,ah		/* mask processor family   */
	movb ah,SYM (x86)	/* put result in x86 var   */

	andb $0xf0, al		/* get model		   */
	shrb $4, al
	movb al,SYM (x86_model) /* store it in x86_model   */

	andb $0x0f, cl		/* get mask revision	   */
	movb cl,SYM (x86_mask)  /* store it in x86_mask	   */

	movl edx,SYM(x86_capability)	/* store feature flags in x86_capability */

	/* get vendor info by using CPUID with EXA = 0 */
	xorl eax, eax
	cpuid

	/*
	 * store results contained in ebx, edx, ecx in
	 * x86_vendor_id variable.
	 */
	movl ebx,SYM(x86_vendor_id)
	movl edx,SYM(x86_vendor_id)+4
	movl ecx,SYM(x86_vendor_id)+8

	movl cr0,eax		/* 486+ */
	andl $(CR0_PAGING | CR0_PROTECTION_ENABLE | CR0_EXTENSION_TYPE), eax
	orl $(CR0_ALIGMENT_MASK | CR0_WRITE_PROTECT | CR0_NUMERIC_ERROR | CR0_MONITOR_COPROC),eax
	jmp 2f

/* Now we test if we have a Cyrix 6x86(L). We didn't test before to avoid
 * clobbering the new BX chipset used with the Pentium II, which has a register
 * at the same addresses as those used to access the Cyrix special configuration
 * registers (CCRs).
 */
	/*
	 * A Cyrix/IBM 6x86(L) preserves flags after dividing 5 by 2
	 * (and it _must_ be 5 divided by 2) while other CPUs change
	 * them in undefined ways. We need to know this since we may
	 * need to enable the CPUID instruction at least.
	 * We couldn't use this test before since the PPro and PII behave
	 * like Cyrix chips in this respect.
	 */
is486x:	xor ax,ax
	sahf
	movb $5,al
	movb $2,bl
	div bl
	lahf
	cmpb $2,ah
	jne ncyrix
	/*
	 * N.B. The pattern of accesses to 0x22 and 0x23 is *essential*
	 *      so do not try to "optimize" it! For the same reason we
	 *	do all this with interrupts off.
	 */
#define setCx86(reg, val) \
	movb reg,al;	\
	outb al,$0x22;	\
	movb val,al;	\
	outb al,$0x23

#define getCx86(reg) \
	movb reg,al;	\
	outb al,$0x22;	\
	inb $0x23,al

	cli
	getCx86($0xc3)		/*  get CCR3 */
	movb al,cl		/* Save old value */
	movb al,bl
	andb $0x0f,bl		/* Enable access to all config registers */
	orb $0x10,bl		/* by setting bit 4 */
	setCx86($0xc3,bl)

	getCx86($0xe8)		/* now we can get CCR4 */
	orb $0x80,al		/* and set bit 7 (CPUIDEN) */
	movb al,bl		/* to enable CPUID execution */
	setCx86($0xe8,bl)

        getCx86($0xfe)          /* DIR0 : let's check this is a 6x86(L) */
        andb $0xf0,al		/* should be 3xh */
	cmpb $0x30,al
	jne n6x86
        getCx86($0xe9)          /* CCR5 : we reset the SLOP bit */
        andb $0xfd,al		/* so that udelay calculation */
        movb al,bl		/* is correct on 6x86(L) CPUs */
        setCx86($0xe9,bl)
	setCx86($0xc3,cl)	/* Restore old CCR3 */
	sti
	jmp isnew		/* We enabled CPUID now */

n6x86:	setCx86($0xc3,cl)	/* Restore old CCR3 */
	sti
ncyrix:				/* restore original EFLAGS */
	popfl
	movl cr0,eax		/* 486 */
	andl $(CR0_PAGING | CR0_EXTENSION_TYPE | CR0_PROTECTION_ENABLE),eax	/* Save PG,PE,ET */
	orl $(CR0_ALIGMENT_MASK | CR0_WRITE_PROTECT | CR0_NUMERIC_ERROR | CR0_MONITOR_COPROC),eax	/* set AM, WP, NE and MP */
	jmp 2f
is386:				/* restore original EFLAGS */
	popfl
	movl cr0,eax		/* 386 */
	andl $(CR0_PAGING | CR0_EXTENSION_TYPE | CR0_PROTECTION_ENABLE),eax	/* Save PG,PE,ET */
	orl $CR0_MONITOR_COPROC,eax		/* set MP */
2:	movl eax,cr0
	call check_x87
	ret


/*
 * We depend on ET to be correct. This checks for 287/387.
 */
check_x87:
	movb $0,SYM(hard_math)
	clts
	fninit
	fstsw ax
	cmpb $0,al
	je 1f
	movl cr0,eax		/* no coprocessor: have to set bits */
	xorl $4,eax		/* set EM */
	movl eax,cr0
	ret
	.align 16
1:	movb $1,SYM(hard_math)
	.byte 0xDB,0xE4		/* fsetpm for 287, ignored by 387 */
	ret

END_CODE

BEGIN_DATA
	PUBLIC(x86)
	PUBLIC(have_cpuid)
	PUBLIC(x86_model)
	PUBLIC(x86_mask)
	PUBLIC(x86_capability)
	PUBLIC(x86_capability_ebx)
	PUBLIC(x86_capability_x)
	PUBLIC(x86_capability_cores)
	PUBLIC(x86_vendor_id)
	PUBLIC(hard_math)

SYM(x86):
	.byte 0
SYM(have_cpuid):
	.long 0
SYM(x86_model):
	.byte 0
SYM(x86_mask):
	.byte 0
SYM(x86_capability):
	.long 0
SYM(x86_capability_ebx):
	.long 0
SYM(x86_capability_x):
	.long 0
SYM(x86_capability_cores):
	.long 0
SYM(x86_vendor_id):
	.zero 13
SYM(hard_math):
	.byte 0
END_DATA
