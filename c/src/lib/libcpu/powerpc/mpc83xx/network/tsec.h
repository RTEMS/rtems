/*===============================================================*\
| Project: RTEMS support for MPC83xx                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2007                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file declares the MPC83xx TSEC networking driver           |
\*===============================================================*/

#ifndef LIBCPU_POWERPC_TSEC_H
#define LIBCPU_POWERPC_TSEC_H

#include <stdint.h>

#include <bsp/irq.h>
#include <bsp/tsec-config.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

  /*
   * this enumeration defines the index
   * of a given rmon mib counter
   * in the tsec_rmon_mib array
   */
typedef enum {
  /* TSEC1 Transmit and Receive Counters */
  TSEC_RMON_TR64,                    /* 0x2_4680 Transmit and receive 64-byte frame counter register R/W 0x0000_0000 15.5.3.7.1/15-60 */
  TSEC_RMON_TR127,                   /* 0x2_4684 Transmit and receive 65- to 127-byte frame counter register R/W 0x0000_0000 15.5.3.7.2/15-61 */
  TSEC_RMON_TR255,                   /* 0x2_4688 Transmit and receive 128- to 255-byte frame counter register R/W 0x0000_0000 15.5.3.7.3/15-61 */
  TSEC_RMON_TR511,                   /* 0x2_468C Transmit and receive 256- to 511-byte frame counter register R/W 0x0000_0000 15.5.3.7.4/15-62 */
  TSEC_RMON_TR1K,                    /* 0x2_4690 Transmit and receive 512- to 1023-byte frame counter register R/W 0x0000_0000 15.5.3.7.5/15-62 */
  TSEC_RMON_TRMAX,                   /* 0x2_4694 Transmit and receive 1024- to 1518-byte frame counter register R/W 0x0000_0000 15.5.3.7.6/15-63 */
  TSEC_RMON_TRMGV,                   /* 0x2_4698 Transmit and receive 1519- to 1522-byte good VLAN frame count register R/W 0x0000_0000 15.5.3.7.7/15-63 */
  /* TSEC1 Receive Counters */
  TSEC_RMON_RBYT,                    /* 0x2_469C Receive byte counter register R/W 0x0000_0000 15.5.3.7.8/15-64 */
  TSEC_RMON_RPKT,                    /* 0x2_46A0 Receive packet counter register R/W 0x0000_0000 15.5.3.7.9/15-64 */
  TSEC_RMON_RFCS,                    /* 0x2_46A4 Receive FCS error counter register R/W 0x0000_0000 15.5.3.7.10/15-65 */
  TSEC_RMON_RMCA,                    /* 0x2_46A8 Receive multicast packet counter register R/W 0x0000_0000 15.5.3.7.11/15-65 */
  TSEC_RMON_RBCA,                    /* 0x2_46AC Receive broadcast packet counter register R/W 0x0000_0000 15.5.3.7.12/15-66 */
  TSEC_RMON_RXCF,                    /* 0x2_46B0 Receive control frame packet counter register R/W 0x0000_0000 15.5.3.7.13/15-66 */
  TSEC_RMON_RXPF,                    /* 0x2_46B4 Receive PAUSE frame packet counter register R/W 0x0000_0000 15.5.3.7.14/15-67 */
  TSEC_RMON_RXUO,                    /* 0x2_46B8 Receive unknown OP code counter register R/W 0x0000_0000 15.5.3.7.15/15-67 */
  TSEC_RMON_RALN,                    /* 0x2_46BC Receive alignment error counter register R/W 0x0000_0000 15.5.3.7.16/15-68 */
  TSEC_RMON_RFLR,                    /* 0x2_46C0 Receive frame length error counter register R/W 0x0000_0000 15.5.3.7.17/15-68 */
  TSEC_RMON_RCDE,                    /* 0x2_46C4 Receive code error counter register R/W 0x0000_0000 15.5.3.7.18/15-69 */
  TSEC_RMON_RCSE,                    /* 0x2_46C8 Receive carrier sense error counter register R/W 0x0000_0000 15.5.3.7.19/15-69 */
  TSEC_RMON_RUND,                    /* 0x2_46CC Receive undersize packet counter register R/W 0x0000_0000 15.5.3.7.20/15-70 */
  TSEC_RMON_ROVR,                    /* 0x2_46D0 Receive oversize packet counter register R/W 0x0000_0000 15.5.3.7.21/15-70 */
  TSEC_RMON_RFRG,                    /* 0x2_46D4 Receive fragments counter register R/W 0x0000_0000 15.5.3.7.22/15-71 */
  TSEC_RMON_RJBR,                    /* 0x2_46D8 Receive jabber counter register R/W 0x0000_0000 15.5.3.7.23/15-71 */
  TSEC_RMON_RDRP,                    /* 0x2_46DC Receive drop register R/W 0x0000_0000 15.5.3.7.24/15-72 */
  /* TSEC1 Transmit Counters */
  TSEC_RMON_TBYT,                    /* 0x2_46E0 Transmit byte counter register R/W 0x0000_0000 15.5.3.7.25/15-72 */
  TSEC_RMON_TPKT,                    /* 0x2_46E4 Transmit packet counter register R/W 0x0000_0000 15.5.3.7.26/15-73 */
  TSEC_RMON_TMCA,                    /* 0x2_46E8 Transmit multicast packet counter register R/W 0x0000_0000 15.5.3.7.27/15-73 */
  TSEC_RMON_TBCA,                    /* 0x2_46EC Transmit broadcast packet counter register R/W 0x0000_0000 15.5.3.7.28/15-74 */
  TSEC_RMON_TXPF,                    /* 0x2_46F0 Transmit PAUSE control frame counter register R/W 0x0000_0000 15.5.3.7.29/15-74 */
  TSEC_RMON_TDFR,                    /* 0x2_46F4 Transmit deferral packet counter register R/W 0x0000_0000 15.5.3.7.30/15-75 */
  TSEC_RMON_TEDF,                    /* 0x2_46F8 Transmit excessive deferral packet counter register R/W 0x0000_0000 15.5.3.7.31/15-75 */
  TSEC_RMON_TSCL,                    /* 0x2_46FC Transmit single collision packet counter register R/W 0x0000_0000 15.5.3.7.32/15-76 */
  TSEC_RMON_TMCL,                    /* 0x2_4700 Transmit multiple collision packet counter register R/W 0x0000_0000 15.5.3.7.33/15-76 */
  TSEC_RMON_TLCL,                    /* 0x2_4704 Transmit late collision packet counter register R/W 0x0000_0000 15.5.3.7.34/15-77 */
  TSEC_RMON_TXCL,                    /* 0x2_4708 Transmit excessive collision packet counter register R/W 0x0000_0000 15.5.3.7.35/15-77 */
  TSEC_RMON_TNCL,                    /* 0x2_470C Transmit total collision counter register R/W 0x0000_0000 15.5.3.7.36/15-78 */
  TSEC_RESERVED1,                    /* 0x2_4710 Reserved, should be cleared R 0x0000_0000  */
  TSEC_RMON_TDRP,                    /* 0x2_4714 Transmit drop frame counter register R/W 0x0000_0000 15.5.3.7.37/15-78 */
  TSEC_RMON_TJBR,                    /* 0x2_4718 Transmit jabber frame counter register R/W 0x0000_0000 15.5.3.7.38/15-79 */
  TSEC_RMON_TFCS,                    /* 0x2_471C Transmit FCS error counter register R/W 0x0000_0000 15.5.3.7.39/15-79 */
  TSEC_RMON_TXCF,                    /* 0x2_4720 Transmit control frame counter register R/W 0x0000_0000 15.5.3.7.40/15-80 */
  TSEC_RMON_TOVR,                    /* 0x2_4724 Transmit oversize frame counter register R/W 0x0000_0000 15.5.3.7.41/15-80 */
  TSEC_RMON_TUND,                    /* 0x2_4728 Transmit undersize frame counter register R/W 0x0000_0000 15.5.3.7.42/15-81 */
  TSEC_RMON_TFRG,                    /* 0x2_472C Transmit fragments frame counter register R/W 0x0000_0000 15.5.3.7.43/15-81 */
  TSEC_RMON_CNT
} tsec_rmon_idx;

  /* TSEC1/2 General Control and Status Registers */
