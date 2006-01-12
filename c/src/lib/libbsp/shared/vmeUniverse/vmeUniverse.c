/* $Id$ */

/* Routines to configure the VME interface
 * Author: Till Straumann <strauman@slac.stanford.edu>
 *         Nov 2000, Oct 2001, Jan 2002
 */

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

#ifdef __rtems__

#include <stdlib.h>
#include <rtems/bspIo.h>	/* printk */
#include <rtems/error.h>
#include <bsp/pci.h>
#include <bsp.h>

/* allow the BSP to override the default routines */
#ifndef BSP_PCI_FIND_DEVICE
#define BSP_PCI_FIND_DEVICE			pci_find_device
#endif
#ifndef BSP_PCI_CONFIG_IN_LONG
#define BSP_PCI_CONFIG_IN_LONG		pci_read_config_dword
#endif
#ifndef BSP_PCI_CONFIG_IN_BYTE
#define BSP_PCI_CONFIG_IN_BYTE		pci_read_config_byte
#endif
#ifndef BSP_PCI_CONFIG_IN_SHORT
#define BSP_PCI_CONFIG_IN_SHORT		pci_read_config_word
#endif
#ifndef BSP_PCI_CONFIG_OUT_SHORT
#define BSP_PCI_CONFIG_OUT_SHORT	pci_write_config_word
#endif

/* PCI_MEM_BASE is a possible offset between CPU- and PCI addresses.
 * Should be defined by the BSP.
 */
typedef unsigned int pci_ulong;
#define PCI_TO_LOCAL_ADDR(memaddr) ((pci_ulong)(memaddr) + PCI_MEM_BASE)


#elif defined(__vxworks)
typedef unsigned long pci_ulong;
#define PCI_TO_LOCAL_ADDR(memaddr) (memaddr)
#define BSP_PCI_FIND_DEVICE		pciFindDevice
#define BSP_PCI_CONFIG_IN_LONG	pciConfigInLong
#define BSP_PCI_CONFIG_IN_BYTE	pciConfigInByte
#else
#error "vmeUniverse not ported to this architecture yet"
#endif

#ifndef PCI_INTERRUPT_LINE
#define PCI_INTERRUPT_LINE		0x3c
#endif

volatile LERegister *vmeUniverse0BaseAddr=0;
int vmeUniverse0PciIrqLine=-1;

#define DFLT_BASE	volatile LERegister *base = vmeUniverse0BaseAddr

#define CHECK_DFLT_BASE(base) \
	do { \
		/* get the universe base address */ \
		if (!base) { \
			if (vmeUniverseInit()) { \
				uprintf(stderr,"unable to find the universe in pci config space\n"); \
				return -1; \
			} else { \
				base = vmeUniverse0BaseAddr; \
			} \
		} \
	} while (0)

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
#elif defined(__rtems__)
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
#elif defined(__rtems__)
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
	
#if defined(__rtems__) && 0
static int
uprintk(char *fmt, va_list ap)
{
int		rval;
extern int k_vsprintf(char *, char *, va_list);
/* during bsp init, there is no malloc and no stdio,
 * hence we assemble the message on the stack and revert
 * to printk
 */
char	buf[200];
	rval = k_vsprintf(buf,fmt,ap);
	if (rval > sizeof(buf))
			BSP_panic("vmeUniverse/uprintk: buffer overrun");
	printk(buf);
	return rval;
}
#endif


/* private printing wrapper */
static void
uprintf(FILE *f, char *fmt, ...)
{
va_list	ap;
	va_start(ap, fmt);
#ifdef __rtems__
	if (!f || !_impure_ptr->__sdidinit) {
		/* Might be called at an early stage when
		 * stdio is not yet initialized.
		 * There is no vprintk, hence we must assemble
		 * to a buffer.
		 */
		vprintk(fmt,ap);
	} else 
#endif
	{
		vfprintf(f,fmt,ap);
	}
	va_end(ap);
}

