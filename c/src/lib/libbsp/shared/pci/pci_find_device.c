/**
 * @file
 *
 * This file implements a BSP independent version of pci_find_device().
 */

/*
 * The software in this file was based upon the pci_find_device()
 * implementation provided by  Till Straumann under the following terms.
 * That implementation had been copied to multiple BSPs. This implementation
 * is BSP independent and follows RTEMS Project coding guidelines.
 *
 * COPYRIGHT (c) 2016.
 * On-Line Applications Research Corporation (OAR).
 *
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


#include <rtems/pci.h>
#include <rtems/bspIo.h>
#include <inttypes.h>
#include <stdio.h>

/*
 *  Public methods from this file
 */
void pci_dump(FILE *f);

/*
 * These are used to construct the handle returned by pci_find_device()
 * but the caller does not need to know how to decode them.
 */
#define PCIB_DEVSIG_BUS(x) (((x)>>8) &0xff)
#define PCIB_DEVSIG_DEV(x) (((x)>>3) & 0x1f)
#define PCIB_DEVSIG_FUNC(x) ((x) & 0x7)
#define PCIB_DEVSIG_MAKE(b,d,f) ((b<<8)|(d<<3)|(f))

/*
 * Function pointer to helper function called during bus iteration.
 */
typedef int (*pci_scan_helper_t)(
  int   bus,
  int   dev,
  int   fun,
  void *uarg
);

/*
 *
 */
static uint32_t pci_scan(
  uint32_t           handle,
  pci_scan_helper_t  helper,
  void              *uarg
);

typedef struct {
  uint16_t   vendor_id;
  uint16_t   device_id;
  int        instance;
  uint8_t    bus;
  uint8_t    device;
  uint8_t    function;
} pci_scan_arg_t;

static int find_dev_helper(
  int   bus,
  int   device,
  int   function,
  void *uarg
)
{
  pci_scan_arg_t  *scan = uarg;
  uint16_t         vendor_tmp;
  uint16_t         device_tmp;

  pci_read_config_word(bus, device, function, PCI_VENDOR_ID, &vendor_tmp);
  if (scan->vendor_id == vendor_tmp) {
    pci_read_config_word(bus, device, function, PCI_DEVICE_ID, &device_tmp);
    if (scan->device_id == device_tmp && scan->instance-- == 0) {
      scan->bus      = bus;
      scan->device   = device;
      scan->function = function;

      return 1;
    }
  }
  return 0;
}

int pci_find_device(
  uint16_t vendorid,
  uint16_t deviceid,
  int      instance,
  int     *bus,
  int     *device,
  int     *function
)
{
  pci_scan_arg_t  scan;

  scan.instance   = instance;
  scan.vendor_id  = vendorid;
  scan.device_id  = deviceid;

  if ( pci_scan(0, find_dev_helper, (void*)&scan) != 0 ) {
    *bus      = scan.bus;
    *device   = scan.device;
    *function = scan.function;
    return 0;
  }
  return -1;
}

static int dump_dev_helper(
  int   bus,
  int   device,
  int   function,
  void *arg
)
{
  uint16_t  vendor_id;
  uint16_t  device_id;
  uint16_t  cmd;
  uint16_t  status;
  uint32_t  base0;
  uint32_t  base1;
  uint8_t   irq_pin;
  uint8_t   int_line;
  FILE     *fp = arg;

  pci_read_config_word (bus, device, function, PCI_VENDOR_ID,      &vendor_id);
  pci_read_config_word (bus, device, function, PCI_DEVICE_ID,      &device_id);
  pci_read_config_word (bus, device, function, PCI_COMMAND,        &cmd);
  pci_read_config_word (bus, device, function, PCI_STATUS,         &status);
  pci_read_config_dword(bus, device, function, PCI_BASE_ADDRESS_0, &base0);
  pci_read_config_dword(bus, device, function, PCI_BASE_ADDRESS_1, &base1);
  pci_read_config_byte (bus, device, function, PCI_INTERRUPT_PIN,  &irq_pin);
  pci_read_config_byte (bus, device, function, PCI_INTERRUPT_LINE, &int_line);

  fprintf(
    fp,
    "%3d:0x%02x:%d   0x%04x:0x%04x 0x%04x 0x%04x 0x%08" PRIx32 " 0x%08" PRIx32 "   %d %3d(0x%02x)\n",
    bus,
    device,
    function,
    vendor_id,
    device_id,
    cmd,
    status,
    base0,
    base1,
    irq_pin,
    int_line,
    int_line
  );
  return 0;
}

void pci_dump(
  FILE *fp
)
{
  if ( !fp )
    fp = stdout;
  fprintf(
    fp,
    "BUS:SLOT:FUN VENDOR:DEV_ID   CMD  STAT   BASE_ADDR0 BASE_ADDR1 INTn IRQ_LINE\n"
  );
  pci_scan(0, dump_dev_helper, fp);
}

static uint32_t pci_scan(
  uint32_t           handle,
  pci_scan_helper_t  helper,
  void               *arg
)
{
  uint32_t vendor;
  uint8_t  bus;
  uint8_t  dev;
  uint8_t  fun;
  uint8_t  hd;

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
        if ((PCI_MAX_DEVICES-1 == dev) && (PCI_MAX_FUNCTIONS-1 == fun) )
          break;

        (void)pci_read_config_dword(bus, dev, 0, PCI_VENDOR_ID, &vendor);
        if (PCI_INVALID_VENDORDEVICEID == vendor)
          continue;

        if ( fun == 0 ) {
          pci_read_config_byte(bus,dev, 0,  PCI_HEADER_TYPE, &hd);
          hd = (hd & PCI_HEADER_TYPE_MULTI_FUNCTION ? PCI_MAX_FUNCTIONS : 1);
        }

        (void)pci_read_config_dword(bus, dev, fun, PCI_VENDOR_ID, &vendor);
        if (PCI_INVALID_VENDORDEVICEID == vendor)
          continue;
        #ifdef PCI_DEBUG
          fprintf(
            stderr,
            "pci_scan: found 0x%08" PRIx32 " at %d/x%02x/%d\n", vendor, bus, dev, fun
          );
        #endif
        if ( (*helper)(bus, dev, fun, arg) > 0 ) {
          if ( ++fun >= hd ) {
            fun = 0;
            if ( ++dev >= PCI_MAX_DEVICES ) {
              dev = 0;
              bus++;
            }
          }
          return PCIB_DEVSIG_MAKE(bus,dev,fun);
        }
      }
    }
  }
  return 0;
}
