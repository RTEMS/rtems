/**
 * @file
 *
 * @ingroup QorIQ
 *
 * @brief QorIQ Configuration, Control and Status Registers.
 */

/*
 * Copyright (c) 2010 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Obere Lagerstr. 30
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.com/license/LICENSE.
 */

#ifndef LIBBSP_POWERPC_QORIQ_QORIQ_H
#define LIBBSP_POWERPC_QORIQ_QORIQ_H

#include <bsp/tsec.h>
#include <bsp/utility.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define QORIQ_FILL(a, b, s) uint8_t reserved_ ## b [b - a - sizeof(s)]
#define QORIQ_RESERVE(a, b) uint8_t reserved_ ## b [b - a]

typedef struct {
	uint32_t ccsrbar;
	uint32_t reserved_0;
	uint32_t altcbar;
	uint32_t reserved_1;
	uint32_t altcar;
	uint32_t reserved_2 [3];
	uint32_t bptr;
} qoriq_lcc;

#define CCSRBAR_BASE_ADDR(val) BSP_BFLD32(val, 8, 23)
#define CCSRBAR_BASE_ADDR_GET(reg) BSP_BFLD32GET(reg, 8, 23)
#define CCSRBAR_BASE_ADDR_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 23)

#define ALTCBAR_BASE_ADDR(val) BSP_BFLD32(val, 8, 23)
#define ALTCBAR_BASE_ADDR_GET(reg) BSP_BFLD32GET(reg, 8, 23)
#define ALTCBAR_BASE_ADDR_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 23)

#define ALTCAR_EN BSP_BBIT32(0)
#define ALTCAR_TRGT_ID(val) BSP_BFLD32(val, 8, 11)
#define ALTCAR_TRGT_ID_GET(reg) BSP_BFLD32GET(reg, 8, 11)
#define ALTCAR_TRGT_ID_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 11)

#define BPTR_EN BSP_BBIT32(0)
#define BPTR_BOOT_PAGE(val) BSP_BFLD32(val, 8, 31)
#define BPTR_BOOT_PAGE_GET(reg) BSP_BFLD32GET(reg, 8, 31)
#define BPTR_BOOT_PAGE_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 31)

typedef struct {
	uint32_t bar;
	uint32_t reserved_0;
	uint32_t ar;
	uint32_t reserved_1 [5];
} qoriq_law;

#define LAWBAR_BASE_ADDR(val) BSP_BFLD32(val, 8, 31)
#define LAWBAR_BASE_ADDR_GET(reg) BSP_BFLD32GET(reg, 8, 31)
#define LAWBAR_BASE_ADDR_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 31)

#define LAWAR_EN BSP_BBIT32(0)
#define LAWAR_TRGT(val) BSP_BFLD32(val, 8, 11)
#define LAWAR_TRGT_GET(reg) BSP_BFLD32GET(reg, 8, 11)
#define LAWAR_TRGT_SET(reg, val) BSP_BFLD32SET(reg, val, 8, 11)
#define LAWAR_SIZE(val) BSP_BFLD32(val, 26, 31)
#define LAWAR_SIZE_GET(reg) BSP_BFLD32GET(reg, 26, 31)
#define LAWAR_SIZE_SET(reg, val) BSP_BFLD32SET(reg, val, 26, 31)

typedef struct {
} qoriq_ecm;

typedef struct {
} qoriq_ddr_controller;

typedef struct {
} qoriq_i2c;

typedef struct {
} qoriq_uart;

typedef struct {
} qoriq_local_bus;

typedef struct {
} qoriq_spi;

typedef struct {
} qoriq_pci_express;

typedef struct {
} qoriq_gpio;

typedef struct {
} qoriq_tdm;

typedef struct {
} qoriq_l2_cache;

typedef struct {
} qoriq_dma;

