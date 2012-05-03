/*  bsp.h
 *
 *  Following defines must reflect the setup of the particular MVME167.
 *  All page references are to the MVME166/MVME167/MVME187 Single Board
 *  Computer Programmer's Reference Guide (MVME187PG/D2) with the April
 *  1993 supplements/addenda (MVME187PG/D2A1).
 *
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  Modifications of respective RTEMS file:
 *  Copyright (c) 1998, National Research Council of Canada
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
#include <rtems/bspIo.h>

#include <mvme16x_hw.h>

/* GCSR is in mvme16x_hw.h */
/* LCSR is in mvme16x_hw.h */
/* i82596 is in mvme16x_hw.h */
/* NVRAM is in mvme16x_hw.h */

#if 0
/*
 *  Representation of the PCCchip2
 */
typedef volatile struct pccchip2_regs_ {
  unsigned char     chip_id;            /* 0xFFF42000 */
  unsigned char     chip_revision;      /* 0xFFF42001 */
  unsigned char     gen_control;        /* 0xFFF42002 */
  unsigned char     vector_base;        /* 0xFFF42003 */
  unsigned long     timer_cmp_1;        /* 0xFFF42004 */
  unsigned long     timer_cnt_1;        /* 0xFFF42008 */
  unsigned long     timer_cmp_2;        /* 0xFFF4200C */
  unsigned long     timer_cnt_2;        /* 0xFFF42010 */
  unsigned char     LSB_prescaler_count;/* 0xFFF42014 */
  unsigned char     prescaler_clock_adjust; /* 0xFFF42015 */
  unsigned char     timer_ctl_2;        /* 0xFFF42016 */
  unsigned char     timer_ctl_1;        /* 0xFFF42017 */
  unsigned char     gpi_int_ctl;        /* 0xFFF42018 */
  unsigned char     gpio_ctl;           /* 0xFFF42019 */
  unsigned char     timer_int_ctl_2;    /* 0xFFF4201A */
  unsigned char     timer_int_ctl_1;    /* 0xFFF4201B */
  unsigned char     SCC_error;          /* 0xFFF4201C */
  unsigned char     SCC_modem_int_ctl;  /* 0xFFF4201D */
  unsigned char     SCC_tx_int_ctl;     /* 0xFFF4201E */
  unsigned char     SCC_rx_int_ctl;     /* 0xFFF4201F */
  unsigned char     reserved1[3];
  unsigned char     modem_piack;        /* 0xFFF42023 */
  unsigned char     reserved2;
  unsigned char     tx_piack;           /* 0xFFF42025 */
  unsigned char     reserved3;
  unsigned char     rx_piack;           /* 0xFFF42027 */
  unsigned char     LANC_error;         /* 0xFFF42028 */
  unsigned char     reserved4;
  unsigned char     LANC_int_ctl;       /* 0xFFF4202A */
  unsigned char     LANC_berr_ctl;      /* 0xFFF4202B */
  unsigned char     SCSI_error;         /* 0xFFF4202C */
  unsigned char     reserved5[2];
  unsigned char     SCSI_int_ctl;       /* 0xFFF4202F */
  unsigned char     print_ack_int_ctl;  /* 0xFFF42030 */
  unsigned char     print_fault_int_ctl;/* 0xFFF42031 */
  unsigned char     print_sel_int_ctl;  /* 0xFFF42032 */
  unsigned char     print_pe_int_ctl;   /* 0xFFF42033 */
  unsigned char     print_busy_int_ctl; /* 0xFFF42034 */
  unsigned char     reserved6;
  unsigned char     print_input_status; /* 0xFFF42036 */
  unsigned char     print_ctl;          /* 0xFFF42037 */
  unsigned char     chip_speed;         /* 0xFFF42038 */
  unsigned char     reserved7;
  unsigned char     print_data;         /* 0xFFF4203A */
  unsigned char     reserved8[3];
  unsigned char     int_level;          /* 0xFFF4203E */
  unsigned char     int_mask;           /* 0xFFF4203F */
} pccchip2_regs;

