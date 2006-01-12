/* $Id$ */

/* Routines to configure the VME interface
 * Author: Till Straumann <strauman@slac.stanford.edu>
 *         Nov 2000, Oct 2001, Jan 2002
 */

#include <rtems.h>
#include <stdio.h>
#include <stdarg.h>
#include <bsp/irq.h>
#include <stdlib.h>
#include <rtems/bspIo.h>	/* printk */
#include <rtems/error.h>	/* printk */
#include <bsp/pci.h>
#include <bsp.h>
#include <libcpu/byteorder.h>

#include "vmeTsi148.h"

#define STATIC static

/* The tsi has 4 'local' wires that can be hooked to a PIC */

#define TSI_NUM_WIRES 			4

#define TSI148_NUM_OPORTS		8 /* number of outbound ports */
#define TSI148_NUM_IPORTS		8 /* number of inbound  ports */

#define NUM_TSI_DEVS			2 /* number of instances supported */

#define PCI_VENDOR_TUNDRA	0x10e3
#define PCI_DEVICE_TSI148	0x0148

#define TSI_OTSAU_SPACING	0x020

#define TSI_OTSAU0_REG		0x100
#define TSI_OTSAL0_REG		0x104
#define TSI_OTEAU0_REG		0x108
#define TSI_OTEAL0_REG		0x10c
#define TSI_OTOFU0_REG		0x110
#define TSI_OTOFL0_REG		0x114
#define TSI_OTBS0_REG		0x118	/* 2eSST broadcast select */
#define TSI_OTAT0_REG		0x11c
#define TSI_OTSAU_REG(port)	(TSI_OTSAU0_REG + ((port)<<5))
#define TSI_OTSAL_REG(port)	(TSI_OTSAL0_REG + ((port)<<5))
#define TSI_OTEAU_REG(port)	(TSI_OTEAU0_REG + ((port)<<5))
#define TSI_OTEAL_REG(port)	(TSI_OTEAL0_REG + ((port)<<5))
#define TSI_OTOFU_REG(port)	(TSI_OTOFU0_REG + ((port)<<5))
#define TSI_OTOFL_REG(port)	(TSI_OTOFL0_REG + ((port)<<5))
#define TSI_OTBS_REG(port)	(TSI_OTBS0_REG + ((port)<<5))
#define TSI_OTAT_REG(port)	(TSI_OTAT0_REG + ((port)<<5))
#	define TSI_OTAT_EN			(1<<31)
#	define TSI_OTAT_MRPFD		(1<<18)
#	define TSI_OTAT_PFS(x)		(((x)&3)<<16)
#	define TSI_OTAT_2eSSTM(x)	(((x)&7)<<11)
#	define TSI_OTAT_TM(x)		(((x)&7)<<8)
#	define TSI_OTAT_DBW(x)		(((x)&3)<<6)
#	define TSI_OTAT_SUP			(1<<5)
#	define TSI_OTAT_PGM			(1<<4)
#	define TSI_OTAT_ADMODE(x)	(((x)&0xf))
#	define TSI_OTAT_ADMODE_A16	0
#	define TSI_OTAT_ADMODE_A24	1
#	define TSI_OTAT_ADMODE_A32	2
#	define TSI_OTAT_ADMODE_A64	4
#	define TSI_OTAT_ADMODE_USR1	8
#	define TSI_OTAT_ADMODE_USR2	9
#	define TSI_OTAT_ADMODE_USR3	0xa
#	define TSI_OTAT_ADMODE_USR4	0xb

#define TSI_VIACK_1_REG		0x204

#define TSI_VEAU_REG		0x260
#define TSI_VEAL_REG		0x264
#define TSI_VEAT_REG		0x268

#define TSI_ITSAU_SPACING	0x020

#define TSI_ITSAU0_REG		0x300
#define TSI_ITSAL0_REG		0x304
#define TSI_ITEAU0_REG		0x308
#define TSI_ITEAL0_REG		0x30c
#define TSI_ITOFU0_REG		0x310
#define TSI_ITOFL0_REG		0x314
#define TSI_ITAT0_REG		0x318
#define TSI_ITSAU_REG(port)	(TSI_ITSAU0_REG + ((port)<<5))
#define TSI_ITSAL_REG(port)	(TSI_ITSAL0_REG + ((port)<<5))
#define TSI_ITEAU_REG(port)	(TSI_ITEAU0_REG + ((port)<<5))
#define TSI_ITEAL_REG(port)	(TSI_ITEAL0_REG + ((port)<<5))
#define TSI_ITOFU_REG(port)	(TSI_ITOFU0_REG + ((port)<<5))
#define TSI_ITOFL_REG(port)	(TSI_ITOFL0_REG + ((port)<<5))
#define TSI_ITAT_REG(port)	(TSI_ITAT0_REG + ((port)<<5))

#	define TSI_ITAT_EN			(1<<31)
#	define TSI_ITAT_TH			(1<<18)
#	define TSI_ITAT_VFS(x)		(((x)&3)<<16)
#	define TSI_ITAT_2eSSTM(x)	(((x)&7)<<12)
#	define TSI_ITAT_2eSSTB		(1<<11)
#	define TSI_ITAT_2eSST		(1<<10)
#	define TSI_ITAT_2eVME		(1<<9)
#	define TSI_ITAT_MBLT		(1<<8)
#	define TSI_ITAT_BLT			(1<<7)
#	define TSI_ITAT_AS(x)		(((x)&7)<<4)
#	define TSI_ITAT_ADMODE_A16	(0<<4)
#	define TSI_ITAT_ADMODE_A24	(1<<4)
#	define TSI_ITAT_ADMODE_A32	(2<<4)
#	define TSI_ITAT_ADMODE_A64	(4<<4)
#	define TSI_ITAT_SUP			(1<<3)
#	define TSI_ITAT_USR			(1<<2)
#	define TSI_ITAT_PGM			(1<<1)
#	define TSI_ITAT_DATA		(1<<0)

