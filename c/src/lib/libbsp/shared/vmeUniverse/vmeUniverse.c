/* $Id$ */

/* Routines to configure the VME interface
 * Author: Till Straumann <strauman@slac.stanford.edu>
 *         Nov 2000, Oct 2001, Jan 2002
 */

#if 0
 * $Log$
 * Revision 1.21  2002/04/11 06:54:48  till
 *  - silenced message about 'successfully configured a port'
 *
 * Revision 1.20  2002/03/27 21:14:50  till
 *  - fix: handler table holds pointers, so hdlrTbl[vector]->usrData etc.
 *    not hdlrTbl[vector].usrData...
 *
 * Revision 1.19  2002/03/09 00:14:36  till
 *  - added vmeUniverseISRGet() to retrieve the currently installed
 *    ISR for a given vector
 *  - swapped the argument order for ISRs to (usrarg, vector)
 *
 * Revision 1.18  2002/02/07 19:53:48  till
 *  - reverted back to publish base_addr/irq_line as variables rather than
 *    through functions: the irq_line is read by the interrupt dispatcher...
 *
 * Revision 1.17  2002/01/24 08:28:10  till
 *  - initialize driver when reading base address or irq line.
 *    however, this requires the pci driver to be working already.
 *
 * Revision 1.16  2002/01/24 08:21:48  till
 *  - replaced public global vars for base address/irq line by routines.
 *
 * Revision 1.15  2002/01/23 06:15:30  till
 *   - changed master port data width to 64 bit.
 *     /* NOTE: reading the CY961 (Echotek ECDR814) with VDW32
 *      *       generated bus errors when reading 32-bit words
 *      *       - very weird, because the registers are 16-bit
 *      *         AFAIK.
 *      *       - 32-bit accesses worked fine on vxWorks which
 *      *         has the port set to 64-bit.
 *      *       ????????
 *      */
 *
 * Revision 1.14  2002/01/11 19:30:54  till
 *  - added more register defines to header
 *  - completed vmeUniverseReset
 *
 * Revision 1.13  2002/01/11 05:06:18  till
 *  - fixed VMEISR failing to check (lint_stat & msk) when determining
 *    the highes level...
 *  - tested interrupt handling & nesting. Seems to work.
 *
 * Revision 1.12  2002/01/11 02:25:55  till
 *  - added interrupt manager
 *
 * Revision 1.11  2002/01/08 03:59:52  till
 *  - vxworks always defines _LITTLE_ENDIAN, fixed the conditionals
 *    so it should work on __vxworks and on __rtems now.
 *  - rtems uprintf wrapper reverts to printk if stdio is not yet
 *    initialized (uses _impure_ptr->__sdidinit)
 *  - tested bus address translation utility routines
 *
 * Revision 1.9  2002/01/05 02:36:32  till
 *  - added vmeUniverseBusToLocalAdrs / vmeUniverseLocalToBusAdrs for address
 *    space translations.
 *  - include bsp.h under rtems to hack around the libcpu/powerpc/shared/io.h
 *    #define _IO_BASE & friends problem.
 *
 * Revision 1.8  2002/01/04 04:12:51  till
 *  - changed some rtems/pci related names
 *
 * Revision 1.7  2002/01/04 03:06:30  till
 *  - added further register definitions
 *
 * Revision 1.6  2001/12/20 04:42:44  till
 *  - fixed endianness stuff; theoretically, PPC could be LITTLE_ENDIAN...
 *
 * Revision 1.4  2001/12/19 01:59:02  till
 *  - started adding interrupt stuff
 *  - private implementation of PCI scanning if necessary
 *
 * Revision 1.3  2001/07/27 22:22:51  till
 *  - added more DMA support routines and defines to include file
 *  - xxxPortsShow can now print to a given file descriptor argument
 *
 * Revision 1.2  2001/07/26 18:06:13  till
 *  - ported to RTEMS
 *  - fixed a couple of wrong pointer calculations.
 *
 * Revision 1.1.1.1  2001/07/12 23:15:19  till
 *  - cvs import
 *
#endif

#include <stdio.h>
#include <stdarg.h>
#include "vmeUniverse.h"

#define UNIV_NUM_MPORTS		8 /* number of master ports */
#define UNIV_NUM_SPORTS		8 /* number of slave ports */

#define PCI_VENDOR_TUNDRA	0x10e3
#define PCI_DEVICE_UNIVERSEII	0
#define PCI_UNIVERSE_BASE0	0x10
#define PCI_UNIVERSE_BASE1	0x14

#define UNIV_REGOFF_PCITGT0_CTRL 0x100
#define UNIV_REGOFF_PCITGT4_CTRL 0x1a0
#define UNIV_REGOFF_VMESLV0_CTRL 0xf00
#define UNIV_REGOFF_VMESLV4_CTRL 0xf90

#define UNIV_CTL_VAS16		(0x00000000)
#define UNIV_CTL_VAS24		(0x00010000)
#define UNIV_CTL_VAS32		(0x00020000)
#define UNIV_CTL_VAS		(0x00070000)

#define UNIV_MCTL_EN		(0x80000000)
#define UNIV_MCTL_PWEN		(0x40000000)
#define UNIV_MCTL_PGM		(0x00004000)
#define UNIV_MCTL_VCT		(0x00000100)
#define UNIV_MCTL_SUPER		(0x00001000)
#define UNIV_MCTL_VDW32		(0x00800000)
#define UNIV_MCTL_VDW64		(0x00c00000)