int
vmeUniverseFindPciBase(
	int instance,
	volatile LERegister **pbase
	)
{
int bus,dev,fun;
unsigned short wrd;
pci_ulong busaddr;
unsigned char irqline;

	if (BSP_PCI_FIND_DEVICE(
			PCI_VENDOR_TUNDRA,
			PCI_DEVICE_UNIVERSEII,
			instance,
			&bus,
			&dev,
			&fun))
		return -1;
	if (BSP_PCI_CONFIG_IN_LONG(bus,dev,fun,PCI_UNIVERSE_BASE0,&busaddr))
		return -1;
	if ((unsigned long)(busaddr) & 1) {
		/* it's IO space, try BASE1 */
		if (BSP_PCI_CONFIG_IN_LONG(bus,dev,fun,PCI_UNIVERSE_BASE1,&busaddr)
		   || ((unsigned long)(busaddr) & 1))
			return -1;
	}
	*pbase=(volatile LERegister*)PCI_TO_LOCAL_ADDR(busaddr);

	if (BSP_PCI_CONFIG_IN_BYTE(bus,dev,fun,PCI_INTERRUPT_LINE,&irqline))
		return -1;

	/* Enable PCI master and memory access */
	BSP_PCI_CONFIG_IN_SHORT(bus, dev, fun, PCI_COMMAND, &wrd);
	BSP_PCI_CONFIG_OUT_SHORT(bus, dev, fun, PCI_COMMAND, wrd | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	return irqline;
}

/* convert an address space selector to a corresponding
 * universe control mode word
 */

static int
am2mode(int ismaster, unsigned long address_space, unsigned long *pmode)
{
unsigned long mode=0;

    /* NOTE: reading the CY961 (Echotek ECDR814) with VDW32
     *       generated bus errors when reading 32-bit words
     *       - very weird, because the registers are 16-bit
     *         AFAIK.
     *       - 32-bit accesses worked fine on vxWorks which
     *         has the port set to 64-bit.
     *       ????????
     */

	if (!ismaster) {
		mode |= UNIV_SCTL_DAT | UNIV_SCTL_PGM;
		mode |= UNIV_SCTL_USER;
		if ( VME_AM_IS_MEMORY & address_space )
			mode |= UNIV_SCTL_PWEN | UNIV_SCTL_PREN;
		mode |= UNIV_SCTL_EN;
	} else {
		mode |= UNIV_MCTL_VDW64 | UNIV_MCTL_VCT /* enable block transfers */;
		if ( VME_AM_IS_MEMORY & address_space )
			mode |= UNIV_MCTL_PWEN;
		mode |= UNIV_MCTL_EN;
	}

	address_space &= ~VME_AM_IS_MEMORY;

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
	if ( VME_AM_IS_SUP(address_space) )
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
	volatile LERegister *base,
	unsigned long	ismaster,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
volatile LERegister *preg;
unsigned long	p=port;
unsigned long	mode=0;

	CHECK_DFLT_BASE(base);

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

	preg=base;

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
		uprintf(f,"%d:    0x%08lx 0x%08lx 0x%08lx ",
			portno,offst,bound-start,start);
	} else {
		uprintf(f,"%d:    0x%08lx 0x%08lx 0x%08lx ",
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
		if ( cntrl & UNIV_MCTL_PWEN )
			uprintf(f,", PWEN");
	} else {
		uprintf(f,"%s %s %s %s", 
			cntrl&UNIV_SCTL_PGM ?   "Pgm," : "    ",
			cntrl&UNIV_SCTL_DAT ?   "Dat," : "    ",
			cntrl&UNIV_SCTL_SUPER ? "Sup," : "    ",
			cntrl&UNIV_SCTL_USER  ? "Usr" :  "");
		if ( cntrl & UNIV_SCTL_PWEN )
			uprintf(f,", PWEN");
		if ( cntrl & UNIV_SCTL_PREN )
			uprintf(f,", PREN");
	}
	uprintf(f,"\n");
	return 0;
}

typedef struct XlatRec_ {
	unsigned long	address;
	unsigned long	aspace;
	unsigned 		reverse; /* find reverse mapping of this port */
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

	if ( ! (VME_MODE_EXACT_MATCH & l->aspace) ) {
		cntrl &= (ismaster ? UNIV_MCTL_AM_MASK : UNIV_SCTL_AM_MASK);
		offst &= (ismaster ? UNIV_MCTL_AM_MASK : UNIV_SCTL_AM_MASK);
	}

	if ( cntrl != offst )
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
	if (l->reverse) {
		/* reverse mapping, i.e. for master ports we map from
		 * VME to PCI, for slave ports we map from VME to PCI
		 */
		if (l->address >= start && l->address < bound) {
				l->address+=offst;
				return 1;
		}
	} else {
		x = l->address - offst;

		if (x >= start && x < bound) {
			/* valid address found */
			l->address = x;
			return 1;
		}
	}
	return 0;
}


static int
mapOverAll(volatile LERegister *base, int ismaster, int (*func)(int,int,volatile LERegister *,void*), void *arg)
{
volatile LERegister	*rptr;
unsigned long	port;
int	rval;

	CHECK_DFLT_BASE(base);

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
}

static void
showUniversePorts(volatile LERegister *base, int ismaster, FILE *f)
{
	if (!f) f=stdout;
	uprintf(f,"Universe %s Ports:\n",ismaster ? "Master" : "Slave");
	uprintf(f,"Port  VME-Addr   Size       PCI-Adrs   Mode:\n");
	mapOverAll(base,ismaster,showUniversePort,f);
}

int
vmeUniverseXlateAddrXX(
	volatile LERegister *base,	/* Universe base address */
	int master, 		/* look in the master windows */
	int reverse,		/* reverse mapping; for masters: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	)
{
int	rval;
XlatRec l;
	l.aspace  = as;
	l.address = aIn;
	l.reverse = reverse;
	/* map result -1/0/1 to -2/-1/0 with 0 on success */
	rval = mapOverAll(base,master,xlatePort,(void*)&l) - 1;
	*paOut = l.address;
	return rval;
}

int
vmeUniverseXlateAddr(
	int master, 		/* look in the master windows */
	int reverse,		/* reverse mapping; for masters: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	)
{
	DFLT_BASE;
	return vmeUniverseXlateAddrXX(base, master, reverse, as, aIn, paOut);
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


	/* I had problems with a Joerger vtr10012_8 card who would
	 * only be accessible after tweaking the U2SPEC register
	 * (the t27 parameter helped).
	 * I use the same settings here that are used by the
	 * Synergy VGM-powerpc BSP for vxWorks.
	 */
	if (2==UNIV_REV(vmeUniverse0BaseAddr))
		vmeUniverseWriteReg(UNIV_U2SPEC_DTKFLTR |
						UNIV_U2SPEC_MASt11   |
						UNIV_U2SPEC_READt27_NODELAY |
						UNIV_U2SPEC_POSt28_FAST |
						UNIV_U2SPEC_PREt28_FAST,
						UNIV_REGOFF_U2SPEC);

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
	if ( (rval=vmeUniverseFindPciBase(0,&vmeUniverse0BaseAddr)) < 0 ) {
		uprintf(stderr,"unable to find the universe in pci config space\n");
	} else {
		vmeUniverse0PciIrqLine = rval;
		rval                   = 0;
		uprintf(stderr,"Universe II PCI-VME bridge detected at 0x%08x, IRQ %d\n",
				(unsigned int)vmeUniverse0BaseAddr, vmeUniverse0PciIrqLine);
	}
	return rval;
}

void
vmeUniverseMasterPortsShowXX(volatile LERegister *base, FILE *f)
{
	showUniversePorts(base,1,f);
}

void
vmeUniverseMasterPortsShow(FILE *f)
{
	DFLT_BASE;
	showUniversePorts(base,1,f);
}

void
vmeUniverseSlavePortsShowXX(volatile LERegister *base, FILE *f)
{
	showUniversePorts(base,0,f);
}

void
vmeUniverseSlavePortsShow(FILE *f)
{
	DFLT_BASE;
	showUniversePorts(base,0,f);
}

int
vmeUniverseMasterPortCfgXX(
	volatile LERegister *base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	return cfgUniversePort(base,1,port,address_space,vme_address,local_address,length);
}

int
vmeUniverseMasterPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	DFLT_BASE;
	return cfgUniversePort(base,1,port,address_space,vme_address,local_address,length);
}

