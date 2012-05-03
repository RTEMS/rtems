/*
 *  mm.c -- Crude memory management for early boot.
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

/* This code is a crude memory manager for early boot for LinuxPPC.
 * As such, it does not try to perform many optimiztions depending
 * on the processor, it only uses features which are common to
 * all processors (no BATs...).
 *
 * On PreP platorms (the only ones on which it works for now),
 * it maps 1:1 all RAM/ROM and I/O space as claimed by the
 * residual data. The holes between these areas can be virtually
 * remapped to any of these, since for some functions it is very handy
 * to have virtually contiguous but physically discontiguous memory.
 *
 * Physical memory allocation is also very crude, since it's only
 * designed to manage a small number of large chunks. For valloc/vfree
 * and palloc/pfree, the unit of allocation is the 4kB page.
 *
 * The salloc/sfree has been added after tracing gunzip and seeing
 * how it performed a very large number of small allocations.
 * For these the unit of allocation is 8 bytes (the s stands for
 * small or subpage). This memory is cleared when allocated.
 *
 */

#include <rtems/bspIo.h>

#include <sys/types.h>
#include <libcpu/spr.h>
#include "bootldr.h"
#include <libcpu/mmu.h>
#include <libcpu/page.h>
#include <limits.h>

extern void (tlb_handlers)(void);
extern void (_handler_glue)(void);

/* We use our own kind of simple memory areas for the loader, but
 * we want to avoid potential clashes with kernel includes.
 * Here a map maps contiguous areas from base to end,
 * the firstpte entry corresponds to physical address and has the low
 * order bits set for caching and permission.
 */

typedef struct _map {
	struct _map *next;
  	u_long base;
	u_long end;
        u_long firstpte;
} map;

/* The LSB of the firstpte entries on map lists other than mappings
 * are constants which can be checked for debugging. All these constants
 * have bit of weight 4 set, this bit is zero in the mappings list entries.
 * Actually firstpte&7 value is:
 * - 0 or 1 should not happen
 * - 2 for RW actual virtual->physical mappings
 * - 3 for RO actual virtual->physical mappings
 * - 6 for free areas to be suballocated by salloc
 * - 7 for salloc'ated areas
 * - 4 or 5 for all others, in this case firtpte & 63 is
 *   - 4 for unused maps (on the free list)
 *   - 12 for free physical memory
 *   - 13 for physical memory in use
 *   - 20 for free virtual address space
 *   - 21 for allocated virtual address space
 *   - 28 for physical memory space suballocated by salloc
 *   - 29 for physical memory that can't be freed
 */

#define MAP_FREE_SUBS 6
#define MAP_USED_SUBS 7

#define MAP_FREE 4
#define MAP_FREE_PHYS 12
#define MAP_USED_PHYS 13
#define MAP_FREE_VIRT 20
#define MAP_USED_VIRT 21
#define MAP_SUBS_PHYS 28
#define MAP_PERM_PHYS 29

SPR_RW(SDR1);
SPR_RO(DSISR);
SPR_RO(PPC_DAR);

/* We need a few statically allocated free maps to bootstrap the
 * memory managment */
static map free_maps[4] = {{free_maps+1, 0, 0, MAP_FREE},
			   {free_maps+2, 0, 0, MAP_FREE},
			   {free_maps+3, 0, 0, MAP_FREE},
			   {NULL, 0, 0, MAP_FREE}};
struct _mm_private {
	void *sdr1;
	u_long hashmask;
  	map *freemaps;     /* Pool of unused map structs */
  	map *mappings;     /* Sorted list of virtual->physical mappings */
  	map *physavail;    /* Unallocated physical address space */
	map *physused;     /* Allocated physical address space */
	map *physperm;	   /* Permanently allocated physical space */
  	map *virtavail;    /* Unallocated virtual address space */
	map *virtused;     /* Allocated virtual address space */
  	map *sallocfree;   /* Free maps for salloc */
  	map *sallocused;   /* Used maps for salloc */
  	map *sallocphys;   /* Physical areas used by salloc */
  	u_int hashcnt;     /* Used to cycle in PTEG when they overflow */
} mm_private = {hashmask: 0xffc0,
		freemaps: free_maps+0};