#define UNIV_MCTL_AM_MASK	(UNIV_CTL_VAS | UNIV_MCTL_PGM | UNIV_MCTL_SUPER)

#define UNIV_SCTL_EN		(0x80000000)
#define UNIV_SCTL_PWEN		(0x40000000)
#define UNIV_SCTL_PREN		(0x20000000)
#define UNIV_SCTL_PGM		(0x00800000)
#define UNIV_SCTL_DAT		(0x00400000)
#define UNIV_SCTL_SUPER		(0x00200000)
#define UNIV_SCTL_USER		(0x00100000)

#define UNIV_SCTL_AM_MASK	(UNIV_CTL_VAS | UNIV_SCTL_PGM | UNIV_SCTL_DAT | UNIV_SCTL_USER | UNIV_SCTL_SUPER)

/* we rely on a vxWorks definition here */
#define VX_AM_SUP		4

#ifdef __rtems

#include <stdlib.h>
#include <rtems/bspIo.h>	/* printk */
#include <bsp/pci.h>
#include <bsp.h>

#define pciFindDevice	BSP_pciFindDevice
#define pciConfigInLong pci_read_config_dword
#define pciConfigInByte pci_read_config_byte

typedef unsigned int pci_ulong;
#define PCI_TO_LOCAL_ADDR(memaddr) ((pci_ulong)(memaddr) + PCI_MEM_BASE)


#elif defined(__vxworks)
typedef unsigned long pci_ulong;
#define PCI_TO_LOCAL_ADDR(memaddr) (memaddr)
#define PCI_INTERRUPT_LINE	0x3c
#else
#error "vmeUniverse not ported to this architecture yet"
#endif


volatile LERegister *vmeUniverse0BaseAddr=0;
int vmeUniverse0PciIrqLine=-1;

#if 0
/* public access functions */
volatile LERegister *
vmeUniverseBaseAddr(void)
{
	if (!vmeUniverse0BaseAddr) vmeUniverseInit();
	return vmeUniverse0BaseAddr;
}

int
vmeUniversePciIrqLine(void)
{
	if (vmeUniverse0PciIrqLine<0) vmeUniverseInit();
	return vmeUniverse0PciIrqLine;
}
#endif

static inline void
WRITE_LE(
	unsigned long val,
	volatile LERegister    *adrs,
	unsigned long off)
{
#if (__LITTLE_ENDIAN__ == 1)
	*(volatile unsigned long*)(((unsigned long)adrs)+off)=val;
#elif (defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)) && (__BIG_ENDIAN__ == 1)
	/* offset is in bytes and MUST not end up in r0 */
	__asm__ __volatile__("stwbrx %1, %0, %2" :: "b"(off),"r"(val),"r"(adrs));
#elif defined(__rtems)
	st_le32((volatile unsigned long*)(((unsigned long)adrs)+off), val);
#else
#error "little endian register writing not implemented"
#endif
}

#if defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)
#define SYNC __asm__ __volatile__("sync")
#else
#define SYNC
#warning "SYNC instruction unknown for this architecture"
#endif

/* registers should be mapped to guarded, non-cached memory; hence 
 * subsequent stores are ordered. eieio is only needed to enforce
 * ordering of loads with respect to stores.
 */
#define EIEIO_REG

static inline unsigned long
READ_LE0(volatile LERegister *adrs)
{
#if (__LITTLE_ENDIAN__ == 1)
	return *(volatile unsigned long *)adrs;
#elif (defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)) && (__BIG_ENDIAN__ == 1)
register unsigned long rval;
__asm__ __volatile__("lwbrx %0, 0, %1":"=r"(rval):"r"(adrs));
	return rval;
#elif defined(__rtems)
	return ld_le32((volatile unsigned long*)adrs);
#else
#error "little endian register reading not implemented"
#endif
}

static inline unsigned long
READ_LE(volatile LERegister *adrs, unsigned long off)
{
#if (__LITTLE_ENDIAN__ == 1)
	return  *((volatile LERegister *)(((unsigned long)adrs)+off));
#elif (defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)) && (__BIG_ENDIAN__ == 1)
register unsigned long rval;
	/* offset is in bytes and MUST not end up in r0 */
__asm__ __volatile__("lwbrx %0, %2, %1"
				: "=r"(rval)
				: "r"(adrs), "b"(off));
#if 0
__asm__ __volatile__("eieio");
#endif
return rval;
#else
return READ_LE0((volatile LERegister *)(((unsigned long)adrs)+off));
#endif
}

#define PORT_UNALIGNED(addr,port) \
	( (port)%4 ? ((addr) & 0xffff) : ((addr) & 4095) ) 


#define UNIV_REV(base) (READ_LE(base,2*sizeof(LERegister)) & 0xff)
	
#ifdef __rtems
static int
uprintk(char *fmt, va_list ap)
{
int		rval;
/* during bsp init, there is no malloc and no stdio,
 * hence we assemble the message on the stack and revert
 * to printk
 */
char	buf[200];
	rval = vsprintf(buf,fmt,ap);
	if (rval > sizeof(buf))
			BSP_panic("vmeUniverse/uprintk: buffer overrun");
	printk(buf);
	return rval;
}
#endif


