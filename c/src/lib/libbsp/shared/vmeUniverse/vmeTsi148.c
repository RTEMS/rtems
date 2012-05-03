/* Driver for the Tundra Tsi148 pci-vme bridge */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#include <rtems.h>
#include <stdio.h>
#include <stdarg.h>
#include <bsp/irq.h>
#include <stdlib.h>
#include <rtems/bspIo.h>	/* printk */
#include <rtems/error.h>	/* printk */
#include <rtems/pci.h>
#include <bsp.h>
#include <libcpu/byteorder.h>

#define __INSIDE_RTEMS_BSP__

#include "vmeTsi148.h"
#include <bsp/VMEDMA.h>
#include "vmeTsi148DMA.h"
#include "bspVmeDmaListP.h"


#define DEBUG

#ifdef DEBUG
#define STATIC
#else
#define STATIC static
#endif

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
#	define TSI_OTAT_2eSSTM_160	TSI_OTAT_2eSSTM(0)
#	define TSI_OTAT_2eSSTM_267	TSI_OTAT_2eSSTM(1)
#	define TSI_OTAT_2eSSTM_320	TSI_OTAT_2eSSTM(2)
#	define TSI_OTAT_TM(x)		(((x)&7)<<8)
#		define TSI_TM_SCT_IDX		0
#		define TSI_TM_BLT_IDX		1
#		define TSI_TM_MBLT_IDX		2
#		define TSI_TM_2eVME_IDX		3
#		define TSI_TM_2eSST_IDX		4
#		define TSI_TM_2eSSTB_IDX	5
#	define TSI_OTAT_DBW(x)		(((x)&3)<<6)
#	define TSI_OTAT_SUP			(1<<5)
#	define TSI_OTAT_PGM			(1<<4)
#	define TSI_OTAT_ADMODE(x)	(((x)&0xf))
#	define TSI_OTAT_ADMODE_A16	0
#	define TSI_OTAT_ADMODE_A24	1
#	define TSI_OTAT_ADMODE_A32	2
#	define TSI_OTAT_ADMODE_A64	4
#	define TSI_OTAT_ADMODE_CSR	5
#	define TSI_OTAT_ADMODE_USR1	8
#	define TSI_OTAT_ADMODE_USR2	9
#	define TSI_OTAT_ADMODE_USR3	0xa
#	define TSI_OTAT_ADMODE_USR4	0xb

#define TSI_VIACK_1_REG		0x204

#define TSI_VMCTRL_REG		0x234
#	define TSI_VMCTRL_VSA		(1<<27)
#	define TSI_VMCTRL_VS		(1<<26)
#	define TSI_VMCTRL_DHB		(1<<25)
#	define TSI_VMCTRL_DWB		(1<<24)
#	define TSI_VMCTRL_RMWEN		(1<<20)
#	define TSI_VMCTRL_A64DS		(1<<16)
#	define TSI_VMCTRL_VTOFF_MSK	(7<<12)
#	define TSI_VMCTRL_VTOFF_0us	(0<<12)
#	define TSI_VMCTRL_VTOFF_1us	(1<<12)
#	define TSI_VMCTRL_VTOFF_2us	(2<<12)
#	define TSI_VMCTRL_VTOFF_4us	(3<<12)
#	define TSI_VMCTRL_VTOFF_8us	(4<<12)
#	define TSI_VMCTRL_VTOFF_16us	(5<<12)
#	define TSI_VMCTRL_VTOFF_32us	(6<<12)
#	define TSI_VMCTRL_VTOFF_64us	(7<<12)
#	define TSI_VMCTRL_VTON_MSK	(7<< 8)
#	define TSI_VMCTRL_VTON_4us	(0<< 8)
#	define TSI_VMCTRL_VTON_8us	(1<< 8)
#	define TSI_VMCTRL_VTON_16us	(2<< 8)
#	define TSI_VMCTRL_VTON_32us	(3<< 8)
#	define TSI_VMCTRL_VTON_64us	(4<< 8)
#	define TSI_VMCTRL_VTON_128us	(5<< 8)
#	define TSI_VMCTRL_VTON_256us	(6<< 8)
#	define TSI_VMCTRL_VTON_512us	(7<< 8)
#	define TSI_VMCTRL_VREL_MSK		(3<< 3)
#	define TSI_VMCTRL_VREL_TON_or_DONE			(0<< 3)
#	define TSI_VMCTRL_VREL_TONandREQ_or_DONE	(1<< 3)
#	define TSI_VMCTRL_VREL_TONandBCLR_or_DONE	(2<< 3)
#	define TSI_VMCTRL_VREL_TONorDONE_and_REQ	(3<< 3)
#	define TSI_VMCTRL_VFAIR		(1<< 2)
#	define TSI_VMCTRL_VREQL_MSK	(3<< 0)
#	define TSI_VMCTRL_VREQL(x)	((x)&3)

#define TSI_VCTRL_REG		0x238
#define TSI_VCTRL_DLT_MSK		(0xf<<24)
#define TSI_VCTRL_NELBB			(1<<20)
#define TSI_VCTRL_SRESET		(1<<17)
#define TSI_VCTRL_LRESET		(1<<16)
#define TSI_VCTRL_SFAILAI		(1<<15)
#define TSI_VCTRL_BID_MSK		(0x1f<<8)
#define TSI_VCTRL_ATOEN			(1<< 7)
#define TSI_VCTRL_ROBIN			(1<< 6)
#define TSI_VCTRL_GTO_MSK		(7<< 0)


#define TSI_VSTAT_REG		0x23c
#	define TSI_VSTAT_CPURST		(1<<15)	/* clear power-up reset bit */
#	define TSI_VSTAT_BDFAIL		(1<<14)
#	define TSI_VSTAT_PURSTS		(1<<12)
#	define TSI_VSTAT_BDFAILS	(1<<11)
#	define TSI_VSTAT_SYSFLS		(1<<10)
#	define TSI_VSTAT_ACFAILS	(1<< 9)
#	define TSI_VSTAT_SCONS		(1<< 8)
#	define TSI_VSTAT_GAP		(1<< 5)
#	define TSI_VSTAT_GA_MSK		(0x1f)

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
#	define TSI_ITAT_2eSSTM_160	TSI_ITAT_2eSSTM(0)
#	define TSI_ITAT_2eSSTM_267	TSI_ITAT_2eSSTM(1)
#	define TSI_ITAT_2eSSTM_320	TSI_ITAT_2eSSTM(2)
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

#define TSI_CBAU_REG		0x40c
#define TSI_CBAL_REG		0x410
#define TSI_CRGAT_REG		0x414
#   define TSI_CRGAT_EN		(1<<7)
#	define TSI_CRGAT_AS_MSK	(7<<4)
#	define TSI_CRGAT_A16	(0<<4)
#	define TSI_CRGAT_A24	(1<<4)
#	define TSI_CRGAT_A32	(2<<4)
#	define TSI_CRGAT_A64	(4<<4)
#   define TSI_CRGAT_SUP	(1<<3)
#   define TSI_CRGAT_USR	(1<<2)
#   define TSI_CRGAT_PGM	(1<<1)
#   define TSI_CRGAT_DATA	(1<<0)

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

