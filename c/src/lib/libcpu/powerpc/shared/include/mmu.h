/*
 * mmu.h
 *
 *	PowerPC memory management structures
 *
 * It is a stripped down version of linux ppc file...
 *
 * Copyright (C) 1999  Eric Valette (valette@crf.canon.fr)
 *                     Canon Centre Recherche France.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _LIBCPU_MMU_H
#define _LIBCPU_MMU_H

#ifndef ASM
/* Hardware Page Table Entry */
typedef struct _PTE {
	unsigned long v:1;	/* Entry is valid */
	unsigned long vsid:24;	/* Virtual segment identifier */
	unsigned long h:1;	/* Hash algorithm indicator */
	unsigned long api:6;	/* Abbreviated page index */
	unsigned long rpn:20;	/* Real (physical) page number */
	unsigned long    :3;	/* Unused */
	unsigned long r:1;	/* Referenced */
	unsigned long c:1;	/* Changed */
	unsigned long w:1;	/* Write-thru cache mode */
	unsigned long i:1;	/* Cache inhibited */
	unsigned long m:1;	/* Memory coherence */
	unsigned long g:1;	/* Guarded */
	unsigned long  :1;	/* Unused */
	unsigned long pp:2;	/* Page protection */
} PTE;

/* Values for PP (assumes Ks=0, Kp=1) */
#define PP_RWXX	0	/* Supervisor read/write, User none */
#define PP_RWRX 1	/* Supervisor read/write, User read */
#define PP_RWRW 2	/* Supervisor read/write, User read/write */
#define PP_RXRX 3	/* Supervisor read,       User read */

/* Segment Register */
typedef struct _SEGREG {
	unsigned long t:1;	/* Normal or I/O  type */
	unsigned long ks:1;	/* Supervisor 'key' (normally 0) */
	unsigned long kp:1;	/* User 'key' (normally 1) */
	unsigned long n:1;	/* No-execute */
	unsigned long :4;	/* Unused */
	unsigned long vsid:24;	/* Virtual Segment Identifier */
} SEGREG;

/* Block Address Translation (BAT) Registers */
typedef struct _P601_BATU {	/* Upper part of BAT for 601 processor */
	unsigned long bepi:15;	/* Effective page index (virtual address) */
	unsigned long :8;	/* unused */
	unsigned long w:1;
	unsigned long i:1;	/* Cache inhibit */
	unsigned long m:1;	/* Memory coherence */
	unsigned long ks:1;	/* Supervisor key (normally 0) */
	unsigned long kp:1;	/* User key (normally 1) */
	unsigned long pp:2;	/* Page access protections */
} P601_BATU;

typedef struct _BATU {		/* Upper part of BAT (all except 601) */
	unsigned long bepi:15;	/* Effective page index (virtual address) */
	unsigned long :4;	/* Unused */
	unsigned long bl:11;	/* Block size mask */
	unsigned long vs:1;	/* Supervisor valid */
	unsigned long vp:1;	/* User valid */
} BATU;

typedef struct _P601_BATL {	/* Lower part of BAT for 601 processor */
	unsigned long brpn:15;	/* Real page index (physical address) */
	unsigned long :10;	/* Unused */
	unsigned long v:1;	/* Valid bit */
	unsigned long bl:6;	/* Block size mask */
} P601_BATL;

typedef struct _BATL {		/* Lower part of BAT (all except 601) */
	unsigned long brpn:15;	/* Real page index (physical address) */
	unsigned long :10;	/* Unused */
	unsigned long w:1;	/* Write-thru cache */
	unsigned long i:1;	/* Cache inhibit */
	unsigned long m:1;	/* Memory coherence */
	unsigned long g:1;	/* Guarded (MBZ in IBAT) */
	unsigned long :1;	/* Unused */
	unsigned long pp:2;	/* Page access protections */
} BATL;

typedef struct _BAT {
	BATU batu;		/* Upper register */
	BATL batl;		/* Lower register */
} BAT;

typedef struct _P601_BAT {
	P601_BATU batu;		/* Upper register */
	P601_BATL batl;		/* Lower register */
} P601_BAT;