/* private printing wrapper */
static int
uprintf(FILE *f, char *fmt, ...)
{
va_list	ap;
int	rval;
	va_start(ap, fmt);
#ifdef __rtems
	if (!f || !_impure_ptr->__sdidinit) {
		/* Might be called at an early stage when
		 * stdio is not yet initialized.
		 * There is no vprintk, hence we must assemble
		 * to a buffer.
		 */
		rval=uprintk(fmt,ap);
	} else 
#endif
	{
		rval=vfprintf(f,fmt,ap);
	}
	va_end(ap);
	return rval;
}

int
vmeUniverseFindPciBase(
	int instance,
	volatile LERegister **pbase
	)
{
int bus,dev,fun;
pci_ulong busaddr;
unsigned char irqline;

	if (pciFindDevice(
			PCI_VENDOR_TUNDRA,
			PCI_DEVICE_UNIVERSEII,
			instance,
			&bus,
			&dev,
			&fun))
		return -1;
	if (pciConfigInLong(bus,dev,fun,PCI_UNIVERSE_BASE0,&busaddr))
		return -1;
	if ((unsigned long)(busaddr) & 1) {
		/* it's IO space, try BASE1 */
		if (pciConfigInLong(bus,dev,fun,PCI_UNIVERSE_BASE1,&busaddr)
		   || ((unsigned long)(busaddr) & 1))
			return -1;
	}
	*pbase=(volatile LERegister*)PCI_TO_LOCAL_ADDR(busaddr);

	if (pciConfigInByte(bus,dev,fun,PCI_INTERRUPT_LINE,&irqline))
		return -1;
	else
		vmeUniverse0PciIrqLine = irqline;

	return 0;
}

/* convert an address space selector to a corresponding
 * universe control mode word
 */

static int
am2mode(int ismaster, unsigned long address_space, unsigned long *pmode)
{
unsigned long mode=0;
	if (!ismaster) {
		mode |= UNIV_SCTL_DAT | UNIV_SCTL_PGM;
		mode |= UNIV_SCTL_USER;
	}
	switch (address_space) {
		case VME_AM_STD_SUP_PGM:
		case VME_AM_STD_USR_PGM:
			if (ismaster)
				mode |= UNIV_MCTL_PGM ;
			else {
				mode &= ~UNIV_SCTL_DAT;
			}
			/* fall thru */
		case VME_AM_STD_SUP_DATA:
		case VME_AM_STD_USR_DATA:
			mode |= UNIV_CTL_VAS24;
			break;

		case VME_AM_EXT_SUP_PGM:
		case VME_AM_EXT_USR_PGM:
			if (ismaster)
				mode |= UNIV_MCTL_PGM ;
			else {
				mode &= ~UNIV_SCTL_DAT;
			}
			/* fall thru */
		case VME_AM_EXT_SUP_DATA:
		case VME_AM_EXT_USR_DATA:
			mode |= UNIV_CTL_VAS32;
			break;

		case VME_AM_SUP_SHORT_IO:
		case VME_AM_USR_SHORT_IO:
			mode |= UNIV_CTL_VAS16;
			break;

		case 0: /* disable the port alltogether */
			break;

		default:
			return -1;
	}
	if (address_space & VX_AM_SUP)
		mode |= (ismaster ? UNIV_MCTL_SUPER : UNIV_SCTL_SUPER);
	*pmode = mode;
	return 0;
}

static int
disableUniversePort(int ismaster, int portno, volatile unsigned long *preg, void *param)
{
unsigned long cntrl;
	cntrl=READ_LE0(preg);
	cntrl &= ~(ismaster ? UNIV_MCTL_EN : UNIV_SCTL_EN);
	WRITE_LE(cntrl,preg,0);
	SYNC; /* make sure this command completed */
	return 0;
}

