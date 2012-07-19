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


/*
**	idttlb.s - fetch the registers associated with and the contents
**		   of the tlb.
**
*/
/* 950308: Ketan patched a few tlb functions that would not have worked.*/
#include <rtems/mips/iregdef.h>
#include <rtems/mips/idtcpu.h>
#include <rtems/asm.h>


	.text

#if __mips == 1
/*
** ret_tlblo -- returns the 'entrylo' contents for the TLB
**	'c' callable - as ret_tlblo(index) - where index is the
**	tlb entry to return the lo value for - if called from assembly
**	language then index should be in register a0.
*/
FRAME(ret_tlblo,sp,0,ra)
	.set	noreorder
	mfc0	t0,C0_SR		# save sr
	nop
	and	t0,~SR_PE		# dont inadvertantly clear PE
	mtc0	zero,C0_SR		# clear interrupts
	mfc0	t1,C0_TLBHI		# save pid
	sll	a0,TLBINX_INXSHIFT	# position index
	mtc0	a0,C0_INX		# write to index register
	nop
	tlbr				# put tlb entry in entrylo and hi
	nop
	mfc0	v0,C0_TLBLO		# get the requested entry lo
	mtc0	t1,C0_TLBHI		# restore pid
	mtc0	t0,C0_SR		# restore status register
	j	ra
	nop
	.set	reorder
ENDFRAME(ret_tlblo)
#endif
#if __mips == 3
/*
** ret_tlblo[01] -- returns the 'entrylo' contents for the TLB
**	'c' callable - as ret_tlblo(index) - where index is the
**	tlb entry to return the lo value for - if called from assembly
**	language then index should be in register a0.
*/
FRAME(ret_tlblo0,sp,0,ra)
	mfc0	t0,C0_SR		# save sr
	mtc0	zero,C0_SR		# clear interrupts
	mfc0	t1,C0_TLBHI		# save pid
	mtc0	a0,C0_INX		# write to index register
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbr				# put tlb entry in entrylo and hi
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mfc0	v0,C0_TLBLO0		# get the requested entry lo
	mtc0	t1,C0_TLBHI		# restore pid
	mtc0	t0,C0_SR		# restore status register
	j	ra
ENDFRAME(ret_tlblo0)

FRAME(ret_tlblo1,sp,0,ra)
	mfc0	t0,C0_SR		# save sr
	mtc0	zero,C0_SR		# clear interrupts
	mfc0	t1,C0_TLBHI		# save pid
	mtc0	a0,C0_INX		# write to index register
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbr				# put tlb entry in entrylo and hi
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mfc0	v0,C0_TLBLO1		# get the requested entry lo
	mtc0	t1,C0_TLBHI		# restore pid
	mtc0	t0,C0_SR		# restore status register
	j	ra
ENDFRAME(ret_tlblo1)

/*
** ret_pagemask(index) -- return pagemask contents of tlb entry "index"
*/
FRAME(ret_pagemask,sp,0,ra)
	mfc0	t0,C0_SR		# save sr
	mtc0	zero,C0_SR		# disable interrupts
	mfc0	t1,C0_TLBHI		# save current pid
	mtc0	a0,C0_INX		# drop it in C0 register
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbr				# read entry to entry hi/lo
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mfc0	v0,C0_PAGEMASK		# to return value
	mtc0	t1,C0_TLBHI		# restore current pid
	mtc0	t0,C0_SR		# restore sr
	j	ra
ENDFRAME(ret_pagemask)

/*
** ret_tlbwired(void) -- return wired register
*/
FRAME(ret_tlbwired,sp,0,ra)
	mfc0	v0,C0_WIRED
	j	ra
ENDFRAME(ret_tlbwired)
#endif