#define TSI_VICR_REG		0x440
#   define TSI_VICR_CNTS(v)		(((v)&3)<<30)
#   define TSI_VICR_CNTS_DIS	(0<<30)
#   define TSI_VICR_CNTS_IRQ1	(1<<30)
#   define TSI_VICR_CNTS_IRQ2	(2<<30)
#   define TSI_VICR_EDGIS(v)	(((v)&3)<<28)
#   define TSI_VICR_EDGIS_DIS	(0<<28)
#   define TSI_VICR_EDGIS_IRQ1	(1<<28)
#   define TSI_VICR_EDGIS_IRQ2	(2<<28)
#   define TSI_VICR_IRQ1F(v)	(((v)&3)<<26)
#   define TSI_VICR_IRQ1F_NORML	(0<<26)
#   define TSI_VICR_IRQ1F_PULSE	(1<<26)
#   define TSI_VICR_IRQ1F_CLOCK	(2<<26)
#   define TSI_VICR_IRQ1F_1MHZ	(3<<26)
#   define TSI_VICR_IRQ2F(v)	(((v)&3)<<24)
#   define TSI_VICR_IRQ2F_NORML	(0<<24)
#   define TSI_VICR_IRQ2F_PULSE	(1<<24)
#   define TSI_VICR_IRQ2F_CLOCK	(2<<24)
#   define TSI_VICR_IRQ2F_1MHZ	(3<<24)
#   define TSI_VICR_BIP			(1<<23)
#   define TSI_VICR_BIPS		(1<<22)
#   define TSI_VICR_IRQC		(1<<15)
#   define TSI_VICR_IRQLS(v)	(((v)&7)<<12)
#   define TSI_VICR_IRQS		(1<<11)
#   define TSI_VICR_IRQL(v)		(((v)&7)<<8)
#   define TSI_VICR_STID(v)		((v)&0xff)
#define TSI_INTEN_REG		0x448
#define TSI_INTEO_REG		0x44c
#define TSI_INTS_REG		0x450
#   define TSI_INTS_IRQ1S	(1<<1)
#   define TSI_INTS_IRQ2S	(1<<2)
#   define TSI_INTS_IRQ3S	(1<<3)
#   define TSI_INTS_IRQ4S	(1<<4)
#   define TSI_INTS_IRQ5S	(1<<5)
#   define TSI_INTS_IRQ6S	(1<<6)
#   define TSI_INTS_IRQ7S	(1<<7)
#   define TSI_INTS_ACFLS	(1<<8)
#   define TSI_INTS_SYSFLS	(1<<9)
#   define TSI_INTS_IACKS	(1<<10)
#   define TSI_INTS_VIES	(1<<11)
#   define TSI_INTS_VERRS	(1<<12)
#   define TSI_INTS_PERRS	(1<<13)
#   define TSI_INTS_MB0S	(1<<16)
#   define TSI_INTS_MB1S	(1<<17)
#   define TSI_INTS_MB2S	(1<<18)
#   define TSI_INTS_MB3S	(1<<19)
#   define TSI_INTS_LM0S	(1<<20)
#   define TSI_INTS_LM1S	(1<<21)
#   define TSI_INTS_LM2S	(1<<22)
#   define TSI_INTS_LM3S	(1<<23)
#   define TSI_INTS_DMA0S	(1<<24)
#   define TSI_INTS_DMA1S	(1<<25)
#define TSI_INTC_REG		0x454
#   define TSI_INTC_ACFLC	(1<<8)
#   define TSI_INTC_SYSFLC	(1<<9)
#   define TSI_INTC_IACKC	(1<<10)
#   define TSI_INTC_VIEC	(1<<11)
#   define TSI_INTC_VERRC	(1<<12)
#   define TSI_INTC_PERRC	(1<<13)
#   define TSI_INTC_MB0C	(1<<16)
#   define TSI_INTC_MB1C	(1<<17)
#   define TSI_INTC_MB2C	(1<<18)
#   define TSI_INTC_MB3C	(1<<19)
#   define TSI_INTC_LM0C	(1<<20)
#   define TSI_INTC_LM1C	(1<<21)
#   define TSI_INTC_LM2C	(1<<22)
#   define TSI_INTC_LM3C	(1<<23)
#   define TSI_INTC_DMA0C	(1<<24)
#   define TSI_INTC_DMA1C	(1<<25)
#define TSI_INTM1_REG		0x458
#define TSI_INTM2_REG		0x45c


#define TSI_RD(base, reg)				in_be32(((base) + (reg)/sizeof(*base)))
#define TSI_RD16(base, reg)				in_be16((volatile unsigned short *)(base) + (reg)/sizeof(short))
#define TSI_RD8(base, reg)				*((volatile unsigned char *)(base) + (reg))
#define TSI_WR(base, reg, val)			out_be32(((base) + (reg)/sizeof(*base)), val)

#define UNIV_SCTL_AM_MASK	(UNIV_CTL_VAS | UNIV_SCTL_PGM | UNIV_SCTL_DAT | UNIV_SCTL_USER | UNIV_SCTL_SUPER)


/* allow the BSP to override the default routines */
#ifndef BSP_PCI_FIND_DEVICE
#define BSP_PCI_FIND_DEVICE		pci_find_device
#endif
#ifndef BSP_PCI_CONFIG_IN_LONG
#define BSP_PCI_CONFIG_IN_LONG	pci_read_config_dword
#endif
#ifndef BSP_PCI_CONFIG_IN_SHORT
#define BSP_PCI_CONFIG_IN_SHORT	pci_read_config_word
#endif
#ifndef BSP_PCI_CONFIG_OUT_SHORT
#define BSP_PCI_CONFIG_OUT_SHORT pci_write_config_word
#endif
#ifndef BSP_PCI_CONFIG_IN_BYTE
#define BSP_PCI_CONFIG_IN_BYTE	pci_read_config_byte
#endif

typedef unsigned int pci_ulong;

/* PCI_MEM_BASE is a possible offset between CPU- and PCI addresses.
 * Should be defined by the BSP.
 */
#define PCI_TO_LOCAL_ADDR(memaddr) ((pci_ulong)(memaddr) + PCI_MEM_BASE)

typedef struct {
	BERegister *base;
	int			irqLine;
	int			pic_pin[TSI_NUM_WIRES];
} Tsi148Dev;

static Tsi148Dev devs[NUM_TSI_DEVS] = {{0}};

/* registers should be mapped to guarded, non-cached memory; hence 
 * subsequent stores are ordered. eieio is only needed to enforce
 * ordering of loads with respect to stores.
 */

/* private printing wrapper */
static void
uprintf(FILE *f, char *fmt, ...)
{
va_list	ap;
	va_start(ap, fmt);
	if (!f || !_impure_ptr->__sdidinit) {
		/* Might be called at an early stage when
		 * to a buffer.
		 */
		vprintk(fmt,ap);
	} else 
	{
		vfprintf(f,fmt,ap);
	}
	va_end(ap);
}

#define CHECK_BASE(base,quiet,rval)	\
	do {							\
		if ( !base ) {				\
			if ( !quiet ) {			\
				uprintf(stderr,"Tsi148: Driver not initialized\n");	\
			}						\
			return rval;			\
		}							\
	} while (0)

int
vmeTsi148FindPciBase(
	int instance,
	BERegister **pbase
	)
{
int					bus,dev,fun;
pci_ulong			busaddr;
unsigned char		irqline;
unsigned short		wrd;

	if (BSP_PCI_FIND_DEVICE(
			PCI_VENDOR_TUNDRA,
			PCI_DEVICE_TSI148,
			instance,
			&bus,
			&dev,
			&fun))
		return -1;
	if (BSP_PCI_CONFIG_IN_LONG(bus,dev,fun,PCI_BASE_ADDRESS_0,&busaddr))
		return -1;
	/* Assume upper BAR is zero */

	*pbase=(BERegister*)(PCI_TO_LOCAL_ADDR(busaddr) & ~0xff);

	if (BSP_PCI_CONFIG_IN_BYTE(bus,dev,fun,PCI_INTERRUPT_LINE,&irqline))
		return -1;

	/* Enable PCI master and memory access */
	BSP_PCI_CONFIG_IN_SHORT(bus, dev, fun, PCI_COMMAND, &wrd);
	BSP_PCI_CONFIG_OUT_SHORT(bus, dev, fun, PCI_COMMAND, wrd | PCI_COMMAND_MEMORY | PCI_COMMAND_MASTER);

	return irqline;
}

