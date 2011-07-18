/* A fake 'bios' which does nothing but move a kernel image 
 * to RAM address zero and then starts that...
 */
#define LD_CACHE_LINE_SIZE  5
#define INIT_STACK (0x100 - 16) /* 16-byte/svr4 aligned */

/* These offsets must correspond to declaration in qemu_fakeres.c */
#define DAT_LEN    0
#define RES_OFF    4
#define CMD_OFF    8
#define CMD_LEN   12
#define IMG_ADR   16

/* Non-volatile registers */
#define OBASE     30
#define PCID      25
#define PCIA      26

#define PCI_MAX_DEV 32

#define BA_OPCODE(tgt)  ((18<<(31-5)) | 2 | ((tgt) & 0x03fffffc))

	.global fake_data
	.global res_set_memsz

	.global _start
_start:
	lis    1,  INIT_STACK@h
	ori    1,1,INIT_STACK@l

		/* qemu 0.14.1 has the wrong exception prefix for 74xx CPUs
		 * (bug 811683). Work around this by putting a stub at 0x00000X00
		 * which simply jumps to high memory. We only need the SC exception
		 * for now.
		 */
	lis  3,    BA_OPCODE(0xfff00000)@h
	ori  3, 3, BA_OPCODE(0xfff00000)@l
	li   4, 0x0c00
	add  3, 3, 4
	stw  3, 0(4)
	dcbf 0, 4
	icbi 0, 4

	bl     pci_irq_set
		/* copy residual to RAM and fix up; 
		 * this routine returns a pointer to
		 * a 'fake_data' struct. If reading
		 * NVRAM failed then the return value
		 * points to a fall-back version in
		 * ROM...
		 */
	bl     res_copy
		/* fake_data pointer to R29 */
	mr     29, 3

		/* Load up R3..R5 with PreP mandated 
		 * values (R3: residual, R4: kernel image,
		 * R5: OpenFirmware PTR (or NULL).
		 */

		/* load R3 with residual pointer  */
	lwz    3, RES_OFF(29)
	add    3, 3, 29

		/* load R4 with image address     */
	lwz    4, IMG_ADR(29)

		/* load R5 with zero (OFW = NULL) */
	li     5, 0
		/* EXTENSION: R6 = cmdline start  */
	lwz    6, CMD_OFF(29)
	add    6, 6, 29
		/* EXTENSION: R7 = cmdline end    */
	lwz    7, CMD_LEN(29)
	add    7, 7, 6

		/* jump to image address          */
	mtctr  4
	bctr

	.org 0x100
	b    _start

	.org 0x110
template:
	mfsrr0 30
	mfsrr1 31
1:  b      1b
template_end:

	.org 0xc00
	b    monitor
	

	.org 0x4000
codemove: /* src/dst are cache-aligned */
	addi   5,5,(1<<LD_CACHE_LINE_SIZE)-1
	srwi   5,5,LD_CACHE_LINE_SIZE
	addi   3,3,-4
	addi   4,4,-4
1:
	li     0,  (1<<LD_CACHE_LINE_SIZE)
	mtctr  0
2:
	lwzu   0,  4(3)
	stwu   0,  4(4)
	bdnz   2b
	dcbf   0,4
	icbi   0,4
	addic. 5,5,-1
	bne  1b
	blr

cpexc:
	lis    3,template@h
	ori    3,3,template@l
	li     5,template_end-template
	b      codemove

monitor:
	stwu   1,-16(1)
	stw    OBASE, 8(1)
	lis    OBASE, 0x80000000@h
	cmplwi 10,0x63 /* enter_monitor -> RESET */
	bne    10f
hwreset:
	li   3,1
	stb  3,0x92(OBASE)
1:  b 1b
10: cmplwi 10,0x1d /* .NETCTRL -> ignore */
	bne    10f
	b      ret_from_mon
10: b hwreset      /* unknown -> RESET */ 

ret_from_mon:
	lwz    OBASE,8(1)
	lwz    1,0(1)
	rfi

rcb:
	stwbrx 3, 0, PCIA
	lbzx   3, 0, PCID
	blr

wcb:
	stwbrx 3, 0, PCIA
	stbx   4, 0, PCID
	blr

rcd:
	stwbrx 3, 0, PCIA
	lwbrx  3, 0, PCID
	blr

/* fixup pci interrupt line register according to what
 * qemu does: line = ((pin-1) +  slot_no) & 1 ? 11 : 9;
 */
pci_irq_set:
		/* set up stack frame */
	stwu    1, -32(1)
	mflr    0
	stw     0,  32+4(1)
		/* load counter with # of PCI devs */	
	li      0, PCI_MAX_DEV
	mtctr   0
		/* save non-volatile registers we use
		 * in stack frame
		 */
	stw    20,               8(1)
	stw  PCIA,              12(1)
	stw  PCID,              16(1)
		/* load non-volatile registers with
		 * intended values.
		 */
	lis  20,         0x80000000@h /* key for slot # 0             */
	lis  PCIA,       0x80000cf8@h /* PCI config space address reg */
	ori  PCIA, PCIA, 0x80000cf8@l 
	addi PCID, PCIA, 4            /* PCI config space data    reg */

		/* loop over all slots and fix up PCI IRQ LINE */
1:
	mr   3, 20
	bl   rcd
	addi 3, 3, 1
	cmplwi 3, 0      /* slot empty (= -1 + 1 = 0) ? */
	beq  2f
	addi 3, 20, 0x3d
	bl   rcb
	cmplwi 3, 0
	beq  2f
	slwi  4, 3, 11
	addi  3, 20, 0x3c
	xor   4, 4, 3    /* bit 11 = slot # + irq_num [zero-based] + 1 */
	andi. 4, 4, 0x0800
	li   4, 11
	beq  3f
	li   4,  9
3:
	bl   wcb
2:
	addi 20, 20, 0x0800 /* next slot */
    bdnz 1b

		/* restore and return */
	lwz 20,  32+4(1)
	mtlr 20
	lwz PCID, 16(1)
	lwz PCIA, 12(1)
	lwz 20,    8(1)
	lwz 1,     0(1)
	blr

	.section .romentry, "ax"
	b     _start
