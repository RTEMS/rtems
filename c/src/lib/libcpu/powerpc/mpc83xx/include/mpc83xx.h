#ifndef _MPC83XX_MPC83XX_H
#define _MPC83XX_MPC83XX_H

#include <bspopts.h>

#if MPC83XX_CHIP_TYPE == 8343
#define M83xx_HAS_PCI      TRUE
#define M83xx_HAS_USB1     TRUE
#elif MPC83XX_CHIP_TYPE == 8347
#define M83xx_HAS_PCI      TRUE
#define M83xx_HAS_USB1     TRUE
#define M83xx_HAS_USB2     TRUE
#elif MPC83XX_CHIP_TYPE == 8349
#define M83xx_HAS_PCI      TRUE
#define M83xx_HAS_WIDE_PCI TRUE
#define M83xx_HAS_USB1     TRUE
#define M83xx_HAS_USB2     TRUE
#elif MPC83XX_CHIP_TYPE == 8360
#define M83xx_HAS_PCI      TRUE
#define M83xx_HAS_QE       TRUE
#endif

#if !defined(ASM)

#include <rtems.h>
#include <bsp/tsec.h>

/* Offset Register Access Reset Section/Page */
/* System Configuration Registers */
typedef struct m83xxSysConRegisters_ {
  volatile uint32_t immrbar;              /* 0x0_00000 Internal memory map base address register R/W 0xFF40_0000 5.2.4.1/5-5 */
  uint8_t reserved0_0004[0x00008-0x00004];/* 0x0_0004 Reserved, should be cleared */
  volatile uint32_t altcbar;              /* 0x0_0008 Alternate configuration base address register R/W 0x0000_0000 5.2.4.2/5-7 */
  uint8_t reserved0_000C[0x00020-0x0000C];/* 0x0_000C--0x0_001C Reserved, should be cleared */
  volatile uint32_t lblawbar0;            /* 0x0_0020 LBC local access window 0 base address register R/W 0x0000_00001 5.2.4.3/5-7 */
  volatile uint32_t lblawar0;             /* 0x0_0024 LBC local access window 0 attribute register R/W 0x0000_00002 5.2.4.4/5-8 */
  volatile uint32_t lblawbar1;            /* 0x0_0028 LBC local access window 1 base address register R/W 0x0000_0000 5.2.4.3/5-7 */
  volatile uint32_t lblawar1;             /* 0x0_002C LBC local access window 1 attribute register R/W 0x0000_0000 5.2.4.4/5-8 */
  volatile uint32_t lblawbar2;            /* 0x0_0030 LBC local access window 2 base address register R/W 0x0000_0000 5.2.4.3/5-7 */
  volatile uint32_t lblawar2;             /* 0x0_0034 LBC local access window 2 attribute register R/W 0x0000_0000 5.2.4.4/5-8 */
  volatile uint32_t lblawbar3;            /* 0x0_0038 LBC local access window 3 base address register R/W 0x0000_0000 5.2.4.3/5-7 */
  volatile uint32_t lblawar3;             /* 0x0_003C LBC local access window 3 attribute register R/W 0x0000_0000 5.2.4.4/5-8 */
  uint8_t reserved0_0040[0x00060-0x00040];/* 0x0_0040--0x0_005C  Reserved, should be cleared    */
  volatile uint32_t pcilawbar0;           /* 0x0_0060 PCI local access window0 base address register R/W 0x0000_00003 5.2.4.5/5-9 */
  volatile uint32_t pcilawar0;            /* 0x0_0064 PCI local access window0 attribute register R/W 0x0000_00004 5.2.4.6/5-10 */
  volatile uint32_t pcilawbar1;           /* 0x0_0068 PCI local access window1 base address register R/W 0x0000_0000 5.2.4.5/5-9 */
  volatile uint32_t pcilawar1;            /* 0x0_006C PCI local access window1 attribute register R/W 0x0000_0000 5.2.4.6/5-10 */
  uint8_t reserved0_0070[0x000A0-0x00070];/* 0x0_0070--0x0_009C  Reserved, should be cleared    */
  volatile uint32_t ddrlawbar0;           /* 0x0_00A0 DDR local access window0 base address register R/W 0x0000_00005 5.2.4.7/5-12 */
  volatile uint32_t ddrlawar0;            /* 0x0_00A4 DDR local access window0 attribute register R/W 0x0000_00006 5.2.4.8/5-13 */
  volatile uint32_t ddrlawbar1;           /* 0x0_00A8 DDR local access window1 base address register R/W 0x0000_0000 5.2.4.7/5-12 */
  volatile uint32_t ddrlawar1;            /* 0x0_00AC DDR local access window1 attribute register R/W 0x0000_0000 5.2.4.8/5-13 */
  uint8_t reserved0_00B0[0x00100-0x000B0];/* 0x0_00B0--0x0_0100  Reserved, should be cleared    */
  volatile uint32_t sgprl;                /* 0x0_0100 general purpose register low (SGPRL) R/W 0x0000_0000 5.3.2.1/5-17 */
  volatile uint32_t sgprh;                /* 0x0_0104 general purpose register high (SGPRH) R/W 0x0000_0000 5.3.2.2/5-17 */
  volatile uint32_t spridr;               /* 0x0_0108 part and revision ID register (SPRIDR) R 0x0000_0000 5.3.2.3/5-18 */
  uint8_t reserved0_010C[0x00110-0x0010C];/* 0x0_010C--0x0_0110  Reserved, should be cleared    */
  volatile uint32_t spcr;                 /* 0x0_0110 priority configuration register (SPCR) R/W 0x0000_0000 5.3.2.4/5-19 */
  volatile uint32_t sicrl;                /* 0x0_0114 I/O configuration register low (SICRL) R/W 0x0000_0000 5.3.2.5/5-21 */
  volatile uint32_t sicrh;                /* 0x0_0118 I/O configuration register high (SICRH) R/W 0x0000_00007 5.3.2.6/5-24 */
  uint8_t reserved0_011C[0x00128-0x0011C];/* 0x0_011C--0x0_0128  Reserved    */
  volatile uint32_t ddrcdr;               /* 0x0_0128 control driver register (DDRCDR) R/W 0x7304_0001 5.3.2.8/5-28 */
  volatile uint32_t ddrdsr;               /* 0x0_012C debug status register (DDRDSR) R 0x3300_0000 5.3.2.9/5-30 */
  uint8_t reserved0_0130[0x00200-0x00130];/* 0x0_0130--0x0_01FC  Reserved    */
} m83xxSysConRegisters_t;
#define M83xx_SYSCON_SPCR_TBEN (1 << (31-9))

/* Watchdog Timer (WDT) Registers */
typedef struct m83xxWDTRegisters_ {
  uint8_t reserved0_0200[0x00204-0x00200];/* 0x0_0200  Reserved, should be cleared    */
  volatile uint32_t swcrr;                /* 0x0_0204 System watchdog control register R/W 0x0000_0007 5.4.4.1/5-33 */
  volatile uint32_t swcnr;                /* 0x0_0208 System watchdog count register R 0x0000_FFFF 5.4.4.2/5-34 */
  uint8_t reserved0_020C[(0x0020E)-0x0020C];/* 0x0_020C  Reserved, should be cleared    */
  volatile uint16_t swsrr;                /* 0x0_020E System watchdog service register R/W 0x0000_0000 5.4.4.3/5-34 */
} m83xxWDTRegisters_t;

/* Real Time Clock Module Registers (RTC) */
typedef struct m83xxRTCRegisters_ {
  volatile uint32_t rtcnr;                /* 0x0_0300 Real time counter control register R/W 0x0000_0000 5.5.5.1/5-40 */
  volatile uint32_t rtldr;                /* 0x0_0304 Real time counter load register R/W 0x0000_0000 5.5.5.2/5-41 */
  volatile uint32_t rtpsr;                /* 0x0_0308 Real time counter prescale register R/W 0x0000_0000 5.5.5.3/5-41 */
  volatile uint32_t rtctr;                /* 0x0_030C Real time counter register R 0x0000_0000 5.5.5.4/5-42 */
  volatile uint32_t rtevr;                /* 0x0_0310 Real time counter event register R/W 0x0000_0000 5.5.5.5/5-42 */
  volatile uint32_t rtalr;                /* 0x0_0314 Real time counter alarm register R/W 0xFFFF_FFFF 5.5.5.6/5-43 */
  uint8_t reserved0_0314[0x00320-0x00318];/* 0x0_0318--0x0_031F Reserved; should be cleared */
} m83xxRTCRegisters_t;

  /* Periodic Interval Timer (PIT) Registers */
typedef struct m83xxPITRegisters_ {
  volatile uint32_t ptcnr;                 /* 0x0_0400 Periodic interval timer control register R/W 0x0000_0000 5.6.5.1/5-47 */
  volatile uint32_t ptldr;                 /* 0x0_0404 Periodic interval timer load register R/W 0x0000_0000 5.6.5.2/5-48 */
  volatile uint32_t ptpsr;                 /* 0x0_0408 Periodic interval timer prescale register R/W 0x0000_0000 5.6.5.3/5-49 */
  volatile uint32_t ptctr;                 /* 0x0_040C Periodic interval timer counter register R 0x0000_0000 5.6.5.4/5-49 */
  volatile uint32_t ptevr;                 /* 0x0_0410 Periodic interval timer event register R/W 0x0000_0000 5.6.5.5/5-50 */
  uint8_t reserved0_0414[0x00500-0x00414]; /* 0x0_0414--0x0_041F Reserved, should be cleared    */
} m83xxPITRegisters_t;

  /* Global Timers Module 1/2 */
#define M83xxGTIdx(n)  (n&3)
#define M83xxGTLowIdx(n)  (n&1)
#define M83xxGTHighIdx(n) (((n)>>1)&1)
#define M83xxGTModIdx(n)  (((n)>>2)&1)

#define M83xxGTIdxCnt  (4)
#define M83xxGTLowCnt  (2)
#define M83xxGTHighCnt (2)
#define M83xxGTModCnt  (2)

typedef struct m83xxGTMRegisters_ {
  struct {
    volatile uint8_t  reg;                     /* 0x0_0500 Timer 1+2/3+4 global timers configuration register R/W 0x00 5.7.5.1/5-57 */
    uint8_t reserved0_0501[0x00504-0x00501];   /* 0x0_0501--0x0_0503 Reserved, should be cleared */
  } gtcfr[M83xxGTHighCnt];
  uint8_t reserved0_0508[0x00510-0x00508];     /* 0x0_0508--0x0_050f Reserved, should be cleared */
  struct {
    volatile uint16_t gtmdr[M83xxGTLowCnt];    /* 0x0_0510 Timer 1/2 global timers mode register R/W 0x0000 5.7.5.2/5-60 */
    volatile uint16_t gtrfr[M83xxGTLowCnt];    /* 0x0_0514 Timer 1/2 global timers reference register R/W 0x0000 5.7.5.3/5-62 */
    volatile uint16_t gtcpr[M83xxGTLowCnt];    /* 0x0_0518 Timer 1/2 global timers capture register R/W 0x0000 5.7.5.4/5-62 */
    volatile uint16_t gtcnr[M83xxGTLowCnt];    /* 0x0_051C Timer 1/2 global timers counter register R/W 0x0000 5.7.5.5/5-63 */
  } gt_tim_regs[M83xxGTHighCnt];
  volatile uint16_t gtevr[M83xxGTIdxCnt];      /* 0x0_0530 Timer 1-4 global timers event register Special 0x0000 5.7.5.6/5-63 */
  volatile uint16_t gtpsr[M83xxGTIdxCnt];      /* 0x0_0538 Timer 1-4 global timers prescale register R/W 0x0003 5.7.5.7/5-64 */
  uint8_t reserved0_0540[0x00600-0x00540];     /* 0x0_0540--0x0_05fc Reserved */
} m83xxGTMRegisters_t;

  /* Integrated Programmable Interrupt Controller (IPIC) */