int
vmeTsi148InitInstance(unsigned instance)
{
int					irq;
BERegister *base;

	if ( instance >= NUM_TSI_DEVS )
		return  -1;
	if ( devs[instance].base )
		return  -1;

	if ((irq=vmeTsi148FindPciBase(instance,&base)) < 0) {
		uprintf(stderr,"unable to find a Tsi148 in pci config space\n");
	} else {
		uprintf(stderr,"Tundra Tsi148 PCI-VME bridge detected at 0x%08x, IRQ %d\n",
				(unsigned int)base, irq);
	}
	devs[0].base    = base;
	devs[0].irqLine = irq;

	return irq < 0 ? -1 : 0;
}

int
vmeTsi148Init(void)
{
	return vmeTsi148InitInstance(0);
}


void
vmeTsi148ResetXX(BERegister *base)
{
int port;

	CHECK_BASE(base,0, );

	vmeTsi148DisableAllOutboundPortsXX(base);
	for ( port=0; port < TSI148_NUM_OPORTS; port++ )
		TSI_WR(base, TSI_OTBS_REG(port), 0);
	TSI_WR(base, TSI_INTEO_REG, 0);
	TSI_WR(base, TSI_INTEN_REG, 0);
	TSI_WR(base, TSI_INTC_REG, 0xffffffff);
	TSI_WR(base, TSI_INTM1_REG, 0);
	TSI_WR(base, TSI_INTM2_REG, 0);
	TSI_WR(base, TSI_VICR_REG, 0);
	TSI_WR(base, TSI_VEAT_REG, TSI_VEAT_VESCL);
}

void
vmeTsi148Reset()
{
	vmeTsi148ResetXX(devs[0].base);
}

/* convert an address space selector to a corresponding
 * Tsi148 control mode word
 */

STATIC int
am2omode(unsigned long address_space, unsigned long *pmode)
{
unsigned long mode=0;
unsigned long tm, mask;

	if ( ! (VME_MODE_DBW32_DISABLE	& address_space ) )
		mode |= TSI_OTAT_DBW(1);
	if ( ! (VME_MODE_PREFETCH_ENABLE & address_space) )
		mode |= TSI_OTAT_MRPFD;
	else {
		mode |= TSI_OTAT_PFS(address_space>>12);
	}
	mode |= TSI_OTAT_2eSSTM(address_space>>16);

	for ( tm = 1, mask = VME_MODE_BLT; ! (mask & address_space); tm++, mask<<=1 ) {
		if ( VME_MODE_2eSST_BCST == mask ) {
			tm = 0;	/* select default: BLT enabled */
			break;
		}
	}
	mode |= TSI_OTAT_TM(tm);

	switch (address_space & VME_AM_MASK) {
		case VME_AM_STD_SUP_PGM:
		case VME_AM_STD_USR_PGM:

			mode |= TSI_OTAT_PGM;

			/* fall thru */
		case VME_AM_STD_SUP_DATA:
		case VME_AM_STD_USR_DATA:

			mode |= TSI_OTAT_ADMODE_A24;
			break;

		case VME_AM_EXT_SUP_PGM:
		case VME_AM_EXT_USR_PGM:
			mode |= TSI_OTAT_PGM;

			/* fall thru */
		case VME_AM_EXT_SUP_DATA:
		case VME_AM_EXT_USR_DATA:
			mode |= TSI_OTAT_ADMODE_A32;
			break;

		case VME_AM_SUP_SHORT_IO:
		case VME_AM_USR_SHORT_IO:
			mode |= TSI_OTAT_ADMODE_A16;
			break;

		case 0: /* disable the port alltogether */
			break;

		default:
			return -1;
	}
	if ( VME_AM_IS_SUP(address_space) )
		mode |= TSI_OTAT_SUP;
	*pmode = mode;
	return 0;
}

STATIC int
am2imode(unsigned long address_space, unsigned long *pmode)
{
unsigned long mode=0;
unsigned long tm, mask;

	mode |= TSI_ITAT_VFS(address_space>>12);
	mode |= TSI_ITAT_2eSSTM(address_space>>16);

	mode |= TSI_ITAT_BLT;

	mask = VME_MODE_BLT;
	tm = TSI_ITAT_BLT;
	do {
		mask<<=1; tm<<=1;
		if ( address_space & mask )
			mode |= tm;
	} while ( TSI_ITAT_2eSSTB != tm );

	tm = 0;

	switch (address_space & VME_AM_MASK) {
		case VME_AM_STD_SUP_PGM:
		case VME_AM_STD_USR_PGM:

			tm = 1;

			/* fall thru */
		case VME_AM_STD_SUP_DATA:
		case VME_AM_STD_USR_DATA:

			mode |= TSI_ITAT_ADMODE_A24;
			break;

		case VME_AM_EXT_SUP_PGM:
		case VME_AM_EXT_USR_PGM:
			tm = 1;

			/* fall thru */
		case VME_AM_EXT_SUP_DATA:
		case VME_AM_EXT_USR_DATA:
			mode |= TSI_ITAT_ADMODE_A32;
			break;

		case VME_AM_SUP_SHORT_IO:
		case VME_AM_USR_SHORT_IO:
			mode |= TSI_ITAT_ADMODE_A16;
			break;

		case 0: /* disable the port alltogether */
			*pmode = 0;
			return 0;

		default:
			return -1;
	}

	if ( VME_AM_IS_SUP(address_space) )
		mode |= TSI_ITAT_SUP;
	else
		mode |= TSI_ITAT_USR;

	if ( tm )
		mode |= TSI_ITAT_PGM;
	else
		mode |= TSI_ITAT_DATA;

	*pmode = mode;
	return 0;
}

static void
readTriple(
	BERegister *base,
	unsigned reg,
	unsigned long long *ps,
	unsigned long long *pl,
	unsigned long long *po)
{
	*ps = TSI_RD(base, reg);
	*ps = (*ps<<32) | (TSI_RD(base, (reg+4)) & 0xffff0000);
	*pl = TSI_RD(base, (reg+8));
	*pl = (*pl<<32) | (TSI_RD(base, (reg+0xc)) & 0xffff0000);
	*po = TSI_RD(base, (reg+0x10));
	*po = (*po<<32) | (TSI_RD(base, (reg+0x14)) & 0xffff0000);
}


static unsigned long
inboundGranularity(unsigned long itat)
{
	switch ( itat & TSI_ITAT_AS(-1) ) {
		case TSI_ITAT_ADMODE_A16: 	return 0xf;
		case TSI_ITAT_ADMODE_A24: 	return 0xfff;
		default:
		break;
	}
	return 0xffff;
}