typedef struct {
  QORIQ_RESERVE(0x000, 0x100);
  uint16_t caplength;
  uint16_t hciversion;
  uint32_t hcsparams;
  uint32_t hccparams;
  QORIQ_RESERVE(0x10c, 0x120);
  uint32_t dciversion;
  uint32_t dccparams;
  QORIQ_RESERVE(0x128, 0x140);
  uint32_t usbcmd;
  uint32_t usbsts;
  uint32_t usbintr;
  uint32_t frindex;
  QORIQ_RESERVE(0x150, 0x154);
  union {
    uint32_t periodiclistbase;
    uint32_t deviceaddr;
  } perbase_devaddr;
  union {
    uint32_t asynclistaddr;
    uint32_t addr;
  } async_addr;
  QORIQ_RESERVE(0x15c, 0x160);
  uint32_t burstsize;
  uint32_t txfilltuning;
  QORIQ_RESERVE(0x168, 0x170);
  uint32_t viewport;
  QORIQ_RESERVE(0x174, 0x180);
  uint32_t configflag;
  uint32_t portsc1;
  QORIQ_RESERVE(0x188, 0x1a8);
  uint32_t usbmode;
  uint32_t endptsetupstat;
  uint32_t endpointprime;
  uint32_t endptflush;
  uint32_t endptstatus;
  uint32_t endptcomplete;
  uint32_t endptctrl[6];
  QORIQ_RESERVE(0x1d8, 0x400);
  uint32_t snoop1;
  uint32_t snoop2;
  uint32_t age_cnt_thresh;
  uint32_t pri_ctrl;
  uint32_t si_ctrl;
  QORIQ_RESERVE(0x414, 0x500);
  uint32_t control;
} qoriq_usb;

typedef struct {
} qoriq_tdm_dma;

typedef struct {
  uint32_t dsaddr;
  uint32_t blkattr;
  uint32_t cmdarg;
  uint32_t xfertyp;
  uint32_t cmdrsp0;
  uint32_t cmdrsp1;
  uint32_t cmdrsp2;
  uint32_t cmdrsp3;
  uint32_t datport;
  uint32_t prsstat;
  uint32_t proctl;
  uint32_t sysctl;
  uint32_t irqstat;
  uint32_t irqstaten;
  uint32_t irqsigen;
  uint32_t autoc12err;
  uint32_t hostcapblt;
  uint32_t wml;
  QORIQ_FILL(0x00044, 0x00050, uint32_t);
  uint32_t fevt;
  QORIQ_FILL(0x00050, 0x000fc, uint32_t);
  uint32_t hostver;
  QORIQ_FILL(0x000fc, 0x0040c, uint32_t);
  uint32_t dcr;
} qoriq_esdhc;

typedef struct {
} qoriq_sec;

typedef struct {
  uint32_t reg;
  QORIQ_FILL(0x00000, 0x00010, uint32_t);
} qoriq_pic_reg;

typedef struct {
  uint32_t ccr;
  QORIQ_FILL(0x00000, 0x00010, uint32_t);
  uint32_t bcr;
  QORIQ_FILL(0x00010, 0x00020, uint32_t);
  uint32_t vpr;
  QORIQ_FILL(0x00020, 0x00030, uint32_t);
  uint32_t dr;
  QORIQ_FILL(0x00030, 0x00040, uint32_t);
} qoriq_pic_global_timer;

#define GTCCR_TOG BSP_BBIT32(0)
#define GTCCR_COUNT_GET(reg) BSP_BFLD32GET(reg, 1, 31)

#define GTBCR_CI BSP_BBIT32(0)
#define GTBCR_COUNT(val) BSP_BFLD32(val, 1, 31)
#define GTBCR_COUNT_GET(reg) BSP_BFLD32GET(reg, 1, 31)
#define GTBCR_COUNT_SET(reg, val) BSP_BFLD32SET(reg, val, 1, 31)

typedef struct {
  uint32_t misc;
  QORIQ_FILL(0x00000, 0x00010, uint32_t);
  uint32_t internal [2];
  QORIQ_FILL(0x00010, 0x00020, uint32_t [2]);
} qoriq_pic_bit_field;