int
vmeUniverseSlavePortCfgXX(
	volatile LERegister *base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	return cfgUniversePort(base,0,port,address_space,vme_address,local_address,length);
}

int
vmeUniverseSlavePortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	local_address,
	unsigned long	length)
{
	DFLT_BASE;
	return cfgUniversePort(base,0,port,address_space,vme_address,local_address,length);
}


void
vmeUniverseDisableAllSlavesXX(volatile LERegister *base)
{
	mapOverAll(base,0,disableUniversePort,0);
}

void
vmeUniverseDisableAllSlaves(void)
{
	DFLT_BASE;
	mapOverAll(base,0,disableUniversePort,0);
}

void
vmeUniverseDisableAllMastersXX(volatile LERegister *base)
{
	mapOverAll(base,1,disableUniversePort,0);
}

void
vmeUniverseDisableAllMasters(void)
{
	DFLT_BASE;
	mapOverAll(base,1,disableUniversePort,0);
}

int
vmeUniverseStartDMAXX(
	volatile LERegister *base,
	unsigned long local_addr,
	unsigned long vme_addr,
	unsigned long count)
{
	if ((local_addr & 7) != (vme_addr & 7)) {
		uprintf(stderr,"vmeUniverseStartDMA: misaligned addresses\n");
		return -1;
	}

	{
	/* help the compiler allocate registers */
	register volatile LERegister *b=base;;
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

int
vmeUniverseStartDMA(
	unsigned long local_addr,
	unsigned long vme_addr,
	unsigned long count)
{
	DFLT_BASE; /* vmeUniverseStartDMAXX doesn't check for a valid base address for efficiency reasons */
	return vmeUniverseStartDMAXX(base, local_addr, vme_addr, count);
}

unsigned long
vmeUniverseReadRegXX(volatile LERegister *base, unsigned long offset)
{
unsigned long rval;
	rval = READ_LE(base,offset);
	return rval;
}


unsigned long
vmeUniverseReadReg(unsigned long offset)
{
unsigned long rval;
	rval = READ_LE(vmeUniverse0BaseAddr,offset);
	return rval;
}

void
vmeUniverseWriteRegXX(volatile LERegister *base, unsigned long value, unsigned long offset)
{
	WRITE_LE(value, base, offset);
}

void
vmeUniverseWriteReg(unsigned long value, unsigned long offset)
{
	WRITE_LE(value, vmeUniverse0BaseAddr, offset);
}

void
vmeUniverseResetBus(void)
{
	vmeUniverseWriteReg(
		vmeUniverseReadReg(UNIV_REGOFF_MISC_CTL) | UNIV_MISC_CTL_SW_SYSRST,
		UNIV_REGOFF_MISC_CTL);
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
			: "=r"(p) : "0"(p) : "r0"
			);
#elif defined(__rtems__)
		p--; st_le32(p, *p);
#else
#error	"vmeUniverse: endian conversion not implemented for this architecture"
#endif
	}
#endif
}