static int
configTsiPort(
	BERegister		*base,
	int				isout,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length)
{
unsigned long long	start, limit, offst;
unsigned long		mode, mask, tat_reg, tsau_reg;
char				*name = (isout ? "Outbound" : "Inbound");

	CHECK_BASE(base,0,-1);

	mode = 0; /* silence warning */

	if ( port >= (isout ? TSI148_NUM_OPORTS : TSI148_NUM_IPORTS) ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid port\n", name);
		return -1;
	}

	if ( length && (isout ? am2omode(address_space, &mode) : am2imode(address_space, &mode)) ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid address space / mode flags\n",name);
		return -1;
	}


	if ( isout ) {
		start     = pci_address;
		offst     = (unsigned long long)vme_address - start;
		mask      = 0xffff;
		tat_reg   = TSI_OTAT_REG(port);
		tsau_reg  = TSI_OTSAU_REG(port);
		mode     |= TSI_OTAT_EN;
	} else {
		start     = vme_address;
		offst     = (unsigned long long)pci_address - start;
		mask      = inboundGranularity(mode);
		tat_reg   = TSI_ITAT_REG(port);
		tsau_reg  = TSI_ITSAU_REG(port);
		mode     |= TSI_ITAT_EN;
	}

	/* If they pass 'length==0' just disable */
	if ( 0 == length ) {
		TSI_WR(base, tat_reg, TSI_RD(base, tat_reg) & ~(isout ? TSI_OTAT_EN : TSI_ITAT_EN));
		return 0;
	}


	if (   (vme_address & mask)
	    || (pci_address & mask)
	    || (length      & mask) ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid address/length; must be multiple of 0x%x\n",
				name,
				mask+1);
		return -1;
	}

	limit  = start + length - 1;

	if ( limit >= (unsigned long long)1<<32 ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid address/length; must be < 1<<32\n", name);
		return -1;
	}

	/* Disable port */
	TSI_WR(base, tat_reg, 0);

	/* Force to 32-bits */
	TSI_WR(base, tsau_reg       , 0);
	TSI_WR(base, tsau_reg + 0x04, (uint32_t)start);
	TSI_WR(base, tsau_reg + 0x08, 0);
	TSI_WR(base, tsau_reg + 0x0c, (uint32_t)limit);
	TSI_WR(base, tsau_reg + 0x10, (uint32_t)(offst>>32));
	TSI_WR(base, tsau_reg + 0x14, (uint32_t)offst);

	/* (outbound only:) leave 2eSST broadcast register alone for user to program */

	/* Set mode and enable */
	TSI_WR(base, tat_reg, mode);
	return 0;
}

static int
disableTsiPort(
	BERegister		*base,
	int				isout,
	unsigned long	port)
{
	return configTsiPort(base, isout, port, 0, 0, 0, 0);
}

int
vmeTsi148InboundPortCfgXX(
	BERegister		*base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length)
{
	return configTsiPort(base, 0, port, address_space, vme_address, pci_address, length);
}

int
vmeTsi148InboundPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length)
{
	return configTsiPort(devs[0].base, 0, port, address_space, vme_address, pci_address, length);
}


int
vmeTsi148OutboundPortCfgXX(
	BERegister		*base,
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length)
{
	return configTsiPort(base, 1, port, address_space, vme_address, pci_address, length);
}

int
vmeTsi148OutboundPortCfg(
	unsigned long	port,
	unsigned long	address_space,
	unsigned long	vme_address,
	unsigned long	pci_address,
	unsigned long	length)
{
	return configTsiPort(devs[0].base, 1, port, address_space, vme_address, pci_address, length);
}


int
vmeTsi148XlateAddrXX(
	BERegister *base,	/* TSI 148 base address */
	int outbound, 		/* look in the outbound windows */
	int reverse,		/* reverse mapping; for outbound ports: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	)
{
unsigned long		mode, mode_msk;
int					port;
unsigned long long	start, limit, offst, a;
unsigned long		tsau_reg, tat_reg, gran, skip;

	CHECK_BASE(base,0,-1);

	mode = 0; /* silence warning */

	if ( VME_MODE_EXACT_MATCH & as ) {
		mode_msk = ~0;
	} else {
		if ( outbound ) 
			mode_msk = TSI_OTAT_PGM | TSI_OTAT_SUP | TSI_OTAT_ADMODE(-1) | TSI_OTAT_EN;
		else
			mode_msk = TSI_ITAT_PGM | TSI_ITAT_DATA | TSI_ITAT_SUP | TSI_ITAT_USR | TSI_ITAT_AS(-1) | TSI_ITAT_EN;
	}

	as &= ~VME_MODE_EXACT_MATCH;

	if ( outbound ? am2omode(as,&mode) : am2imode(as,&mode) ) {
		uprintf(stderr, "vmeTsi148XlateAddr: invalid address space/mode argument");
		return -2;
	}

	if (outbound ) {
		tsau_reg = TSI_OTSAU_REG(0);
		tat_reg  = TSI_OTAT_REG(0);
		skip     = TSI_OTSAU_SPACING;
		mode    |= TSI_OTAT_EN;
		gran	 = 0x10000;
	} else {
		tsau_reg = TSI_ITSAU_REG(0);
		tat_reg  = TSI_ITAT_REG(0);
		skip     = TSI_ITSAU_SPACING;
		mode    |= TSI_ITAT_EN;
		gran	 = inboundGranularity(mode) + 1;
	}

	for ( port = 0; port < TSI148_NUM_OPORTS; port++, tsau_reg += skip, tat_reg += skip ) {

		if ( (mode & mode_msk) == (TSI_RD(base, tat_reg) & mode_msk) ) {

			/* found a window with of the right mode; now check the range */
			readTriple(base, tsau_reg, &start, &limit, &offst);
			limit += gran;

			if ( !reverse ) {
				start += offst;	
				limit += offst;
				offst  = -offst;
			}
			a = aIn;
			if ( aIn >= start && aIn <= limit ) {
				/* found it */
				*paOut = (unsigned long)(a + offst);
				return 0;
			}
		}
	}

	uprintf(stderr, "vmeTsi148XlateAddr: no matching mapping found\n");
	return -1;
}

int
vmeTsi148XlateAddr(
	int outbound, 		/* look in the outbound windows */
	int reverse,		/* reverse mapping; for outbound ports: map local to VME */
	unsigned long as,	/* address space */
	unsigned long aIn,	/* address to look up */
	unsigned long *paOut/* where to put result */
	)
{
	return vmeTsi148XlateAddrXX(devs[0].base, outbound, reverse, as, aIn, paOut);
}


/* printk cannot format %llx */
static void uprintfllx(FILE *f, unsigned long long v)
{
	if ( v >= ((unsigned long long)1)<<32 )
		uprintf(f,"0x%lx%08lx ", (unsigned long)(v>>32), (unsigned long)(v & 0xffffffff));
	else
		uprintf(f,"0x%08lx ", (unsigned long)(v & 0xffffffff));
}