typedef struct {
  uint32_t vpr;
  QORIQ_FILL(0x00000, 0x00010, uint32_t);
  uint32_t dr;
  QORIQ_FILL(0x00010, 0x00020, uint32_t);
} qoriq_pic_src_cfg;

typedef struct {
  QORIQ_RESERVE(0x00000, 0x00040);
  qoriq_pic_reg ipidr [4];
  uint32_t ctpr;
  QORIQ_FILL(0x00080, 0x00090, uint32_t);
  uint32_t whoami;
  QORIQ_FILL(0x00090, 0x000a0, uint32_t);
  uint32_t iack;
  QORIQ_FILL(0x000a0, 0x000b0, uint32_t);
  uint32_t eoi;
  QORIQ_FILL(0x000b0, 0x01000, uint32_t);
} qoriq_pic_per_cpu;

typedef struct {
  uint32_t brr1;
  QORIQ_FILL(0x00000, 0x00010, uint32_t);
  uint32_t brr2;
  QORIQ_FILL(0x00010, 0x00040, uint32_t);
  qoriq_pic_reg ipidr [4];
  uint32_t ctpr;
  QORIQ_FILL(0x00080, 0x00090, uint32_t);
  uint32_t whoami;
  QORIQ_FILL(0x00090, 0x000a0, uint32_t);
  uint32_t iack;
  QORIQ_FILL(0x000a0, 0x000b0, uint32_t);
  uint32_t eoi;
  QORIQ_FILL(0x000b0, 0x01000, uint32_t);
  uint32_t frr;
  QORIQ_FILL(0x01000, 0x01020, uint32_t);
  uint32_t gcr;
  QORIQ_FILL(0x01020, 0x01080, uint32_t);
  uint32_t vir;
  QORIQ_FILL(0x01080, 0x01090, uint32_t);
  uint32_t pir;
  QORIQ_FILL(0x01090, 0x010a0, uint32_t);
  qoriq_pic_reg ipivpr [4];
  uint32_t svr;
  QORIQ_FILL(0x010e0, 0x010f0, uint32_t);
  uint32_t tfrra;
  QORIQ_FILL(0x010f0, 0x01100, uint32_t);
  qoriq_pic_global_timer gta [4];
  QORIQ_RESERVE(0x01200, 0x01300);
  uint32_t tcra;
  QORIQ_FILL(0x01300, 0x01308, uint32_t);
  uint32_t erqsr;
  QORIQ_FILL(0x01308, 0x01310, uint32_t);
  qoriq_pic_bit_field irqsr;
  qoriq_pic_bit_field cisr;
  qoriq_pic_bit_field pm [4];
  QORIQ_RESERVE(0x013d0, 0x01400);
  qoriq_pic_reg msgr03 [4];
  QORIQ_RESERVE(0x01440, 0x01500);
  uint32_t mer03;
  QORIQ_FILL(0x01500, 0x01510, uint32_t);
  uint32_t msr03;
  QORIQ_FILL(0x01510, 0x01600, uint32_t);
  qoriq_pic_reg msir [8];
  QORIQ_RESERVE(0x01680, 0x01720);
  uint32_t msisr;
  QORIQ_FILL(0x01720, 0x01740, uint32_t);
  uint32_t msiir;
  QORIQ_FILL(0x01740, 0x020f0, uint32_t);
  uint32_t tfrrb;
  QORIQ_FILL(0x020f0, 0x02100, uint32_t);
  qoriq_pic_global_timer gtb [4];
  QORIQ_RESERVE(0x02200, 0x02300);
  uint32_t tcrb;
  QORIQ_FILL(0x02300, 0x02400, uint32_t);
  qoriq_pic_reg msgr47 [4];
  QORIQ_RESERVE(0x02440, 0x02500);
  uint32_t mer47;
  QORIQ_FILL(0x02500, 0x02510, uint32_t);
  uint32_t msr47;
  QORIQ_FILL(0x02510, 0x10000, uint32_t);
  qoriq_pic_src_cfg ei [12];
  QORIQ_RESERVE(0x10180, 0x10200);
  qoriq_pic_src_cfg ii [64];
  QORIQ_RESERVE(0x10a00, 0x11600);
  qoriq_pic_src_cfg mi [8];
  QORIQ_RESERVE(0x11700, 0x11c00);
  qoriq_pic_src_cfg msi [8];
  QORIQ_RESERVE(0x11d00, 0x20000);
  qoriq_pic_per_cpu per_cpu [2];
} qoriq_pic;