static int
cfgUniversePort(
	unsigned long	ismaster,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
#define base vmeUniverse0BaseAddr
volatile LERegister *preg=base;
unsigned long	p=port;
unsigned long	mode=0;

	/* check parameters */
	if (port >= (ismaster ? UNIV_NUM_MPORTS : UNIV_NUM_SPORTS)) {
		uprintf(stderr,"invalid port\n");
		return -1;
	}
	/* port start, bound addresses and offset must lie on 64k boundary
	 * (4k for port 0 and 4)
	 */
	if ( PORT_UNALIGNED(local_address,port) ) {
		uprintf(stderr,"local address misaligned\n");
		return -1;
	}
	if ( PORT_UNALIGNED(vme_address,port) ) {
		uprintf(stderr,"vme address misaligned\n");
		return -1;
	}
	if ( PORT_UNALIGNED(length,port) ) {
		uprintf(stderr,"length misaligned\n");
		return -1;
	}

	/* check address space validity */
	if (am2mode(ismaster,address_space,&mode)) {
		uprintf(stderr,"invalid address space\n");
		return -1;
	}

	/* get the universe base address */
	if (!base && vmeUniverseInit()) {
		return -1;
	}

	/* find out if we have a rev. II chip */
	if ( UNIV_REV(base) < 2 ) {
		if (port>3) {
			uprintf(stderr,"Universe rev. < 2 has only 4 ports\n");
			return -1;
		}
	}

	/* finally, configure the port */

	/* find the register set for our port */
	if (port<4) {
		preg += (ismaster ? UNIV_REGOFF_PCITGT0_CTRL : UNIV_REGOFF_VMESLV0_CTRL)/sizeof(LERegister);
	} else {
		preg += (ismaster ? UNIV_REGOFF_PCITGT4_CTRL : UNIV_REGOFF_VMESLV4_CTRL)/sizeof(LERegister);
		p-=4;
	}
	preg += 5 * p;

	/* temporarily disable the port */
	disableUniversePort(ismaster,port,preg,0);

	/* address_space == 0 means disable */
	if (address_space != 0) {
		unsigned long start,offst;
		/* set the port starting address;
		 * this is the local address for the master
		 * and the VME address for the slave
		 */
		if (ismaster) {
			start=local_address;
			/* let it overflow / wrap around 0 */
			offst=vme_address-local_address;
		} else {
			start=vme_address;
			/* let it overflow / wrap around 0 */
			offst=local_address-vme_address;
		}
#undef TSILL
#ifdef TSILL
		uprintf(stderr,"writing 0x%08x to 0x%08x + 4\n",start,preg);
#else
		WRITE_LE(start,preg,4);
#endif
		/* set bound address */
		length+=start;
#ifdef TSILL
		uprintf(stderr,"writing 0x%08x to 0x%08x + 8\n",length,preg);
#else
		WRITE_LE(length,preg,8);
#endif
		/* set offset */
#ifdef TSILL
		uprintf(stderr,"writing 0x%08x to 0x%08x + 12\n",offst,preg);
#else
		WRITE_LE(offst,preg,12);
#endif
		/* calculate configuration word and enable the port */
		/* NOTE: reading the CY961 (Echotek ECDR814) with VDW32
		 *       generated bus errors when reading 32-bit words
                 *       - very weird, because the registers are 16-bit
                 *         AFAIK.
		 *       - 32-bit accesses worked fine on vxWorks which
                 *         has the port set to 64-bit.
                 *       ????????
                 */
		if (ismaster)
			mode |= UNIV_MCTL_EN | UNIV_MCTL_PWEN | UNIV_MCTL_VDW64 | UNIV_MCTL_VCT;
		else 
			mode |= UNIV_SCTL_EN | UNIV_SCTL_PWEN | UNIV_SCTL_PREN;

#ifdef TSILL
		uprintf(stderr,"writing 0x%08x to 0x%08x + 0\n",mode,preg);
#else
		EIEIO_REG;	/* make sure mode is written last */
		WRITE_LE(mode,preg,0);
		SYNC;		/* enforce completion */
#endif

#ifdef TSILL
		uprintf(stderr,
			"universe %s port %lu successfully configured\n",
				ismaster ? "master" : "slave",
				port);
#endif

#ifdef __vxworks
		if (ismaster)
			uprintf(stderr,
			"WARNING: on the synergy, sysMasterPortsShow() may show incorrect settings (it uses cached values)\n");
#endif
	}
	return 0;
#undef base
}


static int
showUniversePort(
		int		ismaster,
		int		portno,
		volatile LERegister *preg,
		void		*parm)
{
	FILE *f=parm ? (FILE *)parm : stdout;
	unsigned long cntrl, start, bound, offst, mask;

	cntrl = READ_LE0(preg++);
#undef TSILL
#ifdef TSILL
	uprintf(stderr,"showUniversePort: *(0x%08x): 0x%08x\n",preg-1,cntrl);
#endif
#undef TSILL

	/* skip this port if disabled */
	if (!(cntrl & (ismaster ? UNIV_MCTL_EN : UNIV_SCTL_EN)))
		return 0;

	/* for the master `start' is the PCI address,
	 * for the slave  `start' is the VME address
	 */
	mask = ~PORT_UNALIGNED(0xffffffff,portno);

	start = READ_LE0(preg++)&mask;
	bound = READ_LE0(preg++)&mask;
	offst = READ_LE0(preg++)&mask;

	offst+=start; /* calc start on the other bus */

	if (ismaster) {
		uprintf(f,"%i:    0x%08lx 0x%08lx 0x%08lx ",
			portno,offst,bound-start,start);
	} else {
		uprintf(f,"%i:    0x%08lx 0x%08lx 0x%08lx ",
			portno,start,bound-start,offst);
	}

	switch (cntrl & UNIV_CTL_VAS) {
		case UNIV_CTL_VAS16: uprintf(f,"A16, "); break;
		case UNIV_CTL_VAS24: uprintf(f,"A24, "); break;
		case UNIV_CTL_VAS32: uprintf(f,"A32, "); break;
		default: uprintf(f,"A??, "); break;
	}

	if (ismaster) {
		uprintf(f,"%s, %s",
			cntrl&UNIV_MCTL_PGM ?   "Pgm" : "Dat",
			cntrl&UNIV_MCTL_SUPER ? "Sup" : "Usr");
	} else {
		uprintf(f,"%s %s %s %s", 
			cntrl&UNIV_SCTL_PGM ?   "Pgm," : "    ",
			cntrl&UNIV_SCTL_DAT ?   "Dat," : "    ",
			cntrl&UNIV_SCTL_SUPER ? "Sup," : "    ",
			cntrl&UNIV_SCTL_USER  ? "Usr" :  "");
	}
	uprintf(f,"\n");
	return 0;
}