typedef struct m83xxIPICRegisters_ {
  volatile uint32_t sicfr;                 /* 0x0_0700 System global interrupt configuration register R/W 0x0000_0000 8.5.1/8-8 */
  volatile uint32_t sivcr;                 /* 0x0_0704 System global interrupt vector register R 0x0000_0000 8.5.2/8-9 */
  volatile uint32_t sipnr[2];              /* 0x0_0708 System internal interrupt pending register H/L R 0x0000_0000 8.5.3/8-11 */
  volatile uint32_t siprr[4];              /* 0x0_0710 System internal interrupt group A-D priority register R/W 0x0530_9770 8.5.4/8-14 */
  volatile uint32_t simsr[2];              /* 0x0_0720 System internal interrupt mask register H/L R/W 0x0000_0000 8.5.6/8-15 */
  uint8_t reserved0_0728[0x0072C-0x00728]; /* 0x0_072C--0x0_0728 Reserved, should be cleared */
  volatile uint32_t sepnr;                 /* 0x0_072C System external interrupt pending register R/W Special 8.5.8/8-18 */
  volatile uint32_t smprr[2];              /* 0x0_0730 System mixed interrupt group A/B priority register R/W 0x0530_9770 8.5.9/8-18 */
  volatile uint32_t semsr;                 /* 0x0_0738 System external interrupt mask register R/W 0x0000_0000 8.5.11/8-20 */
  volatile uint32_t secnr;                 /* 0x0_073C System external interrupt control register R/W 0x0000_0000 8.5.12/8-21 */
  volatile uint32_t sersr;                 /* 0x0_0740 System error status register R/W 0x0000_0000 8.5.13/8-22 */
  volatile uint32_t sermr;                 /* 0x0_0744 System error mask register R/W $ 8.5.14/8-23 */
  volatile uint32_t sercr;                 /* 0x0_0748 System error control register R/W 0x0000_0000 8.5.15/8-24 */
  uint8_t reserved0_074C[0x00750-0x0074C]; /* 0x0_074C--0x0_074F Reserved, should be cleared */
  volatile uint32_t sifcr[2];              /* 0x0_0750 System internal interrupt force register H/L R/W 0x0000_0000 8.5.16/8-25 */
  volatile uint32_t sefcr;                 /* 0x0_0758 System external interrupt force register R/W 0x0000_0000 8.5.17/8-26 */
  volatile uint32_t serfr;                 /* 0x0_075C System error force register R/W 0x0000_0000 8.5.18/8-26 */
  volatile uint32_t scvcr;                 /* 0x0_0760 System critical interrupt vector register R 0x0000_0000 8.5.19/8-27 */
  volatile uint32_t smvcr;                 /* 0x0_0764 System management interrupt vector register R 0x0000_0000 8.5.20/8-27 */
  uint8_t reserved0_0760[0x00800-0x00768]; /* 0x0_0768--0x0_07FF Reserved, should be cleared */
} m83xxIPICRegisters_t;

/* get vector number from vector register content */
#define MPC83xx_VCR_TO_VEC(regval) ((regval) & 0x7f)


  /* System Arbiter Registers */
typedef struct m83xxARBRegisters_ {
  volatile uint32_t acr;                   /* 0x0_0800 Arbiter configuration register R/W 0x0000_0000 6.2.1/6-2 */
  volatile uint32_t atr;                   /* 0x0_0804 Arbiter timers register R/W 0x00FF_00FF 6.2.2/6-4 */
  uint8_t reserved0_0808[0x0080C-0x00808]; /* 0x0_0808 Reserved, should be cleared R 0x0000_0000  */
  volatile uint32_t aer;                   /* 0x0_080C Arbiter event register R/W 0x0000_0000 6.2.3/6-5 */
  volatile uint32_t aidr;                  /* 0x0_0810 Arbiter interrupt definition register R/W 0x0000_0000 6.2.4/6-6 */
  volatile uint32_t amr;                   /* 0x0_0814 Arbiter mask register R/W 0x0000_0000 6.2.5/6-7 */
  volatile uint32_t aeatr;                 /* 0x0_0818 Arbiter event attributes register R 0x0000_0000 6.2.6/6-7 */
  volatile uint32_t aeadr;                 /* 0x0_081C Arbiter event address register R 0x0000_0000 6.2.7/6-9 */
  volatile uint32_t aerr;                  /* 0x0_0820 Arbiter event response register R/W 0x0000_0000 6.2.8/6-10 */
  uint8_t reserved0_0824[0x00900-0x00824]; /* 0x0_0824--0x0_08FF Reserved, should be cleared */
} m83xxARBRegisters_t;

  /* Reset Module */
typedef struct m83xxRESRegisters_ {
  volatile uint32_t rcwlr;                 /* 0x0_0900 Reset configuration word low register R 0x0000_0000 4.5.1.1/4-32 */
  volatile uint32_t rcwhr;                 /* 0x0_0904 Reset configuration word high register R 0x0000_0000 4.5.1.2/4-32 */
  uint8_t reserved0_0908[0x00910-0x00908]; /* 0x0_0908--0x0_090C Reserved, should be cleared */
  volatile uint32_t rsr;                   /* 0x0_0910 Reset status register R/W 0x0000_0000 4.5.1.3/4-33 */
  volatile uint32_t rmr;                   /* 0x0_0914 Reset mode register R/W 0x0000_0000 4.5.1.4/4-34 */
  volatile uint32_t rpr;                   /* 0x0_0918 Reset protection register R/W 0x0000_0000 4.5.1.5/4-35 */
  volatile uint32_t rcr;                   /* 0x0_091C Reset control register R/W 0x0000_0000 4.5.1.6/4-36 */
  volatile uint32_t rcer;                  /* 0x0_0920 Reset control enable register R/W 0x0000_0000 4.5.1.7/4-36 */
  uint8_t reserved0_0924[0x00A00-0x00924]; /* 0x0_0924--0x0_09FC Reserved, should be cleared */
} m83xxRESRegisters_t;

  /* Clock Module */
typedef struct m83xxCLKRegisters_ {
  volatile uint32_t spmr; /* 0x0_0A00 System PLL mode register R 0x0000_0000 4.5.2.1/4-37 */
  volatile uint32_t occr; /* 0x0_0A04 Output clock control register R/W 0x0000_0000 4.5.2.2/4-38 */
  volatile uint32_t sccr; /* 0x0_0A08 System clock control register R/W 0xFFFF_FFFF 4.5.2.3/4-40 */
  uint8_t reserved0_0A08[0x00B00-0x00A0C]; /* 0x0_0A0C--0x0_0AFC Reserved, should be cleared */
} m83xxCLKRegisters_t;
  /* Power Management Control Module */
typedef struct m83xxPMCRegisters_ {
  volatile uint32_t pmccr; /* 0x0_0B00 Power management controller configuration register R/W 0x0000_0000 5.8.3.1/5-69 */
  volatile uint32_t pmcer; /* 0x0_0B04 Power management controller event register R/W 0x0000_0000 5.8.3.2/5-70 */
  volatile uint32_t pmcmr; /* 0x0_0B08 Power management controller mask register R/W 0x0000_0000 5.8.3.3/5-71 */
  uint8_t reserved0_0B10[0x00C00-0x00B0C]; /* 0x0_0B0C--0x0_0BFC Reserved, should be cleared */
} m83xxPMCRegisters_t;
  /* GPIO1 Registers */
typedef struct m83xxGPIORegisters_ {
  volatile uint32_t gpdir; /* 0x0_0C00 GPIO1/2 direction register R/W 0x0000_0000 21.3.1/21-3 */
  volatile uint32_t gpdr;  /* 0x0_0C04 GPIO1/2 open drain register R/W 0x0000_0000 21.3.2/21-4 */
  volatile uint32_t gpdat; /* 0x0_0C08 GPIO1/2 data register R/W 0x0000_0000 21.3.3/21-4 */
  volatile uint32_t gpier; /* 0x0_0C0C GPIO1/2 interrupt event register R/W Undefined 21.3.4/21-5 */
  volatile uint32_t gpimr; /* 0x0_0C10 GPIO1/2 interrupt mask register R/W 0x0000_0000 21.3.5/21-5 */
  volatile uint32_t gpicr; /* 0x0_0C14 GPIO1/2 external interrupt control register R/W 0x0000_0000 21.3.6/21-6 */
  uint8_t reserved0_0C1C[0x00D00-0x00C18]; /* 0x0_0C18--0x0_0CFF Reserved, should be cleared */
} m83xxGPIORegisters_t;

  /* DLL */
typedef struct m83xxDLLRegisters_ {
  uint8_t reserved0_1000[0x01010-0x01000]; /* 0x0_1000--0x0_100F Reserved, should be cleared */
  volatile uint32_t mckenr;                /* 0x0_1010 MCK enable register (MCKENR) R/W 0xFC00_0000 4.5.3/4-41 */
  uint8_t reserved0_1014[0x01100-0x01014]; /* 0x0_1014--0x0_10FF Reserved, should be cleared */
  volatile uint32_t reserved0_1100;        /* 0x0_1100 Reserved. Reset value should be preserved. R/W 0x0500_0280  */
  volatile uint32_t reserved0_1104;        /* 0x0_1104 Reserved. Reset value should be preserved. R/W 0x8004_0810  */
  volatile uint32_t dllovr;                /* 0x0_1108 DLL override register (DLLOVR) R/W 0x0000_0000 22.4.1/22-4 */
  volatile uint32_t dllsr;                 /* 0x0_110C DLL status register (DLLSR) R 0x0000_0000 22.4.2/22-4 */
  volatile uint32_t dllck;                 /* 0x0_1110 DLL clock register (DLLCK) R/W 0xFC00_0000 22.4.3/22-5 */
  uint8_t reserved0_1110[0x01200-0x01114]; /* 0x0_1114--0x0_11FF Reserved, should be cleared */
} m83xxDLLRegisters_t;

  /* DDR Memory Controller Memory Map */
