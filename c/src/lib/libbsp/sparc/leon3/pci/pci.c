/*
 * pci.c :  this file contains basic PCI Io functions.
 *
 *  Copyright (C) 1999 valette@crf.canon.fr
 *
 *  This code is heavily inspired by the public specification of STREAM V2
 *  that can be found at :
 *
 *      <http://www.chorus.com/Documentation/index.html> by following
 *  the STREAM API Specification Document link.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Till Straumann, <strauman@slac.stanford.edu>, 1/2002
 *   - separated bridge detection code out of this file
 *
 *
 *  Adapted to GRPCI
 *  Copyright (C) 2006 Gaisler Research
 *
 */

#include <pci.h>
#include <stdlib.h>
#include <rtems/bspIo.h>

#define PCI_ADDR 0x80000400
#define DMAPCI_ADDR 0x80000500
#define PCI_CONF 0xfff50000
#define PCI_MEM_START 0xe0000000
#define PCI_MEM_END   0xf0000000
#define PCI_MEM_SIZE  (PCI_MEM_START - PCI_MEM_END)

/* If uncommented byte twisting is enabled */
/*#define BT_ENABLED 1*/

/* Define PCI_INFO to get a listing of configured devices at boot time */
#define PCI_INFO 1

#define DEBUG 1

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

/* allow for overriding these definitions */
#ifndef PCI_CONFIG_ADDR
#define PCI_CONFIG_ADDR			0xcf8
#endif
#ifndef PCI_CONFIG_DATA
#define PCI_CONFIG_DATA			0xcfc
#endif

/* define a shortcut */
#define pci	BSP_pci_configuration

/*
 * Bit encode for PCI_CONFIG_HEADER_TYPE register
 */
unsigned char ucMaxPCIBus;
typedef struct {
	volatile unsigned int cfg_stat;
	volatile unsigned int bar0;
	volatile unsigned int page0;
	volatile unsigned int bar1;
	volatile unsigned int page1;
	volatile unsigned int iomap;
	volatile unsigned int stat_cmd;
} LEON3_GRPCI_Regs_Map;

LEON3_GRPCI_Regs_Map *pcic = (LEON3_GRPCI_Regs_Map *) PCI_ADDR;
unsigned int *pcidma = (unsigned int *)DMAPCI_ADDR;

struct pci_res {
    unsigned int size;
    unsigned char bar;
    unsigned char devfn;
};

static inline unsigned int flip_dword (unsigned int l)
{
        return ((l&0xff)<<24) | (((l>>8)&0xff)<<16) | (((l>>16)&0xff)<<8)| ((l>>24)&0xff);
}


/*  The configuration access functions uses the DMA functionality of the
 *  AT697 pci controller to be able access all slots
 */


static int
BSP_pci_read_config_dword(
  unsigned char bus,
  unsigned char slot,
  unsigned char function,
  unsigned char offset,
  uint32_t     *val
)
{
    volatile unsigned int *pci_conf;

    if (offset & 3) return PCIBIOS_BAD_REGISTER_NUMBER;

    if (slot > 21) {
        *val = 0xffffffff;
        return PCIBIOS_SUCCESSFUL;
    }

    pci_conf = (volatile unsigned int *) (PCI_CONF +
        ((slot<<11) | (function<<8) | offset));

#ifdef BT_ENABLED
    *val =  flip_dword(*pci_conf);
#else
    *val = *pci_conf;
#endif

    if (pcic->cfg_stat & 0x100) {
        *val = 0xffffffff;
    }

   DBG("pci_read - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n", bus, slot, function, offset,  (1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f), *val);

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_read_config_word(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned short *val) {
    uint32_t v;

    if (offset & 1) return PCIBIOS_BAD_REGISTER_NUMBER;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);
    *val = 0xffff & (v >> (8*(offset & 3)));

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_read_config_byte(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned char *val) {
    uint32_t v;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);

    *val = 0xff & (v >> (8*(offset & 3)));

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_write_config_dword(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, uint32_t val) {

    volatile unsigned int *pci_conf;
    unsigned int value;

    if (offset & 3 || bus != 0) return PCIBIOS_BAD_REGISTER_NUMBER;


    pci_conf = (volatile unsigned int *) (PCI_CONF +
                  ((slot<<11) | (function<<8) | (offset & ~3)));

#ifdef BT_ENABLED
        value = flip_dword(val);
#else
        value = val;
#endif

    *pci_conf = value;

    DBG("pci write - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n", bus, slot, function, offset, (1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f), value);

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_write_config_word(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned short val) {
    uint32_t v;

    if (offset & 1) return PCIBIOS_BAD_REGISTER_NUMBER;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);

    v = (v & ~(0xffff << (8*(offset&3)))) | ((0xffff&val) << (8*(offset&3)));

    return pci_write_config_dword(bus, slot, function, offset&~3, v);
}


static int
BSP_pci_write_config_byte(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned char val) {
    uint32_t v;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);

    v = (v & ~(0xff << (8*(offset&3)))) | ((0xff&val) << (8*(offset&3)));

    return pci_write_config_dword(bus, slot, function, offset&~3, v);
}



