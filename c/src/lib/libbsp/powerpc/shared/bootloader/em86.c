/*
 *  em86.c -- Include file for bootloader.
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

/*****************************************************************************
*
* Code to interpret Video BIOS ROM routines.
*
*
******************************************************************************/

/* These include are for the development version only */
#include <sys/types.h>
#include "pci.h"
#include <libcpu/byteorder.h>
#ifdef __BOOT__
#include "bootldr.h"
#include <limits.h>
#include <rtems/bspIo.h>
#endif

/* Code options,  put them on the compiler command line */
/* #define EIP_STATS */	/* EIP based profiling */
/* #undef EIP_STATS */

typedef union _reg_type1 {
 	unsigned e;
  	unsigned short x;
  	struct {
		unsigned char l, h;
	} lh;
} reg_type1;

typedef union _reg_type2 {
 	uint32_t e;
  	uint16_t x;
} reg_type2;

typedef struct _x86 {
	reg_type1
	  _eax, _ecx, _edx, _ebx;
	reg_type2
	  _esp, _ebp, _esi, _edi;
  	unsigned
	  es, cs, ss, ds, fs, gs, eip, eflags;
	unsigned char
	  *esbase, *csbase, *ssbase, *dsbase, *fsbase, *gsbase;
	volatile unsigned char *iobase;
	unsigned char *ioperm;
	unsigned
	  reason, nexteip, parm1, parm2, opcode, base;
	unsigned *optable, opreg; /* no more used! */
	unsigned char* vbase;
	unsigned instructions;
#ifdef __BOOT__
	u_char * ram;
	u_char * rom;
	struct pci_dev * dev;
#else
        unsigned filler[14]; /* Skip to  next 64 byte boundary */
        unsigned eipstats[32768][2];
#endif
} x86;

x86 v86_private __attribute__((aligned(32)));

/* Emulator is in another source file */
extern
void em86_enter(x86 * p);

#define EAX (p->_eax.e)
#define ECX (p->_ecx.e)
#define EDX (p->_edx.e)
#define EBX (p->_ebx.e)
#define ESP (p->_esp.e)
#define EBP (p->_ebp.e)
#define ESI (p->_esi.e)
#define EDI (p->_edi.e)
#define AX (p->_eax.x)
#define CX (p->_ecx.x)
#define DX (p->_edx.x)
#define BX (p->_ebx.x)
#define SP (p->_esp.x)
#define BP (p->_ebp.x)
#define SI (p->_esi.x)
#define DI (p->_edi.x)
#define AL (p->_eax.lh.l)
#define CL (p->_ecx.lh.l)
#define DL (p->_edx.lh.l)
#define BL (p->_ebx.lh.l)
#define AH (p->_eax.lh.h)
#define CH (p->_ecx.lh.h)
#define DH (p->_edx.lh.h)
#define BH (p->_ebx.lh.h)

/* Function used to debug */
#ifdef __BOOT__
#define printf printk
#endif
#ifdef DEBUG
static void dump86(x86 * p){
	unsigned char *s = p->csbase + p->eip;
	printf("cs:eip=%04x:%08x, eax=%08x, ecx=%08x, edx=%08x, ebx=%08x\n",
	       p->cs, p->eip, ld_le32(&EAX),
	       ld_le32(&ECX), ld_le32(&EDX), ld_le32(&EBX));
	printf("ss:esp=%04x:%08x, ebp=%08x, esi=%08x, edi=%08x, efl=%08x\n",
	       p->ss, ld_le32(&ESP), ld_le32(&EBP),
	       ld_le32(&ESI), ld_le32(&EDI), p->eflags);
	printf("nip=%08x, ds=%04x, es=%04x, fs=%04x, gs=%04x, total=%d\n",
	       p->nexteip, p->ds, p->es, p->fs, p->gs, p->instructions);
	printf("code: %02x %02x %02x %02x %02x %02x "
	       "%02x %02x %02x %02x %02x %02x\n",
	       s[0], s[1], s[2], s[3], s[4], s[5],
	       s[6], s[7], s[8], s[9], s[10], s[11]);
#ifndef __BOOT__
	printf("op1=%08x, op2=%08x, result=%08x, flags=%08x\n",
	       p->filler[11], p->filler[12], p->filler[13], p->filler[14]);
#endif
}
#else
#define dump86(x)
#endif