/* Block size masks */
#define BL_128K	0x000
#define BL_256K 0x001
#define BL_512K 0x003
#define BL_1M   0x007
#define BL_2M   0x00F
#define BL_4M   0x01F
#define BL_8M   0x03F
#define BL_16M  0x07F
#define BL_32M  0x0FF
#define BL_64M  0x1FF
#define BL_128M 0x3FF
#define BL_256M 0x7FF

/* BAT Access Protection */
#define BPP_XX	0x00		/* No access */
#define BPP_RX	0x01		/* Read only */
#define BPP_RW	0x02		/* Read/write */

/*
 * Simulated two-level MMU.  This structure is used by the kernel
 * to keep track of MMU mappings and is used to update/maintain
 * the hardware HASH table which is really a cache of mappings.
 *
 * The simulated structures mimic the hardware available on other
 * platforms, notably the 80x86 and 680x0.
 */

typedef struct _pte {
   	unsigned long page_num:20;
   	unsigned long flags:12;		/* Page flags (some unused bits) */
} pte;

#define PD_SHIFT (10+12)		/* Page directory */
#define PD_MASK  0x03FF
#define PT_SHIFT (12)			/* Page Table */
#define PT_MASK  0x03FF
#define PG_SHIFT (12)			/* Page Entry */


/* MMU context */

typedef struct _MMU_context {
	SEGREG	segs[16];	/* Segment registers */
	pte	**pmap;		/* Two-level page-map structure */
} MMU_context;

/* Used to set up SDR1 register */
#define HASH_TABLE_SIZE_64K	0x00010000
#define HASH_TABLE_SIZE_128K	0x00020000
#define HASH_TABLE_SIZE_256K	0x00040000
#define HASH_TABLE_SIZE_512K	0x00080000
#define HASH_TABLE_SIZE_1M	0x00100000
#define HASH_TABLE_SIZE_2M	0x00200000
#define HASH_TABLE_SIZE_4M	0x00400000
#define HASH_TABLE_MASK_64K	0x000
#define HASH_TABLE_MASK_128K	0x001
#define HASH_TABLE_MASK_256K	0x003
#define HASH_TABLE_MASK_512K	0x007
#define HASH_TABLE_MASK_1M	0x00F
#define HASH_TABLE_MASK_2M	0x01F
#define HASH_TABLE_MASK_4M	0x03F

/* invalidate a TLB entry */
static inline void _tlbie(unsigned long va)
{
	asm volatile ("tlbie %0" : : "r"(va));
}

extern void _tlbia(void);		/* invalidate all TLB entries */
#endif /* ASM */

/* Control/status registers for the MPC8xx.
 * A write operation to these registers causes serialized access.
 * During software tablewalk, the registers used perform mask/shift-add
 * operations when written/read.  A TLB entry is created when the Mx_RPN
 * is written, and the contents of several registers are used to
 * create the entry.
 */
#define MI_CTR		784	/* Instruction TLB control register */
#define MI_GPM		0x80000000	/* Set domain manager mode */
#define MI_PPM		0x40000000	/* Set subpage protection */
#define MI_CIDEF	0x20000000	/* Set cache inhibit when MMU dis */
#define MI_RSV4I	0x08000000	/* Reserve 4 TLB entries */
#define MI_PPCS		0x02000000	/* Use MI_RPN prob/priv state */
#define MI_IDXMASK	0x00001f00	/* TLB index to be loaded */
#define MI_RESETVAL	0x00000000	/* Value of register at reset */

/* These are the Ks and Kp from the PowerPC books.  For proper operation,
 * Ks = 0, Kp = 1.
 */
#define MI_AP		786
#define MI_Ks		0x80000000	/* Should not be set */
#define MI_Kp		0x40000000	/* Should always be set */

/* The effective page number register.  When read, contains the information
 * about the last instruction TLB miss.  When MI_RPN is written, bits in
 * this register are used to create the TLB entry.
 */
#define MI_EPN		787
#define MI_EPNMASK	0xfffff000	/* Effective page number for entry */
#define MI_EVALID	0x00000200	/* Entry is valid */
#define MI_ASIDMASK	0x0000000f	/* ASID match value */
					/* Reset value is undefined */

/* A "level 1" or "segment" or whatever you want to call it register.
 * For the instruction TLB, it contains bits that get loaded into the
 * TLB entry when the MI_RPN is written.
 */
