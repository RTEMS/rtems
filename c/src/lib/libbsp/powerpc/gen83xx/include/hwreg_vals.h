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
#include <bsp.h>
/*
 * distinguish board characteristics
 */
#if defined(MPC83XX_BOARD_MPC8349EAMDS)
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
#elif defined(MPC83XX_BOARD_HSC_CM01)
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
			  RCWHR_PCI1ARB_DIS  |	\
			  RCWHR_PCI2ARB_DIS  |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_TSEC1M_RGMII |	\
			  RCWHR_TSEC2M_GMII  |	\
			  RCWHR_ENDIAN_BIG   |	\
			  RCWHR_LALE_EARLY   |	\
			  RCWHR_LDP_SPC)

#elif defined(MPC83XX_BOARD_BR_UID)
/*
 * for BR UID
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
#define BSP_CLKIN_FRQ 25000000L
#define RCFG_SYSPLL_MF  5
#define RCFG_COREPLL_MF 5
/*
 * Reset configuration words
 */
#define RESET_CONF_WRD_L \
  (RCWLR_LBIUCM_1_1							\
   | RCWLR_DDRCM_2_1							\
   | RCWLR_SPMF(RCFG_SYSPLL_MF)    					\
   | RCWLR_COREPLL(RCFG_COREPLL_MF)					\
   | RCWLR_CEVCOD_1_2 							\
   | RCWLR_CEPMF(8)							\
   )

#define RESET_CONF_WRD_H (RCWHR_PCI_HOST     |	\
			  RCWHR_PCI_32       |	\
			  RCWHR_PCI1ARB_DIS  |	\
			  RCWHR_CORE_EN      |	\
			  RCWHR_BMS_LOW      |	\
			  RCWHR_BOOTSEQ_NONE |	\
			  RCWHR_SW_DIS       |	\
			  RCWHR_ROMLOC_LB16  |	\
			  RCWHR_RLEXT_LGCY   |	\
			  RCWHR_ENDIAN_BIG)

#elif defined( HAS_UBOOT)

/* TODO */

#else

#error "board type not defined"

#endif

#if defined(MPC83XX_BOARD_MPC8349EAMDS)
/**************************
 * for Freescale MPC83XX_BOARD_MPC8349EAMDS
 */

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

#elif defined(MPC83XX_BOARD_HSC_CM01)
/**************************
 * for JPK HSC_CM01
 */

/* fpga BCSR register */
#define FPGA_START 0xF8000000
#define FPGA_SIZE  0x8000
#define FPGA_END   (FPGA_START+FPGA_SIZE-1)

/*
 * working values for various registers, used in start/start.S
 */

/* fpga config 16 MB size */
#define FPGA_CONFIG_START       0xF8000000
#define FPGA_CONFIG_SIZE        0x01000000
/* fpga register 8 MB size */
#define FPGA_REGISTER_START     0xF9000000
#define FPGA_REGISTER_SIZE      0x00800000
/* fpga fifo 8 MB size */
#define FPGA_FIFO_START         0xF9800000
#define FPGA_FIFO_SIZE          0x00800000

#define FPGA_START (FPGA_CONFIG_START)
// fpga window size 32 MByte
#define FPGA_SIZE  (0x02000000)
#define FPGA_END   (FPGA_START+FPGA_SIZE-1)

/*
 * Local Access Windows
 * FIXME: decode bit settings
 */

#define LBLAWBAR0_VAL  bsp_rom_start
#define LBLAWAR0_VAL   0x80000018
#define LBLAWBAR1_VAL  (FPGA_CONFIG_START)
#define LBLAWAR1_VAL   0x80000018
#define DDRLAWBAR0_VAL bsp_ram_start
#define DDRLAWAR0_VAL  0x8000001B
/*
 * Local Bus (Memory) Controller
 * FIXME: decode bit settings
 */
#define BR0_VAL (0xFE000000 | 0x01001)
#define OR0_VAL 0xFE000E54
// fpga config access range (UPM_A) (32 kByte)
#define BR2_VAL (FPGA_CONFIG_START | 0x01881)
#define OR2_VAL 0xFFFF9100

// fpga register access range (UPM_B) (8 MByte)
#define BR3_VAL (FPGA_REGISTER_START | 0x018A1)
#define OR3_VAL 0xFF801100

// fpga fifo access range (UPM_C) (8 MByte)
#define BR4_VAL (FPGA_FIFO_START | 0x018C1)
#define OR4_VAL 0xFF801100

/*
 * SDRAM registers
 */
#define MRPTR_VAL 0x20000000
#define LSRT_VAL  0x32000000
#define LSDMR_VAL 0x4062D733
#define LCRR_VAL  0x80010004

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
#define DDR_SDRAM_CFG_VAL            0x63000008

#define DDR_ERR_DISABLE_VAL          0x0000008D
#define DDR_ERR_DISABLE_VAL2         0x00000089
#define DDR_SDRAM_DATA_INIT_VAL      0xC01DCAFE
#define DDR_SDRAM_INIT_ADDR_VAL      0
#define DDR_SDRAM_INTERVAL_VAL       0x05080000

#elif defined(MPC83XX_BOARD_BR_UID)
/**************************
 * for BR UID
 */

/*
 * working values for various registers, used in start/start.S
 */

/*
 * Local Access Windows
 * FIXME: decode bit settings
 */

#define LBLAWBAR0_VAL  bsp_rom_start
#define LBLAWAR0_VAL   0x80000018
#define DDRLAWBAR0_VAL bsp_ram_start
#define DDRLAWAR0_VAL  0x8000001B


/*
 * clocking for local bus:
 * ALE active for 1 clock
 * local bus clock = 1/2 csb clock
 */
#define LCRR_VAL  0x80010002

/*
 * DDR-SDRAM registers
 * FIXME: decode bit settings
 */
#define DDRCDR_VAL                   0x00000001
#define CS0_BNDS_VAL                 0x0000000F
#define CS0_CONFIG_VAL               0x80014202
#define TIMING_CFG_0_VAL             0x00220802
#define TIMING_CFG_1_VAL             0x26259222
#define TIMING_CFG_2_VAL             0x111048C7
#define DDR_SDRAM_CFG_2_VAL          0x00401000
#define DDR_SDRAM_MODE_VAL           0x200F1632
#define DDR_SDRAM_MODE_2_VAL         0x40006000
#define DDR_SDRAM_CLK_CNTL_VAL       0x01800000
#define DDR_SDRAM_CFG_VAL            0x43100008

#define DDR_ERR_DISABLE_VAL          0x0000008D
#define DDR_ERR_DISABLE_VAL2         0x00000089
#define DDR_SDRAM_DATA_INIT_VAL      0xC01DCAFE
#define DDR_SDRAM_INIT_ADDR_VAL      0
#define DDR_SDRAM_INTERVAL_VAL       0x01E8222E

#elif defined( HAS_UBOOT)

/* TODO */

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