const pci_config_access_functions pci_access_functions = {
    BSP_pci_read_config_byte,
    BSP_pci_read_config_word,
    BSP_pci_read_config_dword,
    BSP_pci_write_config_byte,
    BSP_pci_write_config_word,
    BSP_pci_write_config_dword
};

rtems_pci_config_t BSP_pci_configuration = {
   (volatile unsigned char*)PCI_CONFIG_ADDR,
   (volatile unsigned char*)PCI_CONFIG_DATA,
   &pci_access_functions
};


int init_grpci(void) {

    volatile unsigned int *page0 =  (unsigned volatile int *) PCI_MEM_START;
    uint32_t data;
#ifndef BT_ENABLED
    uint32_t addr;
#endif

#ifndef BT_ENABLED
    pci_write_config_dword(0,0,0,0x10, 0xffffffff);
    pci_read_config_dword(0,0,0,0x10, &addr);
    pci_write_config_dword(0,0,0,0x10, flip_dword(0x10000000));    /* Setup bar0 to nonzero value (grpci considers BAR==0 as invalid) */
    addr = (~flip_dword(addr)+1)>>1;                               /* page0 is accessed through upper half of bar0 */
    pcic->cfg_stat |= 0x10000000;                                  /* Setup mmap reg so we can reach bar0 */
    page0[addr/4] = 0;                                             /* Disable bytetwisting ... */
#endif

    /* set 1:1 mapping between AHB -> PCI memory */
    pcic->cfg_stat = (pcic->cfg_stat & 0x0fffffff) | PCI_MEM_START;

    /* and map system RAM at pci address 0x40000000 */
    pci_write_config_dword(0, 0, 0, 0x14, 0x40000000);
    pcic->page1 = 0x40000000;

     /* set as bus master and enable pci memory responses */
    pci_read_config_dword(0, 0, 0, 0x4, &data);
    pci_write_config_dword(0, 0, 0, 0x4, data | 0x6);

    return 0;
}

/* DMA functions which uses GRPCIs optional DMA controller (len in words) */
int dma_to_pci(unsigned int ahb_addr, unsigned int pci_addr, unsigned int len) {
    int ret = 0;

    pcidma[0] = 0x82;
    pcidma[1] = ahb_addr;
    pcidma[2] = pci_addr;
    pcidma[3] = len;
    pcidma[0] = 0x83;

    while ( (pcidma[0] & 0x4) == 0)
        ;

    if (pcidma[0] & 0x8) { /* error */
        ret = -1;
    }

    pcidma[0] |= 0xC;
    return ret;

}

int dma_from_pci(unsigned int ahb_addr, unsigned int pci_addr, unsigned int len) {
    int ret = 0;

    pcidma[0] = 0x80;
    pcidma[1] = ahb_addr;
    pcidma[2] = pci_addr;
    pcidma[3] = len;
    pcidma[0] = 0x81;

    while ( (pcidma[0] & 0x4) == 0)
        ;

    if (pcidma[0] & 0x8) { /* error */
        ret = -1;
    }

    pcidma[0] |= 0xC;
    return ret;

}


void pci_mem_enable(unsigned char bus, unsigned char slot, unsigned char function) {
    uint32_t data;

    pci_read_config_dword(0, slot, function, PCI_COMMAND, &data);
    pci_write_config_dword(0, slot, function, PCI_COMMAND, data | PCI_COMMAND_MEMORY);

}