/* A simplified hash table entry declaration */
typedef struct _hash_entry {
	int key;
	u_long rpn;
} hash_entry;

void print_maps(map *, const char *);

/* The handler used for all exceptions although for now it is only
 * designed to properly handle MMU interrupts to fill the hash table.
 */

void _handler(int vec, ctxt *p) {
	map *area;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	u_long vaddr, cause;
	if (vec==4 || vec==7) {	/* ISI exceptions are different */
		vaddr = p->nip;
		cause = p->msr;
	} else { /* Valid for DSI and alignment exceptions */
		vaddr = _read_PPC_DAR();
		cause = _read_DSISR();
	}

	if (vec==3 || vec==4) {
	  	/* Panic if the fault is not PTE not found. */
	  	if (!(cause & 0x40000000)) {
			MMUon();
			printk("\nPanic: vector=%x, cause=%lx\n", vec, cause);
			hang("Memory protection violation at ", vaddr, p);
		}

		for(area=mm->mappings; area; area=area->next) {
		  	if(area->base<=vaddr && vaddr<=area->end) break;
		}

		if (area) {
			u_long hash, vsid, rpn;
			hash_entry volatile *hte, *_hte1;
			u_int i, alt=0, flushva;

			vsid = _read_SR((void *)vaddr);
			rpn = (vaddr&PAGE_MASK)-area->base+area->firstpte;
			hash = vsid<<6;
			hash ^= (vaddr>>(PAGE_SHIFT-6))&0x3fffc0;
			hash &= mm->hashmask;
			/* Find an empty entry in the PTEG, else
			 * replace a random one.
			 */
			hte = (hash_entry *) ((u_long)(mm->sdr1)+hash);
			for (i=0; i<8; i++) {
			  	if (hte[i].key>=0) goto found;
			}
			hash ^= mm->hashmask;
			alt = 0x40; _hte1 = hte;
			hte = (hash_entry *) ((u_long)(mm->sdr1)+hash);

			for (i=0; i<8; i++) {
				if (hte[i].key>=0) goto found;
			}
			alt = 0;
			hte = _hte1;
			/* Chose a victim entry and replace it. There might be
			 * better policies to choose the victim, but in a boot
			 * loader we want simplicity as long as it works.
			 *
			 * We would not need to invalidate the TLB entry since
			 * the mapping is still valid. But this would be a mess
			 * when unmapping so we make sure that the TLB is a
			 * subset of the hash table under all circumstances.
			 */
			i = mm->hashcnt;
			mm->hashcnt = (mm->hashcnt+1)%8;
			/* Note that the hash is already complemented here ! */
			flushva = (~(hash<<9)^((hte[i].key)<<5)) &0x3ff000;
			if (hte[i].key&0x40) flushva^=0x3ff000;
			flushva |= ((hte[i].key<<21)&0xf0000000)
			  | ((hte[i].key<<22)&0x0fc00000);
			hte[i].key=0;
			asm volatile("sync; tlbie %0; sync" : : "r" (flushva));
		found:
			hte[i].rpn = rpn;
			asm volatile("eieio": : );
			hte[i].key = 0x80000000|(vsid<<7)|alt|
			  ((vaddr>>22)&0x3f);
			return;
		} else {
		  	MMUon();
			printk("\nPanic: vector=%x, cause=%lx\n", vec, cause);
			hang("\nInvalid memory access attempt at ", vaddr, p);
		}
	} else {
	  MMUon();
	  printk("\nPanic: vector=%x, dsisr=%lx, faultaddr =%lx, msr=%lx opcode=%lx\n", vec,
		 cause, p->nip, p->msr, * ((unsigned int*) p->nip) );
	  if (vec == 7) {
	    unsigned int* ptr = ((unsigned int*) p->nip) - 4 * 10;
	    for (; ptr <= (((unsigned int*) p->nip) + 4 * 10); ptr ++)
	      printk("Hexdecimal code at address %x = %x\n", ptr, *ptr);
	  }
	  hang("Program or alignment exception at ", vaddr, p);
	}
}

/* Generic routines for map handling.
 */

static inline
void free_map(map *p) {
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	if (!p) return;
	p->next=mm->freemaps;
	mm->freemaps=p;
	p->firstpte=MAP_FREE;
}