int
vmeUniverseIntRaiseXX(volatile LERegister *base, int level, unsigned vector)
{
unsigned long v;
unsigned long b;

	CHECK_DFLT_BASE(base);

	if ( level < 1 || level > 7 || vector > 255 )
		return -1;	/* invalid argument */

	if ( vector & 1 ) /* SW interrupts always ACK an even vector (pp 2-67) */
		return -1;


	/* Check if already asserted */
	if ( vmeUniverseReadRegXX(base, UNIV_REGOFF_VINT_STAT ) & UNIV_VINT_STAT_SWINT(level) ) {
		return -2;  /* already asserted */
	}

	/* Write Vector */
	vmeUniverseWriteRegXX(base, UNIV_VINT_STATID(vector), UNIV_REGOFF_VINT_STATID );

	if ( UNIV_REV(base) >= 2 ) {
		/* universe II has individual bits for individual levels */
		b = UNIV_VINT_STAT_SWINT(level);
	} else {
		/* version that is compatible with universe I */
		v  = vmeUniverseReadRegXX(base, UNIV_REGOFF_VINT_MAP1);
		v &= ~UNIV_VINT_MAP1_SWINT(0x7);
		v |=  UNIV_VINT_MAP1_SWINT(level);
		vmeUniverseWriteRegXX(base, v, UNIV_REGOFF_VINT_MAP1);
		b  = UNIV_VINT_EN_SWINT;
	}
	v = vmeUniverseReadRegXX(base, UNIV_REGOFF_VINT_EN);
	/* make sure it is clear, then assert */
	vmeUniverseWriteRegXX(base, v & ~b, UNIV_REGOFF_VINT_EN );
	vmeUniverseWriteRegXX(base, v |  b, UNIV_REGOFF_VINT_EN );

	return 0;
	
}

int
vmeUniverseIntRaise(int level, unsigned vector)
{
	return vmeUniverseIntRaiseXX(vmeUniverse0BaseAddr, level, vector);
}


/* RTEMS interrupt subsystem */

#ifdef __rtems__
#include <bsp/irq.h>

typedef struct
UniverseIRQEntryRec_ {
		VmeUniverseISR	isr;
		void			*usrData;
} UniverseIRQEntryRec, *UniverseIRQEntry;

static UniverseIRQEntry universeHdlTbl[UNIV_NUM_INT_VECS]={0};

int        vmeUniverseIrqMgrInstalled=0;

/* We support 4 wires between universe + PIC */

#define UNIV_NUM_WIRES 4

static volatile unsigned long	wire_mask[UNIV_NUM_WIRES]     = {0};
/* wires are offset by 1 so we can initialize the wire table to all zeros */
static int						universe_wire[UNIV_NUM_WIRES] = {0};

static int
lvl2bit(unsigned int level)
{
int shift = -1;
	if ( level >= UNIV_DMA_INT_VEC && level <= UNIV_LM3_INT_VEC ) {
		shift = 8 + (level-UNIV_DMA_INT_VEC);
	} else if ( UNIV_VOWN_INT_VEC == level ) {
		shift = 0;
	} else if ( 1 <= level && level <=7 ) {
		shift = level;
	} else {
		/* invalid level */
	}
	return shift;
}