/*
** ret_tlbhi -- return the tlb entry high content for tlb entry
**			index
*/
FRAME(ret_tlbhi,sp,0,ra)
#if __mips == 1
	.set	noreorder
	mfc0	t0,C0_SR		# save sr
	nop
	and	t0,~SR_PE
	mtc0	zero,C0_SR		# disable interrupts
	mfc0	t1,C0_TLBHI		# save current pid
	sll	a0,TLBINX_INXSHIFT	# position index
	mtc0	a0,C0_INX		# drop it in C0 register
	nop
	tlbr				# read entry to entry hi/lo
	nop
	mfc0	v0,C0_TLBHI		# to return value
	mtc0	t1,C0_TLBHI		# restore current pid
	mtc0	t0,C0_SR		# restore sr
	j	ra
	nop
	.set	reorder
#endif
#if __mips == 3
	mfc0	t0,C0_SR		# save sr
	mtc0	zero,C0_SR		# disable interrupts
	mfc0	t1,C0_TLBHI		# save current pid
	mtc0	a0,C0_INX		# drop it in C0 register
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbr				# read entry to entry hi/lo0/lo1/mask
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mfc0	v0,C0_TLBHI		# to return value
	mtc0	t1,C0_TLBHI		# restore current pid
	mtc0	t0,C0_SR		# restore sr
	j	ra
#endif
ENDFRAME(ret_tlbhi)

/*
** ret_tlbpid() -- return tlb pid contained in the current entry hi
*/
FRAME(ret_tlbpid,sp,0,ra)
#if __mips == 1
	.set	noreorder
	mfc0	v0,C0_TLBHI		# fetch tlb high
	nop
	and	v0,TLBHI_PIDMASK	# isolate and position
	srl	v0,TLBHI_PIDSHIFT
	j	ra
	nop
	.set	reorder
#endif
#if __mips == 3
	mfc0	v0,C0_TLBHI	# to return value
	nop
	and	v0,TLBHI_PIDMASK
	j	ra
#endif
ENDFRAME(ret_tlbpid)

/*
** tlbprobe(address, pid) -- probe the tlb to see if address is currently
**				mapped
**	a0 = vpn  - virtual page numbers are 0=0 1=0x1000, 2=0x2000...
**			virtual page numbers for the r3000 are in
**			entry hi bits 31-12
**	a1 = pid  - this is a process id ranging from 0 to 63
**		    this process id is shifted left 6 bits and or'ed into
**		    the entry hi register
**	returns an index value (0-63) if successful -1 -f not
*/
FRAME(tlbprobe,sp,0,ra)
#if __mips == 1
	.set	noreorder
	mfc0	t0,C0_SR		/* fetch status reg */
	and	a0,TLBHI_VPNMASK	/* isolate just the vpn */
	and	t0,~SR_PE		/* don't inadvertantly clear pe */
	mtc0	zero,C0_SR
	mfc0	t1,C0_TLBHI
	sll	a1,TLBHI_PIDSHIFT	/* possition the pid */
	and	a1,TLBHI_PIDMASK
	or	a0,a1			/* build entry hi value */
	mtc0	a0,C0_TLBHI
	nop
	tlbp				/* do the probe */
	nop
	mfc0	v1,C0_INX
	li	v0,-1
	bltz	v1,1f
	nop
	sra	v0,v1,TLBINX_INXSHIFT	/* get index positioned for return */
1:
	mtc0	t1,C0_TLBHI		/* restore tlb hi */
	mtc0	t0,C0_SR		/* restore the status reg */
	j	ra
	nop
	.set	reorder
#endif
#if __mips == 3
	mfc0	t0,C0_SR		# save sr
	mtc0	zero,C0_SR		# disable interrupts
	mfc0	t1,C0_TLBHI		# save current pid
	and	a0,TLBHI_VPN2MASK	# construct tlbhi for probe
	and	a1,TLBHI_PIDMASK
	or	a0,a1
	mtc0	a0,C0_TLBHI
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbp				# probe entry to entry hi/lo0/lo1/mask
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mfc0	v1,C0_INX
	li	v0,-1
	bltz	v1,1f
	move	v0,v1
1:	mtc0	t1,C0_TLBHI		# restore current pid
	mtc0	t0,C0_SR		# restore sr
	j	ra
#endif
ENDFRAME(tlbprobe)

