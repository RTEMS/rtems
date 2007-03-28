/*
 *  AMBA Plag & Play Bus Driver Macros
 *
 *  Macros used for AMBA Plug & Play bus scanning
 *
 *  COPYRIGHT (c) 2004.
 *  Gaisler Research
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#define LEON3_IO_AREA 0xfff00000
#define LEON3_CONF_AREA 0xff000
#define LEON3_AHB_SLAVE_CONF_AREA (1 << 11)

#define LEON3_AHB_CONF_WORDS 8
#define LEON3_APB_CONF_WORDS 2
#define LEON3_AHB_MASTERS 64
#define LEON3_AHB_SLAVES 64
#define LEON3_APB_SLAVES 16
#define LEON3_APBUARTS 8

/* Vendor codes */ 
#define VENDOR_GAISLER   1
#define VENDOR_PENDER    2
#define VENDOR_ESA       4 
#define VENDOR_OPENCORES 8 

/* Gaisler Research device id's */
#define GAISLER_LEON3    0x03 
#define GAISLER_LEON3DSU 0x04 
#define GAISLER_ETHAHB   0x05 
#define	GAISLER_APBMST   0x06 
#define	GAISLER_AHBUART  0x07 
#define	GAISLER_SRCTRL   0x08 
#define GAISLER_SDCTRL   0x09 
#define	GAISLER_APBUART  0x0C 
#define	GAISLER_IRQMP    0x0D 
#define	GAISLER_AHBRAM   0x0E 
#define	GAISLER_GPTIMER  0x11
#define	GAISLER_PCITRG   0x12
#define	GAISLER_PCISBRG  0x13
#define	GAISLER_PCIFBRG  0x14
#define	GAISLER_PCITRACE 0x15
#define	GAISLER_DMACTRL  0x16
#define GAISLER_PIOPORT  0x1A
#define GAISLER_ETHMAC   0x1D
 
#define GAISLER_SPACEWIRE 0x01f

/* European Space Agency device id's */
#define ESA_LEON2        0x2 
#define ESA_MCTRL        0xF 

/* Opencores device id's */
#define OPENCORES_PCIBR  0x4  
#define OPENCORES_ETHMAC 0x5


/* 
 *
 * Macros for manipulating Configuration registers  
 *
 */


#define amba_get_confword(tab, index, word) (*((tab).addr[(index)]+(word)))

#define amba_vendor(x) (((x) >> 24) & 0xff)

#define amba_device(x) (((x) >> 12) & 0xfff)

#define amba_ahb_get_membar(tab, index, nr) (*((tab).addr[(index)]+4+(nr)))

#define amba_apb_get_membar(tab, index) (*((tab).addr[(index)]+1))

#define amba_membar_start(mbar) (((mbar) & 0xfff00000) & (((mbar) & 0xfff0) << 16))

#define amba_iobar_start(base, iobar) ((base) | ((((iobar) & 0xfff00000)>>12) & (((iobar) & 0xfff0)<<4)) )

#define amba_irq(conf) ((conf) & 0xf)


