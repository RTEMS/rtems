/*

Based upon IDT provided code with the following release:

This source code has been made available to you by IDT on an AS-IS
basis. Anyone receiving this source is licensed under IDT copyrights
to use it in any way he or she deems fit, including copying it,
modifying it, compiling it, and redistributing it either with or
without modifications.  No license under IDT patents or patent
applications is to be implied by the copyright license.

Any user of this software should understand that IDT cannot provide
technical support for this software and will not be responsible for
any consequences resulting from the use of this software.

Any person who transfers this source code or any derivative work must
include the IDT copyright notice, this paragraph, and the preceeding
two paragraphs in the transferred software.

COPYRIGHT IDT CORPORATION 1996
LICENSED MATERIAL - PROGRAM PROPERTY OF IDT

*/

/************************************************************************
**
**	idtmem.s - memory and cache functions
**
**	Copyright 1991 Integrated Device Technology, Inc.
**	All Rights Reserved
**
**************************************************************************/

/*
 * 950313: Ketan fixed bugs in mfc0/mtc0 hazards, and removed hack
 * to set mem_size.
 */

#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/asm.h>

	.data
mem_size:
	.word	0
dcache_size:
	.word 	0
icache_size:
#if __mips == 1
	.word	MINCACHE
#endif
#if __mips == 3
	.word	0
#endif

#if __mips == 3
	.data
scache_size:
	.word 	0
icache_linesize:
	.word	0
dcache_linesize:
	.word	0
scache_linesize:
	.word	0
#endif

		.text

#if __mips == 1
#define CONFIGFRM ((2*4)+4)

/*************************************************************************
**
** Config_Dcache() -- determine size of Data cache
**
**************************************************************************/

FRAME(config_Dcache,sp, CONFIGFRM, ra)
	.set	noreorder
	subu	sp,CONFIGFRM
	sw	ra,CONFIGFRM-4(sp)	/* save return address */
	sw	s0,4*4(sp)		/* save s0 in first regsave slot */
	mfc0	s0,C0_SR		/* save SR */
	nop
	mtc0	zero,C0_SR		/* disable interrupts */
	.set	reorder
	jal	_size_cache		/* returns Data cache size in v0 */
	sw	v0, dcache_size		/* save it */
	and 	s0, ~SR_PE		/* do not clear PE */
	.set	noreorder
	mtc0	s0,C0_SR		/* restore SR */
	nop
	.set	reorder
	lw	s0, 4*4(sp)		/* restore s0 */
	lw	ra,CONFIGFRM-4(sp)	/* restore ra */
	addu	sp,CONFIGFRM		/* pop stack */
	j	ra
ENDFRAME(config_Dcache)

/*************************************************************************
**
** Config_Icache() -- determine size of Instruction cache
**		      MUST be run in uncached mode/handled in idt_csu.s
**
**************************************************************************/

FRAME(config_Icache,sp, CONFIGFRM, ra)
	.set	noreorder
	subu	sp,CONFIGFRM
	sw	ra,CONFIGFRM-4(sp)	/* save return address */
	sw	s0,4*4(sp)		/* save s0 in first regsave slot */
	mfc0	s0,C0_SR		/* save SR */
	nop
 	mtc0	zero, C0_SR		/* disable interrupts */
	li	v0,SR_SWC		/* swap caches/disable ints  */
	mtc0	v0,C0_SR
	nop
	.set	reorder
	jal	_size_cache		/* returns instruction cache size */
	.set	noreorder
	mtc0	zero,C0_SR		/* swap back caches */
	nop
	and	s0,~SR_PE		/* do not inadvertantly clear PE */
	mtc0	s0,C0_SR		/* restore SR */
	nop
	.set	reorder
	sw	v0, icache_size		/* save it AFTER caches back */
	lw	s0,4*4(sp)		/* restore s0 */
	lw	ra,CONFIGFRM-4(sp)	/* restore ra */
	addu	sp,CONFIGFRM		/* pop stack */
	j	ra
ENDFRAME(config_Icache)

/************************************************************************
**
** _size_cache()
** returns cache size in v0
**
************************************************************************/