typedef struct XlatRec_ {
	unsigned long	address;
	unsigned long	aspace;
} XlatRec, *Xlat;

/* try to translate an address through the bridge
 *
 * IN:  l->address, l->aspace
 * OUT: l->address (translated address)
 *
 * RETURNS: -1: invalid space
 *           0: invalid address (not found in range)
 *           1: success
 */

static int
xlatePort(int ismaster, int port, volatile LERegister *preg, void *parm)
{
Xlat	l=(Xlat)parm;
unsigned long cntrl, start, bound, offst, mask, x;

	cntrl = READ_LE0(preg++);

	/* skip this port if disabled */
	if (!(cntrl & (ismaster ? UNIV_MCTL_EN : UNIV_SCTL_EN)))
		return 0;

	/* check for correct address space */
	if ( am2mode(ismaster,l->aspace,&offst) ) {
		uprintf(stderr,"vmeUniverse WARNING: invalid adressing mode 0x%x\n",
		               l->aspace);
		return -1;
	}
	if ( (cntrl & (ismaster ? UNIV_MCTL_AM_MASK : UNIV_SCTL_AM_MASK))
	    != offst )
		return 0; /* mode doesn't match requested AM */

	/* OK, we found a matching mode, now we must check the address range */
	mask = ~PORT_UNALIGNED(0xffffffff,port);

	/* for the master `start' is the PCI address,
	 * for the slave  `start' is the VME address
	 */
	start = READ_LE0(preg++) & mask;
	bound = READ_LE0(preg++) & mask;
	offst = READ_LE0(preg++) & mask;

	/* translate address to the other bus */
	x = l->address - offst;

	if (x >= start && x < bound) {
		/* valid address found */
		l->address = x;
		return 1;
	}
	return 0;
}


static int
mapOverAll(int ismaster, int (*func)(int,int,volatile LERegister *,void*), void *arg)
{
#define base	vmeUniverse0BaseAddr
volatile LERegister	*rptr;
unsigned long	port;
int	rval;

	/* get the universe base address */
	if (!base && vmeUniverseInit()) {
		uprintf(stderr,"unable to find the universe in pci config space\n");
		return -1;
	}
	rptr = (base + 
		(ismaster ? UNIV_REGOFF_PCITGT0_CTRL : UNIV_REGOFF_VMESLV0_CTRL)/sizeof(LERegister));
#undef TSILL
#ifdef TSILL
	uprintf(stderr,"mapoverall: base is 0x%08x, rptr 0x%08x\n",base,rptr);
#endif
#undef TSILL
	for (port=0; port<4; port++) {
		if ((rval=func(ismaster,port,rptr,arg))) return rval;
		rptr+=5; /* register block spacing */
	}

	/* only rev. 2 has 8 ports */
	if (UNIV_REV(base)<2) return -1;

	rptr = (base + 
		(ismaster ? UNIV_REGOFF_PCITGT4_CTRL : UNIV_REGOFF_VMESLV4_CTRL)/sizeof(LERegister));
	for (port=4; port<UNIV_NUM_MPORTS; port++) {
		if ((rval=func(ismaster,port,rptr,arg))) return rval;
		rptr+=5; /* register block spacing */
	}
	return 0;
#undef base
}

static void
showUniversePorts(int ismaster, FILE *f)
{
	if (!f) f=stdout;
	uprintf(f,"Universe %s Ports:\n",ismaster ? "Master" : "Slave");
	uprintf(f,"Port  VME-Addr   Size       PCI-Adrs   Mode:\n");
	mapOverAll(ismaster,showUniversePort,f);
}

static int xlate(int ismaster, unsigned long as, unsigned long aIn, unsigned long *paOut)
{
int	rval;
XlatRec l;
	l.aspace = as;
	l.address = aIn;
	/* map result -1/0/1 to -2/-1/0 with 0 on success */
	rval = mapOverAll(ismaster,xlatePort,(void*)&l) - 1;
	*paOut = l.address;
	return rval;
}

/* public functions */
int
vmeUniverseLocalToBusAdrs(unsigned long as, unsigned long localAdrs, unsigned long *pbusAdrs)
{
	return xlate(0,as,localAdrs,pbusAdrs);
}

int
vmeUniverseBusToLocalAdrs(unsigned long as, unsigned long busAdrs, unsigned long *plocalAdrs)
{
	return xlate(1,as,busAdrs,plocalAdrs);
}

