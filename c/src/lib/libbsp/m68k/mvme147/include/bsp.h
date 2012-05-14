/*  bsp.h
 *
 *  This include file contains all MVME147 board IO definitions.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  MVME147 port for TNI - Telecom Bretagne
 *  by Dominique LE CAMPION (Dominique.LECAMPION@enst-bretagne.fr)
 *  May 1996
 */

#ifndef _BSP_H
#define _BSP_H

#ifdef __cplusplus
extern "C" {
#endif

#include <bspopts.h>

#include <rtems.h>
#include <rtems/clockdrv.h>
#include <rtems/console.h>
#include <rtems/iosupp.h>

/* Constants */

#define RAM_START 0x00005000
#define RAM_END   0x00400000

  /* MVME 147 Peripheral controller chip
     see MVME147/D1, 3.4 */

struct pcc_map {
  /* 32 bit registers */
  uint32_t         dma_table_address;            /* 0xfffe1000 */
  uint32_t         dma_data_address;             /* 0xfffe1004 */
  uint32_t         dma_bytecount;                /* 0xfffe1008 */
  uint32_t         dma_data_holding;             /* 0xfffe100c */

  /* 16 bit registers */
  uint16_t         timer1_preload;               /* 0xfffe1010 */
  uint16_t         timer1_count;                 /* 0xfffe1012 */
  uint16_t         timer2_preload;               /* 0xfffe1014 */
  uint16_t         timer2_count;                 /* 0xfffe1016 */

  /* 8 bit registers */
  uint8_t         timer1_int_control;            /* 0xfffe1018 */
  uint8_t         timer1_control;                /* 0xfffe1019 */
  uint8_t         timer2_int_control;            /* 0xfffe101a */
  uint8_t         timer2_control;                /* 0xfffe101b */

  uint8_t         acfail_int_control;            /* 0xfffe101c */
  uint8_t         watchdog_control;              /* 0xfffe101d */

  uint8_t         printer_int_control;           /* 0xfffe101e */
  uint8_t         printer_control;               /* 0xfffe102f */

  uint8_t         dma_int_control;               /* 0xfffe1020 */
  uint8_t         dma_control;                   /* 0xfffe1021 */
  uint8_t         bus_error_int_control;         /* 0xfffe1022 */
  uint8_t         dma_status;                    /* 0xfffe1023 */
  uint8_t         abort_int_control;             /* 0xfffe1024 */
  uint8_t         table_address_function_code;   /* 0xfffe1025 */
  uint8_t         serial_port_int_control;       /* 0xfffe1026 */
  uint8_t         general_purpose_control;       /* 0xfffe1027 */
  uint8_t         lan_int_control;               /* 0xfffe1028 */
  uint8_t         general_purpose_status;        /* 0xfffe1029 */
  uint8_t         scsi_port_int_control;         /* 0xfffe102a */
  uint8_t         slave_base_address;            /* 0xfffe102b */
  uint8_t         software_int_1_control;        /* 0xfffe102c */
  uint8_t         int_base_vector;               /* 0xfffe102d */
  uint8_t         software_int_2_control;        /* 0xfffe102e */
  uint8_t         revision_level;                /* 0xfffe102f */
};

#define pcc      ((volatile struct pcc_map * const) 0xfffe1000)

#define z8530 0xfffe3001

/* interrupt vectors - see MVME146/D1 4.14 */
#define PCC_BASE_VECTOR        0x40 /* First user int */
#define SCC_VECTOR             PCC_BASE_VECTOR+3
#define TIMER_1_VECTOR         PCC_BASE_VECTOR+8
#define TIMER_2_VECTOR         PCC_BASE_VECTOR+9
#define SOFT_1_VECTOR          PCC_BASE_VECTOR+10
#define SOFT_2_VECTOR          PCC_BASE_VECTOR+11

#define USE_CHANNEL_A   1                /* 1 = use channel A for console */
#define USE_CHANNEL_B   0                /* 1 = use channel B for console */

#if (USE_CHANNEL_A == 1)
#define CONSOLE_CONTROL  0xfffe3002
#define CONSOLE_DATA     0xfffe3003
#elif (USE_CHANNEL_B == 1)
#define CONSOLE_CONTROL  0xfffe3000
#define CONSOLE_DATA     0xfffe3001
#endif

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/* functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef __cplusplus
}
#endif

#endif