typedef struct {
  uint8_t reserved0x2_4000[0x24010-0x24000]; /* 0x2_4000--0x2_400F Reserved, should be cleared */
  uint32_t ievent;                  /* 0x2_4010 Interrupt event register R/W 0x0000_0000 15.5.3.1.1/15-19 */
  uint32_t imask;                   /* 0x2_4014 Interrupt mask register R/W 0x0000_0000 15.5.3.1.2/15-22 */
  uint32_t edis;                    /* 0x2_4018 Error disabled register R/W 0x0000_0000 15.5.3.1.3/15-24 */
  uint8_t reserved0x2_401c[0x24020-0x2401c]; /* 0x2_401c--0x2_401f Reserved, should be cleared */
  uint32_t ecntrl;                  /* 0x2_4020 Ethernet control register R/W 0x0000_0000 15.5.3.1.4/15-25 */
  uint32_t minflr;                  /* 0x2_4024 Minimum frame length register R/W 0x0000_0040 15.5.3.1.5/15-26 */
  uint32_t ptv;                     /* 0x2_4028 Pause time value register R/W 0x0000_0000 15.5.3.1.6/15-27 */
  uint32_t dmactrl;                 /* 0x2_402C DMA control register R/W 0x0000_0000 15.5.3.1.7/15-28 */
  uint32_t tbipa;                   /* 0x2_4030 TBI PHY address register R/W 0x0000_0000 15.5.3.1.8/15-29 */
  uint8_t reserved0x2_4034[0x2408c-0x24034]; /* 0x2_4034--0x2_408b Reserved, should be cleared */
  /* TSEC1 FIFO Control and Status Registers */
  uint32_t fifo_tx_thr;             /* 0x2_408C FIFO transmit threshold register R/W 0x0000_0100 15.5.3.2.1/15-30 */
  uint8_t reserved0x2_4090[0x24094-0x24090]; /* 0x2_4090--0x2_4093 Reserved, should be cleared */
  uint32_t fifo_tx_sp;              /* 0x2_4094 FIFO transmit space available register R/W 0x0000_0010 15.5.3.2.2/15-31 */
  uint32_t fifo_tx_starve;          /* 0x2_4098 FIFO transmit starve register R/W 0x0000_0080 15.5.3.2.3/15-31 */
  uint32_t fifo_tx_starve_shutoff;  /* 0x2_409C FIFO transmit starve shutoff register R/W 0x0000_0100 15.5.3.2.4/15-32 */
  uint8_t reserved0x2_40A0[0x24100-0x240A0]; /* 0x2_40A0--0x2_40ff Reserved, should be cleared */
  /* TSEC1 Transmit Control and Status Registers */
  uint32_t tctrl;                   /* 0x2_4100 Transmit control register R/W 0x0000_0000 15.5.3.3.1/15-33 */
  uint32_t tstat;                   /* 0x2_4104 Transmit status register R/W 0x0000_0000 15.5.3.3.2/15-34 */
  uint8_t reserved0x2_4108[0x24110-0x24108]; /* 0x2_4108 Reserved, should be cleared R 0x0000_0000 */
  uint32_t txic;                    /* 0x2_4110 Transmit interrupt coalescing configuration register R/W 0x0000_0000 */
  uint8_t reserved0x2_4114[0x24124-0x24114]; /* 0x2_4114--0x2_4120 Reserved, should be cleared */
  uint32_t ctbptr;                  /* 0x2_4124 Current TxBD pointer register R 0x0000_0000 15.5.3.3.5/15-36 */
  uint8_t reserved0x2_4128[0x24184-0x24128]; /* 0x2_4128--0x2_4180 Reserved, should be cleared */
  uint32_t tbptr;                   /* 0x2_4184 TxBD pointer register R/W 0x0000_0000 15.5.3.3.6/15-36 */
  uint8_t reserved0x2_4188[0x24204-0x24188]; /* 0x2_4188--0x2_4200 Reserved, should be cleared */
  uint32_t tbase;                   /* 0x2_4204 TxBD base address register R/W 0x0000_0000 15.5.3.3.7/15-37 */
  uint8_t reserved0x2_4208[0x242B0-0x24208]; /* 0x2_4208--0x2_42AC Reserved, should be cleared */
  uint32_t ostbd;                   /* 0x2_42B0 Out-of-sequence TxBD register R/W 0x0800_0000 15.5.3.3.8/15-37 */
  uint32_t ostbdp;                  /* 0x2_42B4 Out-of-sequence Tx data buffer pointer register R/W 0x0000_0000 15.5.3.3.9/15-39 */
  uint8_t reserved0x2_42B8[0x24300-0x242B8]; /* 0x2_42B8--0x2_42FC Reserved, should be cleared */
  /* TSEC1 Receive Control and Status Registers */
  uint32_t rctrl;                   /* 0x2_4300 Receive control register R/W 0x0000_0000 15.5.3.4.1/15-40 */
  uint32_t rstat;                   /* 0x2_4304 Receive status register R/W 0x0000_0000 15.5.3.4.2/15-41 */
  uint8_t reserved0x2_4308[0x2430C-0x24308]; /* 0x2_4308 Reserved, should be cleared R 0x0000_0000  */
  uint32_t rbdlen;                  /* 0x2_430C RxBD data length register R 0x0000_0000 15.5.3.4.3/15-41 */
  uint32_t rxic;                    /* 0x2_4310 Receive interrupt coalescing configuration register R/W 0x0000_0000 15.5.3.4.4/15-42 */
  uint8_t reserved0x2_4314[0x24324-0x24314]; /* 0x2_4314--0x2_4320 Reserved, should be cleared */
  uint32_t crbptr;                  /* 0x2_4324 Current RxBD pointer register R 0x0000_0000 15.5.3.4.5/15-43 */
  uint8_t reserved0x2_4328[0x24340-0x24328]; /* 0x2_4328--0x2_433C Reserved, should be cleared */
  uint32_t mrblr;                   /* 0x2_4340 Maximum receive buffer length register R/W 0x0000_0000 15.5.3.4.6/15-43 */
  uint8_t reserved0x2_4344[0x24384-0x24344]; /* 0x2_4344--0x2_4380 Reserved, should be cleared */
  uint32_t rbptr;                   /* 0x2_4384 RxBD pointer register R/W 0x0000_0000 15.5.3.4.7/15-44 */
  uint8_t reserved0x2_4388[0x24404-0x24388]; /* 0x2_4388--0x2_4400 Reserved, should be cleared */
  uint32_t rbase;                   /* 0x2_4404 RxBD base address register R/W 0x0000_0000 15.5.3.4.8/15-44 */
  uint8_t reserved0x2_4408[0x24500-0x24408]; /* 0x2_4408--0x2_44FC Reserved, should be cleared */
  /* TSEC1 MAC Registers */
  uint32_t maccfg1;                 /* 0x2_4500 MAC configuration register 1 R/W, R 0x0000_0000 15.5.3.6.1/15-48 */
  uint32_t maccfg2;                 /* 0x2_4504 MAC configuration register 2 R/W 0x0000_7000 15.5.3.6.2/15-49 */
  uint32_t ipgifg;                  /* 0x2_4508 Inter-packet gap/inter-frame gap register R/W 0x4060_5060 15.5.3.6.3/15-51 */
  uint32_t hafdup;                  /* 0x2_450C Half-duplex register R/W 0x00A1_F037 15.5.3.6.4/15-52 */
  uint32_t maxfrm;                  /* 0x2_4510 Maximum frame length register R/W 0x0000_0600 15.5.3.6.5/15-53 */
  uint8_t reserved0x2_4514[0x24520-0x24514]; /* 0x2_4514--0x2_451C Reserved, should be cleared */
  uint32_t miimcfg;                 /* 0x2_4520 MII management configuration register R/W 0x0000_0000 15.5.3.6.6/15-53 */
  uint32_t miimcom;                 /* 0x2_4524 MII management command register R/W 0x0000_0000 15.5.3.6.7/15-54 */
  uint32_t miimadd;                 /* 0x2_4528 MII management address register R/W 0x0000_0000 15.5.3.6.8/15-55 */
  uint32_t miimcon;                 /* 0x2_452C MII management control register W 0x0000_0000 15.5.3.6.9/15-56 */
  uint32_t miimstat;                /* 0x2_4530 MII management status register R 0x0000_0000 15.5.3.6.10/15-56 */
  uint32_t miimind;                 /* 0x2_4534 MII management indicator register R 0x0000_0000 15.5.3.6.11/15-57 */
  uint8_t reserved0x2_4538[0x2453c-0x24538]; /* 0x2_4538 Reserved, should be cleared  $ $ */
  uint32_t ifstat;                  /* 0x2_453C Interface status register Special 0x0000_0001 15.5.3.6.12/15-58 */
  uint32_t macstnaddr[2];             /* 0x2_4540 Station address register, part 1/2 R/W 0x0000_0000 15.5.3.6.13/15-58 */
  uint8_t reserved0x2_4548[0x24680-0x24548]; /* 0x2_4548--0x2_467C Reserved, should be cleared */

  /* TSEC1 RMON MIB Registers */
  uint32_t rmon_mib[TSEC_RMON_CNT];

  /* TSEC1 General Registers */
  uint32_t car[2];                  /* 0x2_4730 Carry register one/two register R 0x0000_0000 15.5.3.7.44/15-82 */
  uint32_t cam[2];                  /* 0x2_4738 Carry register one/two mask register R/W 0xFE01_FFFF 15.5.3.7.46/15-85 */
  uint8_t reserved0x2_4740[0x24800-0x24740]; /* 0x2_4740--0x2_47FC Reserved, should be cleared */

  /* TSEC1 Hash Function Registers */
  uint32_t iaddr[8];                /* 0x2_4800 Individual address register 0-7 R/W 0x0000_0000 15.5.3.8.1/15-87 */
  uint8_t reserved0x2_4820[0x24880-0x24820]; /* 0x2_4820--0x2_487C Reserved, should be cleared */
  uint32_t gaddr[8];                /* 0x2_4880 Group address register 0-7 R/W 0x0000_0000 15.5.3.8.2/15-88 */
  uint8_t reserved0x2_48A0[0x24B00-0x248A0]; /* 0x2_48A0--0x2_4AFF Reserved, should be cleared */

  /* TSEC1 Attribute Registers */
  uint8_t reserved0x2_4B00[0x24BF8-0x24B00]; /* 0x2_4B00--0x2_4BF4 Reserved, should be cleared */
  uint32_t attr;                    /* 0x2_4BF8 Attribute register R 0x0000_0000 */
  uint32_t attreli;                 /* 0x2_4BFC Attribute extract length and extract index register R/W 0x0000_0000 */
  uint8_t reserved0x2_4C00[0x25000-0x24C00]; /* 0x2_4C00--0x2_4FFF Reserved, should be cleared */
} tsec_registers;