#define TSI_CBAR_REG		0xffc

#define TSI_CSR_OFFSET		0x7f000

#define TSI_CRG_SIZE		(1<<12)		/* 4k */


#define TSI_RD(base, reg)				in_be32((volatile unsigned *)((base) + (reg)/sizeof(*base)))
#define TSI_RD16(base, reg)				in_be16((volatile unsigned short *)(base) + (reg)/sizeof(short))
#define TSI_LE_RD16(base, reg) 			in_le16((volatile unsigned short *)(base) + (reg)/sizeof(short))
#define TSI_LE_RD32(base, reg) 			in_le32((volatile unsigned *)(base) + (reg)/sizeof(*base))
#define TSI_RD8(base, reg)				in_8((volatile unsigned char *)(base) + (reg))
#define TSI_WR(base, reg, val)			out_be32((volatile unsigned *)((base) + (reg)/sizeof(*base)), val)

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

typedef uint32_t pci_ulong;

#ifdef __BIG_ENDIAN__
	static inline void st_be32( uint32_t *a, uint32_t v)
	{
		*a = v;
	}
	static inline uint32_t ld_be32( uint32_t *a )
	{
		return *a;
	}
#elif defined(__LITTLE_ENDIAN__)
#error "You need to implement st_be32/ld_be32"
#else
#error "Undefined endianness??"
#endif

#ifndef BSP_LOCAL2PCI_ADDR
/* try legacy PCI_DRAM_OFFSET */
#ifndef PCI_DRAM_OFFSET
#define PCI_DRAM_OFFSET 0
#endif
#define BSP_LOCAL2PCI_ADDR(l)	(((uint32_t)l)+PCI_DRAM_OFFSET)
#endif

/* PCI_MEM_BASE is a possible offset between CPU- and PCI addresses.
 * Should be defined by the BSP.
 */
#ifndef BSP_PCI2LOCAL_ADDR
#ifndef PCI_MEM_BASE
#define PCI_MEM_BASE 0
#endif
#define BSP_PCI2LOCAL_ADDR(memaddr) ((unsigned long)(memaddr) + PCI_MEM_BASE)
#endif

typedef uint32_t BEValue;

typedef struct {
	BERegister *base;
	int			irqLine;
	int			pic_pin[TSI_NUM_WIRES];
} Tsi148Dev;

static Tsi148Dev devs[NUM_TSI_DEVS] = {{0}};

#define THEBASE (devs[0].base)

/* forward decl */
extern int vmeTsi148RegPort;
extern int vmeTsi148RegCSR;

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

	*pbase=(BERegister*)(((pci_ulong)BSP_PCI2LOCAL_ADDR(busaddr)) & ~0xff);

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
uint32_t v;

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
	/* Clear BDFAIL / (--> SYSFAIL) */
#	define TSI_VSTAT_BDFAIL		(1<<14)
	TSI_WR(base, TSI_VSTAT_REG, TSI_RD(base, TSI_VSTAT_REG) & ~TSI_VSTAT_BDFAIL);
	/* Set (long) bus master timeout; the timeout actually overrides
	 * the DMA block size so that the DMA settings would effectively
	 * not be used.
	 * Also, we enable 'release on request' mode so that we normally
	 * don't have to rearbitrate the bus for every transfer.
	 */
	v = TSI_RD(base, TSI_VMCTRL_REG);
	v &= ~( TSI_VMCTRL_VTON_MSK | TSI_VMCTRL_VREL_MSK );
	v |= (TSI_VMCTRL_VTON_512us | TSI_VMCTRL_VREL_TONorDONE_and_REQ );
	TSI_WR(base, TSI_VMCTRL_REG, v);
}

void
vmeTsi148Reset(void)
{
	vmeTsi148ResetXX(THEBASE);
}

void
vmeTsi148ResetBusXX(BERegister *base)
{
unsigned long flags;
uint32_t      v;

	rtems_interrupt_disable(flags);
	v = TSI_RD(base, TSI_VCTRL_REG);
	TSI_WR(base, TSI_VCTRL_REG, v | TSI_VCTRL_SRESET);
	rtems_interrupt_enable(flags);
}

void
vmeTsi148ResetBus(void)
{
	vmeTsi148ResetBusXX(THEBASE);
}


/* convert an address space selector to a corresponding
 * Tsi148 control mode word
 */

static unsigned long ck2esst(unsigned long am)
{
	if ( VME_AM_IS_2eSST(am) ) {
		/* make sure 2eVME is selected */
		am &= ~VME_AM_MASK;
		am |= VME_AM_2eVME_6U;
	}
	return am;
}

STATIC int
am2omode(unsigned long address_space, unsigned long *pmode)
{
unsigned long mode = 0;
unsigned long tm   = TSI_TM_SCT_IDX;

	switch ( VME_MODE_DBW_MSK & address_space ) {
		case VME_MODE_DBW8:
			return -1;	/* unsupported */

		case VME_MODE_DBW16:
			break;

		default:
		case VME_MODE_DBW32:
			mode |= TSI_OTAT_DBW(1);
			break;
	}

	if ( ! (VME_MODE_PREFETCH_ENABLE & address_space) )
		mode |= TSI_OTAT_MRPFD;
	else {
		mode |= TSI_OTAT_PFS(address_space>>_LD_VME_MODE_PREFETCHSZ);
	}

	address_space = ck2esst(address_space);

	switch (address_space & VME_AM_MASK) {
		case VME_AM_STD_SUP_PGM:
		case VME_AM_STD_USR_PGM:

			mode |= TSI_OTAT_PGM;

			/* fall thru */
		case VME_AM_STD_SUP_BLT:
		case VME_AM_STD_SUP_MBLT:

		case VME_AM_STD_USR_BLT:
		case VME_AM_STD_USR_MBLT:
			switch ( address_space & 3 ) {
				case 0:	tm = TSI_TM_MBLT_IDX; break;
				case 3:	tm = TSI_TM_BLT_IDX; break;
				default: break;
			}

		case VME_AM_STD_SUP_DATA:
		case VME_AM_STD_USR_DATA:

			mode |= TSI_OTAT_ADMODE_A24;
			break;

		case VME_AM_EXT_SUP_PGM:
		case VME_AM_EXT_USR_PGM:
			mode |= TSI_OTAT_PGM;

			/* fall thru */
		case VME_AM_EXT_SUP_BLT:
		case VME_AM_EXT_SUP_MBLT:

		case VME_AM_EXT_USR_BLT:
		case VME_AM_EXT_USR_MBLT:
			switch ( address_space & 3 ) {
				case 0:	tm = TSI_TM_MBLT_IDX; break;
				case 3:	tm = TSI_TM_BLT_IDX; break;
				default: break;
			}

		case VME_AM_EXT_SUP_DATA:
		case VME_AM_EXT_USR_DATA:

			mode |= TSI_OTAT_ADMODE_A32;
			break;

		case VME_AM_SUP_SHORT_IO:
		case VME_AM_USR_SHORT_IO:
			mode |= TSI_OTAT_ADMODE_A16;
			break;

		case VME_AM_CSR:
			mode |= TSI_OTAT_ADMODE_CSR;
			break;

		case VME_AM_2eVME_6U:
		case VME_AM_2eVME_3U:
			mode |= TSI_OTAT_ADMODE_A32;
			if ( VME_AM_IS_2eSST(address_space) ) {
				tm = ( VME_AM_2eSST_BCST & address_space ) ?
	 					TSI_TM_2eSSTB_IDX : TSI_TM_2eSST_IDX;
				switch ( VME_AM_IS_2eSST(address_space) ) {
					default:
					case VME_AM_2eSST_LO:  mode |= TSI_OTAT_2eSSTM_160; break;
					case VME_AM_2eSST_MID: mode |= TSI_OTAT_2eSSTM_267; break;
					case VME_AM_2eSST_HI:  mode |= TSI_OTAT_2eSSTM_320; break;
				}
			} else {
				tm    = TSI_TM_2eVME_IDX;
			}
		break;

		case 0: /* disable the port alltogether */
			break;

		default:
			return -1;
	}

	mode |= TSI_OTAT_TM(tm);

	if ( VME_AM_IS_SUP(address_space) )
		mode |= TSI_OTAT_SUP;
	*pmode = mode;
	return 0;
}