/*
 *  Base address of the PCCchip2.
 *  This is not configurable in the MVME167.
 */
#define pccchip2    ((pccchip2_regs * const) 0xFFF42000)

#endif
/*
 * The MVME167 is equiped with one or two MEMC040 memory controllers at
 * 0xFFF43000 and 0xFFF43100. This port assumes that the controllers
 * were initialized by 167Bug.
 */
typedef volatile struct memc040_regs_ {
  unsigned char     chip_id;            /* 0xFFF43000/0xFFF43100 */
  unsigned char     reserved1[3];
  unsigned char     chip_revision;      /* 0xFFF43004/0xFFF43104 */
  unsigned char     reserved2[3];
  unsigned char     mem_config;         /* 0xFFF43008/0xFFF43108 */
  unsigned char     reserved3[3];
  unsigned char     alt_status;         /* 0xFFF4300C/0xFFF4310C */
  unsigned char     reserved4[3];
  unsigned char     alt_ctl;            /* 0xFFF43010/0xFFF43110 */
  unsigned char     reserved5[3];
  unsigned char     base_addr;          /* 0xFFF43014/0xFFF43114 */
  unsigned char     reserved6[3];
  unsigned char     ram_ctl;            /* 0xFFF43018/0xFFF43118 */
  unsigned char     reserved7[3];
  unsigned char     bus_clk;            /* 0xFFF4301C/0xFFF4311C */
} memc040_regs;

/*
 *  Base address of the MEMC040s.
 *  This is not configurable in the MVME167.
 */
#define memc040_1   ((memc040_regs * const) 0xFFF43000)
#define memc040_2   ((memc040_regs * const) 0xFFF43100)

/*
 *  The MVME167 may be equiped with error-correcting RAM cards. In this case,
 *  each MEMC040 is replaced by two MCECC ECC DRAM controllers. This port
 *  assumes that these controllers, if present, are initialized by 167Bug.
 *  They do not appear to hold information of interest at this time, so they
 *  are not described. However, each MCECC pair lives at the same address as
 *  the MEMC040 is replaces. The first eight registers of the MCECC are
 *  nearly identical to the ones of the MEMC040, and the memc040_X structures
 *  can be used to read those first eight registers.
 */

/*
 *  Representation of the Cirrus Logic CL-CD2401 Multi-Protocol Controller
 */
