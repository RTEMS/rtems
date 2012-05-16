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
 *  Adapted to LEON2 AT697 PCI
 *  Copyright (C) 2006 Gaisler Research
 *
 */

#include <pci.h>
#include <rtems/bspIo.h>
#include <stdlib.h>

/* Define PCI_INFO to get a listing of configured devices at boot time */
#define PCI_INFO 1

/* #define DEBUG 1 */

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
    volatile unsigned int pciid1;        /* 0x80000100 - PCI Device identification register 1         */
    volatile unsigned int pcisc;         /* 0x80000104 - PCI Status & Command                         */
    volatile unsigned int pciid2;        /* 0x80000108 - PCI Device identification register 2         */
    volatile unsigned int pcibhlc;       /* 0x8000010c - BIST, Header type, Cache line size register  */
    volatile unsigned int mbar1;         /* 0x80000110 - Memory Base Address Register 1               */
    volatile unsigned int mbar2;         /* 0x80000114 - Memory Base Address Register 2               */
    volatile unsigned int iobar3;        /* 0x80000118 - IO Base Address Register 3                   */
    volatile unsigned int dummy1[4];     /* 0x8000011c - 0x80000128                                   */
    volatile unsigned int pcisid;        /* 0x8000012c - Subsystem identification register            */
    volatile unsigned int dummy2;        /* 0x80000130                                                */
    volatile unsigned int pcicp;         /* 0x80000134 - PCI capabilities pointer register            */
    volatile unsigned int dummy3;        /* 0x80000138                                                */
    volatile unsigned int pcili;         /* 0x8000013c - PCI latency interrupt register               */
    volatile unsigned int pcirt;         /* 0x80000140 - PCI retry, trdy config                       */
    volatile unsigned int pcicw;         /* 0x80000144 - PCI configuration write register             */
    volatile unsigned int pcisa;         /* 0x80000148 - PCI Initiator Start Address                  */
    volatile unsigned int pciiw;         /* 0x8000014c - PCI Initiator Write Register                 */
    volatile unsigned int pcidma;        /* 0x80000150 - PCI DMA configuration register               */
    volatile unsigned int pciis;         /* 0x80000154 - PCI Initiator Status Register                */
    volatile unsigned int pciic;         /* 0x80000158 - PCI Initiator Configuration                  */
    volatile unsigned int pcitpa;        /* 0x8000015c - PCI Target Page Address Register             */
    volatile unsigned int pcitsc;        /* 0x80000160 - PCI Target Status-Command Register           */
    volatile unsigned int pciite;        /* 0x80000164 - PCI Interrupt Enable Register                */
    volatile unsigned int pciitp;        /* 0x80000168 - PCI Interrupt Pending Register               */
    volatile unsigned int pciitf;        /* 0x8000016c - PCI Interrupt Force Register                 */
    volatile unsigned int pcid;          /* 0x80000170 - PCI Data Register                            */
    volatile unsigned int pcibe;         /* 0x80000174 - PCI Burst End Register                       */
    volatile unsigned int pcidmaa;       /* 0x80000178 - PCI DMA Address Register                     */
} AT697_PCI_Map;

AT697_PCI_Map *pcic = (AT697_PCI_Map *) 0x80000100;

#define PCI_MEM_START 0xa0000000
#define PCI_MEM_END   0xf0000000
#define PCI_MEM_SIZE  (PCI_MEM_START - PCI_MEM_END)


struct pci_res {
    unsigned int size;
    unsigned char bar;
    unsigned char devfn;
};

/*  The configuration access functions uses the DMA functionality of the
 *  AT697 pci controller to be able access all slots
 */