/* Sorted insertion in linked list */
static
int insert_map(map **head, map *p) {
	map *q = *head;
	if (!p) return 0;
	if (q && (q->base < p->base)) {
        	for(;q->next && q->next->base<p->base; q = q->next);
		if ((q->end >= p->base) ||
		    (q->next && p->end>=q->next->base)) {
			free_map(p);
			printk("Overlapping areas!\n");
			return 1;
		}
		p->next = q->next;
		q->next = p;
	} else { /* Insert at head */
	  	if (q && (p->end >= q->base)) {
		  	free_map(p);
			printk("Overlapping areas!\n");
			return 1;
		}
		p->next = q;
		*head = p;
	}
	return 0;
}

/* Removal from linked list */

static
map *remove_map(map **head, map *p) {
	map *q = *head;

	if (!p || !q) return NULL;
  	if (q==p) {
		*head = q->next;
		return p;
	}
	for(;q && q->next!=p; q=q->next);
	if (q) {
		q->next=p->next;
		return p;
	} else {
		return NULL;
	}
}

static
map *remove_map_at(map **head, void * vaddr) {
	map *p, *q = *head;

	if (!vaddr || !q) return NULL;
  	if (q->base==(u_long)vaddr) {
		*head = q->next;
		return q;
	}
	while (q->next && q->next->base != (u_long)vaddr) q=q->next;
	p=q->next;
	if (p) q->next=p->next;
	return p;
}

static inline
map * alloc_map_page(void) {
	map *from, *p;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	/* printk("Allocating new map page !"); */
	/* Get the highest page */
	for (from=mm->physavail; from && from->next; from=from->next);
	if (!from) return NULL;

	from->end -= PAGE_SIZE;

	mm->freemaps = (map *) (from->end+1);

	for(p=mm->freemaps; p<mm->freemaps+PAGE_SIZE/sizeof(map)-1; p++) {
		p->next = p+1;
		p->firstpte = MAP_FREE;
	}
	(p-1)->next=0;

	/* Take the last one as pointer to self and insert
	 * the map into the permanent map list.
	 */

	p->firstpte = MAP_PERM_PHYS;
	p->base=(u_long) mm->freemaps;
	p->end = p->base+PAGE_SIZE-1;

	insert_map(&mm->physperm, p);

	if (from->end+1 == from->base)
		free_map(remove_map(&mm->physavail, from));

	return mm->freemaps;
}

static
map * alloc_map(void) {
	map *p;
	struct _mm_private * mm = (struct _mm_private *) bd->mm_private;

	p = mm->freemaps;
	if (!p) {
		p=alloc_map_page();
	}

	if(p) mm->freemaps=p->next;

	return p;
}

static
void coalesce_maps(map *p) {
  	while(p) {
	  	if (p->next && (p->end+1 == p->next->base)) {
			map *q=p->next;
			p->end=q->end;
			p->next=q->next;
			free_map(q);
		} else {
			p = p->next;
		}
	}
}

/* These routines are used to find the free memory zones to avoid
 * overlapping destructive copies when initializing.
 * They work from the top because of the way we want to boot.
 * In the following the term zone refers to the memory described
 * by one or several contiguous so called segments in the
 * residual data.
 */
#define STACK_PAGES 2
static inline u_long
find_next_zone(RESIDUAL *res, u_long lowpage, u_long flags) {
	u_long i, newmin=0, size=0;
	for(i=0; i<res->ActualNumMemSegs; i++) {
		if (res->Segs[i].Usage & flags
		    && res->Segs[i].BasePage<lowpage
		    && res->Segs[i].BasePage>newmin) {
			newmin=res->Segs[i].BasePage;
			size=res->Segs[i].PageCount;
		}
	}
	return newmin+size;
}

static inline u_long
find_zone_start(RESIDUAL *res, u_long highpage, u_long flags) {
	u_long i;
	int progress;
	do {
		progress=0;
		for (i=0; i<res->ActualNumMemSegs; i++) {
		  	if ( (res->Segs[i].BasePage+res->Segs[i].PageCount
			      == highpage)
			     && res->Segs[i].Usage & flags) {
			  	highpage=res->Segs[i].BasePage;
				progress=1;
			}
		}
	} while(progress);
	return highpage;
}

