/*
 *
 *	PCI defines and function prototypes
 *	Copyright 1994, Drew Eckhardt
 *	Copyright 1997, 1998 Martin Mares <mj@atrey.karlin.mff.cuni.cz>
 *
 *	For more information, please consult the following manuals (look at
 *	http://www.pcisig.com/ for how to get them):
 *
 *	PCI BIOS Specification
 *	PCI Local Bus Specification
 *	PCI to PCI Bridge Specification
 *	PCI System Design Guide
 *
 *      pci.h,v 1.2 2002/05/14 17:10:16 joel Exp
 *
 *      S. Kate Feng : Added vendor/device support for
 *      Marvell and PLX. 3/5/2004
 * 
 */

#ifndef BSP_PCI_H
#define BSP_PCI_H

#include <rtems/pci.h>

#define PCI0_VENDOR_ID          0x00    /* 16 bits */
#define PCI0_DEVICE_ID          0x02    /* 16 bits */
#define PCI0_COMMAND            0x04    /* 16 bits */

#define  PCI_COMMAND_SB_DIS     0x2000  /* PCI configuration read will stop
                                         * acting as sync barrier transaction
                                         */ 
#define  PCI_STATUS_CLRERR_MASK 0xf9000000 /* <SKF> */
 
#define PCI0_CLASS_REVISION	0x08	/* High 24 bits are class, low 8
					   revision */
#define PCI0_REVISION_ID        0x08    /* Revision ID */
#define PCI0_CLASS_PROG         0x09    /* Reg. Level Programming Interface */
#define PCI0_CLASS_DEVICE       0x0a    /* Device class */

#define PCI0_CACHE_LINE_SIZE	0x0c	/* 8 bits */
#define PCI0_LATENCY_TIMER	0x0d	/* 8 bits */
#define PCI0_HEADER_TYPE	0x0e	/* 8 bits */

#define PCI0_CAPABILITY_LIST_POINTER    0x34

/* 0x38-0x3b are reserved */
#define PCI0_INTERRUPT_LINE	0x3c	/* 8 bits */
#define PCI0_INTERRUPT_PIN	0x3d	/* 8 bits */
#define PCI0_MIN_GNT		0x3e	/* 8 bits */
#define PCI0_MAX_LAT		0x3f	/* 8 bits */

/*
 * Under PCI1, each device has 256 bytes of configuration address space,
 * of which the first 64 bytes are standardized as follows:
 */ 
#define PCI1_VENDOR_ID		0x80	/* 16 bits */
#define PCI1_DEVICE_ID		0x82	/* 16 bits */
#define PCI1_COMMAND		0x84	/* 16 bits */
#define PCI1_STATUS		0x86	/* 16 bits */
#define PCI1_CLASS_REVISION	0x88	/* High 24 bits are class, low 8
					   revision */
#define PCI1_REVISION_ID        0x88    /* Revision ID */
#define PCI1_CLASS_PROG         0x89    /* Reg. Level Programming Interface */
#define PCI1_CLASS_DEVICE       0x8a    /* Device class */

#define PCI1_CACHE_LINE_SIZE	0x8c	/* 8 bits */
#define PCI1_LATENCY_TIMER	0x8d	/* 8 bits */
#define PCI1_HEADER_TYPE	0x8e	/* 8 bits */
#define PCI1_BIST		0x8f	/* 8 bits */

#define PCI1_BASE_ADDRESS_0	0x90	/* 32 bits */
#define PCI1_BASE_ADDRESS_1	0x94	/* 32 bits [htype 0,1 only] */
#define PCI1_BASE_ADDRESS_2	0x98	/* 32 bits [htype 0 only] */
#define PCI1_BASE_ADDRESS_3	0x9c	/* 32 bits */
#define PCI1_MEM_BASE_ADDR	0xa0	/* 32 bits */
#define PCI1_IO_BASE_ADDR	0xa4	/* 32 bits */

/* Header type 0 (normal devices) */
#define PCI1_CARDBUS_CIS		0xa8
#define PCI1_SUBSYSTEM_VENDOR_ID	0xac
#define PCI1_SUBSYSTEM_ID	        0xae  
#define PCI1_ROM_ADDRESS		0xb0	/* Bits 31..11 are address, 10..1 reserved */
#define PCI1_CAPABILITY_LIST_POINTER    0xb4

/* 0xb8-0xbb are reserved */
#define PCI1_INTERRUPT_LINE	0xbc	/* 8 bits */
#define PCI1_INTERRUPT_PIN	0xbd	/* 8 bits */
#define PCI1_MIN_GNT		0xbe	/* 8 bits */
#define PCI1_MAX_LAT		0xbf	/* 8 bits */

/* Device classes and subclasses */
#define PCI_CLASS_GT6426xAB             0x0580    /* <SKF> */
             
/*
 * Vendor and card ID's: sort these numerically according to vendor
 * (and according to card ID within vendor). Send all updates to
 * <linux-pcisupport@cck.uni-kl.de>.
 */
#define PCI_VENDOR_ID_MARVELL           0x11ab   /* <SKF> */
#define PCI_DEVICE_ID_MARVELL_GT6426xAB 0x6430   /* <SKF> */

/* Note : The PLX Technology Inc. had the old VENDOR_ID.
 * See PCI_VENDOR_ID_PLX,  PCI_VENDOR_ID_PLX_9050, ..etc.
 */
#define PCI_VENDOR_ID_PLX2              0x3388   /* <SKF> */
#define PCI_DEVICE_ID_PLX2_PCI6154_HB2  0x26     /* <SKF> */

#define PCI_DEVICE_INTEL_82544EI_COPPER 0x1008  /* <SKF> */

typedef struct {
  unsigned int	pci_config_addr;
  unsigned int	pci_config_data;
  /*  const pci_config_access_functions*	pci_functions;*/
} pci_bsp_config;

extern pci_bsp_config BSP_pci_config[2];

#ifndef PCI_MAIN
extern int PCIx_read_config_byte();
extern int PCIx_read_config_word();
extern int PCIx_read_config_dword();
extern int PCIx_write_config_byte();
extern int PCIx_write_config_word();
extern int PCIx_write_config_dword();
#endif

extern int pci_bsp_read_config_byte(
  unsigned char bus, 
  unsigned char dev,
  unsigned char func,
  unsigned char offset,
  unsigned char *val
);

extern int pci_bsp_read_config_word(
  unsigned char bus, 
  unsigned char dev,
  unsigned char func, 
  unsigned char offset, 
  unsigned short *val
);

extern int pci_bsp_read_config_dword(
  unsigned char bus, 
  unsigned char dev,
  unsigned char func, 
  unsigned char offset, 
  unsigned int *val
);

extern int pci_bsp_write_config_byte(
  unchar bus, 
  unchar dev,
  unchar func, 
  unchar offset, 
  unchar val
);

extern int pci_bsp_write_config_word(
  unchar bus, 
  unchar dev,
  unchar func, 
  unchar offset, 
  unsigned short val
);

extern int pci_bsp_write_config_dword(
  unchar bus,
  unchar dev,
  unchar func, 
  unchar offset, 
  unsigned int val
);


/*
 * Override the default pci_read_config... for vmeUniverse.c
 */
#define BSP_PCI_CONFIG_IN_LONG  pci_bsp_read_config_dword
#define BSP_PCI_CONFIG_IN_BYTE  pci_bsp_read_config_byte


int BSP_PCIxFindDevice(unsigned short vendorid, unsigned short deviceid,
		       int instance, int pciNum, int *pbus, int *pdev, int *pfun );

#endif /* BSP_PCI_H */