FRAME(_size_cache,sp,0,ra)
	.set	noreorder
	mfc0	t0,C0_SR		/* save current sr */
	nop
	and	t0,~SR_PE		/* do not inadvertently clear PE */
	or	v0,t0,SR_ISC		/* isolate cache */
	mtc0	v0,C0_SR
	/*
	 * First check if there is a cache there at all
	 */
	move	v0,zero
	li	v1,0xa5a5a5a5		/* distinctive pattern */
	sw	v1,K0BASE		/* try to write into cache */
	lw	t1,K0BASE		/* try to read from cache */
	nop
	mfc0	t2,C0_SR
	nop
	.set	reorder
	and	t2,SR_CM
	bne	t2,zero,3f		/* cache miss, must be no cache */
	bne	v1,t1,3f		/* data not equal -> no cache */
	/*
	 * Clear cache size boundries to known state.
	 */
	li	v0,MINCACHE
1:
	sw	zero,K0BASE(v0)
	sll	v0,1
	ble	v0,MAXCACHE,1b

	li	v0,-1
	sw	v0,K0BASE(zero)		/* store marker in cache */
	li	v0,MINCACHE		/* MIN cache size */

2:	lw	v1,K0BASE(v0)		/* Look for marker */
	bne	v1,zero,3f		/* found marker */
	sll	v0,1			/* cache size * 2 */
	ble	v0,MAXCACHE,2b		/* keep looking */
	move	v0,zero			/* must be no cache */
	.set	noreorder
3:	mtc0	t0,C0_SR		/* restore sr */
	j	ra
	nop
ENDFRAME(_size_cache)
	.set	reorder

#define FLUSHFRM (2*4)

/***************************************************************************
**
** flush_Dcache() -  flush entire Data cache
**
****************************************************************************/
FRAME(flush_Dcache,sp,FLUSHFRM,ra)
	lw      t2, dcache_size
	.set	noreorder
	mfc0	t3,C0_SR		/* save SR */
	nop
	and	t3,~SR_PE		/* dont inadvertently clear PE */
	beq	t2,zero,_Dflush_done	/* no D cache, get out! */
	nop
	li	v0, SR_ISC		/* isolate cache */
	mtc0	v0, C0_SR
	nop
	.set	reorder
	li	t0,K0BASE		/* set loop registers  */
	or	t1,t0,t2

2:	sb	zero,0(t0)
	sb	zero,4(t0)
	sb	zero,8(t0)
	sb	zero,12(t0)
	sb	zero,16(t0)
	sb	zero,20(t0)
	sb	zero,24(t0)
	addu	t0,32
	sb	zero,-4(t0)
	bne	t0,t1,2b

	.set	noreorder
_Dflush_done:
	mtc0	t3,C0_SR		/* restore Status Register */
	.set	reorder
	j	ra
ENDFRAME(flush_Dcache)

/***************************************************************************
**
** flush_Icache() -  flush entire Instruction cache
**
**	NOTE: Icache can only be flushed/cleared when uncached
**	      Code forces into uncached memory regardless of calling mode
**
****************************************************************************/
FRAME(flush_Icache,sp,FLUSHFRM,ra)
	lw	t1,icache_size
	.set	noreorder
	mfc0	t3,C0_SR		/* save SR */
	nop
	la	v0,1f
	li	v1,K1BASE
	or	v0,v1
	j	v0			/* force into non-cached space */
	nop
1:
	and	t3,~SR_PE		/* dont inadvertently clear PE */
	beq	t1,zero,_Iflush_done	/* no i-cache get out */
	nop
	li	v0,SR_ISC|SR_SWC	/* disable intr, isolate and swap */
	mtc0	v0,C0_SR
	li	t0,K0BASE
	.set	reorder
	or	t1,t0,t1

1:	sb	zero,0(t0)
	sb	zero,4(t0)
	sb	zero,8(t0)
	sb	zero,12(t0)
	sb	zero,16(t0)
	sb	zero,20(t0)
	sb	zero,24(t0)
	addu	t0,32
	sb	zero,-4(t0)
	bne	t0,t1,1b
	.set	noreorder
_Iflush_done:
	mtc0	t3,C0_SR		/* un-isolate, enable interrupts */
	.set	reorder
	j	ra
ENDFRAME(flush_Icache)

/**************************************************************************
**
** clear_Dcache(base_addr, byte_count) - flush portion of Data cache
**
**      a0 = base address of portion to be cleared
**      a1 = byte count of length
**
***************************************************************************/
FRAME(clear_Dcache,sp,0,ra)

	lw  	t2, dcache_size		/* Data cache size */
	.set	noreorder
	mfc0	t3,C0_SR		/* save SR */
	nop
	and	t3,~SR_PE		/* dont inadvertently clear PE */
	nop
	nop
	.set	reorder
	/*
	 * flush data cache
	 */

	.set	noreorder
	nop
	li	v0,SR_ISC		/* isolate data cache */
	mtc0	v0,C0_SR
	.set	reorder
	bltu	t2,a1,1f		/* cache is smaller than region */
	move	t2,a1