STATIC int
am2imode(unsigned long address_space, unsigned long *pmode)
{
unsigned long mode=0;
unsigned long pgm = 0;

	mode |= TSI_ITAT_VFS(address_space>>_LD_VME_MODE_PREFETCHSZ);

	if ( VME_AM_IS_2eSST(address_space) ) {
			mode |= TSI_ITAT_2eSST;
		if ( VME_AM_2eSST_BCST & address_space )
			mode |= TSI_ITAT_2eSSTB;
		switch ( VME_AM_IS_2eSST(address_space) ) {
			default:
			case VME_AM_2eSST_LO:  mode |= TSI_ITAT_2eSSTM_160; break;
			case VME_AM_2eSST_MID: mode |= TSI_ITAT_2eSSTM_267; break;
			case VME_AM_2eSST_HI:  mode |= TSI_ITAT_2eSSTM_320; break;
		}
		address_space = ck2esst(address_space);
	}

	mode |= TSI_ITAT_BLT;
	mode |= TSI_ITAT_MBLT;

	mode |= TSI_ITAT_PGM; /* always allow PGM access */
	mode |= TSI_ITAT_USR; /* always allow USR access */

	switch (address_space & VME_AM_MASK) {
		case VME_AM_STD_SUP_PGM:
		case VME_AM_STD_USR_PGM:

			pgm = 1;

			/* fall thru */
		case VME_AM_STD_SUP_BLT:
		case VME_AM_STD_SUP_MBLT:
		case VME_AM_STD_USR_BLT:
		case VME_AM_STD_USR_MBLT:
		case VME_AM_STD_SUP_DATA:
		case VME_AM_STD_USR_DATA:

			mode |= TSI_ITAT_ADMODE_A24;
			break;

		case VME_AM_EXT_SUP_PGM:
		case VME_AM_EXT_USR_PGM:
			pgm = 1;

			/* fall thru */
		case VME_AM_2eVME_6U:
		case VME_AM_2eVME_3U:
		case VME_AM_EXT_SUP_BLT:
		case VME_AM_EXT_SUP_MBLT:
		case VME_AM_EXT_USR_BLT:
		case VME_AM_EXT_USR_MBLT:
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

	if ( !pgm )
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
int					i,s,l;

	CHECK_BASE(base,0,-1);

	mode = 0; /* silence warning */

	if ( port >= (isout ? TSI148_NUM_OPORTS : TSI148_NUM_IPORTS) ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid port\n", name);
		return -1;
	}

	if ( base == THEBASE && isout && vmeTsi148RegPort == port ) {
		uprintf(stderr,"Tsi148 %s Port Cfg: invalid port; reserved by the interrupt manager for CRG\n", name);
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

		/* check for overlap */
		for ( i = 0; i < TSI148_NUM_OPORTS; i++ ) {
			/* ignore 'this' port */
			if ( i == port || ! (TSI_OTAT_EN & TSI_RD(base, TSI_OTAT_REG(i))) )
				continue;

			/* check requested PCI range against current port 'i' config */
			s = TSI_RD(base, TSI_OTSAU_REG(i) + 0x04); /* start */
			l = TSI_RD(base, TSI_OTSAU_REG(i) + 0x0c); /* limit */
			if ( ! ( start + length <= s || start > s + l ) ) {
				uprintf(stderr,"Tsi148 Outbound Port Cfg: PCI address range overlaps with port %i (0x%08x..0x%08x)\n", i, s, l);
				return -1;
			}
		}
	} else {
		start     = vme_address;
		offst     = (unsigned long long)pci_address - start;
		mask      = inboundGranularity(mode);
		tat_reg   = TSI_ITAT_REG(port);
		tsau_reg  = TSI_ITSAU_REG(port);
		mode     |= TSI_ITAT_EN;

		/* check for overlap */
		for ( i = 0; i < TSI148_NUM_IPORTS; i++ ) {
			/* ignore 'this' port */
			if ( i == port || ! (TSI_ITAT_EN & (s=TSI_RD(base, TSI_ITAT_REG(i)))) )
				continue;

			if ( (TSI_ITAT_AS(-1) & s) != (TSI_ITAT_AS(-1) & mode) ) {
				/* different address space */
				continue;
			}

			if ( ! (mode & s & (TSI_ITAT_SUP | TSI_ITAT_USR | TSI_ITAT_PGM | TSI_ITAT_DATA)) ) {
				/* orthogonal privileges */
				continue;
			}

			/* check requested VME range against current port 'i' config */
			s = TSI_RD(base, TSI_ITSAU_REG(i) + 0x04); /* start */
			l = TSI_RD(base, TSI_ITSAU_REG(i) + 0x0c); /* limit */
			if ( ! ( start + length <= s || start > s + l ) ) {
				uprintf(stderr,"Tsi148 Inbound Port Cfg: VME address range overlaps with port %i (0x%08x..0x%08x)\n", i, s, l);
				return -1;
			}
		}
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
	return configTsiPort(THEBASE, 0, port, address_space, vme_address, pci_address, length);
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
	return configTsiPort(THEBASE, 1, port, address_space, vme_address, pci_address, length);
}


static int
xlateFindPort(
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

	switch ( as & VME_MODE_MATCH_MASK ) {
		case VME_MODE_EXACT_MATCH:
			mode_msk = ~0;
		break;

		case VME_MODE_AS_MATCH:
			if ( outbound )
				mode_msk = TSI_OTAT_ADMODE(-1) | TSI_OTAT_EN;
			else
				mode_msk = TSI_ITAT_AS(-1) | TSI_ITAT_EN;
		break;

		default:
			if ( outbound )
				mode_msk = TSI_OTAT_PGM | TSI_OTAT_SUP | TSI_OTAT_ADMODE(-1) | TSI_OTAT_EN;
			else
				mode_msk = TSI_ITAT_PGM | TSI_ITAT_DATA | TSI_ITAT_SUP | TSI_ITAT_USR | TSI_ITAT_AS(-1) | TSI_ITAT_EN;
		break;
	}

	as &= ~VME_MODE_MATCH_MASK;

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
				return port;
			}
		}
	}

	uprintf(stderr, "vmeTsi148XlateAddr: no matching mapping found\n");
	return -1;
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
int port = xlateFindPort( base, outbound, reverse, as, aIn, paOut );
	return port < 0 ? -1 : 0;
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
	return vmeTsi148XlateAddrXX(THEBASE, outbound, reverse, as, aIn, paOut);
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
			case TSI_OTAT_ADMODE_CSR: uprintf(f,"CSR"); break;
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
	vmeTsi148OutboundPortsShowXX(THEBASE, f);
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
	vmeTsi148InboundPortsShowXX(THEBASE, f);
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
	vmeTsi148DisableAllInboundPortsXX(THEBASE);
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
	vmeTsi148DisableAllOutboundPortsXX(THEBASE);
}