void
vmeUniverseReset(void)
{
	/* disable/reset special cycles (ADOH, RMW) */
	vmeUniverseWriteReg(0, UNIV_REGOFF_SCYC_CTL);
	vmeUniverseWriteReg(0, UNIV_REGOFF_SCYC_ADDR);
	vmeUniverseWriteReg(0, UNIV_REGOFF_SCYC_EN);

	/* set coupled window timeout to 0 (release VME after each transaction)
	 * CRT (coupled request timeout) is unused by Universe II
	 */
	vmeUniverseWriteReg(UNIV_LMISC_CRT_128_US, UNIV_REGOFF_LMISC);

	/* disable/reset DMA engine */
	vmeUniverseWriteReg(0, UNIV_REGOFF_DCTL);
	vmeUniverseWriteReg(0, UNIV_REGOFF_DTBC);
	vmeUniverseWriteReg(0, UNIV_REGOFF_DLA);
	vmeUniverseWriteReg(0, UNIV_REGOFF_DVA);
	vmeUniverseWriteReg(0, UNIV_REGOFF_DCPP);

	/* disable location monitor */
	vmeUniverseWriteReg(0, UNIV_REGOFF_LM_CTL);

	/* disable universe register access from VME bus */
	vmeUniverseWriteReg(0, UNIV_REGOFF_VRAI_CTL);

	/* disable VME bus image of VME CSR */
	vmeUniverseWriteReg(0, UNIV_REGOFF_VCSR_CTL);

	/* disable interrupts, reset routing */
	vmeUniverseWriteReg(0, UNIV_REGOFF_LINT_EN);
	vmeUniverseWriteReg(0, UNIV_REGOFF_LINT_MAP0);
	vmeUniverseWriteReg(0, UNIV_REGOFF_LINT_MAP1);

	vmeUniverseWriteReg(0, UNIV_REGOFF_VINT_EN);
	vmeUniverseWriteReg(0, UNIV_REGOFF_VINT_MAP0);
	vmeUniverseWriteReg(0, UNIV_REGOFF_VINT_MAP1);

	vmeUniverseDisableAllSlaves();

	vmeUniverseDisableAllMasters();
	
	vmeUniverseWriteReg(UNIV_VCSR_CLR_SYSFAIL, UNIV_REGOFF_VCSR_CLR);

	/* clear interrupt status bits */
	vmeUniverseWriteReg(UNIV_LINT_STAT_CLR, UNIV_REGOFF_LINT_STAT);
	vmeUniverseWriteReg(UNIV_VINT_STAT_CLR, UNIV_REGOFF_VINT_STAT);

	vmeUniverseWriteReg(UNIV_V_AMERR_V_STAT, UNIV_REGOFF_V_AMERR);

	vmeUniverseWriteReg(
		vmeUniverseReadReg(UNIV_REGOFF_PCI_CSR) |
		UNIV_PCI_CSR_D_PE | UNIV_PCI_CSR_S_SERR | UNIV_PCI_CSR_R_MA |
		UNIV_PCI_CSR_R_TA | UNIV_PCI_CSR_S_TA,
		UNIV_REGOFF_PCI_CSR);

	vmeUniverseWriteReg(UNIV_L_CMDERR_L_STAT, UNIV_REGOFF_L_CMDERR);

	vmeUniverseWriteReg(
		UNIV_DGCS_STOP | UNIV_DGCS_HALT | UNIV_DGCS_DONE |
		UNIV_DGCS_LERR | UNIV_DGCS_VERR | UNIV_DGCS_P_ERR,
		UNIV_REGOFF_DGCS);
}

int
vmeUniverseInit(void)
{
int rval;
	if ((rval=vmeUniverseFindPciBase(0,&vmeUniverse0BaseAddr))) {
		uprintf(stderr,"unable to find the universe in pci config space\n");
	} else {
		uprintf(stderr,"Universe II PCI-VME bridge detected at 0x%08x, IRQ %i\n",
				(unsigned int)vmeUniverse0BaseAddr, vmeUniverse0PciIrqLine);
	}
	return rval;
}

void
vmeUniverseMasterPortsShow(FILE *f)
{
	showUniversePorts(1,f);
}

void
vmeUniverseSlavePortsShow(FILE *f)
{
	showUniversePorts(0,f);
}

int
vmeUniverseMasterPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	return cfgUniversePort(1,port,address_space,vme_address,local_address,length);
}

int
vmeUniverseSlavePortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	return cfgUniversePort(0,port,address_space,vme_address,local_address,length);
}

void
vmeUniverseDisableAllSlaves(void)
{
	mapOverAll(0,disableUniversePort,0);
}

void
vmeUniverseDisableAllMasters(void)
{
	mapOverAll(1,disableUniversePort,0);
}

int
vmeUniverseStartDMA(
	unsigned long local_addr,
	unsigned long vme_addr,
	unsigned long count)
{

	if (!vmeUniverse0BaseAddr && vmeUniverseInit()) return -1;
	if ((local_addr & 7) != (vme_addr & 7)) {
		uprintf(stderr,"vmeUniverseStartDMA: misaligned addresses\n");
		return -1;
	}

	{
	/* help the compiler allocate registers */
	register volatile LERegister *b=vmeUniverse0BaseAddr;
	register unsigned long dgcsoff=UNIV_REGOFF_DGCS,dgcs;

	dgcs=READ_LE(b, dgcsoff);

	/* clear status and make sure CHAIN is clear */
	dgcs &= ~UNIV_DGCS_CHAIN;
	WRITE_LE(dgcs,
		      b, dgcsoff);
	WRITE_LE(local_addr,
		      b, UNIV_REGOFF_DLA);
	WRITE_LE(vme_addr,
		      b, UNIV_REGOFF_DVA);
	WRITE_LE(count,
		      b, UNIV_REGOFF_DTBC);
	dgcs |= UNIV_DGCS_GO;
	EIEIO_REG; /* make sure GO is written after everything else */
	WRITE_LE(dgcs,
		      b, dgcsoff);
	}
	SYNC; /* enforce command completion */
	return 0;
}