typedef volatile struct cd2401_regs_ {
  unsigned char     reserved1[7];
  unsigned char     cor7;           /* 0xFFF45007 - Channel Option 7 */
  unsigned char     reserved2;
  unsigned char     livr;           /* 0xFFF45009 - Local Interrupt Vector */
  unsigned char     reserved3[6];
  unsigned char     cor1;           /* 0xFFF45010 - Channel Option 1 */
  unsigned char     ier;            /* 0xFFF45011 - Interrupt Enable */
  unsigned char     stcr;           /* 0xFFF45012 - Special Transmit Command */
  unsigned char     ccr;            /* 0xFFF45013 - Channel Command */
  unsigned char     cor5;           /* 0xFFF45014 - Channel Option 5 */
  unsigned char     cor4;           /* 0xFFF45015 - Channel Option 4 */
  unsigned char     cor3;           /* 0xFFF45016 - Channel Option 3 */
  unsigned char     cor2;           /* 0xFFF45017 - Channel Option 2 */
  unsigned char     cor6;           /* 0xFFF45018 - Channel Option 6 */
  unsigned char     dmabsts;        /* 0xFFF45019 - DMA Buffer Status */
  unsigned char     csr;            /* 0xFFF4501A - Channel Status */
  unsigned char     cmr;            /* 0xFFF4501B - Channel Mode */
  union {
    struct {
      unsigned char schr4;          /* 0xFFF4501C - Special Character 4 */
      unsigned char schr3;          /* 0xFFF4501D - Special Character 3 */
      unsigned char schr2;          /* 0xFFF4501E - Special Character 2 */
      unsigned char schr1;          /* 0xFFF4501F - Special Character 1 */
    } async;
    struct {
      unsigned char rfar4;          /* 0xFFF4501C - Receive Frame Address 4 */
      unsigned char rfar3;          /* 0xFFF4501D - Receive Frame Address 3 */
      unsigned char rfar2;          /* 0xFFF4501E - Receive Frame Address 2 */
      unsigned char rfar1;          /* 0xFFF4501F - Receive Frame Address 1 */
    } sync;
  } u1;
  unsigned char     reserved4[2];
  unsigned char     scrh;           /* 0xFFF45022 - Special Character Range High */
  unsigned char     scrl;           /* 0xFFF45023 - Special Character Range Low */
  union {
    struct {
      unsigned short rtpr;          /* 0xFFF45024 - Receive Timeout Period */
    } w;
    struct {
      unsigned char rtprh;          /* 0xFFF45024 - Receive Timeout Period High */
      unsigned char rtprl;          /* 0xFFF45025 - Receive Timeout Period Low */
    } b;
  } u2;
  unsigned char     licr;           /* 0xFFF45026 - Local Interrupt Channel */
  unsigned char     reserved5[2];
  union {
    struct {
      unsigned char ttr;            /* 0xFFF45029 - Transmit Timer */
    } async;
    struct {
      unsigned char gt2;            /* 0xFFF45029 - General Timer 2 */
    } sync;
  } u3;
  union {
    struct {
      unsigned short gt1;           /* 0xFFF4502A - General Timer 1 */
    } w;
    struct {
      unsigned char gt1h;           /* 0xFFF4502A - General Timer 2 High */
      unsigned char gt1l;           /* 0xFFF4502B - General Timer 1 Low */
    } b;
  } u4;
  unsigned char     reserved6[2];
  unsigned char     lnxt;           /* 0xFF4502E - LNext Character */
  unsigned char     reserved7;
  unsigned char     rfoc;           /* 0xFFF45030 - Receive FIFO Output Count */
  unsigned char     reserved8[7];
  unsigned short    tcbadru;        /* 0xFF45038 - Transmit Current Buffer Address Upper */
  unsigned short    tcbadrl;        /* 0xFF4503A - Transmit Current Buffer Address Lower */
  unsigned short    rcbadru;        /* 0xFF4503C - Receive Current Buffer Address Upper */
  unsigned short    rcbadrl;        /* 0xFF4503E - Receive Current Buffer Address Lower */
  unsigned short    arbadru;        /* 0xFF45040 - A Receive Buffer Address Upper */
  unsigned short    arbardl;        /* 0xFF45042 - A Receive Buffer Address Lower */
  unsigned short    brbadru;        /* 0xFF45044 - B Receive Buffer Address Upper */
  unsigned short    brbadrl;        /* 0xFF45046 - B Receive Buffer Address Lower */
  unsigned short    brbcnt;         /* 0xFF45048 - B Receive Buffer Byte Count */
  unsigned short    arbcnt;         /* 0xFF4504A - A Receive Buffer Byte Count */
  unsigned short    reserved9;
  unsigned char     brbsts;         /* 0xFF4504E - B Receive Buffer Status */
  unsigned char     arbsts;         /* 0xFF4504F - A Receive Buffer Status */
  unsigned short    atbadru;        /* 0xFF45050 - A Transmit Buffer Address Upper */
  unsigned short    atbadrl;        /* 0xFF45052 - A Transmit Buffer Address Lower */
  unsigned short    btbadru;        /* 0xFF45054 - B Transmit Buffer Address Upper */
  unsigned short    btbadrl;        /* 0xFF45056 - B Transmit Buffer Address Lower */
  unsigned short    btbcnt;         /* 0xFF45058 - B Transmit Buffer Byte Count */
  unsigned short    atbcnt;         /* 0xFF4505A - A Transmit Buffer Byte Count */
  unsigned short    reserved10;
  unsigned char     btbsts;         /* 0xFF4505E - B Transmit Buffer Status */
  unsigned char     atbsts;         /* 0xFF4505F - A Transmit Buffer Status */
  unsigned char     reserved11[32];
  unsigned char     tftc;           /* 0xFFF45080 - Transmit FIFO Transfer Count */
  unsigned char     gfrcr;          /* 0xFFF45081 - Global Firmware Revision Code */
  unsigned char     reserved12[2];
  unsigned char     reoir;          /* 0xFFF45084 - Receive End Of Interrupt */
  unsigned char     teoir;          /* 0xFFF45085 - Transmit End Of Interrupt */
  unsigned char     meoir;          /* 0xFFF45086 - Modem End Of Interrupt */
  union {
    struct {
      unsigned short risr;          /* 0xFFF45088 - Receive Interrupt Status */
    } w;
    struct {
      unsigned char risrh;          /* 0xFFF45088 - Receive Interrupt Status High */
      unsigned char risrl;          /* 0xFFF45089 - Receive Interrupt Status Low */
    } b;
  } u5;
  unsigned char     tisr;           /* 0xFFF4508A - Transmit Interrupt Status */
  unsigned char     misr;           /* 0xFFF4508B - Modem/Timer Interrupt Status */
  unsigned char     reserved13[2];
  unsigned char     bercnt;         /* 0xFFF4508E - Bus Error Retry Count */
  unsigned char     reserved14[49];
  unsigned char     tcor;           /* 0xFFF450C0 - Transmit Clock Option */
  unsigned char     reserved15[2];
  unsigned char     tbpr;           /* 0xFFF450C3 - Transmit Baud Rate Period */
  unsigned char     reserved16[4];
  unsigned char     rcor;           /* 0xFFF450C8 - Receive Clock Option */
  unsigned char     reserved17[2];
  unsigned char     rbpr;           /* 0xFFF450CB - Receive Baud Rate Period */
  unsigned char     reserved18[10];
  unsigned char     cpsr;           /* 0xFFF450D6 - CRC Polynomial Select */
  unsigned char     reserved19[3];
  unsigned char     tpr;            /* 0xFFF450DA - Timer Period */
  unsigned char     reserved20[3];
  unsigned char     msvr_rts;       /* 0xFFF450DE - Modem Signal Value - RTS */
  unsigned char     msvr_dtr;       /* 0xFFF450DF - Modem Signal Value - DTR */
  unsigned char     tpilr;          /* 0xFFF450E0 - Transmit Priority Interrupt Level */
  unsigned char     rpilr;          /* 0xFFF450E1 - Receive Priority Interrupt Level */
  unsigned char     stk;            /* 0xFFF450E2 - Stack */
  unsigned char     mpilr;          /* 0xFFF450E3 - Modem Priority Interrupt Level */
  unsigned char     reserved21[8];
  unsigned char     tir;            /* 0xFFF450EC - Transmit Interrupt */
  unsigned char     rir;            /* 0xFFF450ED - Receive Interrupt */
  unsigned char     car;            /* 0xFFF450EE - Channel Access */
  unsigned char     mir;            /* 0xFFF450EF - Model Interrupt */
  unsigned char     reserved22[6];
  unsigned char     dmr;            /* 0xFFF450F6 - DMA Mode */
  unsigned char     reserved23;
  unsigned char     dr;             /* 0xFFF450F8 - Receive/Transmit Data */
} cd2401_regs;

/*
 *  Base address of the CD2401.
 *  This is not configurable in the MVME167.
 */
#define cd2401          ((cd2401_regs * const) 0xFFF45000)

/* CD2401 is clocked at 20 MHz */
#define CD2401_CLK_RATE 20000000

/* BSP-wide functions */

rtems_isr_entry set_vector(
  rtems_isr_entry     handler,
  rtems_vector_number vector,
  int                 type
);

#ifdef M167_INIT
#undef EXTERN
#define EXTERN
#else
#undef EXTERN
#define EXTERN extern
#endif

extern rtems_isr_entry M68Kvec[];   /* vector table address */

#ifdef __cplusplus
}
#endif

#endif
