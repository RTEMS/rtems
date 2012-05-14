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

#define RAM_START 0x00007000
#define RAM_END   0x003e0000
#define DRAM_END  0x00400000
  /* We leave 128k for the shared memory */

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

/* VME chip configuration registers */

struct vme_lcsr_map {
  uint8_t         unused_1;
  uint8_t         system_controller;             /* 0xfffe2001 */
  uint8_t         unused_2;
  uint8_t         vme_bus_requester;             /* 0xfffe2003 */
  uint8_t         unused_3;
  uint8_t         master_configuration;          /* 0xfffe2005 */
  uint8_t         unused_4;
  uint8_t         slave_configuration;           /* 0xfffe2007 */
  uint8_t         unused_5;
  uint8_t         timer_configuration;           /* 0xfffe2009 */
  uint8_t         unused_6;
  uint8_t         slave_address_modifier;        /* 0xfffe200b */
  uint8_t         unused_7;
  uint8_t         master_address_modifier;       /* 0xfffe200d */
  uint8_t         unused_8;
  uint8_t         interrupt_handler_mask;        /* 0xfffe200f */
  uint8_t         unused_9;
  uint8_t         utility_interrupt_mask;        /* 0xfffe2011 */
  uint8_t         unused_10;
  uint8_t         utility_interrupt_vector;      /* 0xfffe2013 */
  uint8_t         unused_11;
  uint8_t         interrupt_request;             /* 0xfffe2015 */
  uint8_t         unused_12;
  uint8_t         vme_bus_status_id;             /* 0xfffe2017 */
  uint8_t         unused_13;
  uint8_t         bus_error_status;              /* 0xfffe2019 */
  uint8_t         unused_14;
  uint8_t         gcsr_base_address;             /* 0xfffe201b */
};

#define vme_lcsr      ((volatile struct vme_lcsr_map * const) 0xfffe2000)

struct vme_gcsr_map {
  uint8_t         unused_1;
  uint8_t         global_0;                      /* 0xfffe2021 */
  uint8_t         unused_2;
  uint8_t         global_1;                      /* 0xfffe2023 */
  uint8_t         unused_3;
  uint8_t         board_identification;          /* 0xfffe2025 */
  uint8_t         unused_4;
  uint8_t         general_purpose_0;             /* 0xfffe2027 */
  uint8_t         unused_5;
  uint8_t         general_purpose_1;             /* 0xfffe2029 */
  uint8_t         unused_6;
  uint8_t         general_purpose_2;             /* 0xfffe202b */
  uint8_t         unused_7;
  uint8_t         general_purpose_3;             /* 0xfffe202d */
  uint8_t         unused_8;
  uint8_t         general_purpose_4;             /* 0xfffe202f */
};

#define vme_gcsr      ((volatile struct vme_gcsr_map * const) 0xfffe2020)

#define z8530 0xfffe3001

/* interrupt vectors - see MVME147/D1 4.14 */
#define PCC_BASE_VECTOR        0x40 /* First user int */
#define SCC_VECTOR             PCC_BASE_VECTOR+3
#define TIMER_1_VECTOR         PCC_BASE_VECTOR+8
#define TIMER_2_VECTOR         PCC_BASE_VECTOR+9
#define SOFT_1_VECTOR          PCC_BASE_VECTOR+10
#define SOFT_2_VECTOR          PCC_BASE_VECTOR+11

#define VME_BASE_VECTOR        0x50
#define VME_SIGLP_VECTOR       VME_BASE_VECTOR+1

#define USE_CHANNEL_A   1                /* 1 = use channel A for console */
#define USE_CHANNEL_B   0                /* 1 = use channel B for console */

#if (USE_CHANNEL_A == 1)
#define CONSOLE_CONTROL  0xfffe3002
#define CONSOLE_DATA     0xfffe3003
#elif (USE_CHANNEL_B == 1)
#define CONSOLE_CONTROL  0xfffe3000
#define CONSOLE_DATA     0xfffe3001
#endif

#define FOREVER       1                  /* infinite loop */

#ifdef M147_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

extern rtems_isr_entry M68Kvec[];   /* vector table address */

/*
 * NOTE: Use the standard Clock driver entry
 */

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