unsigned long
vmeUniverseReadReg(unsigned long offset)
{
unsigned long rval;
	rval = READ_LE(vmeUniverse0BaseAddr,offset);
	return rval;
}

void
vmeUniverseWriteReg(unsigned long value, unsigned long offset)
{
	WRITE_LE(value, vmeUniverse0BaseAddr, offset);
}

void
vmeUniverseCvtToLE(unsigned long *ptr, unsigned long num)
{
#if !defined(__LITTLE_ENDIAN__) || (__LITTLE_ENDIAN__ != 1)
register unsigned long *p=ptr+num;
	while (p > ptr) {
#if (defined(_ARCH_PPC) || defined(__PPC__) || defined(__PPC)) && (__BIG_ENDIAN__ == 1)
		__asm__ __volatile__(
			"lwzu 0, -4(%0)\n"
			"stwbrx 0, 0, %0\n"
			: "=r"(p) : "r"(p) : "r0"
			);
#elif defined(__rtems)
		p--; st_le32(p, *p);
#else
#error	"vmeUniverse: endian conversion not implemented for this architecture"
#endif
	}
#endif
}

/* RTEMS interrupt subsystem */

#ifdef __rtems
#include <bsp/irq.h>

typedef struct
UniverseIRQEntryRec_ {
		VmeUniverseISR	isr;
		void			*usrData;
} UniverseIRQEntryRec, *UniverseIRQEntry;

static UniverseIRQEntry universeHdlTbl[257]={0};

static int mgrInstalled=0;
static int vmeIrqUnivOut=-1;
static int specialIrqUnivOut=-1;

VmeUniverseISR
vmeUniverseISRGet(unsigned long vector, void **parg)
{
	if (vector>255) return 0;
	if (parg)
		*parg=universeHdlTbl[vector]->usrData;
	return universeHdlTbl[vector]->isr;
}

static void
universeSpecialISR(void)
{
UniverseIRQEntry ip;
	/* try the special handler */
	if ((ip=universeHdlTbl[UNIV_SPECIAL_IRQ_VECTOR])) {
		ip->isr(ip->usrData, UNIV_SPECIAL_IRQ_VECTOR);
	}
	/* clear all special interrupts */
	vmeUniverseWriteReg(
					~((UNIV_LINT_STAT_VIRQ7<<1)-UNIV_LINT_STAT_VIRQ1),
					UNIV_REGOFF_LINT_STAT
					);

/*
 *	clear our line in the VINT_STAT register
 *  seems to be not neccessary...
	vmeUniverseWriteReg(
					UNIV_VINT_STAT_LINT(specialIrqUnivOut),
					UNIV_REGOFF_VINT_STAT);
 */
}

/*
 * interrupts from VME to PCI seem to be processed more or less
 * like this:
 *
 *
 *   VME IRQ ------ 
 *                  & ----- LINT_STAT ---- 
 *                  |                       &  ---------- PCI LINE
 *                  |                       |
 *                  |                       | 
 *       LINT_EN --------------------------- 
 *
 *  I.e. 
 *   - if LINT_EN is disabled, a VME IRQ will not set LINT_STAT.
 *   - while LINT_STAT is set, it will pull the PCI line unless
 *     masked by LINT_EN.
 *   - VINT_STAT(lint_bit) seems to have no effect beyond giving
 *     status info.
 *
 *  Hence, it is possible to
 *    - arm (set LINT_EN, routing etc.)
 *    - receive an irq (sets. LINT_STAT)
 *    - the ISR then:
 *      	  * clears LINT_EN, results in masking LINT_STAT (which
 *      	    is still set to prevent another VME irq at the same
 *      	    level to be ACKEd by the universe.
 *      	  * do PCI_EOI to allow nesting of higher VME irqs.
 *      	    (previous step also cleared LINT_EN of lower levels)
 *      	  * when the handler returns, clear LINT_STAT
 *      	  * re-enable setting LINT_EN.
 */

static void
universeVMEISR(void)
{
UniverseIRQEntry ip;
unsigned long lvl,msk,lintstat,linten,status;

		/* determine the highest priority IRQ source */
		lintstat=vmeUniverseReadReg(UNIV_REGOFF_LINT_STAT);
		for (msk=UNIV_LINT_STAT_VIRQ7, lvl=7;
			 lvl>0;
			 lvl--, msk>>=1) {
			if (lintstat & msk) break;
		}
		if (!lvl) {
				/* try the special handler */
				universeSpecialISR();

				/* 
				 * let the pic end this cycle
				 */
				BSP_PIC_DO_EOI;

				return;
		}
		linten = vmeUniverseReadReg(UNIV_REGOFF_LINT_EN);

		/* mask this and all lower levels */
		vmeUniverseWriteReg(
						linten & ~((msk<<1)-UNIV_LINT_STAT_VIRQ1),
						UNIV_REGOFF_LINT_EN
						);

		/* end this interrupt
		 * cycle on the PCI bus, so higher level interrupts can be
		 * caught from now on...
		 */
		BSP_PIC_DO_EOI;

		/* get vector and dispatch handler */
		status = vmeUniverseReadReg(UNIV_REGOFF_VIRQ1_STATID - 4 + (lvl<<2));
		/* determine the highest priority IRQ source */

		if (status & UNIV_VIRQ_ERR) {
				/* TODO: log error message - RTEMS has no logger :-( */
		} else if (!(ip=universeHdlTbl[status & UNIV_VIRQ_STATID_MASK])) {
				/* TODO: log error message - RTEMS has no logger :-( */
		} else {
				/* dispatch handler, it must clear the IRQ at the device */
				ip->isr(ip->usrData, status&UNIV_VIRQ_STATID_MASK);
		}

		/* clear this interrupt level */
		vmeUniverseWriteReg(msk, UNIV_REGOFF_LINT_STAT);
/*
 *  this seems not to be necessary; we just leave the
 *  bit set to save a couple of instructions...
		vmeUniverseWriteReg(
					UNIV_VINT_STAT_LINT(vmeIrqUnivOut),
					UNIV_REGOFF_VINT_STAT);
*/


		/* re-enable the previous level */
		vmeUniverseWriteReg(linten, UNIV_REGOFF_LINT_EN);
}