typedef struct m83xxDDRRegisters_ {
  volatile uint32_t cs0_bnds;              /* 0x0_2000 Chip select 0 memory bounds R/W 0x0000_0000 9.4.1.1/9-10 */
  uint8_t reserved0_2004[0x02008-0x02004]; /* 0x0_2004--0x0_2008 Reserved, should be cleared */
  volatile uint32_t cs1_bnds;              /* 0x0_2008 Chip select 1 memory bounds R/W 0x0000_0000 */
  uint8_t reserved0_200C[0x02010-0x0200C]; /* 0x0_200C--0x0_2010 Reserved, should be cleared */
  volatile uint32_t cs2_bnds;              /* 0x0_2010 Chip select 2 memory bounds R/W 0x0000_0000 */
  uint8_t reserved0_2014[0x02018-0x02014]; /* 0x0_2014--0x0_2018 Reserved, should be cleared */
  volatile uint32_t cs3_bnds;              /* 0x0_2018 Chip select 3 memory bounds R/W 0x0000_0000 */
  uint8_t reserved0_201C[0x02080-0x0201C]; /* 0x0_201C--0x0_207F Reserved, should be cleared */
  volatile uint32_t cs0_config;            /* 0x0_2080 Chip select 0 configuration R/W 0x0000_0000 9.4.1.2/9-11 */
  volatile uint32_t cs1_config;            /* 0x0_2084 Chip select 1 configuration R/W 0x0000_0000 */
  volatile uint32_t cs2_config;            /* 0x0_2088 Chip select 2 configuration R/W 0x0000_0000 */
  volatile uint32_t cs3_config;            /* 0x0_208C Chip select 3 configuration R/W 0x0000_0000 */
  uint8_t reserved0_2090[0x02100-0x02090]; /* 0x0_2090--0x0_2100 Reserved, should be cleared */
  volatile uint32_t timing_cfg_3;          /* 0x0_2100 DDR SDRAM timing configuration 3 R/W 0x0000_0000 9.4.1.3/9-13 */
  volatile uint32_t timing_cfg_0;          /* 0x0_2104 DDR SDRAM timing configuration 0 R/W 0x0011_0105 9.4.1.4/9-14 */
  volatile uint32_t timing_cfg_1;          /* 0x0_2108 DDR SDRAM timing configuration 1 R/W 0x0000_0000 9.4.1.5/9-16 */
  volatile uint32_t timing_cfg_2;          /* 0x0_210C DDR SDRAM timing configuration 2 R/W 0x0000_0000 9.4.1.6/9-18 */
  volatile uint32_t ddr_sdram_cfg;         /* 0x0_2110 DDR SDRAM control configuration R/W 0x0200_0000 9.4.1.7/9-20 */
  volatile uint32_t ddr_sdram_cfg_2;       /* 0x0_2114 DDR SDRAM control configuration 2 R/W 0x0000_0000 9.4.1.8/9-22 */
  volatile uint32_t ddr_sdram_mode;        /* 0x0;_2118 DDR SDRAM mode configuration R/W 0x0000_0000 9.4.1.9/9-24 */
  volatile uint32_t ddr_sdram_mode_2;      /* 0x0_211C DDR SDRAM mode configuration 2 R/W 0x0000_0000 9.4.1.10/9-24 */
  volatile uint32_t ddr_sdram_md_cntl;     /* 0x0_2120 DDR SDRAM mode control R/W 0x0000_0000 9.4.1.11/9-25 */
  volatile uint32_t ddr_sdram_interval;    /* 0x0_2124 DDR SDRAM interval configuration R/W 0x0000_0000 9.4.1.12/9-27 */
  volatile uint32_t ddr_data_init;         /* 0x0_2128 DDR SDRAM data initialization R/W 0x0000_0000 9.4.1.13/9-28 */
  uint8_t reserved0_212C[0x02130-0x0212C]; /* 0x0_212C Reserved  $ $ */
  volatile uint32_t ddr_sdram_clk_cntl;    /* 0x0_2130 DDR SDRAM clock control R/W 0x0200_0000 9.4.1.14/9-28 */
  uint8_t reserved0_2134[0x02148-0x02134]; /* 0x0_2140 Reserved  $ $ */
  volatile uint32_t ddr_init_address;      /* 0x0_2148 DDR training initialization address R/W 0x0000_0000 9.4.1.15/9-29 */
  uint8_t reserved0_214C[0x02BF8-0x0214C]; /* 0x0_214C Reserved  $ $ */
  volatile uint32_t ddr_ip_rev1;           /* 0x0_2BF8 DDR IP block revision 1 R 0x0002_0200 9.4.1.16/9-30 */
  volatile uint32_t ddr_ip_rev2;           /* 0x0_2BFC DDR IP block revision 2 R 0x0000_0000 9.4.1.17/9-30 */
  uint8_t reserved0_2C00[0x02E00-0x02C00]; /* 0x0_2C00 Reserved  $ $ */
  volatile uint32_t data_err_inject_hi;    /* 0x0_2E00 Memory data path error injection mask high R/W 0x0000_0000 9.4.1.18/9-31 */
  volatile uint32_t data_err_inject_lo;    /* 0x0_2E04 Memory data path error injection mask low R/W 0x0000_0000 9.4.1.19/9-31 */
  volatile uint32_t ecc_err_inject;        /* 0x0_2E08 Memory data path error injection mask ECC R/W 0x0000_0000 9.4.1.20/9-32 */
  uint8_t reserved0_2E0C[0x02E20-0x02E0C]; /* 0x0_2E0C Reserved  $ $ */
  volatile uint32_t capture_data_hi;       /* 0x0_2E20 Memory data path read capture high R/W 0x0000_0000 9.4.1.21/9-32 */
  volatile uint32_t capture_data_lo;       /* 0x0_2E24 Memory data path read capture low R/W 0x0000_0000 9.4.1.22/9-33 */
  volatile uint32_t capture_ecc;           /* 0x0_2E28 Memory data path read capture ECC R/W 0x0000_0000 9.4.1.23/9-33 */
  uint8_t reserved0_2E2C[0x02E40-0x02E2C]; /* 0x0_2E2C Reserved  $ $ */
  volatile uint32_t err_detect;            /* 0x0_2E40 Memory error detect w1c 0x0000_0000 9.4.1.24/9-33 */
  volatile uint32_t err_disable;           /* 0x0_2E44 Memory error disable R/W 0x0000_0000 9.4.1.25/9-34 */
  volatile uint32_t err_int_en;            /* 0x0_2E48 Memory error interrupt enable R/W 0x0000_0000 9.4.1.26/9-35 */
  volatile uint32_t capture_attributes;    /* 0x0_2E4C Memory error attributes capture R/W 0x0000_0000 9.4.1.27/9-36 */
  volatile uint32_t capture_address;       /* 0x0_2E50 Memory error address capture R/W 0x0000_0000 9.4.1.28/9-37 */
  uint8_t reserved0_2E54[0x02E58-0x02E54]; /* 0x0_2E54 Reserved  $ $ */
  volatile uint32_t err_sbe;               /* 0x0_2E58 Single-Bit ECC memory error management R/W 0x0000_0000 9.4.1.29/9-37 */
  uint8_t reserved0_2E5C[0x2F00-0x2E5C];
} m83xxDDRRegisters_t;

  /* I2C Controller */
typedef struct m83xxI2CRegisters_ {
  volatile uint8_t i2cadr;  /* 0x0_3000 I2C1 address register R/W 0x00 17.3.1.1/17-5 */
  uint8_t reserved0_3001[0x03004-0x03001];
  volatile uint8_t i2cfdr; /* 0x0_3004 I2C1 frequency divider register R/W 0x00 17.3.1.2/17-5 */
  uint8_t reserved0_3005[0x03008-0x03005];
  volatile uint8_t i2ccr; /* 0x0_3008 I2C1 control register R/W 0x00 17.3.1.3/17-6 */
  uint8_t reserved0_3009[0x0300C-0x03009];
  volatile uint8_t i2csr; /* 0x0_300C I2C1 status register R/W 0x81 17.3.1.4/17-8 */
  uint8_t reserved0_300D[0x03010-0x0300D];
  volatile uint8_t i2cdr; /* 0x0_3010 I2C1 data register R/W 0x00 17.3.1.5/17-9 */
  uint8_t reserved0_3011[0x03014-0x03011];
  volatile uint8_t i2cdfsrr; /* 0x0_3014 I2C1 digital filter sampling rate register R/W 0x0001_0000 17.3.1.6/17-10 */
  uint8_t reserved0_3015[0x03018-0x03015];
  uint8_t reserved0_3018[0x03100-0x03018]; /* 0x0_3018-30FF Reserved, should be cleared */
} m83xxI2CRegisters_t;

  /* DUART */
typedef struct m83xxDUARTRegisters_ {
  union {
    volatile uint8_t urbr;                /* 0x0_4500 ULCR[DLAB] = 0 UART1 receiver buffer register R 0x00 18.3.1.1/18-6 */
    volatile uint8_t uthr;                /* 0x0_4500 ULCR[DLAB] = 0 UART1 transmitter holding register W 0x00 18.3.1.2/18-6 */
    volatile uint8_t udlb;                /* 0x0_4500 ULCR[DLAB] = 1 UART1 divisor least significant byte register R/W 0x00 18.3.1.3/18-7 */
  } urbr_uthr_udlb;
  union {
    volatile uint8_t uier;                /* 0x0_4501 ULCR[DLAB] = 0 UART1 interrupt enable register R/W 0x00 18.3.1.4/18-8 */
    volatile uint8_t udmb;                /* 0x0_4501 ULCR[DLAB] = 1 UART1 divisor most significant byte register R/W 0x00 18.3.1.3/18-7 */
  } uier_udmb;
  union {
    volatile uint8_t uiir;                /* 0x0_4502 ULCR[DLAB] = 0 UART1 interrupt ID register R 0x01 18.3.1.5/18-9 */
    volatile uint8_t ufcr;                /* 0x0_4502 ULCR[DLAB] = 0 UART1 FIFO control register W 0x00 18.3.1.6/18-10 */
    volatile uint8_t uafr;                /* 0x0_4502 ULCR[DLAB] = 1 UART1 alternate function register R/W 0x00 18.3.1.12/18-16 */
  } uiir_ufcr_uafr;
  volatile uint8_t ulcr;                  /* 0x0_4503 ULCR[DLAB] = x UART1 line control register R/W 0x00 18.3.1.7/18-11 */
  volatile uint8_t umcr;                  /* 0x0_4504 ULCR[DLAB] = x UART1 MODEM control register R/W 0x00 18.3.1.8/18-13 */
  volatile uint8_t ulsr;                  /* 0x0_4505 ULCR[DLAB] = x UART1 line status register R 0x60 18.3.1.9/18-14 */
  volatile uint8_t umsr;                  /* 0x0_4506 ULCR[DLAB] = x UART1 MODEM status register R 0x00 18.3.1.10/18-15 */
  volatile uint8_t uscr;                  /* 0x0_4507 ULCR[DLAB] = x UART1 scratch register R/W 0x00 18.3.1.11/18-16 */
  uint8_t reserved0_4508[0x04510-0x04508];/* 0x0_4508-450F Reserved */
  volatile uint8_t udsr;                  /* 0x0_4510 ULCR[DLAB] = x UART1 DMA status register R 0x01 18.3.1.13/18-17 */
  uint8_t reserved0_4511[0x04600-0x04511];/* 0x0_4511-45FF Reserved */
} m83xxDUARTRegisters_t;

  /* Local Bus Controller (LBC) Registers */