1:	addu	t2,a0			/* ending address + 1 */
	move	t0,a0

1:	sb	zero,0(t0)
	sb	zero,4(t0)
	sb	zero,8(t0)
	sb	zero,12(t0)
	sb	zero,16(t0)
	sb	zero,20(t0)
	sb	zero,24(t0)
	addu	t0,32
	sb	zero,-4(t0)
	bltu	t0,t2,1b

	.set	noreorder
	mtc0	t3,C0_SR		/* un-isolate, enable interrupts */
	nop
	.set	reorder
	j	ra
ENDFRAME(clear_Dcache)

/**************************************************************************
**
** clear_Icache(base_addr, byte_count) - flush portion of Instruction cache
**
**      a0 = base address of portion to be cleared
**      a1 = byte count of length
**
**	NOTE: Icache can only be flushed/cleared when uncached
**	      Code forces into uncached memory regardless of calling mode
**
***************************************************************************/
FRAME(clear_Icache,sp,0,ra)

	lw      t1, icache_size		/* Instruction cache size */
	/*
	 * flush text cache
	 */
	.set	noreorder
	mfc0	t3,C0_SR		/* save SR */
	nop
	la	v0,1f
	li	v1,K1BASE
	or	v0,v1
	j	v0			/* force into non-cached space */
	nop
1:
	and	t3,~SR_PE		/* dont inadvertently clear PE */
	nop
	nop
	li	v0,SR_ISC|SR_SWC	/* disable intr, isolate and swap */
	mtc0	v0,C0_SR
	.set	reorder
	bltu	t1,a1,1f		/* cache is smaller than region */
	move	t1,a1
1:	addu	t1,a0			/* ending address + 1 */
	move	t0,a0

	sb	zero,0(t0)
	sb	zero,4(t0)
	sb	zero,8(t0)
	sb	zero,12(t0)
	sb	zero,16(t0)
	sb	zero,20(t0)
	sb	zero,24(t0)
	addu	t0,32
	sb	zero,-4(t0)
	bltu	t0,t1,1b
	.set	noreorder
	mtc0	t3,C0_SR		/* un-isolate, enable interrupts */
	nop
	nop
	nop				/* allow time for caches to swap */
	.set	reorder
	j	ra
ENDFRAME(clear_Icache)

/**************************************************************************
**
**  get_mem_conf - get memory configuration
**
***************************************************************************/

FRAME(get_mem_conf,sp,0,ra)

	lw	t6, mem_size
	sw	t6, 0(a0)
	lw	t7, icache_size
	sw	t7, 4(a0)
	lw	t8, dcache_size
	sw	t8, 8(a0)
	j	ra

ENDFRAME(get_mem_conf)
#endif /* __mips == 1 */

#if __mips == 3
#define LEAF(label)	FRAME(label,sp,0,ra)
#define XLEAF(label) \
	.globl label ; \
label:

/*
 * cacheop macro to automate cache operations
 * first some helpers...
 */
#define _mincache(size, maxsize) \
	bltu	size,maxsize,8f ;	\
	move	size,maxsize ;		\
8:

#define _align(tmp, minaddr, maxaddr, linesize) \
	subu	tmp,linesize,1 ;	\
	not	tmp ;			\
	and	minaddr,tmp ;		\
	addu	maxaddr,-1 ;		\
	and	maxaddr,tmp

/* This is a bit of a hack really because it relies on minaddr=a0 */
#define _doop1(op1) \
	cache	op1,0(a0)

#define _doop2(op1, op2) \
	cache	op1,0(a0) ;		\
	cache	op2,0(a0)

/* specials for cache initialisation */
#define _doop1lw1(op1) \
	cache	op1,0(a0) ;		\
	lw	zero,0(a0) ;		\
	cache	op1,0(a0)

#define _doop121(op1,op2) \
	cache	op1,0(a0) ;		\
	nop;				\
	cache	op2,0(a0) ;		\
	nop;				\
	cache	op1,0(a0)

#define _oploopn(minaddr, maxaddr, linesize, tag, ops) \
	.set	noreorder ;		\
7: 	_doop##tag##ops ;	 	\
	bne     minaddr,maxaddr,7b ;	\
	addu   	minaddr,linesize ;	\
	.set	reorder