/* The Motorola NT firmware does not provide any setting in the residual
 * data about memory segment usage. The following table provides enough
 * info so that this bootloader can work.
 */
MEM_MAP seg_fix[] = {
    { 0x2000, 0xFFF00, 0x00100 },
    { 0x0020, 0x02000, 0x7E000 },
    { 0x0008, 0x00800, 0x00168 },
    { 0x0004, 0x00000, 0x00005 },
    { 0x0001, 0x006F1, 0x0010F },
    { 0x0002, 0x006AD, 0x00044 },
    { 0x0010, 0x00005, 0x006A8 },
    { 0x0010, 0x00968, 0x00698 },
    { 0x0800, 0xC0000, 0x3F000 },
    { 0x0600, 0xBF800, 0x00800 },
    { 0x0500, 0x81000, 0x3E800 },
    { 0x0480, 0x80800, 0x00800 },
    { 0x0440, 0x80000, 0x00800 } };

/* The Motorola NT firmware does not set up all required info in the residual
 * data. This routine changes some things in a way that the bootloader and
 * linux are happy.
 */
void
fix_residual( RESIDUAL *res )
{
#if 0
    PPC_DEVICE *hostbridge;
#endif
    int i;

    /* Missing memory segment information */
    res->ActualNumMemSegs = sizeof(seg_fix)/sizeof(MEM_MAP);
    for (i=0; i<res->ActualNumMemSegs; i++) {
	res->Segs[i].Usage = seg_fix[i].Usage;
	res->Segs[i].BasePage = seg_fix[i].BasePage;
	res->Segs[i].PageCount = seg_fix[i].PageCount;
    }
    /* The following should be fixed in the current version of the
     * kernel and of the bootloader.
     */
#if 0
    /* PPCBug has this zero */
    res->VitalProductData.CacheLineSize = 0;
    /* Motorola NT firmware sets TimeBaseDivisor to 0 */
    if ( res->VitalProductData.TimeBaseDivisor == 0 ) {
	res->VitalProductData.TimeBaseDivisor = 4000;
    }

    /* Motorola NT firmware records the PCIBridge as a "PCIDEVICE" and
     * sets "PCIBridgeDirect". This bootloader and linux works better if
     * BusId = "PROCESSORDEVICE" and Interface = "PCIBridgeIndirect".
     */
    hostbridge=residual_find_device(PCIDEVICE, NULL,
					BridgeController,
					PCIBridge, -1, 0);
    if (hostbridge) {
	hostbridge->DeviceId.BusId = PROCESSORDEVICE;
	hostbridge->DeviceId.Interface = PCIBridgeIndirect;
    }
#endif
}

/* This routine is the first C code called with very little stack space!
 * Its goal is to find where the boot image can be moved. This will
 * be the highest address with enough room.
 */
int early_setup(u_long image_size) {
	register RESIDUAL *res = bd->residual;
	u_long minpages = PAGE_ALIGN(image_size)>>PAGE_SHIFT;

	if ( residual_fw_is_qemu( res ) ) {
		/* save command-line - QEMU firmware sets R6/R7 to
		 * commandline start/end (NON-PReP STD)
		 */
		int len = bd->r7 - bd->r6;
		if ( len > 0 ) {
			if ( len > sizeof(bd->cmd_line) - 1 )
				len = sizeof(bd->cmd_line) - 1;
			codemove(bd->cmd_line, bd->r6, len, bd->cache_lsize);
			bd->cmd_line[len] = 0;
		}
	}

	/* Fix residual if we are loaded by Motorola NT firmware */
	if ( res && res->VitalProductData.FirmwareSupplier == 0x10000 )
	    fix_residual( res );

	/* FIXME: if OF we should do something different */
	if( !bd->of_entry && res &&
	   res->ResidualLength <= sizeof(RESIDUAL) && res->Version == 0 ) {
		u_long lowpage=ULONG_MAX, highpage;
		u_long imghigh=0, stkhigh=0;
		/* Find the highest and large enough contiguous zone
		   consisting of free and BootImage sections. */
		/* Find 3 free areas of memory, one for the main image, one
		 * for the stack (STACK_PAGES), and page one to put the map
		 * structures. They are allocated from the top of memory.
		 * In most cases the stack will be put just below the image.
		 */
		while((highpage =
		       find_next_zone(res, lowpage, BootImage|Free))) {
			lowpage=find_zone_start(res, highpage, BootImage|Free);
			if ((highpage-lowpage)>minpages &&
			    highpage>imghigh) {
				imghigh=highpage;
				highpage -=minpages;
			}
			if ((highpage-lowpage)>STACK_PAGES &&
			    highpage>stkhigh) {
				stkhigh=highpage;
				highpage-=STACK_PAGES;
			}
		}

		bd->image = (void *)((imghigh-minpages)<<PAGE_SHIFT);
		bd->stack=(void *) (stkhigh<<PAGE_SHIFT);

		/* The code mover is put at the lowest possible place
		 * of free memory. If this corresponds to the loaded boot
		 * partition image it does not matter because it overrides
		 * the unused part of it (x86 code).
		 */
		bd->mover=(void *) (lowpage<<PAGE_SHIFT);

		/* Let us flush the caches in all cases. After all it should
		 * not harm even on 601 and we don't care about performance.
		 * Right now it's easy since all processors have a line size
		 * of 32 bytes. Once again residual data has proved unreliable.
		 */
		bd->cache_lsize = 32;
	}
	/* For now we always assume that it's succesful, we should
	 * handle better the case of insufficient memory.
	 */
	return 0;
}

