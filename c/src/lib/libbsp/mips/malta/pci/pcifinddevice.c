/**
 *  @file
 *
 *  This file was copied from the powerpc and modified slightly.
 *  I think this file could be made generic and put in a general pci
 *  area.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/* find a particular PCI device
 * (we assume, the firmware configured the PCI bus[es] for us)
 */

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2001,
 *      Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * This software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 *      under Contract DE-AC03-76SFO0515 with the Department of Energy.
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

#define PCI_DEBUG

#include <inttypes.h>
#include <bsp/pci.h>
#include <rtems/bspIo.h>
#include <stdio.h>

/* Stolen from i386... */

/*
 * Make device signature from bus number, device number and function
 * number
 */
#define PCIB_DEVSIG_MAKE(b,d,f) ((b<<8)|(d<<3)|(f))

/*
 * Extract various parts from device signature
 */
#define PCIB_DEVSIG_BUS(x) (((x)>>8) &0xff)
#define PCIB_DEVSIG_DEV(x) (((x)>>3) & 0x1f)
#define PCIB_DEVSIG_FUNC(x) ((x) & 0x7)

typedef struct {
  unsigned short  vid,did;
  int             inst;
} fd_arg;

static int find_dev_cb(
  int bus,
  int dev,
  int fun,
  void *uarg
)
{
  fd_arg         *a = uarg;
  unsigned short  vendor;
  unsigned short  device;

  pci_read_config_word(bus,dev,fun,PCI_VENDOR_ID,&vendor);
  pci_read_config_word(bus,dev,fun,PCI_DEVICE_ID,&device);
  printk("  FOUND: Vendor 0x%x Device: 0x%x\n", vendor, device);

  if (a->vid == vendor) {
    if (a->did == device && 0 == a->inst-- ) {
      a->inst = PCIB_DEVSIG_MAKE( bus, dev, fun );
      return 1;
    }
  }
  return 0;
}

int  pci_find_device(
  unsigned short vendorid,
  unsigned short deviceid,
  int instance,
  int *pbus,
  int *pdev,
  int *pfun
)
{
  fd_arg a;
  void   *h;

  a.vid  = vendorid;
  a.did  = deviceid;
  a.inst = instance;

  if ( (h = BSP_pciScan(0, find_dev_cb, (void*)&a)) ) {
    *pbus = PCIB_DEVSIG_BUS(  a.inst );
    *pdev = PCIB_DEVSIG_DEV(  a.inst );
    *pfun = PCIB_DEVSIG_FUNC( a.inst );
    return 0;
  }

  return -1;
}

static int dump_dev_cb(
  int  bus,
  int  dev,
  int  fun,
  void *uarg
)
{
  uint16_t vi,di;
  uint16_t cd,st;
  uint32_t b1,b2;
  uint8_t  il,ip;
  FILE     *f = uarg;

  pci_read_config_word (bus, dev, fun, PCI_VENDOR_ID,      &vi);
  pci_read_config_word (bus, dev, fun, PCI_DEVICE_ID,      &di);
  pci_read_config_word (bus, dev, fun, PCI_COMMAND,        &cd);
  pci_read_config_word (bus, dev, fun, PCI_STATUS,         &st);
  pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_0, &b1);
  pci_read_config_dword(bus, dev, fun, PCI_BASE_ADDRESS_1, &b2);
  pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_LINE, &il);
  pci_read_config_byte (bus, dev, fun, PCI_INTERRUPT_PIN,  &ip);

  fprintf(
    f,
    "%3d:0x%02x:%d    0x%04x-0x%04x:  0x%04x 0x%04x 0x%08" PRIx32
    " 0x%08" PRIx32 "       %d -> %3d (=0x%02x)\n",
    bus,
    dev,
    fun,
    vi,
    di,
    cd,
    st,
    b1,
    b2,
    ip,
    il,
    il
  );
  return 0;
}

void BSP_pciConfigDump(FILE *f)
{
  if ( !f )
    f = stdout;
  fprintf(
    f,
    "BUS:SLOT:FUN  VENDOR-DEV_ID: COMMAND STATUS BASE_ADDR0 BASE_ADDR1 "
    "IRQ_PIN -> IRQ_LINE\n"
  );
  BSP_pciScan(0, dump_dev_cb, f);
}

BSP_PciScanHandle BSP_pciScan(
  BSP_PciScanHandle handle,
  BSP_PciScannerCb cb,
  void *uarg
) {

  uint32_t d;
  unsigned char bus,dev,fun,hd;

  bus = PCIB_DEVSIG_BUS(  (unsigned long)handle );
  dev = PCIB_DEVSIG_DEV(  (unsigned long)handle );
  fun = PCIB_DEVSIG_FUNC( (unsigned long)handle );

  hd = fun > 0 ? PCI_MAX_FUNCTIONS : 1;

  for (; bus<pci_bus_count(); bus++, dev=0) {
    for (; dev<PCI_MAX_DEVICES; dev++, fun=0) {
      for (; fun<hd; fun++) {
        /*
         * The last devfn id/slot is special; must skip it
         */
        if (PCI_MAX_DEVICES-1==dev && PCI_MAX_FUNCTIONS-1 == fun)
          break;

        (void)pci_read_config_dword(bus,dev,0,PCI_VENDOR_ID,&d);
        if (PCI_INVALID_VENDORDEVICEID == d)
          continue;
        #ifdef PCI_DEBUG
          printk(
            "BSP_pciScan: Read _config_dword bus %d "
            "dev 0x%x value 0x%2x\n",
            bus,
            dev,
            d
          );
        #endif

        if ( 0 == fun ) {
          pci_read_config_byte(bus,dev,0, PCI_HEADER_TYPE, &hd);
          #ifdef PCI_DEBUG
            printk(
              "BSP_pciScan: Read _config_byte bus %d dev 0x%x "
              "fun %d value 0x%2x\n",
              bus,
              dev,
              fun,
              d
            );
          #endif
          hd = (hd & PCI_HEADER_TYPE_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
        }

        (void)pci_read_config_dword(bus,dev,fun,PCI_VENDOR_ID,&d);
        if (PCI_INVALID_VENDORDEVICEID == d)
          continue;
        #ifdef PCI_DEBUG
          printk("BSP_pciScan: found 0x%08x at %d/x%02x/%d\n",d,bus,dev,fun);
        #endif
        if ( cb(bus,dev,fun,uarg) > 0 ) {
          if ( ++fun >= hd ) {
            fun = 0;
            if ( ++dev >= PCI_MAX_DEVICES ) {
              dev = 0;
              bus++;
            }
          }
          #ifdef PCI_DEBUG
            printk(
              "BSP_pciScan: Going to Return: bus %d dev 0x%x fun %d\n",
              bus,
              dev,
              fun
            );
          #endif
          return (void*) PCIB_DEVSIG_MAKE(bus,dev,fun);
        }
      }
    }
  }
  return 0;
}