/* finally the cache operation macros */
#define icacheopn(kva, n, cache_size, cache_linesize, tag, ops) \
	_mincache(n, cache_size);	\
 	blez	n,9f ;			\
	addu	n,kva ;			\
	_align(t1, kva, n, cache_linesize) ; \
	_oploopn(kva, n, cache_linesize, tag, ops) ; \
9:

#define vcacheopn(kva, n, cache_size, cache_linesize, tag, ops) \
 	blez	n,9f ;			\
	addu	n,kva ;			\
	_align(t1, kva, n, cache_linesize) ; \
	_oploopn(kva, n, cache_linesize, tag, ops) ; \
9:

#define icacheop(kva, n, cache_size, cache_linesize, op) \
	icacheopn(kva, n, cache_size, cache_linesize, 1, (op))

#define vcacheop(kva, n, cache_size, cache_linesize, op) \
	vcacheopn(kva, n, cache_size, cache_linesize, 1, (op))

	.text

/*
 * static void _size_cache()    R4000
 *
 * Internal routine to determine cache sizes by looking at R4000 config
 * register.  Sizes are returned in registers, as follows:
 *	t2	icache size
 *	t3	dcache size
 *	t6	scache size
 *	t4	icache line size
 *	t5	dcache line size
 *	t7	scache line size
 */
LEAF(_size_cache)
	mfc0	t0,C0_CONFIG

	and	t1,t0,CFG_ICMASK
	srl	t1,CFG_ICSHIFT
	li	t2,0x1000
	sll	t2,t1

	and	t1,t0,CFG_DCMASK
	srl	t1,CFG_DCSHIFT
	li	t3,0x1000
	sll	t3,t1

	li	t4,32
	and	t1,t0,CFG_IB
	bnez	t1,1f
	li	t4,16
1:

	li	t5,32
	and	t1,t0,CFG_DB
	bnez	t1,1f
	li	t5,16
1:

	move	t6,zero			# default to no scache
	move	t7,zero			#

	and	t1,t0,CFG_C_UNCACHED	# test config register
	bnez	t1,1f			# no scache if uncached/non-coherent

	li	t6,0x100000		# assume 1Mb scache <<-NOTE
	and	t1,t0,CFG_SBMASK
	srl	t1,CFG_SBSHIFT
	li	t7,16
	sll	t7,t1
1:	j	ra
ENDFRAME(_size_cache)

/*
 * void config_cache()   R4000
 *
 * Work out size of I, D & S caches, assuming they are already initialised.
 */
LEAF(config_cache)
	lw	t0,icache_size
	bgtz	t0,8f			# already known?
	move	v0,ra
	bal	_size_cache
	move	ra,v0

	sw	t2,icache_size
	sw	t3,dcache_size
	sw	t6,scache_size
	sw	t4,icache_linesize
	sw	t5,dcache_linesize
	sw	t7,scache_linesize
8:	j	ra
ENDFRAME(config_cache)

/*
 * void _init_cache()   R4000
 */
LEAF(_init_cache)
	/*
 	 * First work out the sizes
	 */
	move	v0,ra
	bal	_size_cache
	move	ra,v0

	/*
	 * The caches may be in an indeterminate state,
	 * so we force good parity into them by doing an
	 * invalidate, load/fill, invalidate for each line.
	 */

	/* disable all i/u and cache exceptions */
	mfc0	v0,C0_SR
	and	v1,v0,~SR_IE
	or	v1,SR_DE
	mtc0	v1,C0_SR

	mtc0	zero,C0_TAGLO
	mtc0	zero,C0_TAGHI

	/* assume bottom of RAM will generate good parity for the cache */
	li	a0,PHYS_TO_K0(0)
	move	a2,t2		# icache_size
	move	a3,t4		# icache_linesize
	move	a1,a2
	icacheopn(a0,a1,a2,a3,121,(Index_Store_Tag_I,Fill_I))

	li	a0,PHYS_TO_K0(0)
	move	a2,t3		# dcache_size
	move	a3,t5		# dcache_linesize
	move	a1,a2
	icacheopn(a0,a1,a2,a3,1lw1,(Index_Store_Tag_D))

	/* assume unified I & D in scache <<-NOTE */
	blez	t6,1f
	li	a0,PHYS_TO_K0(0)
	move	a2,t6
	move	a3,t7
	move	a1,a2
	icacheopn(a0,a1,a2,a3,1lw1,(Index_Store_Tag_SD))

1:	mtc0	v0,C0_SR
	j	ra