void * valloc(u_long size) {
	map *p, *q;
	struct _mm_private * mm = (struct _mm_private *) bd->mm_private;

	if (size==0) return NULL;
	size=PAGE_ALIGN(size)-1;
	for (p=mm->virtavail; p; p=p->next) {
		if (p->base+size <= p->end) break;
	}
	if(!p) return NULL;
	q=alloc_map();
	q->base=p->base;
	q->end=q->base+size;
	q->firstpte=MAP_USED_VIRT;
	insert_map(&mm->virtused, q);
	if (q->end==p->end) free_map(remove_map(&mm->virtavail, p));
	else p->base += size+1;
	return (void *)q->base;
}

static
void vflush(map *virtmap) {
	struct _mm_private * mm = (struct _mm_private *) bd->mm_private;
	u_long i, limit=(mm->hashmask>>3)+8;
	hash_entry volatile *p=(hash_entry *) mm->sdr1;

	/* PTE handling is simple since the processor never update
	 * the entries. Writable pages always have the C bit set and
	 * all valid entries have the R bit set. From the processor
	 * point of view the hash table is read only.
	 */
	for (i=0; i<limit; i++) {
	  	if (p[i].key<0) {
			u_long va;
			va = ((i<<9)^((p[i].key)<<5)) &0x3ff000;
			if (p[i].key&0x40) va^=0x3ff000;
			va |= ((p[i].key<<21)&0xf0000000)
			  | ((p[i].key<<22)&0x0fc00000);
			if (va>=virtmap->base && va<=virtmap->end) {
				p[i].key=0;
				asm volatile("sync; tlbie %0; sync" : :
					     "r" (va));
			}
		}
	}
}

void vfree(void *vaddr) {
	map *physmap, *virtmap; /* Actual mappings pertaining to this vm */
	struct _mm_private * mm = (struct _mm_private *) bd->mm_private;

	/* Flush memory queues */
	asm volatile("sync": : : "memory");

	virtmap = remove_map_at(&mm->virtused, vaddr);
	if (!virtmap) return;

	/* Remove mappings corresponding to virtmap */
	for (physmap=mm->mappings; physmap; ) {
		map *nextmap=physmap->next;
		if (physmap->base>=virtmap->base
		    && physmap->base<virtmap->end) {
			free_map(remove_map(&mm->mappings, physmap));
		}
		physmap=nextmap;
	}

	vflush(virtmap);

	virtmap->firstpte= MAP_FREE_VIRT;
	insert_map(&mm->virtavail, virtmap);
	coalesce_maps(mm->virtavail);
}

