#include <libcpu/bf537.h>
#include <libcpu/sicRegs.h>
#include <libcpu/cecRegs.h>
#include <libcpu/dmaRegs.h>
#include <libcpu/coreTimerRegs.h>

#ifndef LO
#define LO(con32) ((con32) & 0xFFFF)
#endif
#ifndef HI
#define HI(con32) (((con32) >> 16) & 0xFFFF)
#endif


	.section .init
	.globl	__init
	.type	__init,@function
__init:
	.section .fini
	.globl	__fini
	.type	__fini,@function
__fini:


	.section .start
	.align  4

	.global __start
__start:
	cli r0;

	/* setup an initial stack */
	sp.h = 0xFFB0;
	sp.l = 0x0F00;

	/* disable timer interrupts */
	p0.h = HI(TCNTL);
	p0.l = LO(TCNTL);
	r0 = 0;
	[p0] = r0;

	/* disable all interrupts routed through sic */
	p0.h = HI(SIC_IMASK);
	p0.l = LO(SIC_IMASK);
	[p0] = r0;

	/* clear any pending interrupts */
	p0.h = HI(CEC_ILAT);
	p0.l = LO(CEC_ILAT);
	r0 = 0xffff (z);
	[p0] = r0;

	/* disable all dma channels */
	p0.h = HI(DMA0_BASE_ADDRESS + DMA_CONFIG_OFFSET);
	p0.l = LO(DMA0_BASE_ADDRESS + DMA_CONFIG_OFFSET);
	p1 = DMA_PITCH;
	p2 = DMA_CHANNELS;
	r0 = ~DMA_CONFIG_DMAEN;
	lsetup(loop1,loop2) lc0 = p2;
loop1:	r1 = w[p0];
	r1 = r0 & r1;
loop2:  w[p0 ++ p1] = r1.l;

	/* this is so we can stay in supervisor mode and still be able to
	   accept interrupts later. */
	p0.h = start;
	p0.l = start;
	p1.h = HI(CEC_EVT15);
	p1.l = LO(CEC_EVT15);

	[p1] = p0;

	r0 = 0x8000 (z);
	sti r0;

	raise 15;

	p0.h = wait;
	p0.l = wait;

	reti = p0;
	rti;

	/* wait for event 15 */
wait:
	jump wait;

start:
	[--sp] = reti; /* allow us to process interrupts later */

	/* mask interrupts for now */
	cli r0;

	p0.h = _bss_start;
	p0.l = _bss_start;
	p1.h = _end;
	p1.l = _end;
	r0 = p0;
	r1 = p1;
	r1 = r1 - r0;
	p1 = r1;
	r0 = 0;

	/* Set _bss_start until _end to zero */
	lsetup(loop3,loop4) lc0 = p1;
loop3:	b[p0] = r0;
loop4:	p0 +=1;

	l0 = 0;
	l1 = 0;
	l2 = 0;
	l3 = 0;
	sp += -12;
	/* r0 == const char *cmdline (currently null) */
	p0.h = _boot_card;
	p0.l = _boot_card;
	call (p0);
	sp += 12;

	HLT
	p0.h = _exit;
	p0.l = _exit;
	jump (p0);

