/**
 * @file tms570-pom.h
 * @ingroup tms570
 * @brief Parameter Overlay Module (POM) header file
 */

/*
 * Copyright (c) 2014 Pavel Pisa <pisa@cmp.felk.cvut.cz>
 *
 * Czech Technical University in Prague
 * Zikova 1903/4
 * 166 36 Praha 6
 * Czech Republic
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef LIBBSP_ARM_TMS570_POM_H
#define LIBBSP_ARM_TMS570_POM_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

#define TMS570_POM_REGIONS 32
#define TMS570_POM_GLBCTRL_ENABLE 0x000000a0a

/* Specification of memory size used for field REGSIZE of tms570_pom_region_t */
#define TMS570_POM_REGSIZE_DISABLED 0x0
#define TMS570_POM_REGSIZE_64B      0x1
#define TMS570_POM_REGSIZE_128B     0x2
#define TMS570_POM_REGSIZE_256B     0x3
#define TMS570_POM_REGSIZE_512B     0x4
#define TMS570_POM_REGSIZE_1KB      0x5
#define TMS570_POM_REGSIZE_2KB      0x6
#define TMS570_POM_REGSIZE_4KB      0x7
#define TMS570_POM_REGSIZE_8KB      0x8
#define TMS570_POM_REGSIZE_16KB     0x9
#define TMS570_POM_REGSIZE_32KB     0xa
#define TMS570_POM_REGSIZE_64KB     0xb
#define TMS570_POM_REGSIZE_128KB    0xc
#define TMS570_POM_REGSIZE_256KB    0xd

#define TMS570_POM_REGADDRMASK    ((1<<23)-1)

typedef struct tms570_pom_region_t {
  uint32_t PROGSTART;
  uint32_t OVLSTART;
  uint32_t REGSIZE;
  uint32_t res0;
} tms570_pom_region_t;

typedef struct tms570_pom_t {
  uint32_t GLBCTRL;                    /* 000h Global Control Register */
  uint32_t REV;                        /* 004h Revision ID */
  uint32_t CLKCTRL;                    /* 008h Clock Gate Control Register */
  uint32_t FLG;                        /* 00Ch Status Register */
  uint32_t reserved1[0x1f0/4];
  tms570_pom_region_t REG[TMS570_POM_REGIONS]; /* 200h Program Regions */
  uint32_t reserved2[0xb00/4];
  uint32_t ITCTRL;                     /* F00h Integration Control Register */
  uint32_t reserved3[0x09c/4];
  uint32_t CLAIMSET;                   /* FA0h Claim Set Register */
  uint32_t CLAIMCLR;                   /* FA4h Claim Clear Register */
  uint32_t reserved4[0x008/4];
  uint32_t LOCKACCESS;                 /* FB0h Lock Access Register */
  uint32_t LOCKSTATUS;                 /* FB4h Lock Status Register */
  uint32_t AUTHSTATUS;                 /* FB8h Authentication Status Register */
  uint32_t reserved5[0x00c/4];
  uint32_t DEVID;                      /* FC8h Device ID Register */
  uint32_t DEVTYPE;                    /* FCCh Device Type Register */
  uint32_t PERIPHERALID4;              /* FD0h Peripheral ID 4 Register */
  uint32_t PERIPHERALID5;              /* FD4h Peripheral ID 5 Register */
  uint32_t PERIPHERALID6;              /* FD8h Peripheral ID 6 Register */
  uint32_t PERIPHERALID7;              /* FDCh Peripheral ID 7 Register */
  uint32_t PERIPHERALID0;              /* FE0h Peripheral ID 0 Register */
  uint32_t PERIPHERALID1;              /* FE4h Peripheral ID 1 Register */
  uint32_t PERIPHERALID2;              /* FE8h Peripheral ID 2 Register */
  uint32_t PERIPHERALID3;              /* FECh Peripheral ID 3 Register */
  uint32_t COMPONENTID0;               /* FF0h Component ID 0 Register */
  uint32_t COMPONENTID1;               /* FF4h Component ID 1 Register */
  uint32_t COMPONENTID2;               /* FF8h Component ID 2 Register */
  uint32_t COMPONENTID3;               /* FFCh Component ID 3 Register */
} tms570_pom_t;

#define TMS570_POM (*(volatile tms570_pom_t*)0xffa04000)

int mem_dump(void *buf, unsigned long start, unsigned long len, int blen);
void tms570_pom_remap(void);

/** @} */

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* LIBBSP_ARM_TMS570_POM_H */