void
vmeTsi148OutboundPortsShowXX(BERegister *base, FILE *f)
{
int				port;
unsigned long	mode;
char			tit = 0;

unsigned long long	start, limit, offst;

	CHECK_BASE(base,0, );

	if (!f) f=stdout;
	uprintf(f,"Tsi148 Outbound Ports:\n");

	for ( port = 0; port < TSI148_NUM_OPORTS; port++ ) {
		mode = TSI_RD(base, TSI_OTAT_REG(port));
		if ( ! (TSI_OTAT_EN & mode) )
			continue; /* skip disabled ports */

		readTriple(base, TSI_OTSAU_REG(port), &start, &limit, &offst);

		/* convert limit to size */
		limit = limit-start+0x10000;
		if ( !tit ) {
			uprintf(f,"Port  VME-Addr   Size       PCI-Adrs   Mode:\n");
			tit = 1;
		}
		uprintf(f,"%d:    ", port);
		uprintfllx(f,start+offst);
		uprintfllx(f,limit);
		uprintfllx(f,start);
		switch( mode & TSI_OTAT_ADMODE(-1) ) {
			case TSI_OTAT_ADMODE_A16: uprintf(f,"A16"); break;
			case TSI_OTAT_ADMODE_A24: uprintf(f,"A24"); break;
			case TSI_OTAT_ADMODE_A32: uprintf(f,"A32"); break;
			case TSI_OTAT_ADMODE_A64: uprintf(f,"A64"); break;
			default:                  uprintf(f,"A??"); break;
		}

		if ( mode & TSI_OTAT_PGM ) uprintf(f,", PGM");
		if ( mode & TSI_OTAT_SUP ) uprintf(f,", SUP");
		if ( ! (TSI_OTAT_MRPFD & mode) ) uprintf(f,", PREFETCH");

		switch ( mode & TSI_OTAT_DBW(-1) ) {
			case TSI_OTAT_DBW(0):	uprintf(f,", D16"); break;
			case TSI_OTAT_DBW(1):	uprintf(f,", D32"); break;
			default: 				uprintf(f,", D??"); break;
		}

		switch( mode & TSI_OTAT_TM(-1) ) {
			case TSI_OTAT_TM(0):	uprintf(f,", SCT"); 		break;
			case TSI_OTAT_TM(1):	uprintf(f,", BLT"); 		break;
			case TSI_OTAT_TM(2):	uprintf(f,", MBLT"); 		break;
			case TSI_OTAT_TM(3):	uprintf(f,", 2eVME"); 		break;
			case TSI_OTAT_TM(4):	uprintf(f,", 2eSST");  		break;
			case TSI_OTAT_TM(5):	uprintf(f,", 2eSST_BCST");	break;
			default: 				uprintf(f," TM??");		break;
		}

		uprintf(f,"\n");
	}
}

void
vmeTsi148OutboundPortsShow(FILE *f)
{
	vmeTsi148OutboundPortsShowXX(devs[0].base, f);
}

void
vmeTsi148InboundPortsShowXX(BERegister *base, FILE *f)
{
int				port;
unsigned long	mode;
char			tit = 0;

unsigned long long	start, limit, offst;

	CHECK_BASE(base,0, );

	if (!f) f=stdout;
	uprintf(f,"Tsi148 Inbound Ports:\n");

	for ( port = 0; port < TSI148_NUM_IPORTS; port++ ) {
		mode = TSI_RD(base, TSI_ITAT_REG(port));
		if ( ! (TSI_ITAT_EN & mode) )
			continue; /* skip disabled ports */

		readTriple(base, TSI_ITSAU_REG(port), &start, &limit, &offst);

		/* convert limit to size */
		limit = limit - start + inboundGranularity(mode) + 1;
		if ( !tit ) {
			uprintf(f,"Port  VME-Addr   Size       PCI-Adrs   Mode:\n");
			tit = 1;
		}
		uprintf(f,"%d:    ", port);
		uprintfllx(f,start);
		uprintfllx(f,limit);
		uprintfllx(f,start+offst);
		switch( mode & TSI_ITAT_AS(-1) ) {
			case TSI_ITAT_ADMODE_A16: uprintf(f,"A16"); break;
			case TSI_ITAT_ADMODE_A24: uprintf(f,"A24"); break;
			case TSI_ITAT_ADMODE_A32: uprintf(f,"A32"); break;
			case TSI_ITAT_ADMODE_A64: uprintf(f,"A64"); break;
			default:                  uprintf(f,"A??"); break;
		}

		if ( mode & TSI_ITAT_PGM )  uprintf(f,", PGM");
		if ( mode & TSI_ITAT_DATA ) uprintf(f,", DAT");
		if ( mode & TSI_ITAT_SUP )  uprintf(f,", SUP");
		if ( mode & TSI_ITAT_USR )  uprintf(f,", USR");

		if ( mode & TSI_ITAT_2eSSTB )  uprintf(f,", 2eSSTB");
		if ( mode & TSI_ITAT_2eSST  )  uprintf(f,", 2eSST");
		if ( mode & TSI_ITAT_2eVME  )  uprintf(f,", 2eVME");
		if ( mode & TSI_ITAT_MBLT   )  uprintf(f,", MBLT");
		if ( mode & TSI_ITAT_BLT    )  uprintf(f,", BLT");

		uprintf(f,"\n");
	}
}

void
vmeTsi148InboundPortsShow(FILE *f)
{
	vmeTsi148InboundPortsShowXX(devs[0].base, f);
}


void
vmeTsi148DisableAllInboundPortsXX(BERegister *base)
{
int port;

	for ( port = 0; port < TSI148_NUM_IPORTS; port++ )
		if ( disableTsiPort(base, 0, port) )
			break;
}

void
vmeTsi148DisableAllInboundPorts(void)
{
	vmeTsi148DisableAllInboundPortsXX(devs[0].base);
}

void
vmeTsi148DisableAllOutboundPortsXX(BERegister *base)
{
int port;

	for ( port = 0; port < TSI148_NUM_IPORTS; port++ )
		if ( disableTsiPort(base, 1, port) )
			break;
}

void
vmeTsi148DisableAllOutboundPorts(void)
{
	vmeTsi148DisableAllOutboundPortsXX(devs[0].base);
}


/* Interrupt Subsystem */

typedef struct
IRQEntryRec_ {
		VmeTsi148ISR	isr;
		void			*usrData;
} IRQEntryRec, *IRQEntry;

static IRQEntry irqHdlTbl[TSI_NUM_INT_VECS]={0};

int        vmeTsi148IrqMgrInstalled=0;

static volatile unsigned long	wire_mask[TSI_NUM_WIRES]     = {0};
/* wires are offset by 1 so we can initialize the wire table to all zeros */
static int						tsi_wire[TSI_NUM_WIRES] = {0};

/* how should we iack a given level, 1,2,4 bytes? */
static unsigned char tsi_iack_width[7] = {
	1,1,1,1,1,1,1
};