/*
 * TSEC IEVENT/IMASK bit definitions
 */
#define TSEC_IEVENT_BABR      (1<<(31- 0))
#define TSEC_IEVENT_RXC       (1<<(31- 1))
#define TSEC_IEVENT_BSY       (1<<(31- 2))
#define TSEC_IEVENT_EBERR     (1<<(31- 3))
#define TSEC_IEVENT_MSRO      (1<<(31- 5))
#define TSEC_IEVENT_GTSC      (1<<(31- 6))
#define TSEC_IEVENT_BABT      (1<<(31- 7))
#define TSEC_IEVENT_TXC       (1<<(31- 8))
#define TSEC_IEVENT_TXE       (1<<(31- 9))
#define TSEC_IEVENT_TXB       (1<<(31-10))
#define TSEC_IEVENT_TXF       (1<<(31-11))
#define TSEC_IEVENT_LC        (1<<(31-13))
#define TSEC_IEVENT_CRL_XDA   (1<<(31-14))
#define TSEC_IEVENT_XFUN      (1<<(31-15))
#define TSEC_IEVENT_RXB       (1<<(31-16))
#define TSEC_IEVENT_MMRD      (1<<(31-21))
#define TSEC_IEVENT_MMWR      (1<<(31-22))
#define TSEC_IEVENT_GRSC      (1<<(31-23))
#define TSEC_IEVENT_RXF       (1<<(31-24))