ENDFRAME(_init_cache)

/*
 * void flush_cache (void)   R4000
 *
 * Flush and invalidate all caches
 */
LEAF(flush_cache)
	/* secondary cacheops do all the work if present */
	lw	a2,scache_size
	blez	a2,1f
	lw	a3,scache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_SD)
	b	2f

1:
	lw	a2,icache_size
	blez	a2,2f
	lw	a3,icache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Invalidate_I)

	lw	a2,dcache_size
	lw	a3,dcache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_D)

2:	j	ra
ENDFRAME(flush_cache)

/*
 * void flush_cache_nowrite (void)   R4000
 *
 * Invalidate all caches
 */
LEAF(flush_cache_nowrite)
	mfc0	v0,C0_SR
	and	v1,v0,~SR_IE
	mtc0	v1,C0_SR

	mtc0	zero,C0_TAGLO
	mtc0	zero,C0_TAGHI

	lw	a2,icache_size
	blez	a2,2f
	lw	a3,icache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Invalidate_I)

	lw	a2,dcache_size
	lw	a3,dcache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Store_Tag_D)

	lw	a2,scache_size
	blez	a2,2f
	lw	a3,scache_linesize
	li	a0,PHYS_TO_K0(0)
	move	a1,a2
	icacheop(a0,a1,a2,a3,Index_Store_Tag_SD)

2:	mtc0	v0,C0_SR
	j	ra
ENDFRAME(flush_cache_nowrite)

/*
 * void clean_cache (unsigned kva, size_t n)   R4000
 *
 * Writeback and invalidate address range in all caches
 */
LEAF(clean_cache)
XLEAF(clear_cache)

	/* secondary cacheops do all the work (if fitted) */
	lw	a2,scache_size
	blez	a2,1f
	lw	a3,scache_linesize
	vcacheop(a0,a1,a2,a3,Hit_Writeback_Inv_SD)
	b	2f

1:	lw	a2,icache_size
	blez	a2,2f
	lw	a3,icache_linesize
	/* save kva & n for subsequent loop */
	move	t8,a0
	move	t9,a1
	vcacheop(a0,a1,a2,a3,Hit_Invalidate_I)

	lw	a2,dcache_size
	lw	a3,dcache_linesize
	/* restore kva & n */
	move	a0,t8
	move	a1,t9
	vcacheop(a0,a1,a2,a3,Hit_Writeback_Inv_D)

2:	j	ra
ENDFRAME(clean_cache)

/*
 * void clean_dcache (unsigned kva, size_t n)   R4000
 *
 * Writeback and invalidate address range in primary data cache
 */
LEAF(clean_dcache)
	lw	a2,dcache_size
	blez	a2,2f
	lw	a3,dcache_linesize

	vcacheop(a0,a1,a2,a3,Hit_Writeback_Inv_D)

2:	j	ra
ENDFRAME(clean_dcache)

/*
 * void clean_dcache_indexed (unsigned kva, size_t n)   R4000
 *
 * Writeback and invalidate indexed range in primary data cache
 */
LEAF(clean_dcache_indexed)
	lw	a2,dcache_size
	blez	a2,2f
	lw	a3,dcache_linesize

#ifdef CPU_ORION
	srl	a2,1			# do one set (half cache) at a time
	move	t8,a0			# save kva & n
	move	t9,a1
	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_D)

	addu	a0,t8,a2		# do next set
	move	a1,t9			# restore n
#endif
	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_D)

2:	j	ra
ENDFRAME(clean_dcache_indexed)

/*
 * void clean_dcache_nowrite (unsigned kva, size_t n)   R4000
 *
 * Invalidate an address range in primary data cache
 */
LEAF(clean_dcache_nowrite)
	lw	a2,dcache_size
	blez	a2,2f
	lw	a3,dcache_linesize

	vcacheop(a0,a1,a2,a3,Hit_Invalidate_D)

2:	j	ra
ENDFRAME(clean_dcache_nowrite)

/*
 * void clean_dcache_nowrite_indexed (unsigned kva, size_t n)   R4000
 *
 * Invalidate indexed range in primary data cache
 */
LEAF(clean_dcache_nowrite_indexed)
	mfc0	v0,C0_SR
	and	v1,v0,~SR_IE
	mtc0	v1,C0_SR

	mtc0	zero,C0_TAGLO
	mtc0	zero,C0_TAGHI

	lw	a2,dcache_size
	blez	a2,2f
	lw	a3,dcache_linesize