/* Map internal register block to VME */

int
vmeTsi148MapCRGXX(BERegister *b, uint32_t vme_base, uint32_t as )
{
uint32_t mode;

	CHECK_BASE( b, 0, -1 );

	if ( vmeTsi148RegPort > -1 && ! vmeTsi148RegCSR ) {
        uprintf(stderr,"vmeTsi148: CRG already mapped and in use by interrupt manager\n");
		return -1;
	}

	/* enable all, SUP/USR/PGM/DATA accesses */
	mode = TSI_CRGAT_EN | TSI_CRGAT_SUP | TSI_CRGAT_USR | TSI_CRGAT_PGM |  TSI_CRGAT_DATA;

	if ( VME_AM_IS_SHORT(as) ) {
		mode |= TSI_CRGAT_A16;
	} else
	if ( VME_AM_IS_STD(as) ) {
		mode |= TSI_CRGAT_A24;
	} else
	if ( VME_AM_IS_EXT(as) ) {
		mode |= TSI_CRGAT_A32;
	} else {
		return -2;
	}

	/* map CRG to VME bus */
	TSI_WR( b, TSI_CBAL_REG, (vme_base & ~(TSI_CRG_SIZE-1)));
	TSI_WR( b, TSI_CRGAT_REG, mode );

	return 0;
}

int
vmeTsi148MapCRG(uint32_t vme_base, uint32_t as )
{
	return vmeTsi148MapCRGXX( THEBASE, vme_base, as );
}

/* Interrupt Subsystem */

typedef struct
IRQEntryRec_ {
		VmeTsi148ISR	isr;
		void			*usrData;
} IRQEntryRec, *IRQEntry;

static IRQEntry irqHdlTbl[TSI_NUM_INT_VECS]={0};