/*
 * TSEC DMACTRL bit definitions
 */
#define TSEC_DMACTL_TDSEN  (1<<(31-24))
#define TSEC_DMACTL_TBDSEN (1<<(31-25))
#define TSEC_DMACTL_GRS    (1<<(31-27))
#define TSEC_DMACTL_GTS    (1<<(31-28))
#define TSEC_DMACTL_WWR    (1<<(31-30))
#define TSEC_DMACTL_WOP    (1<<(31-31))

/*
 * TSEC TSTAT bit definitions
 */
#define TSEC_TSTAT_THLT  (1<<(31-0))

/*
 * TSEC RSTAT bit definitions
 */
#define TSEC_RSTAT_QHLT  (1<<(31-8))
  /*
   * TSEC ECNTRL bit positions
   */
#define TSEC_ECNTRL_CLRCNT (1 << (31-17))  /* Clear stat counters     */
#define TSEC_ECNTRL_AUTOZ  (1 << (31-18))  /* auto-zero read counters */
#define TSEC_ECNTRL_STEN   (1 << (31-19))  /* enable statistics       */
#define TSEC_ECNTRL_TBIM   (1 << (31-26))  /* ten-bit-interface       */
#define TSEC_ECNTRL_RPM    (1 << (31-27))  /* reduced signal mode     */
#define TSEC_ECNTRL_R100M  (1 << (31-28))  /* RGMII100 mode           */
  /*
   * TSEC EDIS bit positions
   */