void pci_master_enable(unsigned char bus, unsigned char slot, unsigned char function) {
    uint32_t data;

    pci_read_config_dword(0, slot, function, PCI_COMMAND, &data);
    pci_write_config_dword(0, slot, function, PCI_COMMAND, data | PCI_COMMAND_MASTER);

}

static inline void swap_res(struct pci_res **p1, struct pci_res **p2) {

    struct pci_res *tmp = *p1;
    *p1 = *p2;
    *p2 = tmp;

}

/* pci_allocate_resources
 *
 * This function scans the bus and assigns PCI addresses to all devices. It handles both
 * single function and multi function devices. All allocated devices are enabled and
 * latency timers are set to 40.
 *
 * NOTE that it only allocates PCI memory space devices (that are at least 1 KB).
 * IO spaces are not enabled. Also, it does not handle pci-pci bridges. They are left disabled.
 *
 *
*/
void pci_allocate_resources(void) {

    unsigned int slot, numfuncs, func, pos, i, swapped, addr, dev, fn;
    uint32_t id, tmp, size;
    unsigned char header;
    struct pci_res **res;

    res = (struct pci_res **) malloc(sizeof(struct pci_res *)*32*8*6);

    for (i = 0; i < 32*8*6; i++) {
        res[i] = (struct pci_res *) malloc(sizeof(struct pci_res));
        res[i]->size = 0;
        res[i]->devfn = i;
    }

    for(slot = 1; slot < PCI_MAX_DEVICES; slot++) {

        pci_read_config_dword(0, slot, 0, PCI_VENDOR_ID, &id);

        if(id == PCI_INVALID_VENDORDEVICEID || id == 0) {
            /*
             * This slot is empty
             */
            continue;
        }

        pci_read_config_byte(0, slot, 0, PCI_HEADER_TYPE, &header);

        if(header & PCI_HEADER_TYPE_MULTI_FUNCTION)	{
            numfuncs = PCI_MAX_FUNCTIONS;
        }
        else {
            numfuncs = 1;
        }

        for(func = 0; func < numfuncs; func++) {

            pci_read_config_dword(0, slot, func, PCI_VENDOR_ID, &id);
            if(id == PCI_INVALID_VENDORDEVICEID || id == 0) {
                continue;
            }

            pci_read_config_dword(0, slot, func, PCI_CLASS_REVISION, &tmp);
            tmp >>= 16;
            if (tmp == PCI_CLASS_BRIDGE_PCI) {
                continue;
            }

            for (pos = 0; pos < 6; pos++) {
                pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_0 + (pos<<2), 0xffffffff);
                pci_read_config_dword(0, slot, func, PCI_BASE_ADDRESS_0 + (pos<<2), &size);

                if (size == 0 || size == 0xffffffff || (size & 0x3f1) != 0){
                    pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_0 + (pos<<2), 0);
                    continue;

                }else {
                    size &= 0xfffffff0;
                    res[slot*8*6+func*6+pos]->size  = ~size+1;
                    res[slot*8*6+func*6+pos]->devfn = slot*8 + func;
                    res[slot*8*6+func*6+pos]->bar   = pos;

                    DBG("Slot: %d, function: %d, bar%d size: %x\n", slot, func, pos, ~size+1);
                }
            }
        }
    }


    /* Sort the resources in descending order */
    swapped = 1;
    while (swapped == 1) {
        swapped = 0;
        for (i = 0; i < 32*8*6-1; i++) {
            if (res[i]->size < res[i+1]->size) {
                swap_res(&res[i], &res[i+1]);
                swapped = 1;
            }
        }
        i++;
    }

    /* Assign the BARs */
    addr = PCI_MEM_START;
    for (i = 0; i < 32*8*6; i++) {

        if (res[i]->size == 0) {
            goto done;
        }
        if ( (addr + res[i]->size) > PCI_MEM_END) {
            printk("Out of PCI memory space, all devices not configured.\n");
            goto done;
        }

        dev = res[i]->devfn >> 3;
        fn  = res[i]->devfn & 7;

        DBG("Assigning PCI addr %x to device %d, function %d, bar %d\n", addr, dev, fn, res[i]->bar);
        pci_write_config_dword(0, dev, fn, PCI_BASE_ADDRESS_0+res[i]->bar*4, addr);
        addr += res[i]->size;

        /* Set latency timer to 64 */
        pci_read_config_dword(0, dev, fn, 0xC, &tmp);
        pci_write_config_dword(0, dev, fn, 0xC, tmp|0x4000);

        pci_mem_enable(0, dev, fn);

    }