typedef struct m83xxLBCRegisters_ {
  struct {
    volatile uint32_t br;                 /* 0x0_5000 Base register 0 ,R/W 0x0000_RR01 10.3.1.1/10-11 */
    volatile uint32_t optionsr;                 /* 0x0_5004 Options register 0 R/W 0x0000_0FF7 10.3.1.2/10-12 */
  } bor[8];
  uint8_t reserved0_5040[0x05068-0x05040];/* 0x0_5040-5067 Reserved */
  volatile uint32_t mar;                  /* 0x0_5068 UPM address register R/W 0x0000_0000 10.3.1.3/10-18 */
  uint8_t reserved0_506C[0x05070-0x0506C];/* 0x0_506C-506F Reserved */
  volatile uint32_t mamr;                 /* 0x0_5070 UPMA mode register R/W 0x0000_0000 10.3.1.4/10-19 */
  volatile uint32_t mbmr;                 /* 0x0_5074 UPMB mode register R/W 0x0000_0000 10.3.1.4/10-19 */
  volatile uint32_t mcmr;                 /* 0x0_5078 UPMC mode register R/W 0x0000_0000 10.3.1.4/10-19 */
  uint8_t reserved0_507C[0x05084-0x0507C];/* 0x0_507C-5083 Reserved */
  volatile uint32_t mrtpr;                /* 0x0_5084 Memory refresh timer prescaler register R/W 0x0000_0000 10.3.1.5/10-21 */
  volatile uint32_t mdr;                  /* 0x0_5088 UPM data register R/W 0x0000_0000 10.3.1.6/10-22 */
  uint8_t reserved0_508C[0x05094-0x0508C];/* 0x0_508C-5093 Reserved */
  volatile uint32_t lsdmr;                /* 0x0_5094 SDRAM mode register R/W 0x0000_0000 10.3.1.7/10-22 */
  uint8_t reserved0_5098[0x050A0-0x05098];/* 0x0_5098-509F Reserved */
  volatile uint32_t lurt;                 /* 0x0_50A0 UPM refresh timer R/W 0x0000_0000 10.3.1.8/10-24 */
  volatile uint32_t lsrt;                 /* 0x0_50A4 SDRAM refresh timer R/W 0x0000_0000 10.3.1.9/10-25 */
  uint8_t reserved0_50A8[0x050B0-0x050A8];/* 0x0_50A8-50AF Reserved */
  volatile uint32_t ltesr;                /* 0x0_50B0 Transfer error status register Read/ bit-reset 0x0000_0000 10.3.1.10/10-26 */
  volatile uint32_t ltedr;                /* 0x0_50B4 Transfer error check disable register R/W 0x0000_0000 10.3.1.11/10-27 */
  volatile uint32_t lteir;                /* 0x0_50B8 Transfer error interrupt enable register R/W 0x0000_0000 10.3.1.12/10-27 */
  volatile uint32_t lteatr;               /* 0x0_50BC Transfer error attributes register R/W 0x0000_0000 10.3.1.13/10-28 */
  volatile uint32_t ltear;                /* 0x0_50C0 Transfer error address register R/W 0x0000_0000 10.3.1.14/10-29 */
  uint8_t reserved0_50C4[0x050D0-0x050C4];/* 0x0_50C4-50CF Reserved */
  volatile uint32_t lbcr;                 /* 0x0_50D0 Local bus configuration register R/W 0x0000_0000 10.3.1.15/10-29 */
  volatile uint32_t lcrr;                 /* 0x0_50D4 Clock ratio register R/W 0x8000_0008 10.3.1.16/10-30 */
  uint8_t reserved0_50D8[0x05100-0x050D8];/* 0x0_50D8-50FF Reserved */
} m83xxLBCRegisters_t;

  /* Serial Peripheral Interface (SPI) */
typedef struct m83xxSPIRegisters_ {
  uint8_t reserved0_7000[0x07020-0x07000];/* 0x0_7000-7020 Reserved, should be cleared */
  volatile uint32_t spmode;               /* 0x0_7020 SPI mode register R/W 0x0000_0000 19.4.1.1/19-9 */
  volatile uint32_t spie;                 /* 0x0_7024 SPI event register R/W 0x0000_0000 19.4.1.2/19-11 */
  volatile uint32_t spim;                 /* 0x0_7028 SPI mask register R/W 0x0000_0000 19.4.1.3/19-13 */
  volatile uint32_t spcom;                /* 0x0_702C SPI command register R/W 0x0000_0000 19.4.1.4/19-14 */
  volatile uint32_t spitd;                /* 0x0_7030 SPI transmit register R/W 0x0000_0000 19.4.1.5/19-14 */
  volatile uint32_t spird;                /* 0x0_7034 SPI receive register R 0xFFFF_FFFF 19.4.1.6/19-15 */
  uint8_t reserved0_7038[0x07100-0x07038];/* 0x0_7038-70FF Reserved */
} m83xxSPIRegisters_t;
                                              /* SPIMODE register fields    */
#define  MPC83XX_SPIMODE_LOOP   (1   << (31- 1)) /* loopback                */
#define  MPC83XX_SPIMODE_CI     (1   << (31- 2)) /* clock invert            */
#define  MPC83XX_SPIMODE_CP     (1   << (31- 3)) /* clock phase             */
#define  MPC83XX_SPIMODE_DIV16  (1   << (31- 4)) /* divide by 16            */
#define  MPC83XX_SPIMODE_REV    (1   << (31- 5)) /* LSB first               */
#define  MPC83XX_SPIMODE_M_S    (1   << (31- 6)) /* master/slave            */
#define  MPC83XX_SPIMODE_EN     (1   << (31- 7)) /* enable                  */
#define  MPC83XX_SPIMODE_LEN(n) ((n) << (31-11)) /* length code             */
#define  MPC83XX_SPIMODE_PM(n)  ((n) << (31-15)) /* prescaler               */
#define  MPC83XX_SPIMODE_OD     (1   << (31-19)) /* open drain              */

                                               /* SPCOM  register fields    */
#define  MPC83XX_SPCOM_LST    (1   << (31- 9))  /* last transfer            */

                                            /* SPIE/M register fields     */
#define  MPC83XX_SPIE_LT     (1 << (31-17)) /* last character transmitted */
#define  MPC83XX_SPIE_DNR    (1 << (31-18)) /* data not ready             */
#define  MPC83XX_SPIE_OV     (1 << (31-19)) /* overrun                    */
#define  MPC83XX_SPIE_UN     (1 << (31-20)) /* unterrun                   */
#define  MPC83XX_SPIE_MME    (1 << (31-21)) /* multi-master error         */
#define  MPC83XX_SPIE_NE     (1 << (31-22)) /* not empty                  */
#define  MPC83XX_SPIE_NF     (1 << (31-23)) /* not full                   */

typedef struct m83xxDMARegisters_ {
  /* DMA Registers */
  uint8_t reserved0_8000[0x08030-0x08000];/* 0x0_8000-0x0_802f Reserved */
  volatile uint32_t omisr;                /* 0x0_8030 Outbound message interrupt status register Special 0x0000_0000 12.4.1/12-4 */
  volatile uint32_t omimr;                /* 0x0_8034 Outbound message interrupt mask register R/W 0x0000_0000 12.4.2/12-6 */
  uint8_t reserved0_8038[0x08050-0x08038];/* 0x0_8038-0x0_804f Reserved */
  volatile uint32_t imr0;                 /* 0x0_8050 Inbound message register 0 R/W 0x0000_0000 12.4.3/12-7 */
  volatile uint32_t imr1;                 /* 0x0_8054 Inbound message register 1 R/W 0x0000_0000 12.4.3/12-7 */
  volatile uint32_t omr0;                 /* 0x0_8058 Outbound message register 0 R/W 0x0000_0000 12.4.4/12-7 */
  volatile uint32_t omr1;                 /* 0x0_805C Outbound message register 1 R/W 0x0000_0000 12.4.4/12-7 */
  volatile uint32_t odr;                  /* 0x0_8060 Outbound doorbell register R/W 0x0000_0000 12.4.5/12-8 */
  uint8_t reserved0_8064[0x08068-0x08064];/* 0x0_8064-0x0_8067 Reserved */
  volatile uint32_t idr;                  /* 0x0_8068 Inbound doorbell register R/W 0x0000_0000 12.4.5/12-8 */
  uint8_t reserved0_806C[0x08080-0x0806C];/* 0x0_806C-0x0_807F Reserved */
  volatile uint32_t imisr;                /* 0x0_8080 Inbound message interrupt status register R/W 0x0000_0000 12.4.6/12-9 */
  volatile uint32_t imimr;                /* 0x0_8084 Inbound message interrupt mask register R/W 0x0000_0000 12.4.7/12-11 */
  uint8_t reserved0_8088[0x080A8-0x08088];/* 0x0_8088-0x0_80A7 Reserved */
  struct m83xxDMAChannelRegisters_ {
    uint8_t reserved0_80A8[0x08100-0x080A8];/* 0x0_80A8-0x0_80FF Reserved */
    volatile uint32_t dmamr0;               /* 0x0_8100 DMA 0 mode register R/W 0x0000_0000 12.4.8.1/12-12 */
    volatile uint32_t dmasr0;               /* 0x0_8104 DMA 0 status register R/W 0x0000_0000 12.4.8.2/12-14 */
    volatile uint32_t dmacdar0;             /* 0x0_8108 DMA 0 current descriptor address register R/W 0x0000_0000 12.4.8.3/12-15 */
    uint8_t reserved0_810C[0x08110-0x0810C];/* 0x0_810C-0x0_810F Reserved */
    volatile uint32_t dmasar0;              /* 0x0_8110 DMA 0 source address register R/W 0x0000_0000 12.4.8.4/12-16 */
    uint8_t reserved0_8114[0x08118-0x08114];/* 0x0_8114-0x0_8117 Reserved */
    volatile uint32_t dmadar0;              /* 0x0_8118 DMA 0 destination address register R/W 0x0000_0000 12.4.8.5/12-16 */
    uint8_t reserved0_811C[0x08120-0x0811C];/* 0x0_8120-0x0_811C Reserved */
    volatile uint32_t dmabcr0;              /* 0x0_8120 DMA 0 byte count register R/W 0x0000_0000 12.4.8.6/12-17 */
    volatile uint32_t dmandar0;             /* 0x0_8124 DMA 0 next descriptor address register R/W 0x0000_0000 12.4.8.7/12-17 */
  }chan[4];
  volatile uint32_t dmagsr;                 /* 0x0_82A8 DMA general status register R 0x0000_0000 12.4.8.8/12-18 */
  uint8_t reserved0_82AC[0x082FF-0x082AC];  /* 0x0_82AC-0x0_82FF Reserved, should be cleared */
} m83xxDMARegisters_t;

/* Registers in DMA section use little-endian byte order */

/* DMA mode register */
#define MPC83XX_DMAMR_DRCNT_1               (5 << 24)
#define MPC83XX_DMAMR_DRCNT_2               (6 << 24)
#define MPC83XX_DMAMR_DRCNT_4               (7 << 24)
#define MPC83XX_DMAMR_DRCNT_8               (8 << 24)
#define MPC83XX_DMAMR_DRCNT_16              (9 << 24)
#define MPC83XX_DMAMR_DRCNT_32              (0xA << 24)

#define MPC83XX_DMAMR_BWC_1                 (0 << 21)
#define MPC83XX_DMAMR_BWC_2                 (1 << 21)
#define MPC83XX_DMAMR_BWC_4                 (2 << 21)
#define MPC83XX_DMAMR_BWC_8                 (3 << 21)
#define MPC83XX_DMAMR_BWC_16                (4 << 21)

#define MPC83XX_DMAMR_DMSEN                 (1 << 20)
#define MPC83XX_DMAMR_IRQS                  (1 << 19)
#define MPC83XX_DMAMR_EMSEN                 (1 << 18)

#define MPC83XX_DMAMR_DAHTS_1               (0 << 16)
#define MPC83XX_DMAMR_DAHTS_2               (1 << 16)
#define MPC83XX_DMAMR_DAHTS_4               (2 << 16)
#define MPC83XX_DMAMR_DAHTS_8               (3 << 16)

#define MPC83XX_DMAMR_SAHTS_1               (0 << 14)
#define MPC83XX_DMAMR_SAHTS_2               (1 << 14)
#define MPC83XX_DMAMR_SAHTS_4               (2 << 14)
#define MPC83XX_DMAMR_SAHTS_8               (3 << 14)

#define MPC83XX_DMAMR_DAHE                  (1 << 13)
#define MPC83XX_DMAMR_SAHE                  (1 << 12)

#define MPC83XX_DMAMR_PRC_PCI_READ          (0 << 10)
#define MPC83XX_DMAMR_PRC_PCI_READ_LINE     (1 << 10)
#define MPC83XX_DMAMR_PRC_PCI_READ_MULTIPLE (2 << 10)

#define MPC83XX_DMAMR_EOIIE                 (1 <<  7)
#define MPC83XX_DMAMR_TEM                   (1 <<  3)
#define MPC83XX_DMAMR_CTM                   (1 <<  2)
#define MPC83XX_DMAMR_CC                    (1 <<  1)
#define MPC83XX_DMAMR_CS                    (1 <<  0)

/* DMA status register */
#define MPC83XX_DMASR_TE                    (1 << 7)
#define MPC83XX_DMASR_CB                    (1 << 2)
#define MPC83XX_DMASR_EOSI                  (1 << 1)
#define MPC83XX_DMASR_EOCDI                 (1 << 0)