#define GTTCR_ROVR(val) BSP_BFLD32(val, 5, 7)
#define GTTCR_ROVR_GET(reg) BSP_BFLD32GET(reg, 5, 7)
#define GTTCR_ROVR_SET(reg, val) BSP_BFLD32SET(reg, val, 5, 7)
#define GTTCR_RTM BSP_BBIT32(15)
#define GTTCR_CLKR(val) BSP_BFLD32(val, 22, 23)
#define GTTCR_CLKR_GET(reg) BSP_BFLD32GET(reg, 22, 23)
#define GTTCR_CLKR_SET(reg, val) BSP_BFLD32SET(reg, val, 22, 23)
#define GTTCR_CASC(val) BSP_BFLD32(val, 29, 31)
#define GTTCR_CASC_GET(reg) BSP_BFLD32GET(reg, 29, 31)
#define GTTCR_CASC_SET(reg, val) BSP_BFLD32SET(reg, val, 29, 31)

typedef struct {
} qoriq_serial_rapid_io;

typedef struct {
} qoriq_global_utilities;

typedef struct {
} qoriq_performance_monitor;

typedef struct {
} qoriq_debug_watchpoint;

typedef struct {
} qoriq_serdes;

typedef struct {
} qoriq_boot_rom;

typedef struct {
  qoriq_lcc lcc;
  QORIQ_FILL(0x00000, 0x00c08, qoriq_lcc);
  qoriq_law law [12];
  QORIQ_FILL(0x00c08, 0x01000, qoriq_law [12]);
  qoriq_ecm ecm;
  QORIQ_FILL(0x01000, 0x02000, qoriq_ecm);
  qoriq_ddr_controller ddr_controller;
  QORIQ_FILL(0x02000, 0x03000, qoriq_ddr_controller);
  qoriq_i2c i2c;
  QORIQ_FILL(0x03000, 0x04000, qoriq_i2c);
  QORIQ_RESERVE(0x04000, 0x04500);
  qoriq_uart uart_0;
  QORIQ_FILL(0x04500, 0x04600, qoriq_uart);
  qoriq_uart uart_1;
  QORIQ_FILL(0x04600, 0x04700, qoriq_uart);
  QORIQ_RESERVE(0x04700, 0x05000);
  qoriq_local_bus local_bus;
  QORIQ_FILL(0x05000, 0x06000, qoriq_local_bus);
  qoriq_spi spi;
  QORIQ_FILL(0x06000, 0x07000, qoriq_spi);
  QORIQ_RESERVE(0x07000, 0x08000);
  qoriq_pci_express pci_express_3;
  QORIQ_FILL(0x08000, 0x09000, qoriq_pci_express);
  qoriq_pci_express pci_express_2;
  QORIQ_FILL(0x09000, 0x0a000, qoriq_pci_express);
  qoriq_pci_express pci_express_1;
  QORIQ_FILL(0x0a000, 0x0b000, qoriq_pci_express);
  QORIQ_RESERVE(0x0b000, 0x0c000);
  qoriq_dma dma_2;
  QORIQ_FILL(0x0c000, 0x0d000, qoriq_dma);
  QORIQ_RESERVE(0x0d000, 0x0f000);
  qoriq_gpio gpio;
  QORIQ_FILL(0x0f000, 0x10000, qoriq_gpio);
  QORIQ_RESERVE(0x10000, 0x16000);
  qoriq_tdm tdm;
  QORIQ_FILL(0x16000, 0x17000, qoriq_tdm);
  QORIQ_RESERVE(0x17000, 0x20000);
  qoriq_l2_cache l2_cache;
  QORIQ_FILL(0x20000, 0x21000, qoriq_l2_cache);
  qoriq_dma dma_1;
  QORIQ_FILL(0x21000, 0x22000, qoriq_dma);
  qoriq_usb usb_1;
  QORIQ_FILL(0x22000, 0x23000, qoriq_usb);
  qoriq_usb usb_2;
  QORIQ_FILL(0x23000, 0x24000, qoriq_usb);
  tsec_registers tsec_1;
  QORIQ_FILL(0x24000, 0x25000, tsec_registers);
  tsec_registers tsec_2;
  QORIQ_FILL(0x25000, 0x26000, tsec_registers);
  tsec_registers tsec_3;
  QORIQ_FILL(0x26000, 0x27000, tsec_registers);
  QORIQ_RESERVE(0x27000, 0x2c000);
  qoriq_tdm_dma tdm_dma;
  QORIQ_FILL(0x2c000, 0x2d000, qoriq_tdm_dma);
  QORIQ_RESERVE(0x2d000, 0x2e000);
  qoriq_esdhc esdhc;
  QORIQ_FILL(0x2e000, 0x2f000, qoriq_esdhc);
  QORIQ_RESERVE(0x2f000, 0x30000);
  qoriq_sec sec;
  QORIQ_FILL(0x30000, 0x31000, qoriq_sec);
  QORIQ_RESERVE(0x31000, 0x40000);
  qoriq_pic pic;
  QORIQ_FILL(0x40000, 0x80000, qoriq_pic);
  QORIQ_RESERVE(0x80000, 0xb0000);
  tsec_registers tsec_1_group_0;
  QORIQ_FILL(0xb0000, 0xb1000, tsec_registers);
  tsec_registers tsec_2_group_0;
  QORIQ_FILL(0xb1000, 0xb2000, tsec_registers);
  tsec_registers tsec_3_group_0;
  QORIQ_FILL(0xb2000, 0xb3000, tsec_registers);
  QORIQ_RESERVE(0xb3000, 0xb4000);
  tsec_registers tsec_1_group_1;
  QORIQ_FILL(0xb4000, 0xb5000, tsec_registers);
  tsec_registers tsec_2_group_1;
  QORIQ_FILL(0xb5000, 0xb6000, tsec_registers);
  tsec_registers tsec_3_group_1;
  QORIQ_FILL(0xb6000, 0xb7000, tsec_registers);
  QORIQ_RESERVE(0xb7000, 0xc0000);
  qoriq_serial_rapid_io serial_rapid_io;
  QORIQ_FILL(0xc0000, 0xe0000, qoriq_serial_rapid_io);
  qoriq_global_utilities global_utilities;
  QORIQ_FILL(0xe0000, 0xe1000, qoriq_global_utilities);
  qoriq_performance_monitor performance_monitor;
  QORIQ_FILL(0xe1000, 0xe2000, qoriq_performance_monitor);
  qoriq_debug_watchpoint debug_watchpoint;
  QORIQ_FILL(0xe2000, 0xe3000, qoriq_debug_watchpoint);
  qoriq_serdes serdes;
  QORIQ_FILL(0xe3000, 0xe4000, qoriq_serdes);
  QORIQ_RESERVE(0xe4000, 0xf0000);
  qoriq_boot_rom boot_rom;
  QORIQ_FILL(0xf0000, 0x100000, qoriq_boot_rom);
} qoriq_ccsr;

extern volatile qoriq_ccsr qoriq;

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_POWERPC_QORIQ_QORIQ_H */