/* map universe compatible vector # to Tsi slot (which maps to bit layout in stat/enable/... regs) */
static int uni2tsi_vec_map[TSI_NUM_INT_VECS-256] = {
	/* 256 no VOWN interrupt */			-1,
	/* TSI_DMA_INT_VEC			257 */	256 + 24 - 8,
	/* TSI_LERR_INT_VEC			258 */	256 + 13 - 8,
	/* TSI_VERR_INT_VEC			259 */	256 + 12 - 8,
	/* 260 is reserved       */			-1,
	/* TSI_VME_SW_IACK_INT_VEC	261 */	256 + 10 - 8,
	/* 262 no PCI SW IRQ     */			-1,
	/* TSI_SYSFAIL_INT_VEC		263 */	256 +  9 - 8,
	/* TSI_ACFAIL_INT_VEC		264 */	256 +  8 - 8,
	/* TSI_MBOX0_INT_VEC		265 */	256 + 16 - 8,
	/* TSI_MBOX1_INT_VEC		266 */	256 + 17 - 8,
	/* TSI_MBOX2_INT_VEC		267 */	256 + 18 - 8,
	/* TSI_MBOX3_INT_VEC		268 */	256 + 19 - 8,
	/* TSI_LM0_INT_VEC			269 */	256 + 20 - 8,
	/* TSI_LM1_INT_VEC			270 */	256 + 21 - 8,
	/* TSI_LM2_INT_VEC			271 */	256 + 22 - 8,
	/* TSI_LM3_INT_VEC			272 */	256 + 23 - 8,
/* New vectors; only on TSI148 */
	/* TSI_VIES_INT_VEC			273 */	256 + 11 - 8,
	/* TSI_DMA1_INT_VEC			274 */	256 + 25 - 8,
};

/* and the reverse; map tsi bit number to universe compatible 'special' vector number */
static int tsi2uni_vec_map[TSI_NUM_INT_VECS - 256] = {
	TSI_ACFAIL_INT_VEC,
	TSI_SYSFAIL_INT_VEC,
	TSI_VME_SW_IACK_INT_VEC,
	TSI_VIES_INT_VEC,
	TSI_VERR_INT_VEC,
	TSI_LERR_INT_VEC,
	-1,
	-1,
	TSI_MBOX0_INT_VEC,
	TSI_MBOX1_INT_VEC,
	TSI_MBOX2_INT_VEC,
	TSI_MBOX3_INT_VEC,
	TSI_LM0_INT_VEC,
	TSI_LM1_INT_VEC,
	TSI_LM2_INT_VEC,
	TSI_LM3_INT_VEC,
	TSI_DMA_INT_VEC,
	TSI_DMA1_INT_VEC,
	-1,
};

static inline int
uni2tsivec(int v)
{
	if ( v < 0 || v >= TSI_NUM_INT_VECS )
		return -1;
	return v < 256 ? v : uni2tsi_vec_map[v-256];
}

static int
lvl2bitno(unsigned int level)
{
	if ( level >= 256 )
		return uni2tsivec(level) + 8 - 256;
	else if ( level < 8 && level > 0 )
		return level;
	return -1;
}

int
vmeTsi148IntRoute(unsigned int level, unsigned int pin)
{
int				i;
unsigned long	mask, shift, mapreg, flags, wire;

	if ( pin >= TSI_NUM_WIRES || ! tsi_wire[pin] || !vmeTsi148IrqMgrInstalled )
		return -1;

	if ( level >= 256 ) {
		if ( (i = uni2tsivec(level)) < 0 )
			return -1;
		shift = 8 + (i-256);
	} else if ( 1 <= level && level <=7 ) {
		shift = level;
	} else {
		return -1;	/* invalid level */
	}

	mask = 1<<shift;

	/* calculate the mapping register and contents */
	if ( shift < 16 ) {
		mapreg = TSI_INTM2_REG;
	} else if ( shift < 32 ) {
		shift -= 16;
		mapreg = TSI_INTM1_REG;
	} else {
		return -1;
	}

	shift <<=1;

	/* wires are offset by 1 so we can initialize the wire table to all zeros */
	wire = (tsi_wire[pin]-1) << shift;

rtems_interrupt_disable(flags);

	for ( i = 0; i<TSI_NUM_WIRES; i++ ) {
		wire_mask[i] &= ~mask;
	}
	wire_mask[pin] |= mask;

	mask = TSI_RD(devs[0].base, mapreg) & ~ (0x3<<shift);
	mask |= wire;
	TSI_WR( devs[0].base, mapreg, mask );

rtems_interrupt_enable(flags);
	return 0;
}

VmeTsi148ISR
vmeTsi148ISRGet(unsigned long vector, void **parg)
{
VmeTsi148ISR	  rval = 0;
unsigned long	  flags;
volatile IRQEntry *p;
int               v = uni2tsivec(vector);


	if ( v < 0 )
		return rval;

	p = irqHdlTbl + v;

	rtems_interrupt_disable(flags);
		if ( *p ) {
			if ( parg )
				*parg = (*p)->usrData;
			rval = (*p)->isr;
		}
	rtems_interrupt_enable(flags);

	return rval;
}

static void
tsiVMEISR(rtems_irq_hdl_param arg)
{
int					pin = (int)arg;
BERegister			*b  = devs[0].base;
IRQEntry			ip;
unsigned long	 	msk,lintstat,vector, vecarg;
int					lvl;

	/* only handle interrupts routed to this pin */
	while ( (lintstat  = (TSI_RD(b, TSI_INTS_REG) & wire_mask[pin])) ) {

		/* bit 0 is never set since it is never set in wire_mask */

		do {
			/* simplicity is king; just handle them in MSB to LSB order; reserved bits read as 0 */
#ifdef __PPC__
			asm volatile("cntlzw %0, %1":"=r"(lvl):"r"(lintstat));
			lvl = 31-lvl;
			msk = 1<<lvl;
#else
			{ static unsigned long m[] = {
											 0xffff0000, 0xff00ff00, 0xf0f0f0f0, 0xcccccccc, 0xaaaaaaaa
										 };
			int      i;
			unsigned tmp;

			/* lintstat has already been checked...
			if ( !lintstat ) {
			   lvl = -1; msk = 0;
			} else
			 */
			for ( i=0, lvl=0, msk = lintstat; i<5; i++ ) {
				lvl <<= 1;
				if ( (tmp = msk & m[i]) ) {
					lvl++;
					msk = tmp;
				} else
					msk = msk & ~m[i];
			}
			}
#endif

			if ( lvl > 7 ) {
				/* clear this interrupt level */
				TSI_WR(b, TSI_INTC_REG, msk);
				vector = 256 + lvl - 8;
				vecarg = tsi2uni_vec_map[lvl-8];
			} else {
				/* need to do get the vector for this level */
				switch ( tsi_iack_width[lvl-1] ) {
					default:
					case  1:
						vector = TSI_RD8(b, TSI_VIACK_1_REG - 4 + (lvl<<2) + 3);
						break;

					case  2:
						vector = TSI_RD16(b, TSI_VIACK_1_REG - 4 + (lvl<<2) + 2);
						break;

					case  4:
						vector = TSI_RD(b, TSI_VIACK_1_REG - 4 + (lvl<<2));
						break;
				}
				vecarg = vector;
			}

			if ( !(ip=irqHdlTbl[vector])) {
				/* TODO: log error message - RTEMS has no logger :-( */
				vmeTsi148IntDisable(lvl);
				printk("vmeTsi148 ISR: ERROR: no handler registered (level %i) IACK 0x%08x -- DISABING level %i\n",
						lvl, vector, lvl);
			} else {
				/* dispatch handler, it must clear the IRQ at the device */
				ip->isr(ip->usrData, vecarg);
			}
		} while ( (lintstat &= ~msk) );
		/* check if a new irq is pending already */
	}
}