/* DMA current descriptor address register */
#define MPC83XX_DMACDAR_SNEN                (1 << 4)
#define MPC83XX_DMACDAR_EOSIE               (1 << 3)

/* DMA next descriptor address register */
#define MPC83XX_DMANDAR_NSNEN               (1 << 4)
#define MPC83XX_DMANDAR_NEOSIE              (1 << 3)
#define MPC83XX_DMANDAR_EOTD                (1 << 0)


typedef struct m83xxPCICfgRegisters_ {
  /* PCI1 Software Configuration Registers */
  volatile uint32_t config_address;         /* 0x0_8300 PCI1 CONFIG_ADDRESS W  13.3.1.1/13-16 */
  volatile uint32_t config_data;            /* 0x0_8304 PCI1 CONFIG_DATA R/W  13.3.1.2/13-18 */
  volatile uint32_t int_ack;                /* 0x0_8308 PCI1 INT_ACK R  13.3.1.3/13-18 */
  uint8_t reserved0_830C[0x08380-0x0830C];  /* 0x0_830C-0x0_837F Reserved */
} m83xxPCICfgRegisters_t;

typedef struct m83xxPCIIosRegisters_ {
  /* Sequencer (IOS) */
  volatile uint32_t potar0;               /* 0x0_8400 PCI outbound translation address register 0 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_8404[0x08408-0x08404];/* 0x0_8404 Reserved */
  volatile uint32_t pobar0;               /* 0x0_8408 PCI outbound base address register 0 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_840C[0x08410-0x0840C];/* 0x0_840C Reserved */
  volatile uint32_t pocmr0;               /* 0x0_8410 PCI outbound comparison mask register 0 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_8414[0x08418-0x08414];/* 0x0_8414 Reserved */
  volatile uint32_t potar1;               /* 0x0_8418 PCI outbound translation address register 1 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_841C[0x08420-0x0841C];/* 0x0_841C Reserved */
  volatile uint32_t pobar1;               /* 0x0_8420 PCI outbound base address register 1 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_8424[0x08428-0x08424];/* 0x0_8424 Reserved */
  volatile uint32_t pocmr1;               /* 0x0_8428 PCI outbound comparison mask register 1 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_842C[0x08430-0x0842C];/* 0x0_842C Reserved */
  volatile uint32_t potar2;               /* 0x0_8430 PCI outbound translation address register 2 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_8434[0x08438-0x08434];/* 0x0_8434 Reserved */
  volatile uint32_t pobar2;               /* 0x0_8438 PCI outbound base address register 2 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_843C[0x08440-0x0843C];/* 0x0_843C Reserved */
  volatile uint32_t pocmr2;               /* 0x0_8440 PCI outbound comparison mask register 2 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_8444[0x08448-0x08444];/* 0x0_8444 Reserved */
  volatile uint32_t potar3;               /* 0x0_8448 PCI outbound translation address register 3 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_844C[0x08450-0x0844C];/* 0x0_844C Reserved */
  volatile uint32_t pobar3;               /* 0x0_8450 PCI outbound base address register 3 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_8454[0x08458-0x08454];/* 0x0_8454 Reserved */
  volatile uint32_t pocmr3;               /* 0x0_8458 PCI outbound comparison mask register 3 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_845C[0x08460-0x0845C];/* 0x0_845C Reserved */
  volatile uint32_t potar4;               /* 0x0_8460 PCI outbound translation address register 4 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_8464[0x08468-0x08464];/* 0x0_8464 Reserved */
  volatile uint32_t pobar4;               /* 0x0_8468 PCI outbound base address register 4 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_846C[0x08470-0x0846C];/* 0x0_846C Reserved */
  volatile uint32_t pocmr4;               /* 0x0_8470 PCI outbound comparison mask register 4 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_8474[0x08478-0x08474];/* 0x0_8474 Reserved */
  volatile uint32_t potar5;               /* 0x0_8478 PCI outbound translation address register 5 R/W 0x0000_0000 11.4.1/11-3 */
  uint8_t reserved0_847C[0x08480-0x0847C];/* 0x0_847C Reserved */
  volatile uint32_t pobar5;               /* 0x0_8480 PCI outbound base address register 5 R/W 0x0000_0000 11.4.2/11-3 */
  uint8_t reserved0_8484[0x08488-0x08484];/* 0x0_8484 Reserved */
  volatile uint32_t pocmr5;               /* 0x0_8488 PCI outbound comparison mask register 5 R/W 0x0000_0000 11.4.3/11-4 */
  uint8_t reserved0_848C[0x084F0-0x0848C];/* 0x0_848C Reserved */
  volatile uint32_t pmcr;                 /* 0x0_84F0 Power management control register R/W 0x0000_0000 11.4.4/11-5 */
  uint8_t reserved0_84F4[0x084F8-0x084F4];/* 0x0_84F4 Reserved */
  volatile uint32_t dtcr;                 /* 0x0_84F8 Discard timer control register R/W 0x0000_0000 11.4.5/11-6 */
  uint8_t reserved0_84FC[0x08500-0x084FC];/* 0x0_84FC Reserved */
} m83xxPCIIosRegisters_t;

typedef struct m83xxPCICtrlRegisters_ {
  /* PCI1 Error Management Registers */
  volatile uint32_t pci_esr;              /* 0x0_8500 PCI error status register R / w1c 0x0000_0000 13.3.2.1/13-18 */
  volatile uint32_t pci_ecdr;             /* 0x0_8504 PCI error capture disable register R/W 0x0000_0000 13.3.2.2/13-19 */
  volatile uint32_t pci_eer;              /* 0x0_8508 PCI error enable register R/W 0x0000_0000 13.3.2.3/13-20 */
  volatile uint32_t pci_eatcr;            /* 0x0_850C PCI error attributes capture register R/W 0x0000_0000 13.3.2.4/13-21 */
  volatile uint32_t pci_eacr;             /* 0x0_8510 PCI error address capture register R 0x0000_0000 13.3.2.5/13-23 */
  volatile uint32_t pci_eeacr;            /* 0x0_8514 PCI error extended address capture register R 0x0000_0000 13.3.2.6/13-23 */
  volatile uint32_t pci_edlcr;            /* 0x0_8518 PCI error data low capture register R 0x0000_0000 13.3.2.7/13-24 */
  volatile uint32_t pci_edhcr;            /* 0x0_851C PCI error data high capture register R 0x0000_0000 13.3.2.8/13-24 */
  /* PCI1 Control and Status Registers */
  volatile uint32_t pci_gcr;              /* 0x0_8520 PCI general control register R/W 0x0000_0000 13.3.2.9/13-24 */
  volatile uint32_t pci_ecr;              /* 0x0_8524 PCI error control register R/W 0x0000_0000 13.3.2.10/13-25 */
  volatile uint32_t pci_gsr;              /* 0x0_8528 PCI general status register R 0x0000_0000 13.3.2.11/13-26 */
  uint8_t reserved0_852C[0x08538-0x0852C];/* 0x0_852C Reserved */
  /* PCI1 Inbound ATU Registers */
  volatile uint32_t pitar2;               /* 0x0_8538 PCI inbound translation address register 2 R/W 0x0000_0000 13.3.2.12/13-26 */
  uint8_t reserved0_853C[0x08540-0x0853C];/* 0x0_853C Reserved, should be cleared  $ $ */
  volatile uint32_t pibar2;               /* 0x0_8540 PCI inbound base address register 2 R/W 0x0000_0000 13.3.2.13/13-27 */
  volatile uint32_t piebar2;              /* 0x0_8544 PCI inbound extended base address register 2 R/W 0x0000_0000 13.3.2.14/13-27 */
  volatile uint32_t piwar2;               /* 0x0_8548 PCI inbound window attributes register 2 R/W 0x0000_0000 13.3.2.15/13-28 */
  uint8_t reserved0_854C[0x08550-0x0854C];/* 0x0_854C Reserved */
  volatile uint32_t pitar1;               /* 0x0_8550 PCI inbound translation address register 1 R/W 0x0000_0000 13.3.2.12/13-26 */
  uint8_t reserved0_8550[0x08558-0x08554];/* 0x0_8554 Reserved, should be cleared  $ $ */
  volatile uint32_t pibar1;               /* 0x0_8558 PCI inbound base address register 1 R/W 0x0000_0000 13.3.2.13/13-27 */
  volatile uint32_t piebar1;              /* 0x0_855C PCI inbound extended base address register 1 R/W 0x0000_0000 13.3.2.14/13-27 */
  volatile uint32_t piwar1;               /* 0x0_8560 PCI inbound window attributes register 1 R/W 0x0000_0000 13.3.2.15/13-28 */
  uint8_t reserved0_8564[0x08568-0x08564];/* 0x0_8564 Reserved */
  volatile uint32_t pitar0;               /* 0x0_8568 PCI inbound translation address register 0 R/W 0x0000_0000 13.3.2.12/13-26 */
  uint8_t reserved0_856c[0x08570-0x0856c];/* 0x0_856C Reserved, should be cleared  $ $ */
  volatile uint32_t pibar0;               /* 0x0_8570 PCI inbound base address register 0 R/W 0x0000_0000 13.3.2.13/13-27 */
  uint8_t reserved0_8574[0x08578-0x08574];/* 0x0_8574 Reserved */
  volatile uint32_t piwar0;               /* 0x0_8578 PCI inbound window attributes register 0 R/W 0x0000_0000 13.3.2.14/13-27 */
  uint8_t reserved0_857c[0x08580-0x0857c];/* 0x0_857C Reserved, should be cleared  $ $ */
  uint8_t reserved0_8580[0x08600-0x08580];/* 0x0_8580 Reserved */
} m83xxPCICtrlRegisters_t;

