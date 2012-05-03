/*
 *  head.S -- Bootloader Entry point
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

#include <rtems/system.h>
#include <sys/types.h>
#include <string.h>
#include "bootldr.h"
#include <libcpu/spr.h>
#include "zlib.h"
#include <libcpu/page.h>
#include <libcpu/byteorder.h>
#include <rtems/bspIo.h>
#include <bsp.h>

SPR_RO(PPC_PVR)

struct inode;
struct wait_queue;
struct buffer_head;
typedef struct { int counter; } atomic_t;

typedef struct page {
	/* these must be first (free area handling) */
	struct page *next;
	struct page *prev;
	struct inode *inode;
	unsigned long offset;
	struct page *next_hash;
	atomic_t count;
	unsigned long flags;	/* atomic flags, some possibly updated asynchronously */
	struct wait_queue *wait;
	struct page **pprev_hash;
	struct buffer_head * buffers;
} mem_map_t;

extern opaque mm_private, pci_private, v86_private, console_private;

#define CONSOLE_ON_SERIAL	"console=ttyS0"

extern struct console_io vacuum_console_functions;
extern opaque log_console_setup, serial_console_setup, vga_console_setup;

boot_data __bd = {0, 0, 0, 0, 0, 0, 0, 0,
		  32, 0, 0, 0, 0, 0, 0,
		  &mm_private,
		  NULL,
		  &pci_private,
		  NULL,
		  &v86_private,
		  "root=/dev/hdc1"
		 };

static void exit(void) __attribute__((noreturn));

static void exit(void) {
	printk("\nOnly way out is to press the reset button!\n");
	asm volatile("": : :"memory");
	while(1);
}

void hang(const char *s, u_long x, ctxt *p) {
	u_long *r1;
#ifdef DEBUG
	print_all_maps("\nMemory mappings at exception time:\n");
#endif
	printk("%s %lx NIP: %p LR: %p\n"
	       "Callback trace (stack:return address)\n",
	       s, x, (void *) p->nip, (void *) p->lr);
	asm volatile("lwz %0,0(1); lwz %0,0(%0); lwz %0,0(%0)": "=b" (r1));
	while(r1) {
		printk("  %p:%p\n", r1, (void *) r1[1]);
		r1 = (u_long *) *r1;
	}
	exit();
};

void *zalloc(void *x, unsigned items, unsigned size)
{
	void *p = salloc(items*size);

	if (!p) {
		printk("oops... not enough memory for gunzip\n");
	}
	return p;
}

void zfree(void *x, void *addr, unsigned nb)
{
	sfree(addr);
}

#define HEAD_CRC	2
#define EXTRA_FIELD	4
#define ORIG_NAME	8
#define COMMENT		0x10
#define RESERVED	0xe0

#define DEFLATED	8

void gunzip(void *dst, int dstlen, unsigned char *src, int *lenp)
{
	z_stream s;
	int r, i, flags;

	/* skip header */
	i = 10;
	flags = src[3];
	if (src[2] != DEFLATED || (flags & RESERVED) != 0) {
		printk("bad gzipped data\n");
		exit();
	}
	if ((flags & EXTRA_FIELD) != 0)
		i = 12 + src[10] + (src[11] << 8);
	if ((flags & ORIG_NAME) != 0)
		while (src[i++] != 0)
			;
	if ((flags & COMMENT) != 0)
		while (src[i++] != 0)
			;
	if ((flags & HEAD_CRC) != 0)
		i += 2;
	if (i >= *lenp) {
		printk("gunzip: ran out of data in header\n");
		exit();
	}

	s.zalloc = zalloc;
	s.zfree = zfree;
	r = inflateInit2(&s, -MAX_WBITS);
	if (r != Z_OK) {
		printk("inflateInit2 returned %d\n", r);
		exit();
	}
	s.next_in = src + i;
	s.avail_in = *lenp - i;
	s.next_out = dst;
	s.avail_out = dstlen;
	r = inflate(&s, Z_FINISH);
	if (r != Z_OK && r != Z_STREAM_END) {
		printk("inflate returned %d\n", r);
		exit();
	}
	*lenp = s.next_out - (unsigned char *) dst;
	inflateEnd(&s);
}

