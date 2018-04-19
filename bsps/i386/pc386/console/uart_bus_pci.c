/*
 * This file was brought over from FreeBSD for the PCI device table.
 * The code for using the table is RTEMS specific is also under the
 * FreeBSD license.
 *
 * COPYRIGHT (c) 1989-2012.
 * On-Line Applications Research Corporation (OAR).
 */

/*-
 * Copyright (c) 2006 Marcel Moolenaar
 * Copyright (c) 2001 M. Warner Losh
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifdef __rtems__
#include <stddef.h>
#include <stdint.h>
#include <i386_io.h>
#else
#include <sys/cdefs.h>
__FBSDID("$FreeBSD$");

#include <sys/param.h>
#include <sys/systm.h>
#include <sys/bus.h>
#include <sys/conf.h>
#include <sys/kernel.h>
#include <sys/module.h>
#include <machine/bus.h>
#include <sys/rman.h>
#include <machine/resource.h>

#include <dev/pci/pcivar.h>

#include <dev/uart/uart.h>
#include <dev/uart/uart_bus.h>

#endif

#define	DEFAULT_RCLK	1843200

#ifndef __rtems__
static int uart_pci_probe(device_t dev);

static device_method_t uart_pci_methods[] = {
	/* Device interface */
	DEVMETHOD(device_probe,		uart_pci_probe),
	DEVMETHOD(device_attach,	uart_bus_attach),
	DEVMETHOD(device_detach,	uart_bus_detach),
	DEVMETHOD(device_resume,	uart_bus_resume),
	DEVMETHOD_END
};

static driver_t uart_pci_driver = {
	uart_driver_name,
	uart_pci_methods,
	sizeof(struct uart_softc),
};
#endif

struct pci_id {
	uint16_t	vendor;
	uint16_t	device;
	uint16_t	subven;
	uint16_t	subdev;
	const char	*desc;
	int		rid;
	int		rclk;
	int		regshft;
};