typedef struct m83xxUSB_MPHRegisters_ {
  /* USB MPH Controller Registers */
  uint8_t reserved0x2_2000[0x22100-0x22000]; /* 0x2_2000--0x2_20FF Reserved, should be cleared */
  volatile uint16_t caplength;               /* 0x2_2100 Capability register length R 0x40 16.3.1.1/16-19 */
  volatile uint16_t hciversion;              /* 0x2_2102 Host interface version number R 0x0100 16.3.1.2/16-19 */
  volatile uint32_t hcsparams;               /* 0x2_2104 Host crtl. structural parameters R 0x0121_0012 16.3.1.3/16-20 */
  volatile uint32_t hccparams;               /* 0x2_2108 Host crtl. capability parameters R 0x0000_0006 16.3.1.4/16-21 */
  uint8_t reserved0x2_210C[0x22140-0x2210C]; /* Reserved  */
  volatile uint32_t usbcmd;                  /* 0x2_2140 USB command R/W 0x0008_nBn0 16.3.2.1/16-23 */
  volatile uint32_t usbsts;                  /* 0x2_2144 USB status R/W 0x0000_0000 16.3.2.2/16-26 */
  volatile uint32_t usbintr;                 /* 0x2_2148 USB interrupt enable R/W 0x0000_0000 16.3.2.3/16-28 */
  volatile uint32_t frindex;                 /* 0x2_214C USB frame index R/W 0x0000_nnnn 16.3.2.4/16-30 */
  uint8_t reserved0x2_2150[0x22154-0x22150]; /* Reserved  */
  volatile uint32_t periodiclistbase;        /* 0x2_2154 Frame list base address R/W 0xnnnn_0000 16.3.2.6/16-31 */
  volatile uint32_t asynclistaddr;           /* 0x2_2158 Next asynchronous list addr R/W 0x0000_0000 16.3.2.8/16-32 */
  volatile uint32_t asyncttsts;              /* 0x2_215C  Asynchronous buffer status for embedded TT TBD 0x0000_0000 16.3.2.10/16-34 */
  volatile uint32_t burstsize;               /* 0x2_2160 Programmable burst size R/W 0x000_1010 16.3.2.11/16-34 */
  volatile uint32_t txfilltuning;            /* 0x2_2164 Host TT transmit pre-buffer packet tuning R/W 0x0002_0000 16.3.2.12/16-35 */
  volatile uint32_t txttfilltuning;          /* 0x2_2168 Host TT transmit pre-buffer packet tuning R/W 0x0000_0000 16.3.2.13/16-37 */
  uint8_t reserved0x2_216c[0x22170-0x2216c]; /* Reserved  */
  volatile uint32_t viewport;                /* 0x2_2170 ULPI ULPI Register Access R/W 0x0000_0000 16.3.2.14/16-37 */
  uint8_t reserved0x2_2174[0x22180-0x22174]; /* Reserved  */
  volatile uint32_t configflag;              /* 0x2_2180 Configured flag register R 0x0000_0001 16.3.2.15/16-39 */
  volatile uint32_t portsc1;                 /* 0x2_2184 Port status/control 1 R/W 0x8C00_0001 16.3.2.16/16-39 */
  volatile uint32_t portsc2;                 /* 0x2_2188 Port status/control 2 R/W 0x8C00_0001 16.3.2.16/16-39 */
  uint8_t reserved0x2_218c[0x221A8-0x2218c]; /* Reserved  */
  volatile uint32_t usbmode;                 /* 0x2_21A8 USB device mode R/W 0x0000_0003 16.3.2.18/16-47 */
  uint8_t reserved0x2_21AC[0x22400-0x221AC]; /* Reserved  */
  volatile uint32_t snoop1;                  /* 0x2_2400 Snoop 1 R/W 0x0000_0000 16.3.2.26/16-53 */
  volatile uint32_t snoop2;                  /* 0x2_2404 Snoop 2 R/W 0x0000_0000 16.3.2.26/16-53 */
  volatile uint32_t age_cnt_thresh;          /* 0x2_2408 Age count threshold R/W 0x0000_0000 16.3.2.27/16-54 */
  volatile uint32_t si_ctrl;                 /* 0x2_240C System interface control R/W 0x0000_0000 16.3.2.28/16-56 */
  volatile uint32_t pri_ctrl;                /* 0x2_2410 Priority control R/W 0x0000_0000 16.3.2.29/16-56 */
  uint8_t reserved0x2_2414[0x22500-0x22414]; /* Reserved  */
  volatile uint32_t control;                 /* 0x2_2500 Control R/W 0x0000_0000 16.3.2.30/16-57 */
  uint8_t reserved0x2_2504[0x23000-0x22504]; /* 0x2_2504--0x2_2FFF Reserved, should be cleared */
} m83xxUSB_MPHRegisters_t;

typedef struct m83xxUSB_DRRegisters_ {
  /* USB DR Controller Registers */
  uint8_t reserved0x2_3000[0x23100-0x23000]; /* 0x2_3000--0x2_30FF Reserved, should be cleared */
  volatile uint16_t caplength;               /* 0x2_3100 Capability register length R 0x40 16.3.1.1/16-19 */
  volatile uint16_t hciversion;              /* 0x2_3102 Host interface version number R 0x0100 16.3.1.2/16-19 */
  volatile uint32_t hcsparams;               /* 0x2_3104 Host crtl. structural parameters R 0x0111_0011 16.3.1.3/16-20 */
  volatile uint32_t hccparams;               /* 0x2_3108 Host crtl. capability parameters R 0x0000_0006 16.3.1.4/16-21 */
  uint8_t reserved0x2_310c[0x23120-0x2310C]; /* 0x2_310c--0x2_311f Reserved */
  volatile uint32_t dciversion;              /* 0x2_3120 Device interface version number R 0x0001 16.3.1.5/16-22 */
  volatile uint32_t dccparams;               /* 0x2_3124 Device controller parameters R 0x0000_0186 16.3.1.6/16-22 */
  uint8_t reserved0x2_3128[0x23140-0x23128]; /* 0x2_3128--0x2_313f Reserved */
  volatile uint32_t usbcmd;                  /* 0x2_3140 USB command R/W 0x0008_nBn0 16.3.2.1/16-23 */
  volatile uint32_t usbsts;                  /* 0x2_3144 USB status R/W 0x0000_0000 16.3.2.2/16-26 */
  volatile uint32_t usbintr;                 /* 0x2_3148 USB interrupt enable R/W 0x0000_0000 16.3.2.3/16-28 */
  volatile uint32_t frindex;                 /* 0x2_314C USB frame index R/W 0x0000_nnnn 16.3.2.4/16-30 */
  uint8_t reserved0x2_3150[0x23154-0x23150]; /* 0x2_3150--0x2_3153 Reserved */
  union {
    volatile uint32_t periodiclistbase;      /* 0x2_3154 Frame list base address R/W 0xnnnn_0000 16.3.2.6/16-31 */
    volatile uint32_t deviceaddr;            /* 0x2_3154 USB device address R/W 0x0000_0000 16.3.2.7/16-32 */
  } perbase_devaddr;
  union {
    volatile uint32_t asynclistaddr;         /* 0x2_3158 Next asynchronous list addr (host mode) R/W 0x0000_0000 16.3.2.8/16-32 */
    volatile uint32_t addr;                  /* 0x2_3158 ENDPOINT Address at endpoint list (device mode) R/W 0x0000_0000 16.3.2.9/16-33 */
  } async_addr;
  uint8_t reserved0x2_315c[0x23160-0x2315c]; /* 0x2_315c--0x2_315f Reserved */
  volatile uint32_t burstsize;               /* 0x2_3160 Programmable burst size R/W 0x0000_1010 16.3.2.11/16-34 */
  volatile uint32_t txfilltuning;            /* 0x2_3164 Host TT transmit pre-buffer packet tuning R/W 0x0002_0000 16.3.2.12/16-35 */
  uint8_t reserved0x2_3168[0x23170-0x23168]; /* 0x2_3168--0x2_316f Reserved */
  volatile uint32_t viewport;                /* 0x2_3170 ULPI ULPI Register Access R/W 0x0000_0000 16.3.2.14/16-37 */
  uint8_t reserved0x2_3174[0x23180-0x23174]; /* 0x2_3174--0x2_317F Reserved */
  volatile uint32_t configflag;              /* 0x2_3180 Configured flag register R 0x0000_0001 16.3.2.15/16-39 */
  volatile uint32_t portsc1;                 /* 0x2_3184 Port status/control R/W 0x9C00_0000 16.3.2.16/16-39 */
  uint8_t reserved0x2_3188[0x231A4-0x23188]; /* 0x2_3188--0x2_31A3 Reserved */
  volatile uint32_t otgsc;                   /* 0x2_31A4 On-the-Go status and control R/W 0x0000_0001 16.3.2.17/16-44 */
  volatile uint32_t usbmode;                 /* 0x2_31A8 USB device mode R/W 0x0000_0000 16.3.2.18/16-47 */
  volatile uint32_t endptsetupstat;          /* 0x2_31AC Endpoint setup status R/W 0x0000_0000 16.3.2.19/16-48 */
  volatile uint32_t endpointprime;           /* 0x2_31B0 Endpoint initialization R/W 0x0000_0000 16.3.2.20/16-48 */
  volatile uint32_t endptflush;              /* 0x2_31B4 Endpoint de-initialize R/W 0x0000_0000 16.3.2.21/16-49 */
  volatile uint32_t endptstatus;             /* 0x2_31B8 Endpoint status R 0x0000_0000 16.3.2.22/16-50 */
  volatile uint32_t endptcomplete;           /* 0x2_31BC Endpoint complete R/W 0x0000_0000 16.3.2.23/16-50 */
  volatile uint32_t endptctrl[6];            /* 0x2_31C0 Endpoint control 0 R/W 0x0080_0080 16.3.2.24/16-51 */
  uint8_t reserved0x2_31D8[0x23400-0x231D8]; /* 0x2_31D8--0x2_33ff Reserved */
  volatile uint32_t snoop1;                  /* 0x2_3400 Snoop 1 R/W 0x0000_0000 16.3.2.26/16-53 */
  volatile uint32_t snoop2;                  /* 0x2_3404 Snoop 2 R/W 0x0000_0000 16.3.2.26/16-53 */
  volatile uint32_t age_cnt_thresh;          /* 0x2_3408 Age count threshold R/W 0x0000_0000 16.3.2.27/16-54 */
  volatile uint32_t pri_ctrl;                /* 0x2_340C Priority control R/W 0x0000_0000 16.3.2.29/16-56 */
  volatile uint32_t si_ctrl;                 /* 0x2_3410 System interface control R/W 0x0000_0000 16.3.2.28/16-56 */
  uint8_t reserved0x2_3414[0x23500-0x23414]; /* 0x2_3414--0x2_34ff Reserved */
  volatile uint32_t control;                 /* 0x2_3500 Control R/W 0x0000_0000 16.3.2.30/16-57 */
  uint8_t reserved0x2_3504[0x24000-0x23504]; /* 0x2_3504--0x2_3FFF Reserved, should be cleared */
} m83xxUSB_DRRegisters_t;

#if 0 /* FIXME: to be formatted soon */
  /* Security Engine Address Map Registers */
  /* Controller Registers */
  volatile uint32_t reserved;## /* 0x3_0000--0x3_0FFF Reserved, should be cleared */
  volatile uint32_t imr; /* 0x3_1008 Interrupt mask register R/W 0x0000_0000_0000_0000 14.7.2.1/14-94 */
  volatile uint32_t isr; /* 0x3_1010 Interrupt status register R 0x0000_0000_0000_0000 14.7.2.2/14-96 */
  volatile uint32_t icr; /* 0x3_1018 Interrupt clear register W 0x0000_0000_0000_0000 14.7.2.3/14-96 */
  volatile uint32_t id; /* 0x3_1020 Identification register R 0x0000_0000_0000_00400x 14.7.2.4/14-98 */
  volatile uint32_t euasr; /* 0x3_1028 EU assignment status register R 0xF0F0_F0F0_00FF_F0F0 14.7.2/14-93 */
  volatile uint32_t mcr; /* 0x3_1030 Master control register R/W 0000_0000_0000_0000 14.7.2.5/14-98 */
  /* Channel 1 */
  volatile uint32_t cccr1; /* 0x3_1108 Crypto-channel 1 configuration register R/W 0x0000_0000_0000_0000 14.6.1.1/14-82 */
  volatile uint32_t ccpsr1; /* 0x3_1110 Crypto-channel 1 pointer status register R 0x0000_0000_0000_0007 14.6.1.2/14-85 */
  volatile uint32_t cdpr1; /* 0x3_1140 Crypto-channel 1 current descriptor pointer register R 0x0000_0000_0000_0000 14.6.1.3/14-90 */
  volatile uint32_t */