void decompress_kernel(int kernel_size, void * zimage_start, int len,
		       void * initrd_start, int initrd_len ) {
	u_char *parea;
	RESIDUAL* rescopy;
	int zimage_size= len;

	/* That's a mess, we have to copy the residual data twice just in
	 * case it happens to be in the low memory area where the kernel
	 * is going to be unpacked. Later we have to copy it back to
	 * lower addresses because only the lowest part of memory is mapped
	 * during boot.
	 */
	parea=__palloc(kernel_size, PA_LOW);
	if(!parea) {
		printk("Not enough memory to uncompress the kernel.");
		exit();
	}

	rescopy=salloc(sizeof(RESIDUAL));
	/* Let us hope that residual data is aligned on word boundary */
	*rescopy =  *bd->residual;
	bd->residual = (void *)PAGE_ALIGN(kernel_size);

	/* Note that this clears the bss as a side effect, so some code
	 * with ugly special case for SMP could be removed from the kernel!
	 */
	memset(parea, 0, kernel_size);
	printk("\nUncompressing the kernel...\n");

	gunzip(parea, kernel_size, zimage_start, &zimage_size);

	bd->of_entry = 0;
	bd->load_address = 0;
	bd->r6 = (char *)bd->residual+PAGE_ALIGN(sizeof(RESIDUAL));
	bd->r7 = bd->r6+strlen(bd->cmd_line);
 	if ( initrd_len ) {
	  	/* We have to leave some room for the hash table and for the
		 * whole array of struct page. The hash table would be better
		 * located at the end of memory if possible. With some bridges
		 * DMA from the last pages of memory is slower because
		 * prefetching from PCI has to be disabled to avoid accessing
		 * non existing memory. So it is the ideal place to put the
		 * hash table.
		 */
	        unsigned tmp = rescopy->TotalMemory;
		/* It's equivalent to tmp & (-tmp), but using the negation
		 * operator on unsigned variables looks so ugly.
		 */
		if ((tmp & (~tmp+1)) != tmp) tmp <<= 1; /* Next power of 2 */
		tmp /= 256; /* Size of hash table */
		if (tmp> (2<<20)) tmp=2<<20;
		tmp = tmp*2 + 0x40000; /* Alignment can double size + 256 kB */
		tmp += (rescopy->TotalMemory / PAGE_SIZE)
		  	       * sizeof(struct page);
 		bd->load_address = (void *)PAGE_ALIGN((int)bd->r7 + tmp);
 		bd->of_entry = (char *)bd->load_address+initrd_len;
 	}
#ifdef DEBUG
	printk("Kernel at 0x%p, size=0x%x\n", NULL, kernel_size);
 	printk("Initrd at 0x%p, size=0x%x\n",bd->load_address, initrd_len);
	printk("Residual data at 0x%p\n", bd->residual);
	printk("Command line at 0x%p\n",bd->r6);
#endif
	printk("done\nNow booting...\n");
	MMUoff();	/* We need to access address 0 ! */
	codemove(0, parea, kernel_size, bd->cache_lsize);
	codemove(bd->residual, rescopy, sizeof(RESIDUAL), bd->cache_lsize);
	codemove(bd->r6, bd->cmd_line, sizeof(bd->cmd_line), bd->cache_lsize);
	/* codemove checks for 0 length */
	codemove(bd->load_address, initrd_start, initrd_len, bd->cache_lsize);
}

static int ticks_per_ms=0;

/* this is from rtems_bsp_delay from libcpu */
void
boot_udelay(uint32_t   _microseconds)
{
   uint32_t   start, ticks, now;

   ticks = _microseconds * ticks_per_ms / 1000;
   CPU_Get_timebase_low( start );
   do {
     CPU_Get_timebase_low( now );
   } while (now - start < ticks);
}