int bios86pci(x86 * p) {
	unsigned reg=ld_le16(&DI);
	reg_type2 tmp;

	if (AL>=8 && AL<=13 && reg>0xff) {
	  	AH = PCIBIOS_BAD_REGISTER_NUMBER;
	} else {
		switch(AL) {
		case 2:	/* find_device */
		  /* Should be improved for BIOS able to handle
		   * multiple devices. We simply suppose the BIOS
		   * inits a single device, and return an error
		   * if it tries to find more...
		   */
		  if (SI) {
		  	AH=PCIBIOS_DEVICE_NOT_FOUND;
		  } else {
		  	BH = p->dev->bus->number;
			BL = p->dev->devfn;
			AH = 0;
		  }
		  break;
		/*
		case 3: find_class not implemented for now.
		*/
		case 8:	      /* read_config_byte */
		  AH=pcibios_read_config_byte(BH, BL, reg, &CL);
		  break;
		case 9:       /* read_config_word */
		  AH=pcibios_read_config_word(BH, BL, reg, &tmp.x);
		  CX=ld_le16(&tmp.x);
		  break;
		case 10:      /* read_config_dword */
		  AH=pcibios_read_config_dword(BH, BL, reg, &tmp.e);
		  ECX=ld_le32(&tmp.e);
		  break;
		case 11:      /* write_config_byte */
		  AH=pcibios_write_config_byte(BH, BL, reg, CL);
		  break;
		case 12:      /* write_config_word */
		  AH=pcibios_write_config_word(BH, BL, reg, ld_le16(&CX));
		  break;
		case 13:      /* write_config_dword */
		  AH=pcibios_write_config_dword(
			BH, BL, reg, ld_le32((uint32_t *)&ECX));
		  break;
		default:
		  printf("Unimplemented or illegal PCI service call #%d!\n",
			 AL);
		  return 1;
		}
	}
	p->eip = p->nexteip;
	/* Set/clear carry according to result */
	if (AH) p->eflags |= 1; else p->eflags &=~1;
	return 0;
}

void push2(x86 *p, unsigned value) {
  	unsigned char * sbase= p->ssbase;
	unsigned newsp = (ld_le16(&SP)-2)&0xffff;
	st_le16(&SP,newsp);
	st_le16((unsigned short *)(sbase+newsp), value);
}

unsigned pop2(x86 *p) {
  	unsigned char * sbase=p->ssbase;
	unsigned oldsp = ld_le16(&SP);
	st_le16(&SP,oldsp+2);
	return ld_le16((unsigned short *)(sbase+oldsp));
}

int int10h(x86 * p) { /* Process BIOS video interrupt */
  	unsigned vector;
	vector=ld_le32((uint32_t *)p->vbase+0x10);
	if (((vector&0xffff0000)>>16)==0xc000) {
		push2(p, p->eflags);
		push2(p, p->cs);
		push2(p, p->nexteip);
		p->cs=vector>>16;
		p->csbase=p->vbase + (p->cs<<4);
		p->eip=vector&0xffff;
#if 1
		p->eflags&=0xfcff;  /* Clear AC/TF/IF */
#else
		p->eflags = (p->eflags&0xfcff)|0x100;  /* Set TF for debugging */
#endif
		/* p->eflags|=0x100; uncomment to force a trap */
		return(0);
	} else {
	  	switch(AH) {
		case 0x12:
		  switch(BL){
		  case 0x32:
		    p->eip=p->nexteip;
		    return(0);
		    break;
		  default:
		    break;
		  }
		default:
		  break;
		}
		printf("unhandled soft interrupt 0x10: vector=%x\n", vector);
		return(1);
	}
}

int process_softint(x86 * p) {
#if 0
  	if (p->parm1!=0x10 || AH!=0x0e) {
		printf("Soft interrupt\n");
		dump86(p);
	}
#endif
	switch(p->parm1) {
	case 0x10: /* BIOS video interrupt */
	  return int10h(p);
	case 0x1a:
	  if(AH==0xb1) return bios86pci(p);
	  break;
	default:
	  break;
	}
	dump86(p);
	printf("Unhandled soft interrupt number 0x%04x, AX=0x%04x\n",
	     p->parm1, ld_le16(&AX));
	return(1);
}

/* The only function called back by the emulator is em86_trap, all
   instructions may that change the code segment are trapped here.
   p->reason is one of the following codes.  */
#define code_zerdiv	0
#define code_trap	1
#define code_int3	3
#define code_into	4
#define code_bound	5
#define code_ud		6
#define code_dna	7

#define code_iretw	256
#define code_iretl	257
#define code_lcallw	258
#define code_lcalll	259
#define code_ljmpw	260
#define code_ljmpl	261
#define code_lretw	262
#define code_lretl	263
#define code_softint	264
#define code_lock	265	/* Lock prefix */
/* Codes 1024 to 2047 are used for I/O port access instructions:
 - The three LSB define the port size (1, 2 or 4)
 - bit of weight 512 means out if set, in if clear
 - bit of weight 256 means ins/outs if set, in/out if clear
 - bit of weight 128 means use esi/edi if set, si/di if clear
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

int em86_trap(x86 *p) {
#ifndef __BOOT__
	  int i;
	  unsigned char command[80];
	  unsigned char *verb, *t;
	  unsigned short *fp;
	  static unsigned char def=0;
	  static unsigned char * bptaddr=NULL;  /* Breakpoint address */
	  static unsigned char bptopc; /* Replaced breakpoint opcode */
	  unsigned char cmd;
	  unsigned tmp;