#define TSEC_EDIS_BSYDIS    (1 << (31- 2))  /* Busy disable            */
#define TSEC_EDIS_EBERRDIS  (1 << (31- 3))  /* bus error disable       */
#define TSEC_EDIS_TXEDIS    (1 << (31- 9))  /* Tx error disable        */
#define TSEC_EDIS_LCDIS     (1 << (31-13))  /* Late collision disable  */
#define TSEC_EDIS_CRLXDADIS (1 << (31-14))  /* Collision Retry disable */
#define TSEC_EDIS_FUNDIS    (1 << (31-15))  /* Tx FIFO underrun disable*/

  /*
   * TSEC RCTRL bit positions
   */
#define TSEC_RCTRL_BC_REJ  (1 << (31-27))  /* Broadcast Reject        */
#define TSEC_RCTRL_PROM    (1 << (31-28))  /* Promiscuous             */
#define TSEC_RCTRL_RSF     (1 << (31-29))  /* Receive short frames    */

  /*
   * TSEC TXIC bit positions
   */
#define TSEC_TXIC_ICEN      (1 << (31- 0))  /* Irq coalescing enable   */
#define TSEC_TXIC_ICFCT(n) (((n)&0xff) << (31-10))  /* Frame coal. cnt */
#define TSEC_TXIC_ICTT(n) (((n)&0xffff) << (31-31))  /* Buf. coal. cnt */

  /*
   * TSEC RXIC bit positions
   */