0x3_1180--0x3_11BF
  DBn /*   volatile; uint32_t ff1, /* 0x3_1148 Crypto-channel 1 fetch FIFO address register W 0x0000_0000_0000_0000 14.6.1.4/14-90 Crypto-channel 1 descriptor buffers [0–7] R 0x0000_0000_0000_0000 14.6.1.5/14-91 */
  /* Channel 2-4: FIXME: same layout as channel 1*/
  /* Data Encryption Standard Execution Unit (DEU) */
  volatile uint32_t deumr; /* 0x3_2000 DEU mode register R/W 0x0000_0000_0000_0000 14.5.2.1/14-35 */
  volatile uint32_t deuksr; /* 0x3_2008 DEU key size register R/W 0x0000_0000_0000_0000 14.5.2.2/14-36 */
  volatile uint32_t deudsr; /* 0x3_2010 DEU data size register R/W 0x0000_0000_0000_0000 14.5.2.3/14-36 */
  volatile uint32_t deurcr; /* 0x3_2018 DEU reset control register R/W 0x0000_0000_0000_0000 14.5.2.4/14-37 */
  volatile uint32_t deusr; /* 0x3_2028 DEU status register R 0x0000_0000_0000_0000 14.5.2.5/14-37 */
  volatile uint32_t deuisr; /* 0x3_2030 DEU interrupt status register R 0x0000_0000_0000_0000 14.5.2.6/14-38 */
  volatile uint32_t deuicr; /* 0x3_2038 DEU interrupt control register R/W 0x0000_0000_0000_3000 14.5.2.7/14-40 */
  volatile uint32_t deueug; /* 0x3_2050 DEU EU-Go register W 0x0000_0000_0000_0000 14.5.2.8/14-41 */
  volatile uint32_t deuiv; /* 0x3_2100 DEU initialization vector register R/W 0x0000_0000_0000_0000 14.5.2.9/14-42 */
  volatile uint32_t deuk1; /* 0x3_2400 DEU key 1 register W $ 14.5.2.10/14-42 */
  volatile uint32_t deuk2; /* 0x3_2408 DEU key 2 register W $ 14.5.2.10/14-42 */
  volatile uint32_t deuk3; /* 0x3_2410 DEU key 3 register W $ 14.5.2.10/14-42 */
0x3_2800--0x3_2FFF
DEU FIFO R/W 0x0000_0000_0000_0000 14.5.2.11/14-42
  /* Advanced Encryption Standard Execution Unit (AESU) */
  volatile uint32_t aesumr; /* 0x3_4000 AESU mode register R/W 0x0000_0000_0000_0000 14.5.6.1/14-68 */
  volatile uint32_t aesuksr; /* 0x3_4008 AESU key size register R/W 0x0000_0000_0000_0000 14.5.6.2/14-71 */
  volatile uint32_t aesudsr; /* 0x3_4010 AESU data size register R/W 0x0000_0000_0000_0000 14.5.6.3/14-71 */
  volatile uint32_t aesurcr; /* 0x3_4018 AESU reset control register R/W 0x0000_0000_0000_0000 14.5.6.4/14-72 */
  volatile uint32_t aesusr; /* 0x3_4028 AESU status register R 0x0000_0000_0000_0000 14.5.6.5/14-73 */
  volatile uint32_t aesuisr; /* 0x3_4030 AESU interrupt status register R 0x0000_0000_0000_0000 14.5.6.6/14-74 */
  volatile uint32_t aesuicr; /* 0x3_4038 AESU interrupt control register R/W 0x0000_0000_0000_1000 14.5.6.7/14-75 */
  volatile uint32_t aesuemr; /* 0x3_4050 AESU end-of-message register W 0x0000_0000_0000_0000 14.5.6.8/14-76 */
0x3_4100 AESU context memory registers R/W 0x0000_0000_0000_0000 14.5.6.9/14-77
0x3_4400--0x3_4408
AESU key memory R/W 0x0000_0000_0000_0000 14.5.6.9.5/14-81
0x3_4800--0x3_4FFF
AESU FIFO R/W 0x0000_0000_0000_0000 14.5.6.9.6/14-81
  /* Message Digest Execution Unit (MDEU) */
  volatile uint32_t mdeumr; /* 0x3_6000 MDEU mode register R/W 0x0000_0000_0000_0000 14.5.4.1/14-51 */
  volatile uint32_t mdeuksr; /* 0x3_6008 MDEU key size register R/W 0x0000_0000_0000_0000 14.5.4.3/14-55 */
  volatile uint32_t mdeudsr; /* 0x3_6010 MDEU data size register R/W 0x0000_0000_0000_0000 14.5.4.4/14-56 */
  volatile uint32_t mdeurcr; /* 0x3_6018 MDEU reset control register R/W 0x0000_0000_0000_0000 14.5.4.5/14-56 */
  volatile uint32_t mdeusr; /* 0x3_6028 MDEU status register R 0x0000_0000_0000_0000 14.5.4.6/14-57 */
  volatile uint32_t mdeuisr; /* 0x3_6030 MDEU interrupt status register R 0x0000_0000_0000_0000 14.5.4.7/14-58 */
  volatile uint32_t mdeuicr; /* 0x3_6038 MDEU interrupt control register R/W 0x0000_0000_0000_1000 14.5.4.8/14-59 */
  volatile uint32_t mdeueug; /* 0x3_6050 MDEU EU-Go register W 0x0000_0000_0000_0000 14.5.4.10/14-61 */
0x3_6100--0x3_6120
MDEU context memory registers R/W 0x0000_0000_0000_0000 14.5.4.11/14-61
0x3_6400--0x3_647F
MDEU key memory W 0x0000_0000_0000_0000 14.5.4.12/14-62
0x3_6800--0x3_6FFF
MDEU FIFO W 0x0000_0000_0000_0000 14.5.4.13/14-63
  /* ARC Four Execution Unit (AFEU) */
  volatile uint32_t afeumr; /* 0x3_8000 AFEU mode register R/W 0x0000_0000_0000_0000 14.5.3.1/14-43 */
  volatile uint32_t afeuksr; /* 0x3_8008 AFEU key size register R/W 0x0000_0000_0000_0000 14.5.3.3/14-44 */
  volatile uint32_t afeudsr; /* 0x3_8010 AFEU data size register R/W 0x0000_0000_0000_0000 14.5.3.4/14-45 */
  volatile uint32_t afeurcr; /* 0x3_8018 AFEU reset control register R/W 0x0000_0000_0000_0000 14.5.3.5/14-46 */
  volatile uint32_t afeusr; /* 0x3_8028 AFEU status register R 0x0000_0000_0000_0000 14.5.3.6/14-46 */
  volatile uint32_t afeuisr; /* 0x3_8030 AFEU interrupt status register R 0x0000_0000_0000_0000 14.5.3.7/14-47 */
  volatile uint32_t afeuicr; /* 0x3_8038 AFEU interrupt control register R/W 0x0000_0000_0000_1000 14.5.3.8/14-49 */
  volatile uint32_t afeuemr; /* 0x3_8050 AFEU end of message register W 0x0000_0000_0000_0000 14.5.3.9/14-50 */
0x3_8100--0x3_81FF
AFEU context memory registers R/W 0x0000_0000_0000_0000 14.5.3.10.1/14-50
0x3_8200 AFEU context memory pointers R/W 0x0000_0000_0000_0000 14.5.3.10.2/14-51
  volatile uint32_t afeuk0; /* 0x3_8400 AFEU key register 0 W $ 14.5.3.11/14-51 */
  volatile uint32_t afeuk1; /* 0x3_848 AFEU key register 1 W $ 14.5.3.11/14-51 */
0x3_8800--0x3_8FFF
AFEU FIFO R/W 0x0000_0000_0000_0000 14.5.3.11.1/14-51
  /* Random Number Generator (RNG) */
  volatile uint32_t rngmr; /* 0x3_A000 RNG mode register R/W 0x0000_0000_0000_0000 14.5.5.1/14-63 */
  volatile uint32_t rngdsr; /* 0x3_A010 RNG data size register R/W 0x0000_0000_0000_0000 14.5.5.2/14-64 */
  volatile uint32_t rngrcr; /* 0x3_A018 RNG reset control register R/W 0x0000_0000_0000_0000 14.5.5.3/14-65 */
  volatile uint32_t rngsr; /* 0x3_A028 RNG status register R 0x0000_0000_0000_0000 14.5.5.4/14-65 */
  volatile uint32_t rngisr; /* 0x3_A030 RNG interrupt status register R 0x0000_0000 */
_0000_0000
14.5.5.5/14-66
  volatile uint32_t rngicr; /* 0x3_A038 RNG interrupt control register R/W 0x0000_0000 */
_0000_1000
14.5.5.6/14-67
  volatile uint32_t rngeug; /* 0x3_A050 RNG EU-Go register W 0x0000_0000 */
_0000_0000
14.5.5.7/14-68
0x3_A800--0x3_AFFF
RNG FIFO R 0x0000_0000
_0000_0000
14.5.5.8/14-68
  /* Public Key Execution Unit (PKEU) */
  volatile uint32_t pkeumr; /* 0x3_C000 PKEU mode register R/W 0x0000_0000_0000_0000 14.5.1.1/14-26 */
  volatile uint32_t pkeuksr; /* 0x3_C008 PKEU key size register R/W 0x0000_0000_0000_0000 14.5.1.2/14-28 */
  volatile uint32_t pkeudsr; /* 0x3_C010 PKEU data size register R/W 0x0000_0000_0000_0000 14.5.1.3/14-28 */
  volatile uint32_t pkeurcr; /* 0x3_C018 PKEU reset control register R/W 0x0000_0000_0000_0000 14.5.1.5/14-29 */
  volatile uint32_t pkeusr; /* 0x3_C028 PKEU status register R 0x0000_0000_0000_0000 14.5.1.6/14-30 */
  volatile uint32_t pkeuisr; /* 0x3_C030 PKEU interrupt status register R 0x0000_0000_0000_0000 14.5.1.7/14-31 */
  volatile uint32_t pkeuicr; /* 0x3_C038 PKEU interrupt control register R/W 0x0000_0000_0000_1000 14.5.1.8/14-32 */
  volatile uint32_t pkeuabs; /* 0x3_C040 PKEU AB size register R/W 0x0000_0000_0000_0000 14.5.1.3/14-28 */
  volatile uint32_t pkeueug; /* 0x3_C050 PKEU EU-Go W 0x0000_0000_0000_0000 14.5.1.9/14-33 */
0x3_C200--0x3_C23F
PKEU parameter memory A0 R/W 0x0000_0000_0000_0000 14.5.1.10/14-34
0x3_C240--0x3_C27F
PKEU parameter memory A1 R/W 0x0000_0000_0000_0000
0x3_C280--0x3_C2BF
PKEU parameter memory A2 R/W 0x0000_0000_0000_0000
0x3_C2C0--0x3_C2FF
PKEU parameter memory A3 R/W 0x0000_0000_0000_0000
0x3_C300--0x3_C33F
PKEU parameter memory B0 R/W 0x0000_0000_0000_0000
0x3_C340--0x3_C37F
PKEU parameter memory B1 R/W 0x0000_0000_0000_0000
0x3_C380--0x3_C3BF
PKEU parameter memory B2 R/W 0x0000_0000_0000_0000
0x3_C3C0--0x3_C3FF
PKEU parameter memory B3 R/W 0x0000_0000_0000_0000
0x3_C400--0x3_C4FF
PKEU parameter memory E W 0x0000_0000_0000_0000
0x3_C800--0x3_C8FF
PKEU parameter memory N R/W 0x0000_0000_0000_0000
#endif