static const struct pci_id pci_ns8250_ids[] = {
{ 0x1028, 0x0008, 0xffff, 0, "Dell Remote Access Card III", 0x14,
	128 * DEFAULT_RCLK },
{ 0x1028, 0x0012, 0xffff, 0, "Dell RAC 4 Daughter Card Virtual UART", 0x14,
	128 * DEFAULT_RCLK },
{ 0x1033, 0x0074, 0x1033, 0x8014, "NEC RCV56ACF 56k Voice Modem", 0x10 },
{ 0x1033, 0x007d, 0x1033, 0x8012, "NEC RS232C", 0x10 },
{ 0x103c, 0x1048, 0x103c, 0x1227, "HP Diva Serial [GSP] UART - Powerbar SP2",
	0x10 },
{ 0x103c, 0x1048, 0x103c, 0x1301, "HP Diva RMP3", 0x14 },
{ 0x103c, 0x1290, 0xffff, 0, "HP Auxiliary Diva Serial Port", 0x18 },
{ 0x103c, 0x3301, 0xffff, 0, "HP iLO serial port", 0x10 },
{ 0x11c1, 0x0480, 0xffff, 0, "Agere Systems Venus Modem (V90, 56KFlex)", 0x14 },
{ 0x115d, 0x0103, 0xffff, 0, "Xircom Cardbus Ethernet + 56k Modem", 0x10 },
{ 0x1282, 0x6585, 0xffff, 0, "Davicom 56PDV PCI Modem", 0x10 },
{ 0x12b9, 0x1008, 0xffff, 0, "3Com 56K FaxModem Model 5610", 0x10 },
{ 0x131f, 0x1000, 0xffff, 0, "Siig CyberSerial (1-port) 16550", 0x18 },
{ 0x131f, 0x1001, 0xffff, 0, "Siig CyberSerial (1-port) 16650", 0x18 },
{ 0x131f, 0x1002, 0xffff, 0, "Siig CyberSerial (1-port) 16850", 0x18 },
{ 0x131f, 0x2000, 0xffff, 0, "Siig CyberSerial (1-port) 16550", 0x10 },
{ 0x131f, 0x2001, 0xffff, 0, "Siig CyberSerial (1-port) 16650", 0x10 },
{ 0x131f, 0x2002, 0xffff, 0, "Siig CyberSerial (1-port) 16850", 0x10 },
{ 0x135c, 0x0190, 0xffff, 0, "Quatech SSCLP-100", 0x18 },
{ 0x135c, 0x01c0, 0xffff, 0, "Quatech SSCLP-200/300", 0x18 },
{ 0x135e, 0x7101, 0xffff, 0, "Sealevel Systems Single Port RS-232/422/485/530",
	0x18 },
{ 0x1407, 0x0110, 0xffff, 0, "Lava Computer mfg DSerial-PCI Port A", 0x10 },
{ 0x1407, 0x0111, 0xffff, 0, "Lava Computer mfg DSerial-PCI Port B", 0x10 },
{ 0x1407, 0x0510, 0xffff, 0, "Lava SP Serial 550 PCI", 0x10 },
{ 0x1409, 0x7168, 0x1409, 0x4025, "Timedia Technology Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x1409, 0x7168, 0x1409, 0x4027, "Timedia Technology Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x1409, 0x7168, 0x1409, 0x4028, "Timedia Technology Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x1409, 0x7168, 0x1409, 0x5025, "Timedia Technology Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x1409, 0x7168, 0x1409, 0x5027, "Timedia Technology Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x1415, 0x950b, 0xffff, 0, "Oxford Semiconductor OXCB950 Cardbus 16950 UART",
	0x10, 16384000 },
{ 0x1415, 0xc120, 0xffff, 0, "Oxford Semiconductor OXPCIe952 PCIe 16950 UART",
	0x10 },
{ 0x14e4, 0x4344, 0xffff, 0, "Sony Ericsson GC89 PC Card", 0x10},
{ 0x151f, 0x0000, 0xffff, 0, "TOPIC Semiconductor TP560 56k modem", 0x10 },
{ 0x1fd4, 0x1999, 0x1fd4, 0x0001, "Sunix SER5xxxx Serial Port", 0x10,
	8 * DEFAULT_RCLK },
{ 0x8086, 0x0f0a, 0xffff, 0, "Intel ValleyView LPIO1 HSUART#1", 0x10,
	24 * DEFAULT_RCLK, 2 },
{ 0x8086, 0x0f0c, 0xffff, 0, "Intel ValleyView LPIO1 HSUART#2", 0x10,
	24 * DEFAULT_RCLK, 2 },
{ 0x8086, 0x1c3d, 0xffff, 0, "Intel AMT - KT Controller", 0x10 },
{ 0x8086, 0x1d3d, 0xffff, 0, "Intel C600/X79 Series Chipset KT Controller", 0x10 },
{ 0x8086, 0x2a07, 0xffff, 0, "Intel AMT - PM965/GM965 KT Controller", 0x10 },
{ 0x8086, 0x2a47, 0xffff, 0, "Mobile 4 Series Chipset KT Controller", 0x10 },
{ 0x8086, 0x2e17, 0xffff, 0, "4 Series Chipset Serial KT Controller", 0x10 },
{ 0x8086, 0x3b67, 0xffff, 0, "5 Series/3400 Series Chipset KT Controller",
	0x10 },
{ 0x8086, 0x8811, 0xffff, 0, "Intel EG20T Serial Port 0", 0x10 },
{ 0x8086, 0x8812, 0xffff, 0, "Intel EG20T Serial Port 1", 0x10 },
{ 0x8086, 0x8813, 0xffff, 0, "Intel EG20T Serial Port 2", 0x10 },
{ 0x8086, 0x8814, 0xffff, 0, "Intel EG20T Serial Port 3", 0x10 },
{ 0x8086, 0x8c3d, 0xffff, 0, "Intel Lynx Point KT Controller", 0x10 },
{ 0x8086, 0x8cbd, 0xffff, 0, "Intel Wildcat Point KT Controller", 0x10 },
{ 0x8086, 0x9c3d, 0xffff, 0, "Intel Lynx Point-LP HECI KT", 0x10 },
{ 0x9710, 0x9820, 0x1000, 1, "NetMos NM9820 Serial Port", 0x10 },
{ 0x9710, 0x9835, 0x1000, 1, "NetMos NM9835 Serial Port", 0x10 },
{ 0x9710, 0x9865, 0xa000, 0x1000, "NetMos NM9865 Serial Port", 0x10 },
{ 0x9710, 0x9900, 0xa000, 0x1000,
	"MosChip MCS9900 PCIe to Peripheral Controller", 0x10 },
{ 0x9710, 0x9901, 0xa000, 0x1000,
	"MosChip MCS9901 PCIe to Peripheral Controller", 0x10 },
{ 0x9710, 0x9904, 0xa000, 0x1000,
	"MosChip MCS9904 PCIe to Peripheral Controller", 0x10 },
{ 0x9710, 0x9922, 0xa000, 0x1000,
	"MosChip MCS9922 PCIe to Peripheral Controller", 0x10 },
{ 0xdeaf, 0x9051, 0xffff, 0, "Middle Digital PC Weasel Serial Port", 0x10 },
{ 0xffff, 0, 0xffff, 0, NULL, 0, 0}
};