static void
my_no_op(const rtems_irq_connect_data * arg)
{}

static int
my_isOn(const rtems_irq_connect_data *arg)
{
		return (int)(TSI_RD(devs[0].base, TSI_INTEO_REG) & TSI_RD(devs[0].base, TSI_INTEN_REG));
}

static void
connectIsr(int shared, rtems_irq_hdl isr, int pic_line, int slot)
{
rtems_irq_connect_data	xx;
	xx.on     = my_no_op; /* at _least_ they could check for a 0 pointer */
	xx.off    = my_no_op;
	xx.isOn   = my_isOn;
	xx.hdl    = isr;
	xx.handle = (rtems_irq_hdl_param)slot; 
	xx.name   = pic_line;

	if ( shared ) {
#if BSP_SHARED_HANDLER_SUPPORT > 0
		if (!BSP_install_rtems_shared_irq_handler(&xx))
			BSP_panic("unable to install vmeTsi148 shared irq handler");
#else
        uprintf(stderr,"vmeTsi148: WARNING: your BSP doesn't support sharing interrupts\n");
		if (!BSP_install_rtems_irq_handler(&xx))
			BSP_panic("unable to install vmeTsi148 irq handler");
#endif
	} else {
		if (!BSP_install_rtems_irq_handler(&xx))
			BSP_panic("unable to install vmeTsi148 irq handler");
	}
}

int
vmeTsi148InstallIrqMgrAlt(int shared, int tsi_pin0, int pic_pin0, ...)
{
int		rval;
va_list	ap;
	va_start(ap, pic_pin0);
	rval = vmeTsi148InstallIrqMgrVa(shared, tsi_pin0, pic_pin0, ap);
	va_end(ap);
	return rval;
}

int
vmeTsi148InstallIrqMgrVa(int shared, int tsi_pin0, int pic_pin0, va_list ap)
{
int	i,j, specialPin, tsi_pin[TSI_NUM_WIRES+1], pic_pin[TSI_NUM_WIRES];

	if (vmeTsi148IrqMgrInstalled)                  return -4;

	/* check parameters */

	if ( tsi_pin0 < 0 || tsi_pin0 > 3 )            return -1;

	tsi_pin[0] = tsi_pin0;
	pic_pin[0] = pic_pin0 < 0 ? devs[0].irqLine : pic_pin0;
	i = 1;
	while ( (tsi_pin[i] = va_arg(ap, int)) >= 0 ) {
		
		if ( i >= TSI_NUM_WIRES ) {
			                                       return -5;
		}

		pic_pin[i] = va_arg(ap,int);

		if ( tsi_pin[i] > 3 )                      return -2;
		if ( pic_pin[i] < 0 )                      return -3;
		i++;
	}

	/* all routings must be different */
	for ( i=0; tsi_pin[i] >= 0; i++ ) {
		for ( j=i+1; tsi_pin[j] >= 0; j++ ) {
			if ( tsi_pin[j] == tsi_pin[i] )        return -6;
			if ( pic_pin[j] == pic_pin[i] )        return -7;
		}
	}

	/* give them a chance to override buggy PCI info */
	if ( pic_pin[0] >= 0 && devs[0].irqLine != pic_pin[0] ) {
		uprintf(stderr,"Overriding main IRQ line PCI info with %d\n",
				pic_pin[0]);
		devs[0].irqLine = pic_pin[0];
	}

	for ( i = 0; tsi_pin[i] >= 0; i++ ) {
		/* offset wire # by one so we can initialize to 0 == invalid */
		tsi_wire[i] = tsi_pin[i] + 1;
		connectIsr(shared, tsiVMEISR, pic_pin[i], i);
	}

	specialPin = tsi_pin[1] >= 0 ? 1 : 0;

	/* setup routing */
	
	/* IntRoute checks for mgr being installed */
	vmeTsi148IrqMgrInstalled=1;

	/* route 7 VME irqs to first / 'normal' pin */
	for ( i=1; i<8; i++ )
		vmeTsi148IntRoute( i, 0 );
	for ( i=TSI_DMA_INT_VEC; i<TSI_NUM_INT_VECS; i++ )
		vmeTsi148IntRoute( i, specialPin );

	for ( i = 0; i<TSI_NUM_WIRES; i++ ) {
		/* remember (for unloading the driver) */
		devs[0].pic_pin[i] = ( ( tsi_pin[i] >=0 ) ? pic_pin[i] : -1 );
	}

	return 0;
}

int
vmeTsi148InstallISR(unsigned long vector, VmeTsi148ISR hdl, void *arg)
{
IRQEntry          ip;
int				  v;
unsigned long	  flags;
volatile IRQEntry *p;

		if ( !vmeTsi148IrqMgrInstalled || (v = uni2tsivec(vector)) < 0 )
			return -1;

		p = irqHdlTbl + v;

		if (*p || !(ip=(IRQEntry)malloc(sizeof(IRQEntryRec))))
				return -1;

		ip->isr=hdl;
		ip->usrData=arg;

		rtems_interrupt_disable(flags);
		if (*p) {
			rtems_interrupt_enable(flags);
			free(ip);
			return -1;
		}
		*p = ip;
		rtems_interrupt_enable(flags);
		return 0;
}

int
vmeTsi148RemoveISR(unsigned long vector, VmeTsi148ISR hdl, void *arg)
{
int               v;
IRQEntry          ip;
unsigned long     flags;
volatile IRQEntry *p;

		if ( !vmeTsi148IrqMgrInstalled || (v = uni2tsivec(vector)) < 0 )
			return -1;

		p = irqHdlTbl + v;

		rtems_interrupt_disable(flags);
		ip = *p;
		if ( !ip || ip->isr!=hdl || ip->usrData!=arg ) {
				rtems_interrupt_enable(flags);
				return -1;
		}
		*p = 0;
		rtems_interrupt_enable(flags);

		free(ip);
		return 0;
}

static int
intDoEnDis(unsigned int level, int dis)
{
BERegister		*b = devs[0].base;
unsigned long	flags, v;
int				shift;

	if (  ! vmeTsi148IrqMgrInstalled || (shift = lvl2bitno(level)) < 0 )
		return -1;

	v = 1<<shift;

	if ( !dis )
		return (int)(v & TSI_RD(b, TSI_INTEO_REG) & TSI_RD(b, TSI_INTEN_REG)) ? 1 : 0;

	rtems_interrupt_disable(flags);
	if ( dis<0 ) {
		TSI_WR(b, TSI_INTEN_REG, TSI_RD(b, TSI_INTEN_REG) & ~v);
		TSI_WR(b, TSI_INTEO_REG, TSI_RD(b, TSI_INTEO_REG) & ~v);
	} else {
		TSI_WR(b, TSI_INTEN_REG, TSI_RD(b, TSI_INTEN_REG) |  v);
		TSI_WR(b, TSI_INTEO_REG, TSI_RD(b, TSI_INTEO_REG) |  v);
	}
	rtems_interrupt_enable(flags);
	return 0;
}

