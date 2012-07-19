/*
 * Board Support Package for CSB360 evaluation board
 * BSP definitions
 *
 * Copyright 2004 Cogent Computer Systems
 * Author: Jay Monkman <jtm@lopingdog.com>
 *
 * Derived from mcf5206elite BSP:
 * Copyright (C) 2000 OKTET Ltd., St.-Petersburg, Russia
 * Author: Victor V. Vengerov <vvv@oktet.ru>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 *
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef _BSP_H
#define _BSP_H

#include <mcf5272/mcf5272.h>


/*** Board resources allocation ***/
#define BSP_MEM_ADDR_SRAM 0x20000000
#define BSP_MEM_SIZE_SRAM 4096

/* Location and size of sdram. Note this includes space used by
 * umon.
 */
#define BSP_MEM_ADDR_SDRAM 0x00000000
#define BSP_MEM_MASK_SDRAM 0x01ffffff
#define BSP_MEM_SIZE_SDRAM (32 * 1024 * 1024)

/* Address to put SIM Modules */
#define BSP_MBAR 0x10000000

/* Address to put SRAM */
#define BSP_RAMBAR BSP_MEM_ADDR_SRAM

/* Interrupt Vectors */
#define BSP_INTVEC_INT1   65
#define BSP_INTVEC_INT2   66
#define BSP_INTVEC_INT3   67
#define BSP_INTVEC_INT4   68
#define BSP_INTVEC_TMR0   69
#define BSP_INTVEC_TMR1   70
#define BSP_INTVEC_TMR2   71
#define BSP_INTVEC_TMR3   72
#define BSP_INTVEC_UART1  73
#define BSP_INTVEC_UART2  74
#define BSP_INTVEC_PLIP   75
#define BSP_INTVEC_PLIA   76
#define BSP_INTVEC_USB0   77
#define BSP_INTVEC_USB1   78
#define BSP_INTVEC_USB2   79
#define BSP_INTVEC_USB3   80
#define BSP_INTVEC_USB4   81
#define BSP_INTVEC_USB5   82
#define BSP_INTVEC_USB6   83
#define BSP_INTVEC_USB7   84
#define BSP_INTVEC_DMA    85
#define BSP_INTVEC_ERX    86
#define BSP_INTVEC_ETX    87
#define BSP_INTVEC_ENTC   88
#define BSP_INTVEC_QSPI   89
#define BSP_INTVEC_INT5   90
#define BSP_INTVEC_INT6   91
#define BSP_INTVEC_SWTO   92

#define BSP_INTLVL_INT1   1
#define BSP_INTLVL_INT2   1
#define BSP_INTLVL_INT3   1
#define BSP_INTLVL_INT4   1
#define BSP_INTLVL_TMR0   1
#define BSP_INTLVL_TMR1   1
#define BSP_INTLVL_TMR2   1
#define BSP_INTLVL_TMR3   1
#define BSP_INTLVL_UART1  1
#define BSP_INTLVL_UART2  1
#define BSP_INTLVL_PLIP   1
#define BSP_INTLVL_PLIA   1
#define BSP_INTLVL_USB0   1
#define BSP_INTLVL_USB1   1
#define BSP_INTLVL_USB2   1
#define BSP_INTLVL_USB3   1
#define BSP_INTLVL_USB4   1
#define BSP_INTLVL_USB5   1
#define BSP_INTLVL_USB6   1
#define BSP_INTLVL_USB7   1
#define BSP_INTLVL_DMA    1
#define BSP_INTLVL_ERX    1
#define BSP_INTLVL_ETX    1
#define BSP_INTLVL_ENTC   1
#define BSP_INTLVL_QSPI   1
#define BSP_INTLVL_INT5   1
#define BSP_INTLVL_INT6   1
#define BSP_INTLVL_SWTO   1



#ifndef ASM

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>
#include <rtems.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>
#include <rtems/clockdrv.h>

struct rtems_bsdnet_ifconfig;
extern int rtems_enet_driver_attach (struct rtems_bsdnet_ifconfig *config);
#define RTEMS_BSP_NETWORK_DRIVER_NAME	"eth0"
#define RTEMS_BSP_NETWORK_DRIVER_ATTACH	rtems_enet_driver_attach

/* System frequency */
#define BSP_SYSTEM_FREQUENCY (66 * 1000 * 1000)

/*
 *  Simple spin delay in microsecond units for device drivers.
 *  This is very dependent on the clock speed of the target.
 */

#define delay( microseconds ) \
  { register uint32_t _delay=(microseconds); \
    register uint32_t _tmp=123; \
    __asm__ volatile( "0: \
                     nbcd      %0 ; \
                     nbcd      %0 ; \
                     dbf       %1,0b" \
                  : "=d" (_tmp), "=d" (_delay) \
                  : "0"  (_tmp), "1"  (_delay) ); \
  }

/*
 * Real-Time Clock Driver Table Entry
 * NOTE: put this entry to the device driver table AFTER I2C bus driver!
 */
#define RTC_DRIVER_TABLE_ENTRY \
    { rtc_initialize, NULL, NULL, NULL, NULL, NULL }
extern rtems_device_driver rtc_initialize(
    rtems_device_major_number major,
    rtems_device_minor_number minor,
    void *arg
);

/* miscellaneous stuff assumed to exist */

extern rtems_isr_entry M68Kvec[];   /* vector table address */

extern rtems_isr (*rtems_clock_hook)(rtems_vector_number);

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif /* ASM */

#endif
