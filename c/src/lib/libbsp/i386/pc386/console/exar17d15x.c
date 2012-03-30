/**
 * @file
 *
 * @brief Driver for Exar XR17D15x Multiport UARTs
 *
 * This driver supports 2, 4 or 8 port Exar parts which are NS16550
 * compatible.
 */

/*
 *  COPYRIGHT (c) 1989-2012.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#include <bsp.h>
#include <termios.h>
#include <stdio.h>
#include <stdlib.h>

#include <rtems/termiostypes.h>
#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include <rtems/bspIo.h>
#include <rtems/pci.h>
#include <bsp/exar17d15x.h>
#include "../../../shared/console_private.h"

#define MAX_BOARDS 4

/*
 *  This is the rate for the clock internal to the parts.
 */
#define EXAR_CLOCK_RATE  (921600*16)

/*
 *  Supported PCI Ids
 */
#define PCI_VENDOR_ID_EXAR 0x13A8
#define PCI_VENDOR_ID_EXAR_XR17D158 0x0158
#define PCI_VENDOR_ID_EXAR_XR17D154 0x0154
#define PCI_VENDOR_ID_EXAR_XR17D152 0x0152

/*
 * Structure to manage each instance found.
 */
typedef struct {
  uint16_t  vendor;
  uint16_t  device;
  uint8_t   ports;
} exar_parts_t;

static exar_parts_t Supported[] = {
  { PCI_VENDOR_ID_EXAR, PCI_VENDOR_ID_EXAR_XR17D158, 8 },
  { PCI_VENDOR_ID_EXAR, PCI_VENDOR_ID_EXAR_XR17D154, 4 },
  { PCI_VENDOR_ID_EXAR, PCI_VENDOR_ID_EXAR_XR17D152, 2 },
  { 0, 0, 0 }
};

/*
 * Information saved from PCI scan
 */
typedef struct {
  bool      found;
  uint32_t  base;
  uint8_t   irq;
  uint8_t   bus;
  uint8_t   slot;
  uint8_t   ports;
} exar17d15x_conf_t;

/*
 *  Register Access Routines
 */
static uint8_t xr17d15x_get_register(uint32_t addr, uint8_t i)
{
  uint8_t          val = 0;
  volatile uint8_t *reg = (volatile uint8_t *)(addr + i);

  val = *reg;
  // printk( "RD %p -> 0x%02x\n", reg, val );
  return val;
}

static void xr17d15x_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint8_t *reg = (volatile uint8_t *)(addr + i);

  // printk( "WR %p <- 0x%02x\n", reg, val );
  *reg = val;
}

rtems_device_driver exar17d15x_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  // int  pbus, pdev, pfun;
  exar17d15x_conf_t  conf[MAX_BOARDS];
  int              boards = 0;
  int              b = 0;
  int              p;
  console_tbl     *ports;
  console_tbl     *port_p;
  int              pbus;
  int              pdev;
  int              pfun;
  int              status;
  int              instance;
  int              i;
  int              total_ports = 0;

  for ( b=0 ; b<MAX_BOARDS ; b++ ) {
    conf[b].found = false;
  }

  /*
   *  Scan for Serial port boards
   *
   *  NOTE: There appear to be Exar parts with 2 and 4 ports which would
   *        be easy to support.  Just change the hard-coded 8 ports per
   *        board to variable and adjust.
   *
   *  NOTE: There are likely other board vendors which could be supported
   *        by this.
   */
  for ( instance=0 ; instance < MAX_BOARDS ; instance++ ) {

    for ( i=0 ; Supported[i].ports != 0 ; i++ ) {
      status = pci_find_device(
        Supported[i].vendor,
        Supported[i].device,
        instance,
        &pbus,
        &pdev,
        &pfun
      );
      if ( status == PCIB_ERR_SUCCESS ) {
        boards++;
        conf[instance].found = true;
        conf[instance].ports = Supported[i].ports;
        total_ports += conf[instance].ports;
        break;
      }
    }

    if ( status != PCIB_ERR_SUCCESS )
      continue;

    pci_read_config_byte(
      pbus,
      pdev,
      pfun,
      PCI_INTERRUPT_LINE,
      &conf[instance].irq
    );
    pci_read_config_dword(
      pbus,
      pdev,
      pfun,
      PCI_BASE_ADDRESS_0,
      &conf[instance].base
    );
    printk(
      "Found Exar 17D15x %d at 0x%08x IRQ %d with %d ports\n",
      instance,
      conf[instance].base,
      conf[instance].irq,
      conf[instance].ports
    );
  }

  /*
   *  Now allocate array of device structures and fill them in
   */
  ports = calloc( total_ports, sizeof( console_tbl ) );
  port_p = ports;
  for ( b=0 ; b<MAX_BOARDS ; b++ ) {
    if ( conf[b].found == false )
      continue;
    for ( p=0 ; p<conf[b].ports ; p++ ) {
      char name[32];

      sprintf( name, "/dev/exar17d15x_%d_%d", b, p );
      //printk("Found %s\n", name );
      port_p->sDeviceName   = strdup( name );
      port_p->deviceType    = SERIAL_NS16550;
      #if 1
        port_p->pDeviceFns    = &ns16550_fns_polled;
      #else
        port_p->pDeviceFns    = &ns16550_fns;
      #endif

      port_p->deviceProbe   = NULL;
      port_p->pDeviceFlow   = NULL;
      port_p->ulMargin      = 16;
      port_p->ulHysteresis  = 8;
      port_p->pDeviceParams = (void *) 9600;
      port_p->ulCtrlPort1   = conf[b].base + (p * 0x0200);
      port_p->ulCtrlPort2   = 0;                   /* NA */
      port_p->ulDataPort    = 0;                   /* NA */
      port_p->getRegister   = xr17d15x_get_register;
      port_p->setRegister   = xr17d15x_set_register;
      port_p->getData       = NULL;                /* NA */
      port_p->setData       = NULL;                /* NA */
      port_p->ulClock       = EXAR_CLOCK_RATE;
      port_p->ulIntVector   = conf[b].irq;

      port_p++;
    }  /* end ports */
  }    /* end boards */

  /*
   *  Register the devices
   */
  if ( boards )
    console_register_devices( ports, total_ports );

  return RTEMS_SUCCESSFUL;
}