int
vmeTsi148IntEnable(unsigned int level)
{
	return intDoEnDis(level, 1);
}

int
vmeTsi148IntDisable(unsigned int level)
{
	return intDoEnDis(level, -1);
}

int
vmeTsi148IntIsEnabled(unsigned int level)
{
	return intDoEnDis(level, 0);
}

/* Set IACK width (1,2, or 4 bytes) for a given interrupt level.
 *
 * 'width' arg may be 0,1,2 or 4. If zero, the currently active
 * value is returned but not modified.
 *
 * RETURNS: old width or -1 if invalid argument.
 */

int
vmeTsi148SetIackWidth(int level, int width)
{
int rval;
	if ( level < 1 || level > 7 || !vmeTsi148IrqMgrInstalled )
		return -1;

	switch ( width ) {
		default: return -1;
		case 0:
		case 1:
		case 2:
		case 4:
		break;
	}

	rval = tsi_iack_width[level-1];
	if ( width )
		tsi_iack_width[level-1] = width;
	return rval;
}

int
vmeTsi148IntRaiseXX(BERegister *base, int level, unsigned vector)
{
unsigned long v;

	CHECK_BASE(base,0,-1);

	if ( level < 1 || level > 7 || vector > 255 )
		return -1;	/* invalid argument */

	/* Check if already asserted */
	if ( (v = TSI_RD(base, TSI_VICR_REG)) & TSI_VICR_IRQS ) {
		return -2;  /* already asserted */
	}

	v &= ~255;

	v |= TSI_VICR_IRQL(level) | TSI_VICR_STID(vector);

	/* Write Vector */
	TSI_WR(base, TSI_VICR_REG, v);

	return 0;
	
}

int
vmeTsi148IntRaise(int level, unsigned vector)
{
	return vmeTsi148IntRaiseXX(devs[0].base, level, vector);
}

/* Loopback test of VME/Tsi148 internal interrupts */

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
		printk("vmeTsi148IntLoopbackTst: (ISR) message queue full / overrun ? disabling IRQ level %i\n", pa->l);
		vmeTsi148IntDisable(pa->l);
	}
}

int
vmeTsi148IntLoopbackTst(int level, unsigned vector)
{
BERegister			*b = devs[0].base;
rtems_status_code	sc;
rtems_id			q = 0;
int					installed = 0;
int					i, err = 0;
int					doDisable = 0;
uint32_t			size;
unsigned long		msg;
char *				irqfmt  = "VME IRQ @vector %3i %s";
char *				iackfmt = "VME IACK            %s";
LoopbackTstArgs		a;

	CHECK_BASE(b,0,-1);

	/* arg check */
	if ( level < 1 || level > 7 || vector > 255 )
		return -1;

	/* Create message queue */
	if ( RTEMS_SUCCESSFUL != (sc=rtems_message_queue_create(
        								rtems_build_name('t' ,'U','I','I'),
								        4,
        								sizeof(unsigned long),
        								0,  /* default attributes: fifo, local */
        								&q)) ) {
		rtems_error(sc, "vmeTsi148IntLoopbackTst: Unable to create message queue");
		goto bail;
	}

	a.q = q;
	a.l = level;

	/* Install handlers */
	if ( vmeTsi148InstallISR(vector, loopbackTstIsr, (void*)&a) ) {
		uprintf(stderr,"Unable to install VME ISR to vector %i\n",vector);
		goto bail;
	}
	installed++;
	if ( vmeTsi148InstallISR(TSI_VME_SW_IACK_INT_VEC, loopbackTstIsr, (void*)&a) ) {
		uprintf(stderr,"Unable to install VME ISR to IACK special vector %i\n",TSI_VME_SW_IACK_INT_VEC);
		goto bail;
	}
	installed++;

	if ( !vmeTsi148IntIsEnabled(level) && 0==vmeTsi148IntEnable(level) )
		doDisable = 1;
	
	/* make sure there are no pending interrupts */
	TSI_WR(b, TSI_INTC_REG, TSI_INTC_IACKC);

	if ( vmeTsi148IntEnable( TSI_VME_SW_IACK_INT_VEC ) ) {
		uprintf(stderr,"Unable to enable IACK interrupt\n");
		goto bail;
	}

	printf("vmeTsi148 VME interrupt loopback test; STARTING...\n");
	printf(" --> asserting VME IRQ level %i\n", level);
	vmeTsi148IntRaise(level, vector);

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
		} else if ( msg == TSI_VME_SW_IACK_INT_VEC ) {
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
		vmeTsi148IntDisable(level);
	vmeTsi148IntDisable( TSI_VME_SW_IACK_INT_VEC );
	if ( installed > 0 )
		vmeTsi148RemoveISR(vector, loopbackTstIsr, (void*)&a);
	if ( installed > 1 )
		vmeTsi148RemoveISR(TSI_VME_SW_IACK_INT_VEC, loopbackTstIsr, (void*)&a);
	if ( q )
		rtems_message_queue_delete(q);

	return sc ? sc : err;
}

unsigned long
vmeTsi148ClearVMEBusErrorsXX(BERegister *base, unsigned long long *paddr)
{
unsigned long rval;

	CHECK_BASE(base,1,-1);

	rval = TSI_RD(base, TSI_VEAT_REG);
	if ( rval & TSI_VEAT_VES ) {
		if ( paddr ) {
			*paddr  = ((unsigned long long)TSI_RD(base, TSI_VEAU_REG))<<32;
			*paddr |= TSI_RD(base, TSI_VEAL_REG);
		}
		/* clear errors */
		TSI_WR(base, TSI_VEAT_REG, TSI_VEAT_VESCL);
	} else {
		rval = 0;
	}
	return rval;
}

unsigned long
vmeTsi148ClearVMEBusErrors(unsigned long long *paddr)
{
	return vmeTsi148ClearVMEBusErrorsXX(devs[0].base, paddr);
}

#ifdef DEBUG_MODULAR
void
_cexpModuleInitialize(void* unused)
{
	vmeTsi148Init();
	vmeTsi148Reset();
}

int
_cexpModuleFinalize(void *unused)
{
int		i;
int		rval = 1;
void    (*isrs[TSI_NUM_WIRES])() = {
	isr_pin0,
	isr_pin1,
	isr_pin2,
	isr_pin3,
};

rtems_irq_connect_data	xx;
	xx.on   = my_no_op; /* at _least_ they could check for a 0 pointer */
	xx.off  = my_no_op;
	xx.isOn = my_isOn;

	TSI_WR(devs[0].base, TSI_INTEO_REG, 0);

	for ( i=0; i<TSI_NUM_INT_VECS; i++) {
		/* Dont even bother to uninstall handlers */
	}
	if ( vmeTsi148IrqMgrInstalled ) {
		for ( i=0; i<TSI_NUM_WIRES; i++ ) {
			if ( (int)(xx.name = devs[0].pic_pin[i]) >=0 ) {
				xx.hdl  = isrs[i];
				rval    = rval && BSP_remove_rtems_irq_handler(&xx);
			}
		}
	}
	return !rval;
}
#endif
