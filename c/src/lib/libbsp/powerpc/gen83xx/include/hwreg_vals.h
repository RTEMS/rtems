/*===============================================================*\
| Project: RTEMS generic MPC83xx BSP                              |
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
| this file contains board specific definitions                   |
\*===============================================================*/

#ifndef __GEN83xx_HWREG_VALS_h
#define __GEN83xx_HWREG_VALS_h

#include <mpc83xx/mpc83xx.h>
/*
 * distinguish board characteristics
 */
#if defined(MPC8349EAMDS)
/*
 * for Freescale MPC8349 EAMDS
 */
/*
 * two DUART channels supported
 */
#define GEN83xx_DUART_AVAIL_MASK 0x03

/* we need the low level initialization in start.S*/
#define NEED_LOW_LEVEL_INIT
/*
 * clocking infos
 */
#define BSP_CLKIN_FRQ 66000000L
#define RCFG_SYSPLL_MF 4 
#define RCFG_COREPLL_MF 4

/*
 * Reset configuration words
 */
#define RESET_CONF_WRD_L (RCWLR_LBIUCM_1_1 |	\
			  RCWLR_DDRCM_1_1  |	\
			  RCWLR_SPMF(RCFG_SYSPLL_MF)    |	\
			  RCWLR_COREPLL(RCFG_COREPLL_MF))

#define RESET_CONF_WRD_H (RCWHR_PCI_HOST     |	\
			  RCWHR_PCI_32       |	\
			  RCWHR_PCI1ARB_EN   |	\
			  RCWHR_PCI2ARB_EN   |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_TSEC1M_GMII  |	\
			  RCWHR_TSEC2M_GMII  |	\
			  RCWHR_ENDIAN_BIG   |	\
			  RCWHR_LALE_NORM    |	\
			  RCWHR_LDP_PAR)
#elif defined(HSC_CM01)
/*
 * for JPK HSC_CM01
 */
/*
 * one DUART channel (UART1) supported
 */
#define GEN83xx_DUART_AVAIL_MASK 0x01

/* we need the low level initialization in start.S*/
#define NEED_LOW_LEVEL_INIT
/*
 * clocking infos
 */
#define BSP_CLKIN_FRQ 30000000L
#define RCFG_SYSPLL_MF 11 
#define RCFG_COREPLL_MF 4
/*
 * Reset configuration words
 */
#define RESET_CONF_WRD_L (RCWLR_LBIUCM_1_1 |	\
			  RCWLR_DDRCM_1_1  |	\
			  RCWLR_SPMF(RCFG_SYSPLL_MF)    |	\
			  RCWLR_COREPLL(RCFG_COREPLL_MF))

#define RESET_CONF_WRD_H (RCWHR_PCI_HOST     |	\
			  RCWHR_PCI_32       |	\
			  RCWHR_PCI1ARB_EN   |	\
			  RCWHR_PCI2ARB_EN   |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_TSEC1M_RGMII |	\
			  RCWHR_TSEC2M_GMII  |	\
			  RCWHR_ENDIAN_BIG   |	\
			  RCWHR_LALE_NORM    |	\
			  RCWHR_LDP_PAR)
#else
#error "board type not defined"
#endif

#if defined(MPC8349EAMDS) 
/**************************
 * for Freescale MPC8349EAMDS
 */
/*
 * address range definitions
 */
/* ROM definitions (8 MB, mirrored multiple times) */
#define	ROM_START	0xFE000000
#define ROM_SIZE        0x02000000
#define	ROM_END		(ROM_START+ROM_SIZE-1)
#define	BOOT_START      ROM_START
#define	BOOT_END        ROM_END

/* SDRAM definitions (256 MB) */
#define	RAM_START       0x00000000
#define RAM_SIZE        0x10000000
#define	RAM_END		(RAM_START+RAM_SIZE-1)


/* working internal memory map base address */
#define	IMMRBAR         0xE0000000

/*
 * working values for various registers, used in start/start.S
 */
/*
 * Local Access Windows
 * FIXME: decode bit settings 
 */
#define LBLAWBAR0_VAL  0xFE000000
#define LBLAWAR0_VAL   0x80000016
#define LBLAWBAR1_VAL  0xF8000000
#define LBLAWAR1_VAL   0x8000000E
#define LBLAWBAR2_VAL  0xF0000000
#define LBLAWAR2_VAL   0x80000019
#define DDRLAWBAR0_VAL 0x00000000
#define DDRLAWAR0_VAL  0x8000001B
/*
 * Local Bus (Memory) Controller
 * FIXME: decode bit settings 
 */
