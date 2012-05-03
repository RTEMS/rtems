/*
 *  bootldr.h -- Include file for bootloader.
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

#ifndef _PPC_BOOTLDR_H
#define _PPC_BOOTLDR_H

#ifndef ASM
#include <stdint.h>
#include <bsp/residual.h>
#include <bsp/consoleIo.h>
#include "pci.h"

#define abs __builtin_abs

#define PTE_REFD 0x100
#define PTE_CHNG (0x80|PTE_REFD)	/* Modified implies referenced */
#define PTE_WTHR 0x040
#define PTE_CINH 0x020
#define PTE_COHER 0x010
#define PTE_GUAR 0x008
#define PTE_RO   0x003
#define PTE_RW   0x002

#define PTE_RAM (PTE_CHNG|PTE_COHER|PTE_RW)
#define PTE_ROM (PTE_REFD|PTE_RO)
#define PTE_IO  (PTE_CHNG|PTE_CINH|PTE_GUAR|PTE_RW)

typedef struct {}opaque;

/* The context passed during MMU interrupts. */
typedef struct _ctxt {
	u_long lr, ctr;
	u_int cr, xer;
	u_long nip, msr;
	u_long regs[32];
} ctxt;

/* The main structure which is pointed to permanently by r13. Things
 * are not separated very well between parts because it would cause
 * too much code bloat for such a simple program like the bootloader.
 * The code is designed to be compiled with the -m relocatable option and
 * tries to minimize the number of relocations/fixups and the number of
 * functions who have to access the .got2 sections (this increases the
 * size of the prologue in every function).
 */
typedef struct _boot_data {
	RESIDUAL *residual;
	void *load_address;
        void *of_entry;
	void *r6, *r7, *r8, *r9, *r10;
	u_long cache_lsize;
	void *image;  /* Where to copy ourselves */
	void *stack;
	void *mover;  /* where to copy codemove to avoid overlays */
	u_long o_msr, o_hid0, o_r31;
	opaque * mm_private;
	const struct pci_bootloader_config_access_functions* pci_functions;
	opaque * pci_private;
	struct pci_dev * pci_devices;
	opaque * v86_private;
	char cmd_line[256];
} boot_data;

register boot_data *bd __asm__("r13");

static inline int
pcibios_read_config_byte(u_char bus, u_char dev_fn,
			 u_char where, uint8_t *val) {
	return bd->pci_functions->read_config_byte(bus, dev_fn, where, val);
}

static inline int
pcibios_read_config_word(u_char bus, u_char dev_fn,
			 u_char where, uint16_t *val) {
	return bd->pci_functions->read_config_word(bus, dev_fn, where, val);
}

static inline int
pcibios_read_config_dword(u_char bus, u_char dev_fn,
			 u_char where, uint32_t *val) {
	return bd->pci_functions->read_config_dword(bus, dev_fn, where, val);
}

static inline int
pcibios_write_config_byte(u_char bus, u_char dev_fn,
			 u_char where, uint8_t val) {
	return bd->pci_functions->write_config_byte(bus, dev_fn, where, val);
}

static inline int
pcibios_write_config_word(u_char bus, u_char dev_fn,
			 u_char where, uint16_t val) {
	return bd->pci_functions->write_config_word(bus, dev_fn, where, val);
}

static inline int
pcibios_write_config_dword(u_char bus, u_char dev_fn,
			 u_char where, uint32_t val) {
	return bd->pci_functions->write_config_dword(bus, dev_fn, where, val);
}

static inline int
pci_bootloader_read_config_byte(struct pci_dev *dev, u_char where, uint8_t *val) {
	return bd->pci_functions->read_config_byte(dev->bus->number,
						   dev->devfn,
						   where, val);
}

static inline int
pci_bootloader_read_config_word(struct pci_dev *dev, u_char where, uint16_t *val) {
	return bd->pci_functions->read_config_word(dev->bus->number,
						   dev->devfn,
						   where, val);
}

static inline int
pci_bootloader_read_config_dword(struct pci_dev *dev, u_char where, uint32_t *val) {
	return bd->pci_functions->read_config_dword(dev->bus->number,
						    dev->devfn,
						    where, val);
}

static inline int
pci_bootloader_write_config_byte(struct pci_dev *dev, u_char where, uint8_t val) {
	return bd->pci_functions->write_config_byte(dev->bus->number,
						    dev->devfn,
						    where, val);
}

static inline int
pci_bootloader_write_config_word(struct pci_dev *dev, u_char where, uint16_t val) {
	return bd->pci_functions->write_config_word(dev->bus->number,
						    dev->devfn,
						    where, val);
}

static inline int
pci_bootloader_write_config_dword(struct pci_dev *dev, u_char where, uint32_t val) {
	return bd->pci_functions->write_config_dword(dev->bus->number,
						     dev->devfn,
						     where, val);
}

/* codemove is like memmove, but it also gets the cache line size
 * as 4th parameter to synchronize them. If this last parameter is
 * zero, it performs more or less like memmove. No copy is performed if
 * source and destination addresses are equal. However the caches
 * are synchronized. Note that the size is always rounded up to the
 * next mutiple of 4.
 */
extern void * codemove(void *, const void *, size_t, unsigned long);

/* The physical memory allocator allows to align memory by
 * powers of 2 given by the lower order bits of flags.
 * By default it allocates from higher addresses towrds lower ones,
 * setting PA_LOW reverses this behaviour.
 */

#define palloc(size) __palloc(size,0)

#define isa_io_base (bd->io_base)

void * __palloc(u_long, int);
void  pfree(void *);

#define PA_LOW 0x100
#define PA_PERM 0x200		/* Not freeable by pfree */
#define PA_SUBALLOC 0x400	/* Allocate for suballocation by salloc */
#define PA_ALIGN_MASK 0x1f

void * valloc(u_long size);
void vfree(void *);

int vmap(void *, u_long, u_long);
void vunmap(void *);

void * salloc(u_long size);
void sfree(void *);

void pci_init(void);

void * memset(void *p, int c, size_t n);

void gunzip(void *, int, unsigned char *, int *);

void print_all_maps(const char *);
void print_hash_table(void);
void MMUon(void);
void MMUoff(void);
void hang(const char *, u_long, ctxt *) __attribute__((noreturn));

int init_v86(void);
void cleanup_v86_mess(void);
void em86_main(struct pci_dev *);
int find_max_mem(struct pci_dev *);

#endif

#ifdef ASM
/* These definitions simplify the ugly declarations necessary for
 * GOT definitions.
 */

#define GOT_ENTRY(NAME) .L_ ## NAME = . - .LCTOC1	; .long	NAME
#define GOT(NAME) .L_ ## NAME (r30)

#define START_GOT	\
	.section	".got2","aw"; \
.LCTOC1 = .+ 0x8000

#define END_GOT \
	.text

#define GET_GOT \
	bl      1f; \
	.text	2; \
0:	.long   .LCTOC1-1f; \
	.text	; \
1:	mflr	r30; \
	lwz	r0,0b-1b(r30); \
        add	r30,r0,r30

#define	bd r13
#define cache_lsize 32	/* Offset into bd area */
#define	image	36
#define stack	40
#define mover	44
#define o_msr	48
#define o_hid0	52
#define o_r31   56
/* Stack offsets for saved registers on exceptions */
#define save_lr    8(r1)
#define save_ctr  12(r1)
#define save_cr   16(r1)
#define save_xer  20(r1)
#define save_nip  24(r1)
#define save_msr  28(r1)
#define save_r(n) 32+4*n(r1)
#endif

#endif