#endif
	  switch(p->reason) {
	  case code_int3:
#ifndef __BOOT__
	    if(p->csbase+p->eip == bptaddr) {
	      *bptaddr=bptopc;
	      bptaddr=NULL;
	    }
	    else printf("Unexpected ");
#endif
	    printf("Breakpoint Interrupt !\n");
	    /* Note that this fallthrough (no break;) is on purpose */
#ifdef __BOOT__
	    return 0;
#else
	  case code_trap:
	    dump86(p);
	    for(;;) {
	      	printf("b(reakpoint, g(o, q(uit, s(tack, t(race ? [%c] ", def);
		fgets(command,sizeof(command),stdin);
		verb = strtok(command," 	\n");
		if(verb) cmd=*verb; else cmd=def;
		def=0;
		switch(cmd) {
		case 'b':
		case 'B':
		  if(bptaddr) *bptaddr=bptopc;
		  t=strtok(0," 	\n");
		  i=sscanf(t,"%x",&tmp);
		  if(i==1) {
		    bptaddr=p->vbase + tmp;
		    bptopc=*bptaddr;
		    *bptaddr=0xcc;
		  } else bptaddr=NULL;
		  break;
		case 'q':
		case 'Q':
		  return 1;
		  break;

		case 'g':
		case 'G':
		  p->eflags &= ~0x100;
		  return 0;
		  break;

		case 's':
		case 'S': /* Print the 8 stack top words */
		  fp = (unsigned short *)(p->ssbase+ld_le16(&SP));
		  printf("Stack [%04x:%04x]: %04x %04x %04x %04x %04x %04x %04x %04x\n",
			 p->ss, ld_le16(&SP),
			 ld_le16(fp+0), ld_le16(fp+1), ld_le16(fp+2), ld_le16(fp+3),
			 ld_le16(fp+4), ld_le16(fp+5), ld_le16(fp+6), ld_le16(fp+7));
		  break;
		case 't':
		case 'T':
		  p->eflags |= 0x10100;  /* Set the resume and trap flags */
		  def='t';
		  return 0;
		  break;
		  /* Should add some code to edit registers */
		}
	    }
#endif
	    break;
	  case code_ud:
	    printf("Attempt to execute an unimplemented"
		   "or undefined opcode!\n");
	    dump86(p);
	    return(1); /* exit interpreter */
	    break;
	  case code_dna:
	    printf("Attempt to execute a floating point instruction!\n");
	    dump86(p);
	    return(1);
	    break;
	  case code_softint:
	    return process_softint(p);
	    break;
	  case code_iretw:
	    p->eip=pop2(p);
	    p->cs=pop2(p);
	    p->csbase=p->vbase + (p->cs<<4);
	    p->eflags= (p->eflags&0xfffe0000)|pop2(p);
	    /* p->eflags|= 0x100; */ /* Uncomment to trap after iretws */
	    return(0);
	    break;
#ifndef __BOOT__
	  case code_inb:
	  case code_inw:
	  case code_inl:
	  case code_insb_a16:
	  case code_insw_a16:
	  case code_insl_a16:
	  case code_insb_a32:
	  case code_insw_a32:
	  case code_insl_a32:
	  case code_outb:
	  case code_outw:
	  case code_outl:
	  case code_outsb_a16:
	  case code_outsw_a16:
	  case code_outsl_a16:
	  case code_outsb_a32:
	  case code_outsw_a32:
	  case code_outsl_a32:
	    /* For now we simply enable I/O to the ports and continue */
	    for(i=p->parm1; i<p->parm1+(p->reason&7); i++) {
	      p->ioperm[i/8] &= ~(1<<i%8);
	    }
	    printf("Access to ports %04x-%04x enabled.\n",
		   p->parm1, p->parm1+(p->reason&7)-1);
	    return(0);
#endif
	  case code_lretw:
	    /* Check for the exit eyecatcher */
	    if ( *(u_int *)(p->ssbase+ld_le16(&SP)) == UINT_MAX) return 1;
	    /* No break on purpose */
	  default:
	    dump86(p);
	    printf("em86_trap called with unhandled reason code !\n");
	    return(1);

	  }
}