typedef struct m83xxRegisters_ {
  m83xxSysConRegisters_t  syscon;
  m83xxWDTRegisters_t     wdt;
  uint8_t                 reserved0_0210[0x0300-0x0210];
  m83xxRTCRegisters_t     rtc;
  uint8_t                 reserved0_0320[0x0400-0x0320];
  m83xxPITRegisters_t     pit;
  m83xxGTMRegisters_t     gtm[M83xxGTModCnt];
  m83xxIPICRegisters_t    ipic;
  m83xxARBRegisters_t     arb;
  m83xxRESRegisters_t     res;
  m83xxCLKRegisters_t     clk;
  m83xxPMCRegisters_t     pmc;
  m83xxGPIORegisters_t    gpio[2];
  uint8_t                 reserved0_0E00[0x1000-0x0E00];
  m83xxDLLRegisters_t     dll;
  uint8_t                 reserved0_1200[0x2000-0x1200];
  m83xxDDRRegisters_t     ddr;
  uint8_t                 reserved0_2F00[0x3000-0x2F00];
  m83xxI2CRegisters_t     i2c[2];
  uint8_t                 reserved0_3200[0x4000-0x3200];
  uint8_t                 reserved0_4000[0x4500-0x4000];
  m83xxDUARTRegisters_t   duart[2];
  uint8_t                 reserved0_4700[0x5000-0x4700];
  m83xxLBCRegisters_t     lbc;
  uint8_t                 reserved0_5100[0x7000-0x5100];
  m83xxSPIRegisters_t     spi;
  uint8_t                 reserved0_7100[0x8000-0x7100];
  m83xxDMARegisters_t     dma;
  m83xxPCICfgRegisters_t  pcicfg[2];
  m83xxPCIIosRegisters_t  pciios;
  m83xxPCICtrlRegisters_t pcictrl[2];
  uint8_t                 reserved0_8700[0x22000-0x8700];
  m83xxUSB_MPHRegisters_t usb_mph;
  m83xxUSB_DRRegisters_t  usb_dr;
  volatile tsec_registers tsec[TSEC_COUNT];
} m83xxRegisters_t;

extern m83xxRegisters_t mpc83xx;

#endif /* !defined ASM */
/*
 * some definitions used in assembler startup
 */
/*
 * default address of IMMRBAR
 */
#define IMMRBAR_DEFAULT 0xFF400000
/*
 * offsets of some registers
 */
#define LBLAWBAR0_OFF  0x00020
#define LBLAWAR0_OFF   0x00024
#define LBLAWBAR1_OFF  0x00028
#define LBLAWAR1_OFF   0x0002C
#define LBLAWBAR2_OFF  0x00030
#define LBLAWAR2_OFF   0x00034
#define LBLAWBAR3_OFF  0x00038
#define LBLAWAR3_OFF   0x0003C
#define PCILAWBAR0_OFF 0x00060
#define PCILAWAR0_OFF  0x00064
#define PCILAWBAR1_OFF 0x00068
#define PCILAWAR1_OFF  0x0006C
#define DDRLAWBAR0_OFF 0x000A0
#define DDRLAWAR0_OFF  0x000A4
#define DDRLAWBAR1_OFF 0x000A8
#define DDRLAWAR1_OFF  0x000AC

#define BR0_OFF  0x05000
#define OR0_OFF  0x05004
#define BR1_OFF  0x05008
#define OR1_OFF  0x0500C
#define BR2_OFF  0x05010
#define OR2_OFF  0x05014
#define BR3_OFF  0x05018
#define OR3_OFF  0x0501C
#define BR4_OFF  0x05020
#define OR4_OFF  0x05024
#define BR5_OFF  0x05028
#define OR5_OFF  0x0502C
#define BR6_OFF  0x05030
#define OR6_OFF  0x05034
#define BR7_OFF  0x05038
#define OR7_OFF  0x0503C

#define MRPTR_OFF                    0x05084
#define LSDMR_OFF                    0x05094
#define LSRT_OFF                     0x050A4
#define LCRR_OFF                     0x050d4


#define CS0_BNDS_OFF       0x02000
#define CS1_BNDS_OFF       0x02008
#define CS2_BNDS_OFF       0x02010
#define CS3_BNDS_OFF       0x02018
#define CS0_CONFIG_OFF     0x02080
#define CS1_CONFIG_OFF     0x02084
#define CS2_CONFIG_OFF     0x02088
#define CS3_CONFIG_OFF     0x0208C
#define TIMING_CFG_3_OFF   0x02100
#define TIMING_CFG_0_OFF   0x02104
#define TIMING_CFG_1_OFF   0x02108
#define TIMING_CFG_2_OFF   0x0210C
#define DDR_SDRAM_CFG_OFF            0x02110
#define DDR_SDRAM_CFG_2_OFF          0x02114
#define DDR_SDRAM_MODE_OFF           0x02118
#define DDR_SDRAM_MODE_2_OFF         0x0211C
#define DDR_SDRAM_MD_CNTL_OFF        0x02120
#define DDR_SDRAM_INTERVAL_OFF       0x02124
#define DDR_SDRAM_DATA_INIT_OFF      0x02128
#define DDRCDR_OFF                   0x0012C
#define DDR_SDRAM_CLK_CNTL_OFF       0x02130
#define DDR_SDRAM_INIT_ADDR_OFF      0x02148
#define DDR_ERR_DISABLE_OFF          0x02E44

/*
 * some bits in DDR_SDRAM_CFG register
 */
#define DDR_SDRAM_CFG_MEM_EN (1 << (31- 0))     /* enable memory        */
/*
 * bits in DDR_SDRAM_CFG_2 register
 */
#define DDR_SDRAM_CFG_2_D_FRC_SR (1 << (31- 0)) /* force self refresh        */
#define DDR_SDRAM_CFG_2_D_SR_IE  (1 << (31- 1)) /* self refresh interrupt en */
#define DDR_SDRAM_CFG_2_D_DLL_RST_DIS  (1 << (31- 2)) /* DLL reset disable   */
#define DDR_SDRAM_CFG_2_D_DQS_CFG_DIF  (1 << (31- 5)) /* use diff. DQS       */
#define DDR_SDRAM_CFG_2_D_INIT   (1 << (31-27)) /* Init DRAM with pattern    */

/*
 * bits in reset configuration words/registers
 */
                                           /* Local bus clocking mode */
#define RCWLR_LBIUCM_1_1 (0 << (31- 0))    /* 1:1 */
#define RCWLR_LBIUCM_2_1 (1 << (31- 0))    /* 2:1 */
                                           /* DDR clocking mode      */
#define RCWLR_DDRCM_1_1  (0 << (31- 1))    /* 1:1 */
#define RCWLR_DDRCM_2_1  (1 << (31- 1))    /* 2:1 */
                                          /* System PLL mult. factor */
#define RCWLR_SPMF(n) (((n)&0xf)<<(31- 7))
                                          /* Core PLL mult. factor   */
#define RCWLR_COREPLL(n) (((n)&0xff)<<(31-15))

/* for MPC8309: */
#define RCWLR_CEVCOD_1_4 (0<<(31-25))     /* QUICC internal PLL divider 1:4 */
#define RCWLR_CEVCOD_1_2 (2<<(31-25))     /* QUICC internal PLL divider 1:2 */
                                          /* QUICC Engine PLL mult. factor */
#define RCWLR_CEPDF_2     (1<<(31-26))    /* QUICC Engine divide PLL out by 2*/
                                          /* QUICC Engine PLL mult. factor */
#define RCWLR_CEPMF(n) (((n)&0x1f)<<(31-31))

                                           /* PCI host mode          */
#define RCWHR_PCI_AGENT  (0 << (31- 0))    /* agent mode             */
#define RCWHR_PCI_HOST   (1 << (31- 0))    /* host mode              */

#define RCWHR_PCI_32     (0 << (31- 1))    /* PCI bus width 32 bit   */
#define RCWHR_PCI_64     (1 << (31- 1))    /* PCI bus width 64 bit   */

#define RCWHR_PCI1ARB_DIS (0 << (31- 2))   /* PCI1 arbiter disabled */
#define RCWHR_PCI1ARB_EN  (1 << (31- 2))   /* PCI1 arbiter enabled  */
#define RCWHR_PCI2ARB_DIS (0 << (31- 3))   /* PCI2 arbiter disabled */
#define RCWHR_PCI2ARB_EN  (1 << (31- 3))   /* PCI2 arbiter enabled  */

#define RCWHR_CORE_DIS    (1 << (31- 4))   /* CPU core disabled     */
#define RCWHR_CORE_EN     (0 << (31- 4))   /* CPU core enabled      */

#define RCWHR_BMS_LOW     (0 << (31- 5))   /* Boot from low addr  0x00000100 */
#define RCWHR_BMS_HIGH    (1 << (31- 5))   /* Boot from high addr 0xFFF00100 */

#define RCWHR_BOOTSEQ_NONE (0 <<(31- 7))   /* Bootsequencer off              */
#define RCWHR_BOOTSEQ_NORM (1 <<(31- 7))   /* Bootsequencer normal I2C       */
#define RCWHR_BOOTSEQ_EXTD (2 <<(31- 7))   /* Bootsequencer extended I2C     */
#define RCWHR_BOOTSEQ_RSRV (3 <<(31- 7))   /* Bootsequencer reserved         */

#define RCWHR_SW_DIS      (0 << (31- 8))   /* Watchdog disabled     */
#define RCWHR_SW_EN       (1 << (31- 8))   /* Watchdog enabled      */

#define RCWHR_ROMLOC_DDR  (0 << (31-11))   /* Initial ROM location:DDR Ram   */
#define RCWHR_ROMLOC_PCI1 (1 << (31-11))   /* Initial ROM location:PCI 1     */
#define RCWHR_ROMLOC_PCI2 (2 << (31-11))   /* Initial ROM location:PCI 2     */
#define RCWHR_ROMLOC_RSV1 (3 << (31-11))   /* Initial ROM location:Reserved  */
#define RCWHR_ROMLOC_RSV2 (4 << (31-11))   /* Initial ROM location:Reserved  */
#define RCWHR_ROMLOC_LB08 (5 << (31-11))   /* Initial ROM location:LBus  8bit*/
#define RCWHR_ROMLOC_LB16 (6 << (31-11))   /* Initial ROM location:LBus 16bit*/
#define RCWHR_ROMLOC_LB32 (7 << (31-11))   /* Initial ROM location:LBus 32bit*/

#define RCWHR_TSEC1M_RGMII (0 << (31-17))  /* TSEC1 Mode: RGMII  */
#define RCWHR_TSEC1M_RTBI  (1 << (31-17))  /* TSEC1 Mode: RTBI   */
#define RCWHR_TSEC1M_GMII  (2 << (31-17))  /* TSEC1 Mode: GMII   */
#define RCWHR_TSEC1M_TBI   (3 << (31-17))  /* TSEC1 Mode: TBI    */

#define RCWHR_TSEC2M_RGMII (0 << (31-19))  /* TSEC2 Mode: RGMII  */
#define RCWHR_TSEC2M_RTBI  (1 << (31-19))  /* TSEC2 Mode: RTBI   */
#define RCWHR_TSEC2M_GMII  (2 << (31-19))  /* TSEC2 Mode: GMII   */
#define RCWHR_TSEC2M_TBI   (3 << (31-19))  /* TSEC2 Mode: TBI    */

#define RCWHR_ENDIAN_BIG  (0 << (31-28))   /* Big Endian Mode         */
#define RCWHR_ENDIAN_LIT  (1 << (31-28))   /* True Little Endian Mode */

#define RCWHR_LALE_NORM   (0 << (31-29))   /* normal LALE timing      */
#define RCWHR_LALE_EARLY  (1 << (31-29))   /* early LALE negation     */

#define RCWHR_LDP_PAR     (0 << (31-30))   /* LDP0-3 are parity pins  */
#define RCWHR_LDP_SPC     (1 << (31-30))   /* LDP0-3 are special pins */

/*
 * For MPC8309:
 */
#define RCWHR_RLEXT_LGCY  (0 << (31-13))  /* Boot ROM loc. extension: Legacy */
#define RCWHR_RLEXT_NAND  (1 << (31-13))  /* Boot ROM loc. extension: NAND Fl.*/
#define RCWHR_RLEXT_RSV2  (2 << (31-13))  /* Boot ROM loc. extension: resrvd */
#define RCWHR_RLEXT_RSV3  (3 << (31-13))  /* Boot ROM loc. extension: resrvd */
#endif /* _MPC83XX_MPC83XX_H */