#ifdef CPU_ORION
	srl	a2,1			# do one set (half cache) at a time
	move	t8,a0			# save kva & n
	move	t9,a1
	icacheop(a0,a1,a2,a3,Index_Store_Tag_D)

	addu	a0,t8,a2		# do next set
	move	a1,t9			# restore n
#endif
	icacheop(a0,a1,a2,a3,Index_Store_Tag_D)

2:	mtc0	v0,C0_SR
	j	ra
ENDFRAME(clean_dcache_nowrite_indexed)

/*
 * void clean_icache (unsigned kva, size_t n)   R4000
 *
 * Invalidate address range in primary instruction cache
 */
LEAF(clean_icache)
	lw	a2,icache_size
	blez	a2,2f
	lw	a3,icache_linesize

	vcacheop(a0,a1,a2,a3,Hit_Invalidate_I)

2:	j	ra
ENDFRAME(clean_icache)

/*
 * void clean_icache_indexed (unsigned kva, size_t n)   R4000
 *
 * Invalidate indexed range in primary instruction cache
 */
LEAF(clean_icache_indexed)
	lw	a2,icache_size
	blez	a2,2f
	lw	a3,icache_linesize

#ifdef CPU_ORION
	srl	a2,1			# do one set (half cache) at a time
	move	t8,a0			# save kva & n
	move	t9,a1
	icacheop(a0,a1,a2,a3,Index_Invalidate_I)

	addu	a0,t8,a2		# do next set
	move	a1,t9			# restore n
#endif
	icacheop(a0,a1,a2,a3,Index_Invalidate_I)

2:	j	ra
ENDFRAME(clean_icache_indexed)

/*
 * void clean_scache (unsigned kva, size_t n)   R4000
 *
 * Writeback and invalidate address range in secondary cache
 */
LEAF(clean_scache)
	lw	a2,scache_size
	blez	a2,2f
	lw	a3,scache_linesize
	vcacheop(a0,a1,a2,a3,Hit_Writeback_Inv_SD)

2:	j	ra
ENDFRAME(clean_scache)

/*
 * void clean_scache_indexed (unsigned kva, size_t n)   R4000
 *
 * Writeback and invalidate indexed range in secondary cache
 */
LEAF(clean_scache_indexed)
	lw	a2,scache_size
	blez	a2,2f
	lw	a3,scache_linesize

	icacheop(a0,a1,a2,a3,Index_Writeback_Inv_SD)

2:	j	ra
ENDFRAME(clean_scache_indexed)

/*
 * void clean_scache_nowrite (unsigned kva, size_t n)   R4000
 *
 * Invalidate an address range in secondary cache
 */
LEAF(clean_scache_nowrite)
	lw	a2,scache_size
	blez	a2,2f
	lw	a3,scache_linesize

	vcacheop(a0,a1,a2,a3,Hit_Invalidate_SD)

2:	j	ra
ENDFRAME(clean_scache_nowrite)

/*
 * void clean_scache_nowrite_indexed (unsigned kva, size_t n)   R4000
 *
 * Invalidate indexed range in secondary cache
 */
LEAF(clean_scache_nowrite_indexed)
	mfc0	v0,C0_SR
	and	v1,v0,~SR_IE
	mtc0	v1,C0_SR

	mtc0	zero,C0_TAGLO
	mtc0	zero,C0_TAGHI

	lw	a2,scache_size
	blez	a2,2f
	lw	a3,scache_linesize

	icacheop(a0,a1,a2,a3,Index_Store_Tag_SD)

2:	mtc0	v0,C0_SR
	j	ra
ENDFRAME(clean_scache_nowrite_indexed)

/**************************************************************************
**
**  get_mem_conf - get memory configuration  R4000
**
***************************************************************************/

FRAME(get_mem_conf,sp,0,ra)

	lw	t6, mem_size
	sw	t6, 0(a0)
	lw	t7, icache_size
	sw	t7, 4(a0)
	lw	t8, dcache_size
	sw	t8, 8(a0)
	lw	t7, scache_size
	sw	t7, 12(a0)
	j	ra

ENDFRAME(get_mem_conf)

#endif /* __mips == 3 */

/*
 * void set_mem_size (mem_size)
 *
 * config_memory()'s memory size gets written into mem_size here.
 * Now we don't need to call config_cache() with memory size - New to IDTC6.0
 */
FRAME(set_memory_size,sp,0,ra)
	sw	a0, mem_size
	j	ra
ENDFRAME(set_memory_size)