void vunmap(void *vaddr) {
	map *physmap, *virtmap; /* Actual mappings pertaining to this vm */
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	/* Flush memory queues */
	asm volatile("sync": : : "memory");

	/* vaddr must be within one of the vm areas in use and
	 * then must correspond to one of the physical areas
	 */
	for (virtmap=mm->virtused; virtmap; virtmap=virtmap->next) {
	  	if (virtmap->base<=(u_long)vaddr &&
		    virtmap->end>=(u_long)vaddr) break;
	}
	if (!virtmap) return;

	physmap = remove_map_at(&mm->mappings, vaddr);
	if(!physmap) return;
	vflush(physmap);
	free_map(physmap);
}

int vmap(void *vaddr, u_long p, u_long size) {
	map *q;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	size=PAGE_ALIGN(size);
	if(!size) return 1;
	/* Check that the requested area fits in one vm image */
	for (q=mm->virtused; q; q=q->next) {
	  	if ((q->base <= (u_long)vaddr) &&
		    (q->end>=(u_long)vaddr+size -1)) break;
	}
	if (!q) return 1;
	q= alloc_map();
	if (!q) return 1;
	q->base = (u_long)vaddr;
	q->end = (u_long)vaddr+size-1;
	q->firstpte = p;
	return insert_map(&mm->mappings, q);
}

static
void create_identity_mappings(int type, int attr) {
	u_long lowpage=ULONG_MAX, highpage;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	RESIDUAL * res=bd->residual;

	while((highpage = find_next_zone(res, lowpage, type))) {
		map *p;
		lowpage=find_zone_start(res, highpage, type);
		p=alloc_map();
		/* Do not map page 0 to catch null pointers */
		lowpage = lowpage ? lowpage : 1;
		p->base=lowpage<<PAGE_SHIFT;
		p->end=(highpage<<PAGE_SHIFT)-1;
		p->firstpte = (lowpage<<PAGE_SHIFT)|attr;
		insert_map(&mm->mappings, p);
	}
}

static inline
void add_free_map(u_long base, u_long end) {
	map *q=NULL;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	if (base<end) q=alloc_map();
	if (!q) return;
	q->base=base;
	q->end=end-1;
	q->firstpte=MAP_FREE_VIRT;
	insert_map(&mm->virtavail, q);
}

static inline
void create_free_vm(void) {
	map *p;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	u_long vaddr=PAGE_SIZE;	/* Never map vaddr 0 */
	for(p=mm->mappings; p; p=p->next) {
		add_free_map(vaddr, p->base);
		vaddr=p->end+1;
	}
	/* Special end of memory case */
	if (vaddr) add_free_map(vaddr,0);
}

/* Memory management initialization.
 * Set up the mapping lists.
 */

static inline
void add_perm_map(u_long start, u_long size) {
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	map *p=alloc_map();
	p->base = start;
	p->end = start + size - 1;
	p->firstpte = MAP_PERM_PHYS;
	insert_map(& mm->physperm , p);
}

void mm_init(u_long image_size)
{
	u_long lowpage=ULONG_MAX, highpage;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	RESIDUAL * res=bd->residual;
	int i;
	map *p;

	/* The checks are simplified by the fact that the image
	 * and stack area are always allocated at the upper end
	 * of a free block.
	 */
	while((highpage = find_next_zone(res, lowpage, BootImage|Free))) {
		lowpage=find_zone_start(res, highpage, BootImage|Free);
		if ( ( ((u_long)bd->image+PAGE_ALIGN(image_size))>>PAGE_SHIFT)
		     == highpage) {
		  	highpage=(u_long)(bd->image)>>PAGE_SHIFT;
			add_perm_map((u_long)bd->image, image_size);
		}
		if ( (( u_long)bd->stack>>PAGE_SHIFT) == highpage) {
		  	highpage -= STACK_PAGES;
			add_perm_map(highpage<<PAGE_SHIFT,
				     STACK_PAGES*PAGE_SIZE);
		}
		/* Protect the interrupt handlers that we need ! */
		if (lowpage<2) lowpage=2;
		/* Check for the special case of full area! */
		if (highpage>lowpage) {
			p = alloc_map();
			p->base = lowpage<<PAGE_SHIFT;
			p->end = (highpage<<PAGE_SHIFT)-1;
			p->firstpte=MAP_FREE_PHYS;
			insert_map(&mm->physavail, p);
		}
	}

	/* Allocate the hash table */
	mm->sdr1=__palloc(0x10000, PA_PERM|16);
	_write_SDR1((u_long)mm->sdr1);
	memset(mm->sdr1, 0, 0x10000);
	mm->hashmask = 0xffc0;

	/* Setup the segment registers as we want them */
	for (i=0; i<16; i++) _write_SR(i, (void *)(i<<28));
	/* Create the maps for the physical memory, firwmarecode does not
	 * seem to be necessary. ROM is mapped read-only to reduce the risk
	 * of reprogramming it because it's often Flash and some are
	 * amazingly easy to overwrite.
	 */
	create_identity_mappings(BootImage|Free|FirmwareCode|FirmwareHeap|
				 FirmwareStack, PTE_RAM);
	create_identity_mappings(SystemROM, PTE_ROM);
	create_identity_mappings(IOMemory|SystemIO|SystemRegs|
				 PCIAddr|PCIConfig|ISAAddr, PTE_IO);

	create_free_vm();

	/* Install our own MMU and trap handlers. */
	codemove((void *) 0x300, _handler_glue, 0x100, bd->cache_lsize);
	codemove((void *) 0x400, _handler_glue, 0x100, bd->cache_lsize);
	codemove((void *) 0x600, _handler_glue, 0x100, bd->cache_lsize);
	codemove((void *) 0x700, _handler_glue, 0x100, bd->cache_lsize);
}