#define BR0_VAL 0xFE001001
#define OR0_VAL 0xFF806FF7
#define BR1_VAL 0xF8000801
#define OR1_VAL 0xFFFFE8F0
#define BR2_VAL 0xF0001861
#define OR2_VAL 0xFC006901
/*
 * SDRAM registers
 * FIXME: decode bit settings 
 */
#define MRPTR_VAL 0x20000000
#define LSRT_VAL  0x32000000
#define LSDMR_VAL 0x4062D733
#define LCRR_VAL  0x80000004

/*
 * DDR-SDRAM registers
 * FIXME: decode bit settings 
 */
#define CS2_BNDS_VAL                 0x00000007
#define CS3_BNDS_VAL                 0x0008000F
#define CS2_CONFIG_VAL               0x80000101
#define CS3_CONFIG_VAL               0x80000101
#define TIMING_CFG_1_VAL             0x36333321
#define TIMING_CFG_2_VAL             0x00000800
#define DDR_SDRAM_CFG_VAL            0xC2000000
#define DDR_SDRAM_MODE_VAL           0x00000022
#define DDR_SDRAM_INTTVL_VAL         0x045B0100
#define DDR_SDRAM_CLK_CNTL_VAL       0x00000000

#elif defined(HSC_CM01)
/**************************
 * for JPK HSC_CM01
 */
/*
 * address range definitions
 */
/* ROM definitions (8 MB, mirrored multiple times) */
#define	ROM_START	0xFE000000
#define ROM_SIZE        0x02000000
#define	ROM_END		(ROM_START+ROM_SIZE-1)
#define	BOOT_START      ROM_START
#define	BOOT_END        ROM_END

/* SDRAM definitions (256 MB) */
#define	RAM_START       0x00000000
#define RAM_SIZE        0x10000000
#define	RAM_END		(RAM_START+RAM_SIZE-1)


/* working internal memory map base address */
#define	IMMRBAR         0xE0000000

/*
 * working values for various registers, used in start/start.S
 */
/*
 * Local Access Windows
 * FIXME: decode bit settings 
 */

#define LBLAWBAR0_VAL  ROM_START
#define LBLAWAR0_VAL   0x80000018
#define LBLAWBAR1_VAL  0xF8000000
#define LBLAWAR1_VAL   0x80000015
#define DDRLAWBAR0_VAL RAM_START
#define DDRLAWAR0_VAL  0x8000001B
/*
 * Local Bus (Memory) Controller
 * FIXME: decode bit settings 
 */
#define BR0_VAL 0xFE001001
#define OR0_VAL 0xFE000E54
#define BR3_VAL 0xF8001881
#define OR3_VAL 0xFFC01100
/*
 * Local (memory) bus divider
 * FIXME: decode bit settings 
 */
#define LCRR_VAL  0x00010004

/*
 * DDR-SDRAM registers
 * FIXME: decode bit settings 
 */
#define DDRCDR_VAL                   0x00000001
#define CS0_BNDS_VAL                 0x0000000F
#define CS0_CONFIG_VAL               0x80810102
#define TIMING_CFG_0_VAL             0x00420802
#define TIMING_CFG_1_VAL             0x3735A322
#define TIMING_CFG_2_VAL             0x2F9044C7
#define DDR_SDRAM_CFG_2_VAL          0x00401000
#define DDR_SDRAM_MODE_VAL           0x44521632
#define DDR_SDRAM_CLK_CNTL_VAL       0x01800000
#define DDR_SDRAM_CFG_VAL            0x43000008

#define DDR_ERR_DISABLE_VAL          0x0000008D
#define DDR_ERR_DISABLE_VAL2         0x00000089
#define DDR_SDRAM_DATA_INIT_VAL      0xC01DCAFE
#define DDR_SDRAM_INIT_ADDR_VAL      0
#define DDR_SDRAM_INTERVAL_VAL       0x05080000
#else
#error "board type not defined"
#endif


/**************************
 * derived values for all boards
 */
/* value of input clock divider (derived from pll mode reg) */
#define BSP_SYSPLL_CKID (((mpc83xx.clk.spmr>>(31-8))&0x01)+1)
/* value of system pll (derived from pll mode reg) */
#define BSP_SYSPLL_MF    ((mpc83xx.clk.spmr>>(31-7))&0x0f) 
/* value of system pll (derived from pll mode reg) */
#define BSP_COREPLL_MF   ((mpc83xx.clk.spmr>>(31-15))&0x7f) 

#endif /* __GEN83xx_HWREG_VALS_h */