done:

#ifdef PCI_INFO
    printk("\nPCI devices found and configured:\n");
    for (slot = 1; slot < PCI_MAX_DEVICES; slot++) {

        pci_read_config_byte(0, slot, 0, PCI_HEADER_TYPE, &header);

        if(header & PCI_HEADER_TYPE_MULTI_FUNCTION)	{
            numfuncs = PCI_MAX_FUNCTIONS;
        }
        else {
            numfuncs = 1;
        }

        for (func = 0; func < numfuncs; func++) {

            pci_read_config_dword(0, slot, func, PCI_COMMAND, &tmp);

            if (tmp & PCI_COMMAND_MEMORY) {

                pci_read_config_dword(0, slot, func, PCI_VENDOR_ID,  &id);

                if (id == PCI_INVALID_VENDORDEVICEID || id == 0) continue;

                printk("\nSlot %d function: %d\nVendor id: 0x%x, device id: 0x%x\n", slot, func, id & 0xffff, id>>16);

                for (pos = 0; pos < 6; pos++) {
                    pci_read_config_dword(0, slot, func, PCI_BASE_ADDRESS_0 + pos*4, &tmp);

                    if (tmp != 0 && tmp != 0xffffffff && (tmp & 0x3f1) == 0) {

                        printk("\tBAR %d: %x\n", pos, tmp);
                    }

                }
                printk("\n");

            }

        }
     }
    printk("\n");
#endif

    for (i = 0; i < 1536; i++) {
        free(res[i]);
    }
    free(res);
}



int init_pci(void)
{
    unsigned char ucSlotNumber, ucFnNumber, ucNumFuncs;
    unsigned char ucHeader;
    unsigned char ucMaxSubordinate;
    uint32_t      ulClass, ulDeviceID;

    DBG("Initializing PCI\n");
    if ( init_grpci() ) {
      return -1;
    }
    pci_allocate_resources();
    DBG("PCI resource allocation done\n");
/*
 * Scan PCI bus 0 looking for PCI-PCI bridges
 */
    for(ucSlotNumber=0;ucSlotNumber<PCI_MAX_DEVICES;ucSlotNumber++) {
        (void)pci_read_config_dword(0,
                                    ucSlotNumber,
                                    0,
                                    PCI_VENDOR_ID,
                                    &ulDeviceID);
        if(ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
/*
 * This slot is empty
 */
            continue;
        }
        (void)pci_read_config_byte(0,
                                   ucSlotNumber,
                                   0,
                                   PCI_HEADER_TYPE,
                                   &ucHeader);
        if(ucHeader&PCI_HEADER_TYPE_MULTI_FUNCTION)	{
            ucNumFuncs=PCI_MAX_FUNCTIONS;
        }
        else {
            ucNumFuncs=1;
        }
        for(ucFnNumber=0;ucFnNumber<ucNumFuncs;ucFnNumber++) {
            (void)pci_read_config_dword(0,
                                        ucSlotNumber,
                                        ucFnNumber,
                                        PCI_VENDOR_ID,
                                        &ulDeviceID);
            if(ulDeviceID==PCI_INVALID_VENDORDEVICEID) {
/*
 * This slot/function is empty
 */
                continue;
            }

/*
 * This slot/function has a device fitted.
 */
            (void)pci_read_config_dword(0,
                                        ucSlotNumber,
                                        ucFnNumber,
                                        PCI_CLASS_REVISION,
                                        &ulClass);
            ulClass >>= 16;
            if (ulClass == PCI_CLASS_BRIDGE_PCI) {
/*
 * We have found a PCI-PCI bridge
 */
                (void)pci_read_config_byte(0,
                                           ucSlotNumber,
                                           ucFnNumber,
                                           PCI_SUBORDINATE_BUS,
                                           &ucMaxSubordinate);
                if(ucMaxSubordinate>ucMaxPCIBus) {
                    ucMaxPCIBus=ucMaxSubordinate;
                }
            }
        }
    }
    return 0;
}

/*
 * Return the number of PCI busses in the system
 */
unsigned char BusCountPCI(void)
{
    return(ucMaxPCIBus+1);
}