void * salloc(u_long size) {
	map *p, *q;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	if (size==0) return NULL;

	size = (size+7)&~7;

	for (p=mm->sallocfree; p; p=p->next) {
		if (p->base+size <= p->end) break;
	}
	if(!p) {
		void *m;
		m = __palloc(size, PA_SUBALLOC);
		p = alloc_map();
		if (!m && !p) return NULL;
		p->base = (u_long) m;
		p->firstpte = MAP_FREE_SUBS;
		p->end = (u_long)m+PAGE_ALIGN(size)-1;
		insert_map(&mm->sallocfree, p);
		coalesce_maps(mm->sallocfree);
		coalesce_maps(mm->sallocphys);
	};
	q=alloc_map();
	q->base=p->base;
	q->end=q->base+size-1;
	q->firstpte=MAP_USED_SUBS;
	insert_map(&mm->sallocused, q);
	if (q->end==p->end) free_map(remove_map(&mm->sallocfree, p));
	else p->base += size;
	memset((void *)q->base, 0, size);
	return (void *)q->base;
}

void sfree(void *p) {
	map *q;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	q=remove_map_at(&mm->sallocused, p);
	if (!q) return;
	q->firstpte=MAP_FREE_SUBS;
	insert_map(&mm->sallocfree, q);
	coalesce_maps(mm->sallocfree);
}

/* first/last area fit, flags is a power of 2 indicating the required
 * alignment. The algorithms are stupid because we expect very little
 * fragmentation of the areas, if any. The unit of allocation is the page.
 * The allocation is by default performed from higher addresses down,
 * unless flags&PA_LOW is true.
 */