#ifndef __rtems__
const static struct pci_id *
uart_pci_match(device_t dev, const struct pci_id *id)
{
	uint16_t device, subdev, subven, vendor;

	vendor = pci_get_vendor(dev);
	device = pci_get_device(dev);
	while (id->vendor != 0xffff &&
	    (id->vendor != vendor || id->device != device))
		id++;
	if (id->vendor == 0xffff)
		return (NULL);
	if (id->subven == 0xffff)
		return (id);
	subven = pci_get_subvendor(dev);
	subdev = pci_get_subdevice(dev);
	while (id->vendor == vendor && id->device == device &&
	    (id->subven != subven || id->subdev != subdev))
		id++;
	return ((id->vendor == vendor && id->device == device) ? id : NULL);
}

static int
uart_pci_probe(device_t dev)
{
	struct uart_softc *sc;
	const struct pci_id *id;
	int result;

	sc = device_get_softc(dev);

	id = uart_pci_match(dev, pci_ns8250_ids);
	if (id != NULL) {
		sc->sc_class = &uart_ns8250_class;
		goto match;
	}
	/* Add checks for non-ns8250 IDs here. */
	return (ENXIO);

 match:
	result = uart_bus_probe(dev, id->regshft, id->rclk, id->rid, 0);
	/* Bail out on error. */
	if (result > 0)
		return (result);
	/* Set/override the device description. */
	if (id->desc)
		device_set_desc(dev, id->desc);
	return (result);
}

DRIVER_MODULE(uart, pci, uart_pci_driver, uart_devclass, NULL, NULL);
#endif

#ifdef __rtems__

#include <bsp.h>
#include <bsp/bspimpl.h>

#include <stdio.h>
#include <stdlib.h>

#include <libchip/serial.h>
#include <libchip/ns16550.h>
#include <rtems/bspIo.h>
#include <rtems/pci.h>
#include "../../shared/dev/serial/legacy-console.h"

#define MAX_BOARDS 4

/*
 * Information saved from PCI scan
 */
typedef struct {
  bool        found;
  const char* desc;
  uint32_t    base;
  uint8_t     irq;
  uint8_t     bus;
  uint8_t     slot;
  int         ports;
  uint32_t    clock;
} port_instance_conf_t;

/*
 *  Memory Mapped Register Access Routines
 */

#define UART_PCI_IO (0)

static uint8_t pci_ns16550_mem_get_register(uint32_t addr, uint8_t i)
{
  uint8_t          val = 0;
  volatile uint32_t *reg = (volatile uint32_t *)(addr + (i*4));
  val = *reg;
  if (UART_PCI_IO)
    printk( "RD(%p -> 0x%02x) ", reg, val );
  return val;
}

static void pci_ns16550_mem_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  volatile uint32_t *reg = (volatile uint32_t *)(addr + (i*4));
  if (UART_PCI_IO)
    printk( "WR(%p <- 0x%02x) ", reg, val );
  *reg = val;
}

/*
 *  IO Register Access Routines
 */
static uint8_t pci_ns16550_io_get_register(uint32_t addr, uint8_t i)
{
  uint8_t val = rtems_inb(addr + i);
  if (UART_PCI_IO)
    printk( "RD(%p -> 0x%02x) ", (void*) addr + i, val );
  return val;
}

static void pci_ns16550_io_set_register(uint32_t addr, uint8_t i, uint8_t val)
{
  if (UART_PCI_IO)
    printk( "WR(%p <- 0x%02x) ", (void*) addr + i, val );
  rtems_outb(addr + i, val);
}