static int
BSP_pci_read_config_dword(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned int *val) {

    volatile unsigned int data;

    if (offset & 3) return PCIBIOS_BAD_REGISTER_NUMBER;

    pcic->pciitp = 0xff; /* clear interrupts */

    pcic->pcisa = (  1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f);
    pcic->pcidma = 0xa01;
    pcic->pcidmaa = (unsigned int) &data;

    while (pcic->pciitp == 0)
        ;

    pcic->pciitp = 0xff; /* clear interrupts */

    if (pcic->pcisc & 0x20000000)  { /* Master Abort */
        pcic->pcisc |= 0x20000000;
        *val = 0xffffffff;
    }
    else
        *val = data;

    DBG("pci_read - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n", bus, slot, function, offset,  (1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f), *val);

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_read_config_word(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned short *val) {
    unsigned int v;

    if (offset & 1) return PCIBIOS_BAD_REGISTER_NUMBER;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);
    *val = 0xffff & (v >> (8*(offset & 3)));

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_read_config_byte(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned char *val) {
    unsigned int v;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);

    *val = 0xff & (v >> (8*(offset & 3)));

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_write_config_dword(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned int val) {

    if (offset & 3) return PCIBIOS_BAD_REGISTER_NUMBER;

    pcic->pciitp = 0xff; /* clear interrupts */

    pcic->pcisa = (  1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f);
    pcic->pcidma = 0xb01;
    pcic->pcidmaa = (unsigned int) &val;

    while (pcic->pciitp == 0)
        ;

    if (pcic->pcisc & 0x20000000)  { /* Master Abort */
        pcic->pcisc |= 0x20000000;
    }

    pcic->pciitp = 0xff; /* clear interrupts */

/*    DBG("pci write - bus: %d, dev: %d, fn: %d, off: %d => addr: %x, val: %x\n", bus, slot, function, offset, (1<<(11+slot) ) | ((function & 7)<<8) |  (offset&0x3f), val); */

    return PCIBIOS_SUCCESSFUL;
}


static int
BSP_pci_write_config_word(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned short val) {
    unsigned int v;

    if (offset & 1) return PCIBIOS_BAD_REGISTER_NUMBER;

    pci_read_config_dword(bus, slot, function, offset&~3, &v);

    v = (v & ~(0xffff << (8*(offset&3)))) | ((0xffff&val) << (8*(offset&3)));

    return pci_write_config_dword(bus, slot, function, offset&~3, v);
}


static int
BSP_pci_write_config_byte(unsigned char bus, unsigned char slot, unsigned char function, unsigned char offset, unsigned char val) {
    unsigned int v;

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


void init_at697_pci(void) {

    /* Reset */
    pcic->pciic = 0xffffffff;

    /* Map system RAM at pci address 0x40000000 and system SDRAM to pci address 0x60000000  */
    pcic->mbar1  = 0x40000000;
    pcic->mbar2  = 0x60000000;
    pcic->pcitpa = 0x40006000;

    /* Enable PCI master and target memory command response  */
    pcic->pcisc |= 0x40 | 0x6;

    /* Set latency timer to 64 */
    pcic->pcibhlc = 0x00004000;

    /* Set Inititator configuration so that AHB slave accesses generate memory read/write commands */
    pcic->pciic = 0x41;

    pcic->pciite = 0xff;

}

/* May not pass a 1k boundary */
int dma_from_pci_1k(unsigned int addr, unsigned int paddr, unsigned char len) {

    int retval = 0;

    if (addr & 3) {
        return -1;
    }

    pcic->pciitp = 0xff; /* clear interrupts */

    pcic->pcisa = paddr;
    pcic->pcidma = 0xc00 | len;
    pcic->pcidmaa = addr;

    while (pcic->pciitp == 0)
        ;

    if (pcic->pciitp & 0x7F) {
        retval = -1;
    }

    pcic->pciitp = 0xff; /* clear interrupts */

    if (pcic->pcisc & 0x20000000)  { /* Master Abort */
        pcic->pcisc |= 0x20000000;
        retval = -1;
    }

    return retval;
}

/* May not pass a 1k boundary */
int dma_to_pci_1k(unsigned int addr, unsigned int paddr, unsigned char len) {

    int retval = 0;

    if (addr & 3) return -1;

    pcic->pciitp = 0xff; /* clear interrupts */

    pcic->pcisa = paddr;
    pcic->pcidma = 0x700 | len;
    pcic->pcidmaa = addr;

    while (pcic->pciitp == 0)
        ;

    if (pcic->pciitp & 0x7F) retval = -1;

    pcic->pciitp = 0xff; /* clear interrupts */

    if (pcic->pcisc & 0x20000000)  { /* Master Abort */
        pcic->pcisc |= 0x20000000;
        retval =  -1;
    }

    return retval;
}

/* Transfer len number of words from addr to paddr */
int dma_to_pci(unsigned int addr, unsigned int paddr, unsigned int len) {

    int tmp_len;

    /* Align to 1k boundary */
    tmp_len = ((addr + 1024)  & 0xfffffc00) - addr;

    tmp_len = (tmp_len/4 < len) ? tmp_len : (len*4);

    if (dma_to_pci_1k(addr, paddr, tmp_len/4) < 0)
        return -1;

    addr += tmp_len;
    paddr += tmp_len;
    len -= tmp_len/4;

    /* Transfer all 1k blocks */
    while (len >= 128) {

        if (dma_to_pci_1k(addr, paddr, 128) < 0)
            return -1;

        addr += 512;
        paddr += 512;
        len -= 128;

    }

    /* Transfer last words */
    if (len) return dma_to_pci_1k(addr, paddr, len);

    return 0;
}

/* Transfer len number of words from paddr to addr */
int dma_from_pci(unsigned int addr, unsigned int paddr, unsigned int len) {

    int tmp_len;

    /* Align to 1k boundary */
    tmp_len = ((addr + 1024)  & 0xfffffc00) - addr;

    tmp_len = (tmp_len/4 < len) ? tmp_len : (len*4);

    if (dma_from_pci_1k(addr, paddr, tmp_len/4) < 0)
        return -1;

    addr += tmp_len;
    paddr += tmp_len;
    len -= tmp_len/4;

    /* Transfer all 1k blocks */
    while (len >= 128) {

        if (dma_from_pci_1k(addr, paddr, 128) < 0)
            return -1;
        addr += 512;
        paddr += 512;
        len -= 128;

    }

    /* Transfer last words */
    if (len) return dma_from_pci_1k(addr, paddr, len);

    return 0;
}

void pci_mem_enable(unsigned char bus, unsigned char slot, unsigned char function) {
    unsigned int data;

    pci_read_config_dword(0, slot, function, PCI_COMMAND, &data);
    pci_write_config_dword(0, slot, function, PCI_COMMAND, data | PCI_COMMAND_MEMORY);

}

void pci_master_enable(unsigned char bus, unsigned char slot, unsigned char function) {
    unsigned int data;

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
 * NOTE that it only allocates PCI memory space devices. IO spaces are not enabled.
 * Also, it does not handle pci-pci bridges. They are left disabled.
 *
 *
*/
void pci_allocate_resources(void) {

    unsigned int slot, numfuncs, func, id, pos, size, tmp, i, swapped, addr, dev, fn;
    unsigned char header;
    struct pci_res **res;

    res = (struct pci_res **) malloc(sizeof(struct pci_res *)*32*8*6);

    for (i = 0; i < 32*8*6; i++) {
        res[i] = (struct pci_res *) malloc(sizeof(struct pci_res));
        res[i]->size = 0;
        res[i]->devfn = i;
    }

    for(slot = 0; slot< PCI_MAX_DEVICES; slot++) {

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

                if (size == 0 || size == 0xffffffff || (size & 0xff) != 0) {
                    pci_write_config_dword(0, slot, func, PCI_BASE_ADDRESS_0 + (pos<<2), 0);
                    continue;
                }

                else {
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
    for (slot = 0; slot < PCI_MAX_DEVICES; slot++) {

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

                    if (tmp != 0 && tmp != 0xffffffff && (tmp & 0xff) == 0) {

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







/*
 * This routine determines the maximum bus number in the system
 */
int init_pci(void)
{
    unsigned char ucSlotNumber, ucFnNumber, ucNumFuncs;
    unsigned char ucHeader;
    unsigned char ucMaxSubordinate;
    unsigned int  ulClass, ulDeviceID;

    init_at697_pci();
    pci_allocate_resources();

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