int
vmeUniverseIntRoute(unsigned int level, unsigned int pin)
{
int				i, shift;
unsigned long	mask, mapreg, flags, wire;

	if ( pin >= UNIV_NUM_WIRES || ! universe_wire[pin] || !vmeUniverseIrqMgrInstalled )
		return -1;

	if ( (shift = lvl2bit(level)) < 0 ) {
		return -1; /* invalid level */
	}

	mask = 1<<shift;

	/* calculate the mapping register and contents */
	if ( shift < 8 ) {
		mapreg = UNIV_REGOFF_LINT_MAP0;
	} else if ( shift < 16 ) {
		shift -= 8;
		mapreg = UNIV_REGOFF_LINT_MAP1;
	} else if ( shift < 24 ) {
		shift -= 16;
		mapreg = UNIV_REGOFF_LINT_MAP2;
	} else {
		return -1;
	}

	shift <<=2;

	/* wires are offset by 1 so we can initialize the wire table to all zeros */
	wire = (universe_wire[pin]-1) << shift;

rtems_interrupt_disable(flags);

	for ( i = 0; i<UNIV_NUM_WIRES; i++ ) {
		wire_mask[i] &= ~mask;
	}
	wire_mask[pin] |= mask;

	mask = vmeUniverseReadReg(mapreg) & ~ (0xf<<shift);
	mask |= wire;
	vmeUniverseWriteReg( mask, mapreg );

rtems_interrupt_enable(flags);
	return 0;
}

VmeUniverseISR
vmeUniverseISRGet(unsigned long vector, void **parg)
{
unsigned long             flags;
VmeUniverseISR			  rval = 0;
volatile UniverseIRQEntry *pe  = universeHdlTbl + vector;

	if ( vector>=UNIV_NUM_INT_VECS || ! *pe )
		return 0;

	rtems_interrupt_disable(flags);
		if ( *pe ) {
			if (parg)
				*parg=(*pe)->usrData;
			rval = (*pe)->isr;
		}
	rtems_interrupt_enable(flags);
	return rval;
}

#define SPECIAL_IRQ_MSK  ( ~((UNIV_LINT_STAT_VIRQ7<<1)-UNIV_LINT_STAT_VIRQ1) )