#define TSEC_RXIC_ICEN      (1 << (31- 0))  /* Irq coalescing enable   */
#define TSEC_RXIC_ICFCT(n) (((n)&0xff) << (31-10))  /* Frame coal. cnt */
#define TSEC_RXIC_ICTT(n) (((n)&0xffff) << (31-31))  /* Buf. coal. cnt */

  /*
   * TSEC MACCFG1 bit positions
   */
#define TSEC_MACCFG1_SOFTRST   (1 << (31- 0))  /* Soft Reset           */
#define TSEC_MACCFG1_RES_RXMC  (1 << (31-12))  /* Reset Rx MAC block   */
#define TSEC_MACCFG1_RES_TXMC  (1 << (31-13))  /* Reset Tx MAC block   */
#define TSEC_MACCFG1_RES_RXFUN (1 << (31-14))  /* Reset Rx function blk*/
#define TSEC_MACCFG1_RES_TXFUN (1 << (31-15))  /* Reset Tx function blk*/
#define TSEC_MACCFG1_LOOPBACK  (1 << (31-23))  /* Loopback mode        */
#define TSEC_MACCFG1_RX_FLOW   (1 << (31-26))  /* Receive Flow Ctrl    */
#define TSEC_MACCFG1_TX_FLOW   (1 << (31-27))  /* Transmit Flow Ctrl   */
#define TSEC_MACCFG1_SYNVRXEN  (1 << (31-28))  /* Sync Receive Enable  */
#define TSEC_MACCFG1_RXEN      (1 << (31-29))  /* Receive Enable       */
#define TSEC_MACCFG1_SYNVTXEN  (1 << (31-30))  /* Sync Transmit Enable */
#define TSEC_MACCFG1_TXEN      (1 << (31-31))  /* Transmit Enable      */

  /*
   * TSEC MACCFG2 bit positions
   */
#define TSEC_MACCFG2_PRELEN(n) (((n)&0x0f) << (31-19))  /* Preamble len*/

