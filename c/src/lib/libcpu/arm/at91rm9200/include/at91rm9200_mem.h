/*
 * AT91RM9200 Memory Controller definitions
 *
 * Copyright (c) 2002 by Cogent Computer Systems
 * Written by Mike Kelly <mike@cogcomp.com>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
#ifndef AT91RM9200_MEM_H
#define AT91RM9200_MEM_H

/**********************************************************************
 *       External Bus Interface Unit
 **********************************************************************/
#define EBI_CSA             0x00    /* Chip Select Assignment Register */
#define EBI_CFGR            0x04    /* Configuration Register */

/* Bit Defines */
/* EBI_CSA - Chip Select Assignment Register */
#define EBI_CSA_CS4_CF        BIT4    /* 1 = CS4-6 are assigned to Compact Flash, 0 = Chip Selects */
#define EBI_CSA_CS3_SMM       BIT3    /* 1 = CS3 is assigned to SmartMedia, 0 = Chip Select */
#define EBI_CSA_CS1_SDRAM     BIT1    /* 1 = CS1 is assigned to SDRAM, 0 = Chip Select */
#define EBI_CSA_CS0_BF        BIT0    /* 1 = CS0 is assigned to Burst Flash, 0 = Chip Select */

/* EBI_CFGR     - Configuration Register */
#define EBI_CFGR_DBPU         BIT0    /* 1 = Disable D0-15 pullups         */

/***************************************************************************
 * Static Memory Interface Unit
 ***************************************************************************/
#define SMC_CSR0            0x00    /* Chip Select Register 0  */
#define SMC_CSR1            0x04    /* Chip Select Register 1  */
#define SMC_CSR2            0x08    /* Chip Select Register 2  */
#define SMC_CSR3            0x0C    /* Chip Select Register 3  */
#define SMC_CSR4            0x10    /* Chip Select Register 4  */
#define SMC_CSR5            0x14    /* Chip Select Register 5  */
#define SMC_CSR6            0x18    /* Chip Select Register 6  */
#define SMC_CSR7            0x1C    /* Chip Select Register 7  */

/* Bit Defines */
/* SMC_CSR0 -7 - Chip Selects 0 - 7 Register */
#define SMC_CSR_RWHOLD(_x_)        ((_x_ & 0x3) << 28)     /* Hold CS after R/W strobes */
#define SMC_CSR_RWSETUP(_x_)    ((_x_ & 0x3) << 24)     /* Setup CS before R/W strobes */
#define SMC_CSR_ACSS_0        (0 << 16)           /* Setup/Hold Address 0 clocks before/after CS */
#define SMC_CSR_ACSS_1        (1 << 16)           /* Setup/Hold Address 1 clock before/after CS */
#define SMC_CSR_ACSS_2        (2 << 16)           /* Setup/Hold Address 2 clocks before/after CS */
#define SMC_CSR_ACSS_3        (3 << 16)           /* Setup/Hold Address 3 clocks before/after CS */
#define SMC_CSR_DRP_NORMAL    0                 /* 0 = normal read protocol */
#define SMC_CSR_DRP_EARLY     BIT15          /* 1 = early read protocol */
#define SMC_CSR_DBW_16        (1 << 13)           /* CS DataBus Width = 16-Bits */
#define SMC_CSR_DBW_8         (2 << 13)           /* CS DataBus Width = 8 Bits */
#define SMC_CSR_BAT_16_1      0                 /* Single 16-Bit device (when DBW is 16) */
#define SMC_CSR_BAT_16_2      BIT12          /* Dual 8-Bit devices (when DBW is 16) */
#define SMC_CSR_TDF(_x_)      ((_x_ & 0xf) << 8)    /* Intercycle Data Float Time */
#define SMC_CSR_WSEN          BIT7           /* 1 = wait states are enabled */
#define SMC_CSR_NWS(_x_)      ((_x_ & 0x7f) << 0)     /* Wait States + 1 */