/*
** resettlb(index) Invalidate the  TLB entry specified by index
*/
FRAME(resettlb,sp,0,ra)
#if __mips == 1
	.set	noreorder
	mfc0	t0,C0_TLBHI		# fetch the current hi
	mfc0	v0,C0_SR		# fetch the status reg.
	li	t2,K0BASE&TLBHI_VPNMASK
	and	v0,~SR_PE		# dont inadvertantly clear PE
	mtc0	zero,C0_SR
	mtc0	t2,C0_TLBHI		# set up tlbhi
	mtc0	zero,C0_TLBLO
	sll	a0,TLBINX_INXSHIFT
	mtc0	a0,C0_INX
	nop
	tlbwi				# do actual invalidate
	nop
	mtc0	t0,C0_TLBHI
	mtc0	v0,C0_SR
	j	ra
	nop
	.set	reorder
#endif
#if __mips == 3
	li	t2,K0BASE&TLBHI_VPN2MASK
	mfc0	t0,C0_TLBHI		# save current TLBHI
	mfc0	v0,C0_SR		# save SR and disable interrupts
	mtc0	zero,C0_SR
	mtc0	t2,C0_TLBHI		# invalidate entry
	mtc0	zero,C0_TLBLO0
	mtc0	zero,C0_TLBLO1
	mtc0	a0,C0_INX
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbwi
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mtc0	t0,C0_TLBHI
	mtc0	v0,C0_SR
	j	ra
#endif
ENDFRAME(resettlb)

#if __mips == 1
/*
** Setup TLB entry
**
** map_tlb(index, tlbhi, phypage)
** 	a0  =  TLB entry index
**	a1  =  virtual page number and PID
**	a2  =  physical page
*/
FRAME(map_tlb,sp,0,ra)
	.set	noreorder
	sll	a0,TLBINX_INXSHIFT
	mfc0	v0,C0_SR		# fetch the current status
	mfc0	a3,C0_TLBHI		# save the current hi
	and	v0,~SR_PE		# dont inadvertantly clear parity

	mtc0	zero,C0_SR
	mtc0	a1,C0_TLBHI		# set the hi entry
	mtc0	a2,C0_TLBLO		# set the lo entry
	mtc0	a0,C0_INX		# load the index
	nop
	tlbwi				# put the hi/lo in tlb entry indexed
	nop
	mtc0	a3,C0_TLBHI		# put back the tlb hi reg
	mtc0	v0,C0_SR		# restore the status register
	j	ra
	nop
	.set	reorder
ENDFRAME(map_tlb)
#endif
#if __mips == 3
/*
** Setup R4000 TLB entry
**
** map_tlb4000(mask_index, tlbhi, pte_even, pte_odd)
** 	a0  =  TLB entry index and page mask
**	a1  =  virtual page number and PID
**      a2  =  pte -- contents of even pte
**      a3  =  pte -- contents of odd pte
*/
FRAME(map_tlb4000,sp,0,ra)
	and	t2,a0,TLBPGMASK_MASK
	and	a0,TLBINX_INXMASK
	mfc0	t1,C0_TLBHI		# save current TLBPID
	mfc0	v0,C0_SR		# save SR and disable interrupts
	mtc0	zero,C0_SR
	mtc0	t2,C0_PAGEMASK		# set
	mtc0	a1,C0_TLBHI		# set VPN and TLBPID
	mtc0	a2,C0_TLBLO0		# set PPN and access bits
	mtc0	a3,C0_TLBLO1		# set PPN and access bits
	mtc0	a0,C0_INX		# set INDEX to wired entry
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	tlbwi				# drop it in
	.set noreorder
	nop; nop; nop; nop; nop; nop; nop; nop
	.set reorder
	mtc0	t1,C0_TLBHI		# restore TLBPID
	mtc0	v0,C0_SR		# restore SR
	j	ra
ENDFRAME(map_tlb4000)
#endif


/*
** Set current TLBPID. This assumes PID is positioned correctly in reg.
**			a0.
*/
FRAME(set_tlbpid,sp,0,ra)
	.set	noreorder
	mtc0	a0,C0_TLBHI
	j	ra
	nop
	.set	reorder
ENDFRAME(set_tlbpid)