void
setup_hw(void)
{
	char *cp, ch;
	register RESIDUAL * res;
	/* PPC_DEVICE * nvram; */
	struct pci_dev *default_vga;
	int timer, err;
	u_short default_vga_cmd;
	static unsigned int indic;

	indic = 0;

	res=bd->residual;
	default_vga=NULL;
	default_vga_cmd = 0;

#define vpd res->VitalProductData
	if (_read_PPC_PVR()>>16 != 1) {
		if ( res && vpd.ProcessorBusHz ) {
			ticks_per_ms = vpd.ProcessorBusHz/
			    (vpd.TimeBaseDivisor ? vpd.TimeBaseDivisor : 4000);
		} else {
			ticks_per_ms = 16500; /* assume 66 MHz on bus */
		}
	}

	select_console(CONSOLE_LOG);

	/* We check that the keyboard is present and immediately
	 * select the serial console if not.
	 */
#if defined(BSP_KBD_IOBASE)
	err = kbdreset();
        if (err) select_console(CONSOLE_SERIAL);
#else
	err = 1;
	select_console(CONSOLE_SERIAL);
#endif

	printk("\nModel: %s\nSerial: %s\n"
	       "Processor/Bus frequencies (Hz): %ld/%ld\n"
	       "Time Base Divisor: %ld\n"
	       "Memory Size: %lx\n"
		   "Residual: %lx (length %u)\n",
 	       vpd.PrintableModel,
	       vpd.Serial,
	       vpd.ProcessorHz,
               vpd.ProcessorBusHz,
	       (vpd.TimeBaseDivisor ? vpd.TimeBaseDivisor : 4000),
	       res->TotalMemory,
		   (unsigned long)res, res->ResidualLength);

	/* This reconfigures all the PCI subsystem */
        pci_init();

	/* The Motorola NT firmware does not set the correct mem size */
	if ( vpd.FirmwareSupplier == 0x10000 ) {
		int memsize;
		memsize = find_max_mem(bd->pci_devices);
		if ( memsize != res->TotalMemory ) {
			printk("Changed Memory size from %lx to %x\n",
				res->TotalMemory, memsize);
			res->TotalMemory = memsize;
			res->GoodMemory = memsize;
		}
	}
#define	ENABLE_VGA_USAGE
#undef ENABLE_VGA_USAGE
#ifdef ENABLE_VGA_USAGE
	/* Find the primary VGA device, chosing the first one found
	 * if none is enabled. The basic loop structure has been copied
	 * from linux/drivers/char/bttv.c by Alan Cox.
	 */
	for (p = bd->pci_devices; p; p = p->next) {
		u_short cmd;
		if (p->class != PCI_CLASS_NOT_DEFINED_VGA &&
		    ((p->class) >> 16 != PCI_BASE_CLASS_DISPLAY))
		  	continue;
		if (p->bus->number != 0) {
			printk("VGA device not on bus 0 not initialized!\n");
			continue;
		}
		/* Only one can be active in text mode, which for now will
		 * be assumed as equivalent to having I/O response enabled.
		 */
		pci_bootloader_read_config_word(p, PCI_COMMAND, &cmd);
		if(cmd & PCI_COMMAND_IO || !default_vga) {
		  	default_vga=p;
			default_vga_cmd=cmd;
		}
	}

	/* Disable the enabled VGA device, if any. */
	if (default_vga)
		pci_bootloader_write_config_word(default_vga, PCI_COMMAND,
				      default_vga_cmd&
				      ~(PCI_COMMAND_IO|PCI_COMMAND_MEMORY));
	init_v86();
	/* Same loop copied from bttv.c, this time doing the serious work */
	for (p = bd->pci_devices; p; p = p->next) {
		u_short cmd;
		if (p->class != PCI_CLASS_NOT_DEFINED_VGA &&
		    ((p->class) >> 16 != PCI_BASE_CLASS_DISPLAY))
		  	continue;
		if (p->bus->number != 0) continue;
		pci_bootloader_read_config_word(p, PCI_COMMAND, &cmd);
		pci_bootloader_write_config_word(p, PCI_COMMAND,
				      cmd|PCI_COMMAND_IO|PCI_COMMAND_MEMORY);
		printk("Calling the emulator.\n");
		em86_main(p);
		pci_bootloader_write_config_word(p, PCI_COMMAND, cmd);
	}

	cleanup_v86_mess();
#endif
	/* Reenable the primary VGA device */
	if (default_vga) {
		pci_bootloader_write_config_word(default_vga, PCI_COMMAND,
				      default_vga_cmd|
				      (PCI_COMMAND_IO|PCI_COMMAND_MEMORY));
		if (err) {
			printk("Keyboard error %d, using serial console!\n",
			       err);
		} else {
			select_console(CONSOLE_VGA);
		}
	} else if (!err) {
		select_console(CONSOLE_SERIAL);
		if (bd->cmd_line[0] == '\0') {
		  strcat(&bd->cmd_line[0], CONSOLE_ON_SERIAL);
		}
		else {
		  int s = strlen (bd->cmd_line);
		  bd->cmd_line[s + 1] = ' ';
		  bd->cmd_line[s + 2] = '\0';
		  strcat(&bd->cmd_line[0], CONSOLE_ON_SERIAL);
		}
	}
#if 0
	/* In the future we may use the NVRAM to store default
	 * kernel parameters.
	 */
	nvram=residual_find_device(~0UL, NULL, SystemPeripheral, NVRAM,
				   ~0UL, 0);
	if (nvram) {
		PnP_TAG_PACKET * pkt;
		switch (nvram->DevId.Interface) {
		case IndirectNVRAM:
			  pkt=PnP_find_packet(res->DevicePnpHeap
				      +nvram->AllocatedOffset,
					      )
		}
	}
#endif

	printk("\nRTEMS 4.x/PPC load: ");
	timer = 0;
	cp = bd->cmd_line+strlen(bd->cmd_line);
	while (timer++ < 5*1000) {
		if (debug_tstc()) {
			while ((ch = debug_getc()) != '\n' && ch != '\r') {
				if (ch == '\b' || ch == 0177) {
					if (cp != bd->cmd_line) {
						cp--;
						printk("\b \b");
					}
				} else {
					*cp++ = ch;
					debug_putc(ch);
				}
			}
			break;  /* Exit 'timer' loop */
		}
		boot_udelay(1000);  /* 1 msec */
	}
	*cp = 0;
}