/* STUPID API */
static void
my_no_op(const rtems_irq_connect_data * arg)
{}

static int
my_isOn(const rtems_irq_connect_data *arg)
{
		return (int)vmeUniverseReadReg(UNIV_REGOFF_LINT_EN);
}

int
vmeUniverseInstallIrqMgr(int vmeOut, int specialOut, int specialIrqPicLine)
{
rtems_irq_connect_data aarrggh;

	/* check parameters */
	if ((vmeIrqUnivOut=vmeOut) < 0 || vmeIrqUnivOut > 7) return -1;
	if ((specialIrqUnivOut=specialOut) > 7) return -2;
	if (specialIrqPicLine < 0) return -3;

	if (mgrInstalled) return -4;

	aarrggh.on=my_no_op; /* at _least_ they could check for a 0 pointer */
	aarrggh.off=my_no_op;
	aarrggh.isOn=my_isOn;
	aarrggh.hdl=universeVMEISR;
	aarrggh.name=vmeUniverse0PciIrqLine + BSP_PCI_IRQ0;
	if (!BSP_install_rtems_irq_handler(&aarrggh))
			BSP_panic("unable to install vmeUniverse irq handler");
	if (specialIrqUnivOut > 0) {
			/* install the special handler to a separate irq */
			aarrggh.hdl=universeSpecialISR;
			aarrggh.name=specialIrqPicLine + BSP_PCI_IRQ0;
			if (!BSP_install_rtems_irq_handler(&aarrggh))
				BSP_panic("unable to install vmeUniverse secondary irq handler");
	} else {
		specialIrqUnivOut = vmeIrqUnivOut;
	}
	/* setup routing */

	vmeUniverseWriteReg(
		(UNIV_LINT_MAP0_VIRQ7(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ6(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ5(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ4(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ3(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ2(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VIRQ1(vmeIrqUnivOut) |
		 UNIV_LINT_MAP0_VOWN(specialIrqUnivOut)
		),
		UNIV_REGOFF_LINT_MAP0);
	vmeUniverseWriteReg(
		(UNIV_LINT_MAP1_ACFAIL(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_SYSFAIL(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_SW_INT(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_SW_IACK(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_VERR(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_LERR(specialIrqUnivOut) |
		 UNIV_LINT_MAP1_DMA(specialIrqUnivOut)
		),
		UNIV_REGOFF_LINT_MAP1);
	mgrInstalled=1;
	return 0;
}


int
vmeUniverseInstallISR(unsigned long vector, VmeUniverseISR hdl, void *arg)
{
UniverseIRQEntry ip;

		if (vector>sizeof(universeHdlTbl)/sizeof(universeHdlTbl[0]) || !mgrInstalled)
				return -1;

		ip=universeHdlTbl[vector];

		if (ip || !(ip=(UniverseIRQEntry)malloc(sizeof(UniverseIRQEntryRec))))
				return -1;
		ip->isr=hdl;
		ip->usrData=arg;
		universeHdlTbl[vector]=ip;
		return 0;
}

int
vmeUniverseRemoveISR(unsigned long vector, VmeUniverseISR hdl, void *arg)
{
UniverseIRQEntry ip;

		if (vector>sizeof(universeHdlTbl)/sizeof(universeHdlTbl[0]) || !mgrInstalled)
				return -1;

		ip=universeHdlTbl[vector];

		if (!ip || ip->isr!=hdl || ip->usrData!=arg)
				return -1;
		universeHdlTbl[vector]=0;
		free(ip);
		return 0;
}

int
vmeUniverseIntEnable(unsigned int level)
{
		if (!mgrInstalled || level<1 || level>7)
				return -1;
		vmeUniverseWriteReg(
				(vmeUniverseReadReg(UNIV_REGOFF_LINT_EN) | 
				 (UNIV_LINT_EN_VIRQ1 << (level-1))
				),
				UNIV_REGOFF_LINT_EN);
		return 0;
}

int
vmeUniverseIntDisable(unsigned int level)
{
		if (!mgrInstalled || level<1 || level>7)
				return -1;
		vmeUniverseWriteReg(
				(vmeUniverseReadReg(UNIV_REGOFF_LINT_EN) & 
				 ~ (UNIV_LINT_EN_VIRQ1 << (level-1))
				),
				UNIV_REGOFF_LINT_EN);
		return 0;
}


#endif