void cleanup_v86_mess(void) {
	x86 *p = (x86 *) bd->v86_private;

	/* This automatically removes the mappings ! */
	vfree(p->vbase);
	p->vbase = 0;
	pfree(p->ram);
	p->ram = 0;
	sfree(p->ioperm);
	p->ioperm=0;
}

int init_v86(void) {
	x86 *p = (x86 *) bd->v86_private;

	/* p->vbase is non null when the v86 is properly set-up */
	if (p->vbase) return 0;

	/* Set everything to 0 */
	memset(p, 0, sizeof(*p));
	p->ioperm = salloc(65536/8+1);
	p->ram = palloc(0xa0000);
	p->iobase = ptr_mem_map->io_base;

	if (!p->ram || !p->ioperm) return 1;

	/* The ioperm array must have an additional byte at the end ! */
	p->ioperm[65536/8] = 0xff;

	p->vbase = valloc(0x110000);
	if (!p->vbase) return 1;

	/* These calls should never fail. */
	vmap(p->vbase, (u_long)p->ram|PTE_RAM, 0xa0000);
	vmap(p->vbase+0x100000, (u_long)p->ram|PTE_RAM, 0x10000);
	vmap(p->vbase+0xa0000,
	     ((u_long)ptr_mem_map->isa_mem_base+0xa0000)|PTE_IO, 0x20000);
	return 0;
}

void em86_main(struct pci_dev *dev){
	x86 *p = (x86 *) bd->v86_private;
	u_short signature;
	u_char length;
	volatile u_int *src;
	u_int *dst, left;
	uint32_t saved_rom;
#if defined(MONITOR_IO) && !defined(__BOOT__)
#define IOMASK 0xff
#else
#define IOMASK 0
#endif

#ifndef __BOOT__
	int i;
	/* Allow or disable access to all ports */
	for(i=0; i<65536/8; i++) p->ioperm[i]=IOMASK;
	p->ioperm[i] = 0xff; /* Last unused byte must have this value */
#endif
	p->dev = dev;
	memset(p->vbase, 0, 0xa0000);
	/* Set up a few registers */
	p->cs = 0xc000; p->csbase = p->vbase + 0xc0000;
	p->ss = 0x1000; p->ssbase = p->vbase + 0x10000;
	p->eflags=0x200;
	st_le16(&SP,0xfffc); p->eip=3;

	p->dsbase = p->esbase = p->fsbase = p->gsbase = p->vbase;

	/* Follow the PCI BIOS specification */
	AH=dev->bus->number;
	AL=dev->devfn;

	/* All other registers are irrelevant except ES:DI which
	 * should point to a PnP installation check block. This
	 * is not yet implemented due to lack of references. */

	/* Store a return address of 0xffff:0xffff as eyecatcher */
	*(u_int *)(p->ssbase+ld_le16(&SP)) = UINT_MAX;

	/* Interrupt for BIOS EGA services is 0xf000:0xf065 (int 0x10) */
	st_le32((uint32_t *)p->vbase + 0x10, 0xf000f065);

	/* Enable the ROM, read it and disable it immediately */
	pci_bootloader_read_config_dword(dev, PCI_ROM_ADDRESS, &saved_rom);
	pci_bootloader_write_config_dword(dev, PCI_ROM_ADDRESS, 0x000c0001);

	/* Check that there is an Intel ROM. Should we also check that
	 * the first instruction is a jump (0xe9 or 0xeb) ?
	 */
	signature = *(u_short *)(ptr_mem_map->isa_mem_base+0xc0000);
	if (signature!=0x55aa) {
	  	printf("bad signature: %04x.\n", signature);
		return;
	}
	/* Allocate memory and copy the video rom to vbase+0xc0000; */
	length = ptr_mem_map->isa_mem_base[0xc0002];
	p->rom = palloc(length*512);
	if (!p->rom) return;

	for(dst=(u_int *) p->rom,
	    src=(volatile u_int *)(ptr_mem_map->isa_mem_base+0xc0000),
	    left = length*512/sizeof(u_int);
	    left--;
	    *dst++=*src++);

	/* Disable the ROM and map the copy in virtual address space, note
	 * that the ROM has to be mapped as RAM since some BIOSes (at least
	 * Cirrus) perform write accesses to their own ROM. The reason seems
	 * to be that they check that they must execute from shadow RAM
	 * because accessing the ROM prevents accessing the video RAM
	 * according to comments in linux/arch/alpha/kernel/bios32.c.
	 */

	pci_bootloader_write_config_dword(dev, PCI_ROM_ADDRESS, saved_rom);
	vmap(p->vbase+0xc0000, (u_long)p->rom|PTE_RAM, length*512);

	/* Now actually emulate the ROM init routine */
	em86_enter(p);

	/* Free the acquired resources */
	vunmap(p->vbase+0xc0000);
	pfree(p->rom);
}