#define TSEC_MACCFG2_IFMODE_MSK (3 << (31-23)) /* mode mask            */
#define TSEC_MACCFG2_IFMODE_NIB (1 << (31-23)) /* nibble mode          */
#define TSEC_MACCFG2_IFMODE_BYT (2 << (31-23)) /* byte mode            */

#define TSEC_MACCFG2_HUGE_FRAME (1 << (31-26)) /* Huge Frame           */
#define TSEC_MACCFG2_LENGTH_CHK (1 << (31-27)) /* Length Check         */
#define TSEC_MACCFG2_PAD_CRC    (1 << (31-29)) /* MAC adds PAD/CRC     */
#define TSEC_MACCFG2_CRC_EN     (1 << (31-30)) /* CRC enable           */
#define TSEC_MACCFG2_FULLDUPLEX (1 << (31-31)) /* Full Duplex Mode     */

  /*
   * TSEC MIIMADD bit positions
   */
#define TSEC_MIIMADD_PHY(n)     (((n) & 0x3f)<<(31- 23))  /* PHY addr  */
#define TSEC_MIIMADD_REGADDR(n) (((n) & 0x3f)<<(31- 31))  /* PHY addr  */

  /*
   * TSEC MIIMCOM bit positions
   */
#define TSEC_MIIMCOM_SCAN       (1 << (31-30))     /* Scan command     */
#define TSEC_MIIMCOM_READ       (1 << (31-31))     /* Read command     */

  /*
   * TSEC MIIMIND bit positions
   */
#define TSEC_MIIMIND_NVAL       (1 << (31-29))     /* not valid        */
#define TSEC_MIIMIND_SCAN       (1 << (31-30))     /* Scan in progress */
#define TSEC_MIIMIND_BUSY       (1 << (31-31))     /* Acc. in progress */

  /*
   * TSEC ATTR bit positions
   */
#define TSEC_ATTR_RDSEN         (1 << (31-24))     /* read data snoop  */
#define TSEC_ATTR_RBDSEN        (1 << (31-25))     /* read BD snoop    */

typedef struct {
  volatile uint16_t  status;
  volatile uint16_t  length;
  volatile void     *buffer;
} PQBufferDescriptor_t;

/*
 * Bits in receive buffer descriptor status word
 */
#define BD_EMPTY           (1<<15)
#define BD_RO1             (1<<14)
#define BD_WRAP            (1<<13)
#define BD_INTERRUPT       (1<<12)
#define BD_LAST            (1<<11)
#define BD_CONTROL_CHAR    (1<<11)
#define BD_FIRST_IN_FRAME  (1<<10)
#define BD_MISS            (1<<8)
#define BD_BROADCAST       (1<<7)
#define BD_MULTICAST       (1<<6)
#define BD_LONG            (1<<5)
#define BD_NONALIGNED      (1<<4)
#define BD_SHORT           (1<<3)
#define BD_CRC_ERROR       (1<<2)
#define BD_OVERRUN         (1<<1)
#define BD_COLLISION       (1<<0)

/*
 * Bits in transmit buffer descriptor status word
 * Many bits have the same meaning as those in receiver buffer descriptors.
 */
#define BD_READY           (1<<15)
#define BD_PAD_CRC         (1<<14)
/* WRAP/Interrupt as in Rx BDs */
#define BD_TX_CRC          (1<<10)
#define BD_DEFER           (1<<9)
#define BD_TO1             (1<<8)
#define BD_HFE_            (1<<7)
#define BD_LATE_COLLISION  (1<<7)
#define BD_RETRY_LIMIT     (1<<6)
#define BD_RETRY_COUNT(x)  (((x)&0x3C)>>2)
#define BD_UNDERRUN        (1<<1)
#define BD_TXTRUNC         (1<<0)

struct rtems_bsdnet_ifconfig;

typedef struct {
  int unit_number;
  char *unit_name;
  volatile tsec_registers *reg_ptr;
  volatile tsec_registers *mdio_ptr;
  rtems_irq_number irq_num_tx;
  rtems_irq_number irq_num_rx;
  rtems_irq_number irq_num_err;
  int phy_default;
} tsec_config;

int tsec_driver_attach_detach(
  struct rtems_bsdnet_ifconfig *config,
  int attaching
);

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBCPU_POWERPC_TSEC_H */