/* Functions to deal with the residual data */
static int same_DevID(unsigned short vendor,
	       unsigned short Number,
	       unsigned char * str)
{
	static unsigned const char hexdigit[]="0123456789ABCDEF";
	if (strlen((char*)str)!=7) return 0;
	if ( ( ((vendor>>10)&0x1f)+'A'-1 == str[0])  &&
	     ( ((vendor>>5)&0x1f)+'A'-1 == str[1])   &&
	     ( (vendor&0x1f)+'A'-1 == str[2])        &&
	     (hexdigit[(Number>>12)&0x0f] == str[3]) &&
	     (hexdigit[(Number>>8)&0x0f] == str[4])  &&
	     (hexdigit[(Number>>4)&0x0f] == str[5])  &&
	     (hexdigit[Number&0x0f] == str[6]) ) return 1;
	return 0;
}

PPC_DEVICE *residual_find_device(unsigned long BusMask,
			 unsigned char * DevID,
			 int BaseType,
			 int SubType,
			 int Interface,
			 int n)
{
	int i;
	RESIDUAL *res = bd->residual;
	if ( !res || !res->ResidualLength ) return NULL;
	for (i=0; i<res->ActualNumDevices; i++) {
#define Dev res->Devices[i].DeviceId
		if ( (Dev.BusId&BusMask)                                  &&
		     (BaseType==-1 || Dev.BaseType==BaseType)             &&
		     (SubType==-1 || Dev.SubType==SubType)                &&
		     (Interface==-1 || Dev.Interface==Interface)          &&
		     (DevID==NULL || same_DevID((Dev.DevId>>16)&0xffff,
						Dev.DevId&0xffff, DevID)) &&
		     !(n--) ) return res->Devices+i;
#undef Dev
	}
	return 0;
}

PnP_TAG_PACKET *PnP_find_packet(unsigned char *p,
				unsigned packet_tag,
				int n)
{
	unsigned mask, masked_tag, size;
	if(!p) return 0;
	if (tag_type(packet_tag)) mask=0xff; else mask=0xF8;
	masked_tag = packet_tag&mask;
	for(; *p != END_TAG; p+=size) {
		if ((*p & mask) == masked_tag && !(n--))
			return (PnP_TAG_PACKET *) p;
		if (tag_type(*p))
			size=ld_le16((unsigned short *)(p+1))+3;
		else
			size=tag_small_count(*p)+1;
	}
	return 0; /* not found */
}

PnP_TAG_PACKET *PnP_find_small_vendor_packet(unsigned char *p,
					     unsigned packet_type,
					     int n)
{
	int next=0;
	while (p) {
		p = (unsigned char *) PnP_find_packet(p, 0x70, next);
		if (p && p[1]==packet_type && !(n--))
			return (PnP_TAG_PACKET *) p;
		next = 1;
	};
	return 0; /* not found */
}

PnP_TAG_PACKET *PnP_find_large_vendor_packet(unsigned char *p,
					   unsigned packet_type,
					   int n)
{
	int next=0;
	while (p) {
		p = (unsigned char *) PnP_find_packet(p, 0x84, next);
		if (p && p[3]==packet_type && !(n--))
			return (PnP_TAG_PACKET *) p;
		next = 1;
	};
	return 0; /* not found */
}

/* Find out the amount of installed memory. For MPC105 and IBM 660 this
 * can be done by finding the bank with the highest memory ending address
 */
int
find_max_mem( struct pci_dev *dev )
{
	u_char banks,tmp;
	int i, top, max;

	max = 0;
	for ( ; dev; dev = dev->next) {
		if ( ((dev->vendor == PCI_VENDOR_ID_MOTOROLA) &&
		      (dev->device == PCI_DEVICE_ID_MOTOROLA_MPC105)) ||
		     ((dev->vendor == PCI_VENDOR_ID_IBM) &&
		      (dev->device == 0x0037/*IBM 660 Bridge*/)) ) {
			pci_bootloader_read_config_byte(dev, 0xa0, &banks);
			for (i = 0; i < 8; i++) {
				if ( banks & (1<<i) ) {
					pci_bootloader_read_config_byte(dev, 0x90+i, &tmp);
					top = tmp;
					pci_bootloader_read_config_byte(dev, 0x98+i, &tmp);
					top |= (tmp&3)<<8;
					if ( top > max ) max = top;
				}
			}
			if ( max ) return ((max+1)<<20);
			else return(0);
		}
	}
	return(0);
}