int        vmeTsi148IrqMgrInstalled = 0;
int        vmeTsi148RegPort         = -1;
int        vmeTsi148RegCSR			= 0;
BERegister *vmeTsi148RegBase        = 0;

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

	mask = TSI_RD(THEBASE, mapreg) & ~ (0x3<<shift);
	mask |= wire;
	TSI_WR( THEBASE, mapreg, mask );

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
BERegister			*b  = THEBASE;
IRQEntry			ip;
unsigned long	 	msk,lintstat,vector, vecarg;
int					lvl;

	/* only handle interrupts routed to this pin */

	/* NOTE: we read the status register over VME, thus flushing the FIFO
	 *       where the user ISR possibly left write commands to clear
	 *       the interrupt condition at the device.
	 *       This is very important - otherwise, the IRQ level may still be
	 *       asserted and we would detect an interrupt here but the subsequent
	 *       IACK would fail since the write operation was flushed to the
	 *       device in the mean time.
	 */
	while ( (lintstat  = (TSI_RD(vmeTsi148RegBase, TSI_INTS_REG) & wire_mask[pin])) ) {

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
				printk("vmeTsi148 ISR: ERROR: no handler registered (level %i) IACK 0x%08x -- DISABLING level %i\n",
						lvl, vector, lvl);
			} else {
				/* dispatch handler, it must clear the IRQ at the device */
				ip->isr(ip->usrData, vecarg);
				/* convenience for disobedient users who don't use in_xxx/out_xxx; make
				 * sure we order the subsequent read from the status register after
				 * their business.
				 */
				iobarrier_rw();
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
		return (int)(TSI_RD(THEBASE, TSI_INTEO_REG) & TSI_RD(THEBASE, TSI_INTEN_REG));
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

#ifndef BSP_EARLY_PROBE_VME
#define BSP_EARLY_PROBE_VME(addr)	\
	(																									\
		vmeTsi148ClearVMEBusErrorsXX( THEBASE, 0 ),												\
		( ((PCI_DEVICE_TSI148 << 16) | PCI_VENDOR_TUNDRA ) == TSI_LE_RD32( ((BERegister*)(addr)), 0 )	\
		 && 0 == vmeTsi148ClearVMEBusErrorsXX( THEBASE, 0 ) )										\
	)
#endif

/* Check if there is a vme address/as is mapped in any of the outbound windows
 * and look for the PCI vendordevice ID there.
 * RETURNS: -1 on error (no mapping or probe failure), outbound window # (0..7)
 *          on success. Address translated into CPU address is returned in *pcpu_addr.
 */
static int
mappedAndProbed(unsigned long vme_addr, unsigned as, unsigned long *pcpu_addr)
{
int j;
char *regtype = (as & VME_AM_MASK) == VME_AM_CSR ? "CSR" : "CRG";

	/* try to find mapping */
	if ( 0 > (j = xlateFindPort(
				THEBASE,
				1, 0,
				as | VME_MODE_AS_MATCH,
				vme_addr,
				pcpu_addr ) ) ) {
			uprintf(stderr,"vmeTsi148 - Unable to find mapping for %s VME base (0x%08x)\n", regtype, vme_addr);
			uprintf(stderr,"            in outbound windows.\n");
	}
	else {
			/* found a slot number; probe it */
			*pcpu_addr = BSP_PCI2LOCAL_ADDR( *pcpu_addr );
			if ( BSP_EARLY_PROBE_VME(*pcpu_addr) ) {
				uprintf(stderr,"vmeTsi148 - IRQ manager using VME %s to flush FIFO\n", regtype);
				return j;
			} else {
				uprintf(stderr,"vmeTsi148 - Found slot info but detection of tsi148 in VME %s space failed\n", regtype);
			}
	}
	return -1;
}

int
vmeTsi148InstallIrqMgrVa(int shared, int tsi_pin0, int pic_pin0, va_list ap)
{
int	i,j, specialPin, tsi_pin[TSI_NUM_WIRES+1], pic_pin[TSI_NUM_WIRES];
unsigned long cpu_base, vme_reg_base;

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

	i        = -1;

	/* first try VME CSR space; do we have a base address set ? */

	uprintf(stderr,"vmeTsi148 IRQ manager: looking for registers on VME...\n");

	if ( ( i = ((TSI_RD( THEBASE, TSI_CBAR_REG ) & 0xff) >> 3) ) > 0 ) {
		uprintf(stderr,"Trying to find CSR on VME...\n");
		vme_reg_base = i*0x80000 + TSI_CSR_OFFSET;
		i = mappedAndProbed( vme_reg_base, VME_AM_CSR , &cpu_base);
		if ( i >= 0 )
			vmeTsi148RegCSR = 1;
	} else {
		i = -1;
	}

	if ( -1 == i ) {

		uprintf(stderr,"Trying to find CRG on VME...\n");

		/* Next we see if the CRG block is mapped to VME */

		if ( (TSI_CRGAT_EN & (j = TSI_RD( THEBASE, TSI_CRGAT_REG ))) ) {
			switch ( j & TSI_CRGAT_AS_MSK ) {
				case TSI_CRGAT_A16 : i = VME_AM_SUP_SHORT_IO; break;
				case TSI_CRGAT_A24 : i = VME_AM_STD_SUP_DATA; break;
				case TSI_CRGAT_A32 : i = VME_AM_EXT_SUP_DATA; break;
				default:
				break;
			}
			vme_reg_base = TSI_RD( THEBASE, TSI_CBAL_REG ) & ~ (TSI_CRG_SIZE - 1);
		}

		if ( -1 == i ) {
		} else {
			i = mappedAndProbed( vme_reg_base, (i & VME_AM_MASK), &cpu_base );
		}
	}

	if ( i < 0 ) {
			uprintf(stderr,"vmeTsi148 IRQ manager - BSP configuration error: registers not found on VME\n");
			uprintf(stderr,"(should open outbound window to CSR space or map CRG [vmeTsi148MapCRG()])\n");
			uprintf(stderr,"Falling back to PCI but you might experience spurious VME interrupts; read a register\n");
			uprintf(stderr,"back from user ISR to flush posted-write FIFO as a work-around\n");
			cpu_base = (unsigned long)THEBASE;
			i        = -1;
	}

	vmeTsi148RegBase = (BERegister*)cpu_base;
	vmeTsi148RegPort = i;

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
BERegister		*b = THEBASE;
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
	return vmeTsi148IntRaiseXX(THEBASE, level, vector);
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
BERegister			*b = THEBASE;
rtems_status_code	sc;
rtems_id			q = 0;
int					installed = 0;
int					i, err = 0;
int					doDisable = 0;
size_t				size;
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
vmeTsi148ClearVMEBusErrorsXX(BERegister *base, uint32_t *paddr)
{
unsigned long rval;

	CHECK_BASE(base,1,-1);

	rval = TSI_RD(base, TSI_VEAT_REG);
	if ( rval & TSI_VEAT_VES ) {
		if ( paddr ) {
#if 0 /* no 64-bit support yet */
			*paddr  = ((unsigned long long)TSI_RD(base, TSI_VEAU_REG))<<32;
			*paddr |= TSI_RD(base, TSI_VEAL_REG);
#else
			*paddr = TSI_RD(base, TSI_VEAL_REG);
#endif
		}
		/* clear errors */
		TSI_WR(base, TSI_VEAT_REG, TSI_VEAT_VESCL);
	} else {
		rval = 0;
	}
	return rval;
}

unsigned long
vmeTsi148ClearVMEBusErrors(uint32_t *paddr)
{
	return vmeTsi148ClearVMEBusErrorsXX(THEBASE, paddr);
}

/** DMA Support **/

/* descriptor must be 8-byte aligned */
typedef struct VmeTsi148DmaListDescriptorRec_ {
	BEValue						dsau,  dsal;
	BEValue						ddau,  ddal;
	BEValue						dsat,  ddat;
	BEValue						dnlau, dnlal;
	BEValue						dcnt,  ddbs;
} VmeTsi148DmaListDescriptorRec;

static void     tsi_desc_init  (DmaDescriptor);
static int      tsi_desc_setup (DmaDescriptor, uint32_t, uint32_t, uint32_t, uint32_t, uint32_t);
static void     tsi_desc_setnxt(DmaDescriptor, DmaDescriptor);
static void     tsi_desc_dump  (DmaDescriptor);
static int      tsi_desc_start (volatile void *controller_addr, int channel, DmaDescriptor p);

VMEDmaListClassRec	vmeTsi148DmaListClass = {
	desc_size:  sizeof(VmeTsi148DmaListDescriptorRec),
	desc_align: 8,
	freeList:   0,
	desc_alloc: 0,
	desc_free:  0,
	desc_init:  tsi_desc_init,
	desc_setnxt:tsi_desc_setnxt,
	desc_setup: tsi_desc_setup,
	desc_start: tsi_desc_start,
	desc_refr:  0,
	desc_dump:	tsi_desc_dump,
};

/* DMA Control */
#define TSI_DMA_REG(off,i)	((off)+(((i)&1)<<7))

#define TSI_DCTL_REG(i)		TSI_DMA_REG(0x500,i)
#define TSI_DCTL0_REG		0x500
#define TSI_DCTL1_REG		0x580
#   define TSI_DCTL_ABT		(1<<27)	/* abort */
#   define TSI_DCTL_PAU		(1<<26) /* pause */
#   define TSI_DCTL_DGO		(1<<25) /* GO    */
#   define TSI_DCTL_MOD		(1<<23)	/* linked list: 0, direct: 1 */
#   define TSI_DCTL_VFAR	(1<<17) /* flush FIFO on VME error: 1 (discard: 0) */
#   define TSI_DCTL_PFAR	(1<<16) /* flush FIFO on PCI error: 1 (discard: 0) */

#   define TSI_DCTL_VBKS(i)	(((i)&7)<<12) /* VME block size */
#   define TSI_DCTL_VBKS_32	TSI_DCTL_VBKS(0)
#   define TSI_DCTL_VBKS_64	TSI_DCTL_VBKS(1)
#   define TSI_DCTL_VBKS_128	TSI_DCTL_VBKS(2)
#   define TSI_DCTL_VBKS_256	TSI_DCTL_VBKS(3)
#   define TSI_DCTL_VBKS_512	TSI_DCTL_VBKS(4)
#   define TSI_DCTL_VBKS_1024	TSI_DCTL_VBKS(5)
#   define TSI_DCTL_VBKS_2048	TSI_DCTL_VBKS(6)
#   define TSI_DCTL_VBKS_4096	TSI_DCTL_VBKS(7)

#   define TSI_DCTL_VBOT(i)	(((i)&7)<< 8) /* VME back-off time */
#   define TSI_DCTL_VBOT_0us	TSI_DCTL_VBOT(0)
#   define TSI_DCTL_VBOT_1us	TSI_DCTL_VBOT(1)
#   define TSI_DCTL_VBOT_2us	TSI_DCTL_VBOT(2)
#   define TSI_DCTL_VBOT_4us	TSI_DCTL_VBOT(3)
#   define TSI_DCTL_VBOT_8us	TSI_DCTL_VBOT(4)
#   define TSI_DCTL_VBOT_16us	TSI_DCTL_VBOT(5)
#   define TSI_DCTL_VBOT_32us	TSI_DCTL_VBOT(6)
#   define TSI_DCTL_VBOT_64us	TSI_DCTL_VBOT(7)

#   define TSI_DCTL_PBKS(i)	(((i)&7)<< 4) /* PCI block size */
#   define TSI_DCTL_PBKS_32	TSI_DCTL_PBKS(0)
#   define TSI_DCTL_PBKS_64	TSI_DCTL_PBKS(1)
#   define TSI_DCTL_PBKS_128	TSI_DCTL_PBKS(2)
#   define TSI_DCTL_PBKS_256	TSI_DCTL_PBKS(3)
#   define TSI_DCTL_PBKS_512	TSI_DCTL_PBKS(4)
#   define TSI_DCTL_PBKS_1024	TSI_DCTL_PBKS(5)
#   define TSI_DCTL_PBKS_2048	TSI_DCTL_PBKS(6)
#   define TSI_DCTL_PBKS_4096	TSI_DCTL_PBKS(7)

#   define TSI_DCTL_PBOT(i)	(((i)&7)<< 0) /* PCI back-off time */
#   define TSI_DCTL_PBOT_0us	TSI_DCTL_PBOT(0)
#   define TSI_DCTL_PBOT_1us	TSI_DCTL_PBOT(1)
#   define TSI_DCTL_PBOT_2us	TSI_DCTL_PBOT(2)
#   define TSI_DCTL_PBOT_4us	TSI_DCTL_PBOT(3)
#   define TSI_DCTL_PBOT_8us	TSI_DCTL_PBOT(4)
#   define TSI_DCTL_PBOT_16us	TSI_DCTL_PBOT(5)
#   define TSI_DCTL_PBOT_32us	TSI_DCTL_PBOT(6)
#   define TSI_DCTL_PBOT_64us	TSI_DCTL_PBOT(7)

/* DMA Status */
#define TSI_DSTA_REG(i)		TSI_DMA_REG(0x504,i)
#define TSI_DSTA0_REG		0x504
#define TSI_DSTA1_REG		0x584
#   define TSI_DSTA_ERR		(1<<28)
#   define TSI_DSTA_ABT		(1<<27)
#   define TSI_DSTA_PAU		(1<<26)
#   define TSI_DSTA_DON		(1<<25)
#   define TSI_DSTA_BSY		(1<<24)
#   define TSI_DSTA_ERRS	(1<<20)	/* Error source; PCI:1, VME:0 */
#   define TSI_DSTA_ERT_MSK	(3<<16) /* Error type                 */
#   define TSI_DSTA_ERT_BERR_E	(0<<16) /* 2eVME even or other bus error */
#   define TSI_DSTA_ERT_BERR_O	(1<<16) /* 2eVME odd bus error        */
#   define TSI_DSTA_ERT_SLVE_E	(2<<16) /* 2eVME even or other slave termination */
#   define TSI_DSTA_ERT_SLVE_O	(3<<16) /* 2eVME odd slave termination; 2eSST read last word invalid */

/* DMA Current source address upper */
#define TSI_DCSAU_REG(i)	TSI_DMA_REG(0x508,i)
#define TSI_DCSAU0_REG		0x508
#define TSI_DCSAU1_REG		0x588

/* DMA Current source address lower */
#define TSI_DCSAL_REG(i)	TSI_DMA_REG(0x50c,i)
#define TSI_DCSAL0_REG		0x50c
#define TSI_DCSAL1_REG		0x58c

/* DMA Current destination address upper */
#define TSI_DCDAU_REG(i)	TSI_DMA_REG(0x510,i)
#define TSI_DCDAU0_REG		0x510
#define TSI_DCDAU1_REG		0x590

/* DMA Current destination address lower */
#define TSI_DCDAL_REG(i)	TSI_DMA_REG(0x514,i)
#define TSI_DCDAL0_REG		0x514
#define TSI_DCDAL1_REG		0x594

/* DMA Current link address upper */
#define TSI_DCLAU_REG(i)	TSI_DMA_REG(0x518,i)
#define TSI_DCLAU0_REG		0x518
#define TSI_DCLAU1_REG		0x598

/* DMA Current link address lower */
#define TSI_DCLAL_REG(i)	TSI_DMA_REG(0x51c,i)
#define TSI_DCLAL0_REG		0x51c
#define TSI_DCLAL1_REG		0x59c

/* DMA Source address upper */
#define TSI_DSAU_REG(i)		TSI_DMA_REG(0x520,i)
#define TSI_DSAU0_REG		0x520
#define TSI_DSAU1_REG		0x5a0

/* DMA Source address lower */
#define TSI_DSAL_REG(i)		TSI_DMA_REG(0x524,i)
#define TSI_DSAL0_REG		0x524
#define TSI_DSAL1_REG		0x5a4

/* DMA Destination address upper */
#define TSI_DDAU_REG(i)		TSI_DMA_REG(0x528,i)
#define TSI_DDAU0_REG		0x528
#define TSI_DDAU1_REG		0x5a8

/* DMA Destination address lower */
#define TSI_DDAL_REG(i)		TSI_DMA_REG(0x52c,i)
#define TSI_DDAL0_REG		0x52c
#define TSI_DDAL1_REG		0x5ac

/* DMA Source Attribute */
#define TSI_DSAT_REG(i)		TSI_DMA_REG(0x530,i)
#define TSI_DSAT0_REG		0x530
#define TSI_DSAT1_REG		0x5b0

/* DMA Destination Attribute */
#define TSI_DDAT_REG(i)		TSI_DMA_REG(0x534,i)
#define TSI_DDAT0_REG		0x534
#define TSI_DDAT1_REG		0x5b4

#   define TSI_DXAT_TYP(i)	(((i)&3)<<28)	/* Xfer type */
#   define TSI_DXAT_TYP_PCI	TSI_DXAT_TYP(0)
#   define TSI_DXAT_TYP_VME	TSI_DXAT_TYP(1)
#   define TSI_DSAT_TYP_PAT	TSI_DXAT_TYP(2) /* pattern */

#   define TSI_DSAT_PSZ		(1<<25) /* pattern size 32-bit: 0, 8-bit: 1 */
#   define TSI_DSAT_NIN		(1<<24)	/* no-increment */

#	define TSI_DXAT_OTAT_MSK	((1<<13)-1)	/* get bits compatible with OTAT */

#   define TSI_DXAT_SSTM(i)	(((i)&3)<<11)	/* 2eSST Xfer rate (MB/s) */
#   define TSI_DXAT_SSTM_116	TSI_DXAT_SSTM(0)
#   define TSI_DXAT_SSTM_267	TSI_DXAT_SSTM(1)
#   define TSI_DXAT_SSTM_320	TSI_DXAT_SSTM(2)

#   define TSI_DXAT_TM(i)	(((i)&7)<< 8) /* VME Xfer mode */
#   define TSI_DXAT_TM_SCT	TSI_DXAT_TM(0)
#   define TSI_DXAT_TM_BLT	TSI_DXAT_TM(1)
#   define TSI_DXAT_TM_MBLT	TSI_DXAT_TM(2)
#   define TSI_DXAT_TM_2eVME	TSI_DXAT_TM(3)
#   define TSI_DXAT_TM_2eSST	TSI_DXAT_TM(4)
#   define TSI_DSAT_TM_2eSST_B	TSI_DXAT_TM(5)	/* 2eSST broadcast */

#   define TSI_DXAT_DBW(i)	(((i)&3)<< 6)	/* VME Data width */
#   define TSI_DXAT_DBW_16	TSI_DXAT_DBW(0)
#   define TSI_DXAT_DBW_32	TSI_DXAT_DBW(1)

#   define TSI_DXAT_SUP		(1<<5)	/* supervisor access */
#   define TSI_DXAT_PGM		(1<<4)	/* program access    */

#   define TSI_DXAT_AM(i)	(((i)&15)<<0)	/* VME Address mode */
#   define TSI_DXAT_AM_A16	TSI_DXAT_AM(0)
#   define TSI_DXAT_AM_A24	TSI_DXAT_AM(1)
#   define TSI_DXAT_AM_A32	TSI_DXAT_AM(2)
#   define TSI_DXAT_AM_A64	TSI_DXAT_AM(4)
#   define TSI_DXAT_AM_CSR	TSI_DXAT_AM(5)

/* DMA Next link address upper */
#define TSI_DNLAU_REG(i)	TSI_DMA_REG(0x538,i)
#define TSI_DNLAU0_REG		0x538
#define TSI_DNLAU1_REG		0x5b8

/* DMA Next link address lower */
#define TSI_DNLAL_REG(i)	TSI_DMA_REG(0x53c,i)
#define TSI_DNLAL0_REG		0x53c
#define TSI_DNLAL1_REG		0x5bc

#	define TSI_DNLAL_LLA	1	/* last element in chain */

/* DMA Byte Count */
#define TSI_DCNT_REG(i)		TSI_DMA_REG(0x540,i)
#define TSI_DCNT0_REG		0x540
#define TSI_DCNT1_REG		0x54c

/* DMA 2eSST destination broadcast select */
#define TSI_DDBS_REG(i)		TSI_DMA_REG(0x544,i)
#define TSI_DDBS0_REG		0x544
#define TSI_DDBS1_REG		0x5c4

/* Convert canonical xfer_mode into Tsi148 bits; return -1 if invalid */
static uint32_t
vme_attr(uint32_t xfer_mode)
{
uint32_t vme_mode;
	if ( am2omode(xfer_mode, &vme_mode) )
		return BSP_VMEDMA_STATUS_UNSUP;

	/* am2omode may set prefetch and other bits */
	vme_mode &= TSI_DXAT_OTAT_MSK;
	vme_mode |= TSI_DXAT_TYP_VME;

	if ( BSP_VMEDMA_MODE_NOINC_VME & xfer_mode )  {
		/* no-incr. only supported on source address */
		if ( (BSP_VMEDMA_MODE_PCI2VME & xfer_mode) )
			return BSP_VMEDMA_STATUS_UNSUP;
		vme_mode |= TSI_DSAT_NIN;
	}

	return vme_mode;
}

static uint32_t
pci_attr(uint32_t xfer_mode)
{
uint32_t pci_mode = 0;
	if ( BSP_VMEDMA_MODE_NOINC_PCI & xfer_mode )  {
		/* no-incr. only supported on source address */
		if ( ! (BSP_VMEDMA_MODE_PCI2VME & xfer_mode) )
			return BSP_VMEDMA_STATUS_UNSUP;
		pci_mode |= TSI_DSAT_NIN;
	}
	return pci_mode;
}

static void tsi_desc_init(DmaDescriptor p)
{
VmeTsi148DmaListDescriptor d = p;
	st_be32( &d->dnlau, 0 );
	st_be32( &d->dnlal, TSI_DNLAL_LLA );
	st_be32( &d->ddbs, (1<<22)-1 );	/* SSTB broadcast not yet fully supported */
}

static void
tsi_desc_setnxt(DmaDescriptor p, DmaDescriptor n)
{
VmeTsi148DmaListDescriptor d = p;
	if ( 0 == n ) {
		st_be32( &d->dnlal, TSI_DNLAL_LLA );
	} else {
		st_be32( &d->dnlal, BSP_LOCAL2PCI_ADDR((uint32_t)n) );
	}
}

static void
tsi_desc_dump(DmaDescriptor p)
{
VmeTsi148DmaListDescriptor d = p;
		printf("   DSA: 0x%08lx%08lx\n", ld_be32(&d->dsau),  ld_be32(&d->dsal));
		printf("   DDA: 0x%08lx%08lx\n", ld_be32(&d->ddau),  ld_be32(&d->ddal));
		printf("   NLA: 0x%08lx%08lx\n", ld_be32(&d->dnlau), ld_be32(&d->dnlal));
		printf("   SAT: 0x%08lx              DAT: 0x%08lx\n", ld_be32(&d->dsat), ld_be32(&d->ddat));
		printf("   CNT: 0x%08lx\n",      ld_be32(&d->dcnt));
}


int
vmeTsi148DmaSetupXX(BERegister *base, int channel, uint32_t mode, uint32_t xfer_mode, void *custom)
{
uint32_t ctl = 0;
uint32_t vmeatt, pciatt, sat, dat;

	if ( channel < 0 || channel > 1 )
		return BSP_VMEDMA_STATUS_UNSUP;

	/* Check bus mode */
	if ( (uint32_t)BSP_VMEDMA_STATUS_UNSUP == (vmeatt = vme_attr(xfer_mode)) )
		return -2;

	/* Check PCI bus mode */
	if ( (uint32_t)BSP_VMEDMA_STATUS_UNSUP == (pciatt = pci_attr(xfer_mode)) )
		return -3;

	/* Compute control word; bottleneck is VME; */
	ctl |= TSI_DCTL_PBKS_32;
	ctl |= (BSP_VMEDMA_OPT_THROUGHPUT == mode ? TSI_DCTL_PBOT_0us : TSI_DCTL_PBOT_1us);

	switch ( mode ) {
		case BSP_VMEDMA_OPT_THROUGHPUT:
			ctl |= TSI_DCTL_VBKS_1024;
			ctl |= TSI_DCTL_VBOT_0us;
		break;

		case BSP_VMEDMA_OPT_LOWLATENCY:
			ctl |= TSI_DCTL_VBKS_32;
			ctl |= TSI_DCTL_VBOT_0us;
		break;

		case BSP_VMEDMA_OPT_SHAREDBUS:
			ctl |= TSI_DCTL_VBKS_128;
			ctl |= TSI_DCTL_VBOT_64us;
		break;

		case BSP_VMEDMA_OPT_CUSTOM:
			ctl = *(uint32_t*)custom;
		break;

		default:
		case BSP_VMEDMA_OPT_DEFAULT:
			ctl = 0;
		break;
	}
	TSI_WR(base, TSI_DCTL_REG(channel), ctl);
	if ( BSP_VMEDMA_MODE_PCI2VME & xfer_mode ) {
		dat = vmeatt; sat = pciatt;
	} else {
		sat = vmeatt; dat = pciatt;
	}
	TSI_WR(base, TSI_DSAT_REG(channel), sat);
	TSI_WR(base, TSI_DDAT_REG(channel), dat);
	return 0;
}

int
vmeTsi148DmaSetup(int channel, uint32_t mode, uint32_t xfer_mode, void *custom)
{
BERegister *base = THEBASE;
	return vmeTsi148DmaSetupXX(base, channel, mode, xfer_mode, custom);
}


int
vmeTsi148DmaListStartXX(BERegister *base, int channel, VmeTsi148DmaListDescriptor d)
{
uint32_t ctl;

	if ( d ) {
		/* Set list pointer and start */
		if ( channel < 0 || channel > 1 )
			return BSP_VMEDMA_STATUS_UNSUP;

		if ( TSI_DSTA_BSY & TSI_RD(base, TSI_DSTA_REG(channel)) )
			return BSP_VMEDMA_STATUS_BUSY; 	/* channel busy */

		TSI_WR(base, TSI_DNLAL_REG(channel), (uint32_t)BSP_LOCAL2PCI_ADDR(d));

		asm volatile("":::"memory");

		/* Start transfer */
		ctl  = TSI_RD(base, TSI_DCTL_REG(channel)) | TSI_DCTL_DGO;
		ctl &= ~TSI_DCTL_MOD;
		TSI_WR(base, TSI_DCTL_REG(channel), ctl);
	}
	/* else: list vs. direct mode is set by the respective start commands */
	return 0;
}

int
vmeTsi148DmaListStart(int channel, VmeTsi148DmaListDescriptor d)
{
BERegister *base = THEBASE;
	return vmeTsi148DmaListStartXX(base, channel, d);
}

int
vmeTsi148DmaStartXX(BERegister *base, int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes)
{
uint32_t src, dst, ctl;

	if ( channel < 0 || channel > 1 )
		return BSP_VMEDMA_STATUS_UNSUP;

	if ( TSI_DSTA_BSY & TSI_RD(base, TSI_DSTA_REG(channel)) )
		return BSP_VMEDMA_STATUS_BUSY; 	/* channel busy */

	/* retrieve direction from dst attribute */
	if ( TSI_DXAT_TYP_VME & TSI_RD(base, TSI_DDAT_REG(channel)) ) {
		dst = vme_addr;
		src = pci_addr;
	} else {
		src = vme_addr;
		dst = pci_addr;
	}
	/* FIXME: we leave the 'upper' registers (topmost 32bits) alone.
	 *        Probably, we should reset them at init...
	 */
	TSI_WR(base, TSI_DSAL_REG(channel), src);
	TSI_WR(base, TSI_DDAL_REG(channel), dst);
	TSI_WR(base, TSI_DCNT_REG(channel), n_bytes);

	asm volatile("":::"memory");

	/* Start transfer */
	ctl  = TSI_RD(base, TSI_DCTL_REG(channel)) | TSI_DCTL_DGO | TSI_DCTL_MOD;
	TSI_WR(base, TSI_DCTL_REG(channel), ctl);

	return 0;
}

int
vmeTsi148DmaStart(int channel, uint32_t pci_addr, uint32_t vme_addr, uint32_t n_bytes)
{
BERegister *base = THEBASE;
	return vmeTsi148DmaStartXX(base, channel, pci_addr, vme_addr, n_bytes);
}

uint32_t
vmeTsi148DmaStatusXX(BERegister *base, int channel)
{
uint32_t	st = TSI_RD(base, TSI_DSTA_REG(channel));

	if ( channel < 0 || channel > 1 )
		return BSP_VMEDMA_STATUS_UNSUP;

	st = TSI_RD(base, TSI_DSTA_REG(channel));

	/* Status can be zero if an empty list (all counts == 0) is executed */
	if ( (TSI_DSTA_DON & st) || 0 == st )
		return BSP_VMEDMA_STATUS_OK;

	if ( TSI_DSTA_BSY & st )
		return BSP_VMEDMA_STATUS_BUSY; 	/* channel busy */

	if ( TSI_DSTA_ERR & st ) {
		if ( TSI_DSTA_ERRS & st )
			return BSP_VMEDMA_STATUS_BERR_PCI;
		if ( ! (TSI_DSTA_ERT_SLVE_E	& st) )
			return BSP_VMEDMA_STATUS_BERR_VME;
	}

	return BSP_VMEDMA_STATUS_OERR;
}

uint32_t
vmeTsi148DmaStatus(int channel)
{
BERegister *base = THEBASE;
	return vmeTsi148DmaStatusXX(base, channel);
}

#define ALL_BITS_NEEDED	(BSP_VMEDMA_MSK_ATTR | BSP_VMEDMA_MSK_PCIA | BSP_VMEDMA_MSK_VMEA)

static int
tsi_desc_setup (
		DmaDescriptor p,
		uint32_t	attr_mask,
		uint32_t	xfer_mode,
		uint32_t	pci_addr,
		uint32_t	vme_addr,
		uint32_t	n_bytes)
{
VmeTsi148DmaListDescriptor	d = p;
uint32_t	vmeatt = 0, pciatt = 0, tmp, src, dst, dat, sat;

	/* argument check */

	/* since we must vme/pci into src/dst we need the direction
	 * bit. Reject requests that have only part of the mask
	 * bits set. It would be possible to be more sophisticated
	 * by caching more information but we try to be simple here...
	 */
	tmp = attr_mask & ALL_BITS_NEEDED;
	if ( tmp != 0 && tmp != ALL_BITS_NEEDED )
		return -1;

	if ( BSP_VMEDMA_MSK_ATTR & attr_mask ) {
		/* Check VME bus mode */
		vmeatt = vme_attr(xfer_mode);
		if ( (uint32_t)BSP_VMEDMA_STATUS_UNSUP == vmeatt  )
			return -1;

		/* Check PCI bus mode */
		pciatt = pci_attr(xfer_mode);
		if ( (uint32_t)BSP_VMEDMA_STATUS_UNSUP == pciatt  )
			return -1;
	}

	if ( BSP_VMEDMA_MSK_ATTR & attr_mask ) {
		if ( BSP_VMEDMA_MODE_PCI2VME & xfer_mode ) {
			dat = vmeatt;   sat = pciatt;
			dst = vme_addr; src = pci_addr;
		} else {
			sat = vmeatt;   dat = pciatt;
			src = vme_addr; dst = pci_addr;
		}
		st_be32( &d->dsau, 0 );   st_be32( &d->dsal, src );
		st_be32( &d->ddau, 0 );   st_be32( &d->ddal, dst );
		st_be32( &d->dsat, sat ); st_be32( &d->ddat, dat );
	}

	if ( BSP_VMEDMA_MSK_BCNT & attr_mask )
		st_be32( &d->dcnt, n_bytes);

	return 0;
}

static int
tsi_desc_start (volatile void *controller_addr, int channel, DmaDescriptor p)
{
VmeTsi148DmaListDescriptor d = p;
	if ( !controller_addr )
		controller_addr = THEBASE;
	return vmeTsi148DmaListStartXX((BERegister*)controller_addr, channel, d);
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

	TSI_WR(THEBASE, TSI_INTEO_REG, 0);

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