void pci_uart_probe(void)
{
  port_instance_conf_t  conf[MAX_BOARDS];
  int                   boards = 0;
  int                   b = 0;
  console_tbl          *ports;
  console_tbl          *port_p;
  int                   bus;
  int                   dev;
  int                   fun;
  int                   status;
  int                   instance;
  int                   i;
  int                   total_ports = 0;

  for ( b=0 ; b<MAX_BOARDS ; b++ ) {
    conf[b].found = false;
  }

  /*
   *  Scan for Serial port boards
   */
  for ( instance=0 ; instance < MAX_BOARDS ; instance++ ) {

    for ( i=0 ; pci_ns8250_ids[i].vendor != 0xffff ; i++ ) {
      if ( pci_ns8250_ids[i].vendor == 0xffff ) {
        break;
      }
/*
      printk("Looking for 0x%04x:0x%04x\n",
        pci_ns8250_ids[i].vendor,
        pci_ns8250_ids[i].device);
*/
      status = pci_find_device(
        pci_ns8250_ids[i].vendor,
        pci_ns8250_ids[i].device,
        instance,
        &bus,
        &dev,
        &fun
      );
      if ( status == PCIB_ERR_SUCCESS ) {
	uint8_t  irq;
	uint32_t base;
	bool io;

	pci_read_config_dword( bus, dev, fun, PCI_BASE_ADDRESS_0, &base );

	/*
	 * Reject memory mapped 64-bit boards. We need 2 BAR registers and the
	 * driver's base field is only 32-bits any way.
	 */
	io = (base & 1) == 1;
	if (io || (!io && (((base >> 1) & 3) != 2))) {
	  boards++;
	  conf[instance].found = true;
	  conf[instance].desc = pci_ns8250_ids[i].desc;
	  conf[instance].clock =  pci_ns8250_ids[i].rclk;
	  conf[instance].ports = 1;
	  total_ports += conf[instance].ports;

	  pci_read_config_byte( bus, dev, fun, PCI_INTERRUPT_LINE, &irq );

	  conf[instance].irq  = irq;
	  conf[instance].base = base;
	}
      }
    }
  }

  /*
   *  Now allocate array of device structures and fill them in
   */
  if (boards) {
    int device_instance;

    ports = calloc( total_ports, sizeof( console_tbl ) );
    if (ports != NULL) {
      port_p = ports;
      device_instance = 1;
      for (b = 0; b < MAX_BOARDS; b++) {
	uint32_t base = 0;
	bool io;
	const char* locatable = "";
	const char* prefectable = locatable;
	char name[32];
	if ( conf[b].found == false )
	  continue;
	sprintf( name, "/dev/pcicom%d", device_instance++ );
	port_p->sDeviceName   = strdup( name );
	port_p->deviceType    = SERIAL_NS16550;
	if ( conf[b].irq <= 15 ) {
	  port_p->pDeviceFns    = &ns16550_fns;
	} else {
	  printk(
            "%s IRQ=%d >= 16 requires APIC support, using polling\n",
	    name,
	    conf[b].irq
          );
	  port_p->pDeviceFns    = &ns16550_fns_polled;
	}

	/*
	 * PCI BAR (http://wiki.osdev.org/PCI#Base_Address_Registers):
	 *
	 *  Bit 0: 0 = memory, 1 = IO
	 *
	 * Memory:
	 *  Bit 2-1  : 0 = any 32bit address,
	 *             1 = < 1M
	 *             2 = any 64bit address
	 *  Bit 3    : 0 = no, 1 = yes
	 *  Bit 31-4 : base address (16-byte aligned)
	 *
	 * IO:
	 *  Bit 1    : reserved
	 *  Bit 31-2 : base address (4-byte aligned)
	 */

	io = (conf[b].base & 1) == 1;

	if (io) {
	  base = conf[b].base & 0xfffffffc;
	} else {
	  int loc = (conf[b].base >> 1) & 3;
	  if (loc == 0) {
	    base = conf[b].base & 0xfffffff0;
	    locatable = ",A32";
	  } else if (loc == 1) {
	    base = conf[b].base & 0x0000fff0;
	    locatable = ",A16";
	  }
	  prefectable = (conf[b].base & (1 << 3)) == 0 ? ",no-prefech" : ",prefetch";
	}

	port_p->deviceProbe   = NULL;
	port_p->pDeviceFlow   = NULL;
	port_p->ulMargin      = 16;
	port_p->ulHysteresis  = 8;
	port_p->pDeviceParams = (void *) 9600;
	port_p->ulCtrlPort1   = base;
	port_p->ulCtrlPort2   = 0;                   /* NA */
	port_p->ulDataPort    = 0;                   /* NA */
	if (io) {
	  port_p->getRegister = pci_ns16550_io_get_register;
	  port_p->setRegister = pci_ns16550_io_set_register;
	} else {
	  port_p->getRegister = pci_ns16550_mem_get_register;
	  port_p->setRegister = pci_ns16550_mem_set_register;
	}
	port_p->getData       = NULL;                /* NA */
	port_p->setData       = NULL;                /* NA */
	port_p->ulClock       = conf[b].clock;
	port_p->ulIntVector   = conf[b].irq;


        printk(
          "%s:%d:%s,%s:0x%lx%s%s,irq:%d,clk:%lu\n", /*  */
	  name, b, conf[b].desc,
	  io ? "io" : "mem", base, locatable, prefectable,
	  conf[b].irq, conf[b].clock
        );


	port_p++;
      }    /* end boards */

      /*
       *  Register the devices
       */
      console_register_devices( ports, total_ports );

      /*
       *  Do not free the ports memory, the console hold this memory for-ever.
       */
    }
  }
}
#endif