static void
universeSpecialISR(unsigned long status)
{
register UniverseIRQEntry	ip;
register unsigned			vec;
register unsigned long		s;

	/* handle all LINT bits except for the 'normal' VME interrupts */

	/* clear all detected special interrupts */
	vmeUniverseWriteReg( (status & SPECIAL_IRQ_MSK), UNIV_REGOFF_LINT_STAT );

	/* do VOWN first */
	vec=UNIV_VOWN_INT_VEC;
	if ( (status & UNIV_LINT_STAT_VOWN) && (ip=universeHdlTbl[vec]))
		ip->isr(ip->usrData,vec);

	/* now continue with DMA and scan through all bits;
	 * we assume the vectors are in the right order!
	 *
	 * The initial right shift brings the DMA bit into position 0;
	 * the loop is left early if there are no more bits set.
	 */
	for ( s = status>>8; s; s >>= 1) {
		vec++;
		if ( (s&1) && (ip=universeHdlTbl[vec]) )
			ip->isr(ip->usrData,vec);
	}

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
universeVMEISR(rtems_irq_hdl_param arg)
{
int					pin = (int)arg;
UniverseIRQEntry	ip;
unsigned long	 	msk,lintstat,status;
int					lvl;
#ifdef BSP_PIC_DO_EOI
unsigned long 		linten;
#endif

		/* determine the highest priority IRQ source */
		lintstat  = vmeUniverseReadReg(UNIV_REGOFF_LINT_STAT);

		/* only handle interrupts routed to this pin */
		lintstat &= wire_mask[pin];

#ifdef __PPC__
		asm volatile("cntlzw %0, %1":"=r"(lvl):"r"(lintstat & ~SPECIAL_IRQ_MSK));
		lvl = 31-lvl;
		msk = 1<<lvl;
#else
		for (msk=UNIV_LINT_STAT_VIRQ7, lvl=7;
			 lvl>0;
			 lvl--, msk>>=1) {
			if (lintstat & msk) break;
		}
#endif

#ifndef BSP_PIC_DO_EOI /* Software priorities not supported */

		if ( (status = (lintstat & SPECIAL_IRQ_MSK)) )
			universeSpecialISR( status );

		if ( lvl <= 0)
			return;

#else
		if ( lvl <= 0 ) {
				/* try the special handler */
				universeSpecialISR( lintstat & SPECIAL_IRQ_MSK );

				/* 
				 * let the pic end this cycle
				 */
				if ( 0 == pin )
					BSP_PIC_DO_EOI;

				return;
		}
		linten = vmeUniverseReadReg(UNIV_REGOFF_LINT_EN);

		/* mask this and all lower levels that are routed to the same pin */
		vmeUniverseWriteReg(
						linten & ~( ((msk<<1)-UNIV_LINT_STAT_VIRQ1) & wire_mask[pin]),
						UNIV_REGOFF_LINT_EN
						);

		/* end this interrupt
		 * cycle on the PCI bus, so higher level interrupts can be
		 * caught from now on...
		 */
		if ( 0 == pin )
			BSP_PIC_DO_EOI;
#endif

		/* get vector and dispatch handler */
		status = vmeUniverseReadReg(UNIV_REGOFF_VIRQ1_STATID - 4 + (lvl<<2));
		/* determine the highest priority IRQ source */

		if (status & UNIV_VIRQ_ERR) {
				/* TODO: log error message - RTEMS has no logger :-( */
#ifdef BSP_PIC_DO_EOI
			linten &= ~msk;
#else
			vmeUniverseIntDisable(lvl);
#endif
			printk("vmeUniverse ISR: error read from STATID register; (level: %i) STATID: 0x%08x -- DISABLING\n", lvl, status);
		} else if (!(ip=universeHdlTbl[status & UNIV_VIRQ_STATID_MASK])) {
#ifdef BSP_PIC_DO_EOI
			linten &= ~msk;
#else
			vmeUniverseIntDisable(lvl);
#endif
				/* TODO: log error message - RTEMS has no logger :-( */
			printk("vmeUniverse ISR: no handler installed for this vector; (level: %i) STATID: 0x%08x -- DISABLING\n", lvl, status);
		} else {
				/* dispatch handler, it must clear the IRQ at the device */
				ip->isr(ip->usrData, status&UNIV_VIRQ_STATID_MASK);
		}

		/* clear this interrupt level; allow the universe to handler further interrupts */
		vmeUniverseWriteReg(msk, UNIV_REGOFF_LINT_STAT);

/*
 *  this seems not to be necessary; we just leave the
 *  bit set to save a couple of instructions...
		vmeUniverseWriteReg(
					UNIV_VINT_STAT_LINT(vmeIrqUnivOut),
					UNIV_REGOFF_VINT_STAT);
*/

#ifdef BSP_PIC_DO_EOI

		/* re-enable the previous level */
		vmeUniverseWriteReg(linten, UNIV_REGOFF_LINT_EN);
#endif
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

typedef struct {
	int uni_pin, pic_pin;
} IntRoute;

static void
connectIsr(int shared, rtems_irq_hdl isr, int pic_line, int pic_pin)
{
rtems_irq_connect_data	aarrggh;
	aarrggh.on     = my_no_op; /* at _least_ they could check for a 0 pointer */
	aarrggh.off    = my_no_op;
	aarrggh.isOn   = my_isOn;
	aarrggh.hdl    = isr;
	aarrggh.handle = (rtems_irq_hdl_param)pic_pin;
	aarrggh.name   = pic_line;

	if ( shared ) {
#if BSP_SHARED_HANDLER_SUPPORT > 0
		if (!BSP_install_rtems_shared_irq_handler(&aarrggh))
			BSP_panic("unable to install vmeUniverse shared irq handler");
#else
		uprintf(stderr,"vmeUniverse: WARNING: your BSP doesn't support sharing interrupts\n");
		if (!BSP_install_rtems_irq_handler(&aarrggh))
			BSP_panic("unable to install vmeUniverse irq handler");
#endif
	} else {
		if (!BSP_install_rtems_irq_handler(&aarrggh))
			BSP_panic("unable to install vmeUniverse irq handler");
	}
}

int
vmeUniverseInstallIrqMgrAlt(int shared, int uni_pin0, int pic_pin0, ...)
{
int		rval;
va_list	ap;
	va_start(ap, pic_pin0);
	rval = vmeUniverseInstallIrqMgrVa(shared, uni_pin0, pic_pin0, ap);
	va_end(ap);
	return rval;
}

int
vmeUniverseInstallIrqMgrVa(int shared, int uni_pin0, int pic_pin0, va_list ap)
{
int	i,j, specialPin, uni_pin[UNIV_NUM_WIRES+1], pic_pin[UNIV_NUM_WIRES];

	if (vmeUniverseIrqMgrInstalled)                return -4;

	/* check parameters */

	if ( uni_pin0 < 0 || uni_pin0 > 7 )            return -1;

	uni_pin[0] = uni_pin0;
	pic_pin[0] = pic_pin0 < 0 ? vmeUniverse0PciIrqLine : pic_pin0;
	i = 1;
	while ( (uni_pin[i] = va_arg(ap, int)) >= 0 ) {
		
		if ( i >= UNIV_NUM_WIRES ) {
			                                       return -5;
		}

		pic_pin[i] = va_arg(ap,int);

		if ( uni_pin[i] > 7 ) {
                                                   return -2;
		}
		if ( pic_pin[i] < 0 ) {
                                                   return -3;
		}
		i++;
	}

	/* all routings must be different */
	for ( i=0; uni_pin[i] >= 0; i++ ) {
		for ( j=i+1; uni_pin[j] >= 0; j++ ) {
			if ( uni_pin[j] == uni_pin[i] )        return -6;
			if ( pic_pin[j] == pic_pin[i] )        return -7;
		}
	}

	/* give them a chance to override buggy PCI info */
	if ( pic_pin[0] >= 0 && vmeUniverse0PciIrqLine != pic_pin[0] ) {
		uprintf(stderr,"Overriding main IRQ line PCI info with %d\n",
				pic_pin[0]);
		vmeUniverse0PciIrqLine=pic_pin[0];
	}

	for ( i = 0; uni_pin[i] >= 0; i++ ) {
		/* offset wire # by one so we can initialize to 0 == invalid */
		universe_wire[i] = uni_pin[i] + 1;
		connectIsr(shared, universeVMEISR, pic_pin[i], i);
	}

	specialPin = uni_pin[1] >= 0 ? 1 : 0;

	/* setup routing */

	/* IntRoute checks for mgr being installed */
	vmeUniverseIrqMgrInstalled=1;

	/* route 7 VME irqs to first / 'normal' pin */
	for ( i=1; i<8; i++ )
		vmeUniverseIntRoute( i, 0 );
	for ( i=UNIV_VOWN_INT_VEC; i<=UNIV_LM3_INT_VEC; i++ ) {
		if ( vmeUniverseIntRoute( i, specialPin ) )
			printk("Routing lvl %i -> wire # %i failed\n", i, specialPin);
	}

	return 0;
}

int
vmeUniverseInstallIrqMgr(int vmeIrqUnivOut,
						 int vmeIrqPicLine,
						 int specialIrqUnivOut,
						 int specialIrqPicLine)
{
	return vmeUniverseInstallIrqMgrAlt(
				0,	/* bwds compat. */
				vmeIrqUnivOut, vmeIrqPicLine,
				specialIrqUnivOut, specialIrqPicLine,
				-1);
}

int
vmeUniverseInstallISR(unsigned long vector, VmeUniverseISR hdl, void *arg)
{
UniverseIRQEntry          ip;
unsigned long             flags;
volatile UniverseIRQEntry *pe;

		if (vector>sizeof(universeHdlTbl)/sizeof(universeHdlTbl[0]) || !vmeUniverseIrqMgrInstalled)
				return -1;

		pe = universeHdlTbl + vector;

		if (*pe || !(ip=(UniverseIRQEntry)malloc(sizeof(UniverseIRQEntryRec))))
				return -1;

		ip->isr=hdl;
		ip->usrData=arg;

	rtems_interrupt_disable(flags);
		if ( *pe ) {
			/* oops; someone intervened */
			rtems_interrupt_enable(flags);
			free(ip);
			return -1;
		}
		*pe = ip;
	rtems_interrupt_enable(flags);
		return 0;
}

int
vmeUniverseRemoveISR(unsigned long vector, VmeUniverseISR hdl, void *arg)
{
UniverseIRQEntry          ip;
unsigned long             flags;
volatile UniverseIRQEntry *pe;

		if (vector>sizeof(universeHdlTbl)/sizeof(universeHdlTbl[0]) || !vmeUniverseIrqMgrInstalled)
				return -1;

		pe = universeHdlTbl + vector;

	rtems_interrupt_disable(flags);
		ip = *pe;
		if (!ip || ip->isr!=hdl || ip->usrData!=arg) {
			rtems_interrupt_enable(flags);
			return -1;
		}
		*pe = 0;
	rtems_interrupt_enable(flags);
		free(ip);
		return 0;
}

static int
intDoEnDis(unsigned int level, int dis)
{
unsigned long	flags, v;
int				shift;

	if (  ! vmeUniverseIrqMgrInstalled || (shift = lvl2bit(level)) < 0 )
		return -1;

	v = 1<<shift;

	if ( !dis )
		return vmeUniverseReadReg(UNIV_REGOFF_LINT_EN) & v ? 1 : 0;

	rtems_interrupt_disable(flags);
	if ( dis<0 )
		vmeUniverseWriteReg( vmeUniverseReadReg(UNIV_REGOFF_LINT_EN) & ~v, UNIV_REGOFF_LINT_EN ); 
	else {
		vmeUniverseWriteReg( vmeUniverseReadReg(UNIV_REGOFF_LINT_EN) |  v, UNIV_REGOFF_LINT_EN  ); 
	}
	rtems_interrupt_enable(flags);
		return 0;
}

int
vmeUniverseIntEnable(unsigned int level)
{
	return intDoEnDis(level, 1);
}

int
vmeUniverseIntDisable(unsigned int level)
{
	return intDoEnDis(level, -1);
}

int
vmeUniverseIntIsEnabled(unsigned int level)
{
	return intDoEnDis(level, 0);
}

/* Loopback test of VME/universe interrupts */

typedef struct {
	rtems_id	q;
	int			l;
} LoopbackTstArgs;

static void
loopbackTstIsr(void *arg, unsigned long vector)
{
LoopbackTstArgs *pa = arg;
	if ( RTEMS_SUCCESSFUL != rtems_message_queue_send(pa->q, (void*)&vector, sizeof(vector)) ) {
		/* Overrun ? */
		printk("vmeUniverseIntLoopbackTst: (ISR) message queue full / overrun ? disabling IRQ level %i\n", pa->l);
		vmeUniverseIntDisable(pa->l);
	}
}

int
vmeUniverseIntLoopbackTst(int level, unsigned vector)
{
DFLT_BASE;
rtems_status_code	sc;
rtems_id			q = 0;
int					installed = 0;
int					i, err = 0;
int					doDisable = 0;
uint32_t		size;
unsigned long		msg;
char *				irqfmt  = "VME IRQ @vector %3i %s";
char *				iackfmt = "VME IACK            %s";
LoopbackTstArgs		a;

	CHECK_DFLT_BASE(base);

	/* arg check */
	if ( level < 1 || level > 7 || vector > 255 )
		return -1;

	if ( UNIV_REV(base) < 2 && vector != 0 ) {
		fprintf(stderr,
			"vmeUniverseIntLoopbackTst(): Universe 1 has a bug. IACK in response to\n");
		fprintf(stderr,
			"self-generated VME interrupt yields always a zero vector. As a workaround,\n");
		fprintf(stderr,
			"use vector 0, please.\n");
		return -1;
	}

	/* Create message queue */
	if ( RTEMS_SUCCESSFUL != (sc=rtems_message_queue_create(
        								rtems_build_name('t' ,'U','I','I'),
								        4,
        								sizeof(unsigned long),
        								0,  /* default attributes: fifo, local */
        								&q)) ) {
		rtems_error(sc, "vmeUniverseIntLoopbackTst: Unable to create message queue");
		goto bail;
	}

	a.q = q;
	a.l = level;

	/* Install handlers */
	if ( vmeUniverseInstallISR(vector, loopbackTstIsr, (void*)&a) ) {
		fprintf(stderr,"Unable to install VME ISR to vector %i\n",vector);
		goto bail;
	}
	installed++;
	if ( vmeUniverseInstallISR(UNIV_VME_SW_IACK_INT_VEC, loopbackTstIsr, (void*)&a) ) {
		fprintf(stderr,"Unable to install VME ISR to IACK special vector %i\n",UNIV_VME_SW_IACK_INT_VEC);
		goto bail;
	}
	installed++;

	if ( !vmeUniverseIntIsEnabled(level) && 0==vmeUniverseIntEnable(level) )
		doDisable = 1;
	
	/* make sure there are no pending interrupts */
	vmeUniverseWriteReg( UNIV_LINT_STAT_SW_IACK,  UNIV_REGOFF_LINT_STAT );

	if ( vmeUniverseIntEnable( UNIV_VME_SW_IACK_INT_VEC ) ) {
		fprintf(stderr,"Unable to enable IACK interrupt\n");
		goto bail;
	}	

	printf("vmeUniverse VME interrupt loopback test; STARTING...\n");
	printf(" --> asserting VME IRQ level %i\n", level);
	vmeUniverseIntRaise(level, vector);

	for ( i = 0; i< 3; i++ ) {
    	sc = rtems_message_queue_receive(
       			    q,
		            &msg,
		            &size,
		            RTEMS_WAIT,
		            20);
		if ( sc ) {
			if ( RTEMS_TIMEOUT == sc && i>1 ) {
				/* OK; we dont' expect more to happen */
				sc = 0;
			} else {
				rtems_error(sc,"Error waiting for interrupts");
			}
			break;
		}
		if ( msg == vector ) {
			if ( !irqfmt ) {
				printf("Excess VME IRQ received ?? -- BAD\n");
				err = 1;
			} else {
				printf(irqfmt, vector, "received -- PASSED\n");
				irqfmt = 0;
			}
		} else if ( msg == UNIV_VME_SW_IACK_INT_VEC ) {
			if ( !iackfmt ) {
				printf("Excess VME IACK received ?? -- BAD\n");
				err = 1;
			} else {
				printf(iackfmt, "received -- PASSED\n");
				iackfmt = 0;
			}
		} else {
			printf("Unknown IRQ (vector %lu) received -- BAD\n", msg);
			err = 1;
		}
	}


	/* Missing anything ? */
	if ( irqfmt ) {
		printf(irqfmt,vector, "MISSED -- BAD\n");
		err = 1;
	}
	if ( iackfmt ) {
		printf(iackfmt, "MISSED -- BAD\n");
		err = 1;
	}

	printf("FINISHED.\n");

bail:
	if ( doDisable )
		vmeUniverseIntDisable(level);
	vmeUniverseIntDisable( UNIV_VME_SW_IACK_INT_VEC );
	if ( installed > 0 )
		vmeUniverseRemoveISR(vector, loopbackTstIsr, (void*)&a);
	if ( installed > 1 )
		vmeUniverseRemoveISR(UNIV_VME_SW_IACK_INT_VEC, loopbackTstIsr, (void*)&a);
	if ( q )
		rtems_message_queue_delete(q);

	return sc ? sc : err;
}

#endif
