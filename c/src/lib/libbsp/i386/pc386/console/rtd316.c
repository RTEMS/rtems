/**
 *  @file
 *
 *  @brief Driver for RTD316 ISA SCC Board
 *
 *  The RTD316 has a single Z85C30.
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
#include <libchip/z85c30.h>
#include <rtems/bspIo.h>
#include <bsp/rtd316.h>
#include <rtems/score/i386.h>

#define RTD_CLOCK_RATE  (460800 * 32)

uint8_t rtd316_com_get_register(uint32_t addr, uint8_t reg)
{
  register uint8_t val = 0;

  outport_byte( addr, reg );
  /* It appears the no delay is needed between the accesses. */
  inport_byte( addr, val );

  return val;
}

void rtd316_com_set_register(uint32_t addr,uint8_t reg, uint8_t val)
{
  outport_byte( addr, reg );
  /* It appears the no delay is needed between the accesses. */
  outport_byte( addr, val );
}

rtems_device_driver rtd316_initialize(
  rtems_device_major_number  major,
  rtems_device_minor_number  minor_arg,
  void                      *arg
)
{
  int           p;
  console_tbl  *ports;
  console_tbl  *port_p;

  /*
   *  Now allocate array of device structures and fill them in
   */
  ports = calloc( 2, sizeof( console_tbl ) );
  port_p = ports;

  for ( p=0 ; p<2 ; p++ ) {
    char name[32];
    sprintf( name, "/dev/rtd316_1_%d", p );
    printk("Found %s\n", name );
    port_p->sDeviceName   = strdup( name );
    port_p->deviceType    = SERIAL_Z85C30;
    #if 0
      port_p->pDeviceFns  = &z85c30_fns_polled;
    #else
      port_p->pDeviceFns  = &z85c30_fns;
    #endif

    port_p->deviceProbe   = NULL;
    port_p->pDeviceFlow   = NULL;
    port_p->ulMargin      = 16;
    port_p->ulHysteresis  = 8;
    port_p->pDeviceParams = (void *) 9600;
    port_p->getRegister   = rtd316_com_get_register;
    port_p->setRegister   = rtd316_com_set_register;
    port_p->getData       = NULL;
    port_p->setData       = NULL;
    port_p->ulClock       = RTD_CLOCK_RATE;
    port_p->ulIntVector   = 9;

    if ( p==0 ) {
      port_p->ulDataPort    = 0;
      port_p->ulCtrlPort1   = 0x340;
      port_p->ulCtrlPort2   = 0x341;
    } else {
      port_p->ulDataPort    = 1;
      port_p->ulCtrlPort1   = 0x342;
      port_p->ulCtrlPort2   = 0x343;
    }
    port_p++;
  }  /* end ports */

  /*
   *  Register the devices
   */
  console_register_devices( ports, 2 );

  return RTEMS_SUCCESSFUL;
}