void * __palloc(u_long size, int flags)
{
	u_long mask = ((1<<(flags&PA_ALIGN_MASK))-1);
	map *newmap, *frommap, *p, *splitmap=0;
	map **queue;
	u_long qflags;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;

	/* Asking for a size which is not a multiple of the alignment
	   is likely to be an error. */

	if (size & mask) return NULL;
	size = PAGE_ALIGN(size);
	if(!size) return NULL;

	if (flags&PA_SUBALLOC) {
		queue = &mm->sallocphys;
		qflags = MAP_SUBS_PHYS;
	} else if (flags&PA_PERM) {
	  	queue = &mm->physperm;
		qflags = MAP_PERM_PHYS;
	} else {
	  	queue = &mm->physused;
		qflags = MAP_USED_PHYS;
	}
	/* We need to allocate that one now so no two allocations may attempt
	 * to take the same memory simultaneously. Alloc_map_page does
	 * not call back here to avoid infinite recursion in alloc_map.
	 */

	if (mask&PAGE_MASK) {
		splitmap=alloc_map();
		if (!splitmap) return NULL;
	}

	for (p=mm->physavail, frommap=NULL; p; p=p->next) {
		u_long high = p->end;
		u_long limit  = ((p->base+mask)&~mask) + size-1;
		if (high>=limit && ((p->base+mask)&~mask)+size>p->base) {
			frommap = p;
			if (flags&PA_LOW) break;
		}
	}

	if (!frommap) {
		if (splitmap) free_map(splitmap);
		return NULL;
	}

	newmap=alloc_map();

	if (flags&PA_LOW) {
		newmap->base = (frommap->base+mask)&~mask;
	} else {
	  	newmap->base = (frommap->end +1 - size) & ~mask;
	}

	newmap->end = newmap->base+size-1;
	newmap->firstpte = qflags;

	/* Add a fragment if we don't allocate until the end. */

	if (splitmap) {
		splitmap->base=newmap->base+size;
		splitmap->end=frommap->end;
		splitmap->firstpte= MAP_FREE_PHYS;
		frommap->end=newmap->base-1;
	} else if (flags & PA_LOW) {
		frommap->base=newmap->base+size;
	} else {
	  	frommap->end=newmap->base-1;
	}

        /* Remove a fragment if it becomes empty. */
 	if (frommap->base == frommap->end+1) {
		free_map(remove_map(&mm->physavail, frommap));
	}

	if (splitmap) {
	  	if (splitmap->base == splitmap->end+1) {
			free_map(remove_map(&mm->physavail, splitmap));
		} else {
			insert_map(&mm->physavail, splitmap);
		}
	}

	insert_map(queue, newmap);
	return (void *) newmap->base;

}

void pfree(void * p) {
	map *q;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	q=remove_map_at(&mm->physused, p);
	if (!q) return;
	q->firstpte=MAP_FREE_PHYS;
	insert_map(&mm->physavail, q);
	coalesce_maps(mm->physavail);
}

#ifdef DEBUG
/* Debugging functions */
void print_maps(map *chain, const char *s) {
	map *p;
	printk("%s",s);
	for(p=chain; p; p=p->next) {
		printk("    %08lx-%08lx: %08lx\n",
		       p->base, p->end, p->firstpte);
	}
}

void print_all_maps(const char * s) {
	u_long freemaps;
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	map *free;
	printk("%s",s);
	print_maps(mm->mappings, "  Currently defined mappings:\n");
	print_maps(mm->physavail, "  Currently available physical areas:\n");
	print_maps(mm->physused, "  Currently used physical areas:\n");
	print_maps(mm->virtavail, "  Currently available virtual areas:\n");
	print_maps(mm->virtused, "  Currently used virtual areas:\n");
	print_maps(mm->physperm, "  Permanently used physical areas:\n");
	print_maps(mm->sallocphys, "  Physical memory used for salloc:\n");
	print_maps(mm->sallocfree, "  Memory available for salloc:\n");
	print_maps(mm->sallocused, "  Memory allocated through salloc:\n");
	for (freemaps=0, free=mm->freemaps; free; freemaps++, free=free->next);
	printk("  %ld free maps.\n", freemaps);
}

void print_hash_table(void) {
	struct _mm_private *mm = (struct _mm_private *) bd->mm_private;
	hash_entry *p=(hash_entry *) mm->sdr1;
	u_int i, valid=0;
	for (i=0; i<((mm->hashmask)>>3)+8; i++) {
		if (p[i].key<0) valid++;
	}
	printk("%u valid hash entries on pass 1.\n", valid);
	valid = 0;
	for (i=0; i<((mm->hashmask)>>3)+8; i++) {
		if (p[i].key<0) valid++;
	}
	printk("%u valid hash entries on pass 2.\n"
	       "     vpn:rpn_attr, p/s, pteg.i\n", valid);
	for (i=0; i<((mm->hashmask)>>3)+8; i++) {
	  	if (p[i].key<0) {
			u_int pteg=(i>>3);
			u_long vpn;
			vpn = (pteg^((p[i].key)>>7)) &0x3ff;
			if (p[i].key&0x40) vpn^=0x3ff;
			vpn |= ((p[i].key<<9)&0xffff0000)
			  | ((p[i].key<<10)&0xfc00);
			printk("%08lx:%08lx, %s, %5d.%d\n",
			       vpn,  p[i].rpn, p[i].key&0x40 ? "sec" : "pri",
			       pteg, i%8);
		}
	}
}

#endif