/* ***************************************************************************** */
/* SDRAM Memory Interface Unit */
/* ***************************************************************************** */
#define SDRC_MR             0x00       /* Mode Register */
#define SDRC_TR             0x04       /* Refresh Timer Register */
#define SDRC_CR             0x08       /* Configuration Register */
#define SDRC_SRR            0x0C       /* Self Refresh Register */
#define SDRC_LPR            0x10       /* Low Power Register */
#define SDRC_IER            0x14       /* Interrupt Enable Register */
#define SDRC_IDR            0x18       /* Interrupt Disable Register */
#define SDRC_IMR            0x1C       /* Interrupt Mask Register */
#define SDRC_ISR            0x20       /* Interrupt Status Register */

/* Bit Defines */
/* SDRC_MR - Mode Register */
#define SDRC_MR_DBW_16        BIT4       /* 1 = SDRAM is 16-bits wide, 0 = 32-bits */
#define SDRC_MR_NORM          (0 << 0)   /* Normal Mode - All accesses to SDRAM are decoded normally */
#define SDRC_MR_NOP         (1 << 0)   /* NOP Command is sent to SDRAM */
#define SDRC_MR_PRE         (2 << 0)   /* Precharge All Command is sent to SDRAM */
#define SDRC_MR_MRS         (3 << 0)   /* Mode Register Set Command is sent to SDRAM */
#define SDRC_MR_REF         (4 << 0)   /* Refresh Command is sent to SDRAM */

/* SDRC_TR - Refresh Timer Register */
#define SDRC_TR_COUNT(_x_)    ((_x_ & 0xfff) << 0)

/* SDRC_CR - Configuration Register */
#define SDRC_CR_TXSR(_x_)     ((_x_ & 0xf) << 27)     /* CKE to ACT Time */
#define SDRC_CR_TRAS(_x_)     ((_x_ & 0xf) << 23)     /* ACT to PRE Time */
#define SDRC_CR_TRCD(_x_)     ((_x_ & 0xf) << 19)     /* RAS to CAS Time */
#define SDRC_CR_TRP(_x_)      ((_x_ & 0xf) << 15)     /* PRE to ACT Time */
#define SDRC_CR_TRC(_x_)      ((_x_ & 0xf) << 11)     /* REF to ACT Time */
#define SDRC_CR_TWR(_x_)      ((_x_ & 0xf) << 7)      /* Write Recovery Time */
#define SDRC_CR_CAS_2         (2 << 5)   /* Cas Delay = 2, this is the only supported value */
#define SDRC_CR_NB_2          0        /* 2 Banks per device */
#define SDRC_CR_NB_4          BIT4       /* 4 Banks per device */
#define SDRC_CR_NR_11         (0 << 2)   /* Number of rows = 11 */
#define SDRC_CR_NR_12         (1 << 2)   /* Number of rows = 12 */
#define SDRC_CR_NR_13         (2 << 2)   /* Number of rows = 13 */
#define SDRC_CR_NC_8          (0 << 0)   /* Number of columns = 8 */
#define SDRC_CR_NC_9          (1 << 0)   /* Number of columns = 9 */
#define SDRC_CR_NC_10         (2 << 0)   /* Number of columns = 10 */
#define SDRC_CR_NC_11         (3 << 0)   /* Number of columns = 11 */

/* SDRC_SRR - Self Refresh Register */
#define SDRC_SRR_SRCB         BIT0       /* 1 = Enter Self Refresh */

/* SDRC_LPR - Low Power Register */
#define SDRC_LPR_LPCB         BIT0       /* 1 = De-assert CKE between accesses */

/* SDRC_IER - Interrupt Enable Register */
/* SDRC_IDR - Interrupt Disable Register */
/* SDRC_ISR - Interrupt Mask Register */
/* SDRC_IMR - Interrupt Mask Register */
#define SDRC_INT_RES          BIT0       /* Refresh Error Status */

#endif