#define MI_TWC		789
#define MI_APG		0x000001e0	/* Access protection group (0) */
#define MI_GUARDED	0x00000010	/* Guarded storage */
#define MI_PSMASK	0x0000000c	/* Mask of page size bits */
#define MI_PS8MEG	0x0000000c	/* 8M page size */
#define MI_PS512K	0x00000004	/* 512K page size */
#define MI_PS4K_16K	0x00000000	/* 4K or 16K page size */
#define MI_SVALID	0x00000001	/* Segment entry is valid */
					/* Reset value is undefined */

/* Real page number.  Defined by the pte.  Writing this register
 * causes a TLB entry to be created for the instruction TLB, using
 * additional information from the MI_EPN, and MI_TWC registers.
 */
#define MI_RPN		790

/* Define an RPN value for mapping kernel memory to large virtual
 * pages for boot initialization.  This has real page number of 0,
 * large page size, shared page, cache enabled, and valid.
 * Also mark all subpages valid and write access.
 */
#define MI_BOOTINIT	0x000001fd

#define MD_CTR		792	/* Data TLB control register */
#define MD_GPM		0x80000000	/* Set domain manager mode */
#define MD_PPM		0x40000000	/* Set subpage protection */
#define MD_CIDEF	0x20000000	/* Set cache inhibit when MMU dis */
#define MD_WTDEF	0x10000000	/* Set writethrough when MMU dis */
#define MD_RSV4I	0x08000000	/* Reserve 4 TLB entries */
#define MD_TWAM		0x04000000	/* Use 4K page hardware assist */
#define MD_PPCS		0x02000000	/* Use MI_RPN prob/priv state */
#define MD_IDXMASK	0x00001f00	/* TLB index to be loaded */
#define MD_RESETVAL	0x04000000	/* Value of register at reset */

#define M_CASID		793	/* Address space ID (context) to match */
#define MC_ASIDMASK	0x0000000f	/* Bits used for ASID value */


/* These are the Ks and Kp from the PowerPC books.  For proper operation,
 * Ks = 0, Kp = 1.
 */
#define MD_AP		794
#define MD_Ks		0x80000000	/* Should not be set */
#define MD_Kp		0x40000000	/* Should always be set */

/* The effective page number register.  When read, contains the information
 * about the last instruction TLB miss.  When MD_RPN is written, bits in
 * this register are used to create the TLB entry.
 */
#define MD_EPN		795
#define MD_EPNMASK	0xfffff000	/* Effective page number for entry */
#define MD_EVALID	0x00000200	/* Entry is valid */
#define MD_ASIDMASK	0x0000000f	/* ASID match value */
					/* Reset value is undefined */

/* The pointer to the base address of the first level page table.
 * During a software tablewalk, reading this register provides the address
 * of the entry associated with MD_EPN.
 */
#define M_TWB		796
#define	M_L1TB		0xfffff000	/* Level 1 table base address */
#define M_L1INDX	0x00000ffc	/* Level 1 index, when read */
					/* Reset value is undefined */

/* A "level 1" or "segment" or whatever you want to call it register.
 * For the data TLB, it contains bits that get loaded into the TLB entry
 * when the MD_RPN is written.  It is also provides the hardware assist
 * for finding the PTE address during software tablewalk.
 */
#define MD_TWC		797
#define MD_L2TB		0xfffff000	/* Level 2 table base address */
#define MD_L2INDX	0xfffffe00	/* Level 2 index (*pte), when read */
#define MD_APG		0x000001e0	/* Access protection group (0) */
#define MD_GUARDED	0x00000010	/* Guarded storage */
#define MD_PSMASK	0x0000000c	/* Mask of page size bits */
#define MD_PS8MEG	0x0000000c	/* 8M page size */
#define MD_PS512K	0x00000004	/* 512K page size */
#define MD_PS4K_16K	0x00000000	/* 4K or 16K page size */
#define MD_WT		0x00000002	/* Use writethrough page attribute */
#define MD_SVALID	0x00000001	/* Segment entry is valid */
					/* Reset value is undefined */


/* Real page number.  Defined by the pte.  Writing this register
 * causes a TLB entry to be created for the data TLB, using
 * additional information from the MD_EPN, and MD_TWC registers.
 */
#define MD_RPN		798

/* This is a temporary storage register that could be used to save
 * a processor working register during a tablewalk.
 */
#define M_TW		799
#endif /* _LIBCPU_MMU_H */
