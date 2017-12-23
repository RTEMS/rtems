/*
 * Header file for RTEMS SATCAN FPGA driver
 *
 * COPYRIGHT (c) 2009.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef __SATCAN_H__
#define __SATCAN_H__

#ifdef __cplusplus
extern "C" {
#endif

/* Config structure passed to SatCAN_init(..) */
typedef struct {
	/* Configuration */
	int nodeno;
	int dps;
	/* Callback functions */
	void (*ahb_irq_callback)(void);
	void (*pps_irq_callback)(void);
	void (*m5_irq_callback)(void);
	void (*m4_irq_callback)(void);
	void (*m3_irq_callback)(void);
	void (*m2_irq_callback)(void);
	void (*m1_irq_callback)(void);
	void (*sync_irq_callback)(void);
	void (*can_irq_callback)(unsigned int fifo);
} satcan_config;

#define SATCAN_HEADER_SIZE    4
#define SATCAN_HEADER_NMM_POS 3
#define SATCAN_PAYLOAD_SIZE   8

/* SatCAN message */
typedef struct {
	unsigned char header[SATCAN_HEADER_SIZE];   /* Header of SatCAN message */
	unsigned char payload[SATCAN_PAYLOAD_SIZE]; /* Payload of SatCAN message */
} satcan_msg;

/* SatCAN modify register structure */
typedef struct {
	unsigned int reg;
	unsigned int val;
} satcan_regmod;

/* Driver interface */
int satcan_register(satcan_config *conf);

/* SatCAN interrupt IDs */
#define SATCAN_IRQ_NONACT_TO_ACT    0
#define SATCAN_IRQ_ACTIVE_TO_NONACT 1
#define SATCAN_IRQ_STR1_TO_DPS      2
#define SATCAN_IRQ_DPS_TO_STR1      3
#define SATCAN_IRQ_STR2_TO_DPS      4
#define SATCAN_IRQ_DPS_TO_STR2      5
#define SATCAN_IRQ_STR3_TO_DPS      6
#define SATCAN_IRQ_DPS_TO_STR3      7
#define SATCAN_IRQ_PLD1_TO_DPS      8
#define SATCAN_IRQ_DPS_TO_PLD1      9
#define SATCAN_IRQ_PLD2_TO_DPS      10
#define SATCAN_IRQ_DPS_TO_PLD2      11
#define SATCAN_IRQ_SYNC             16
#define SATCAN_IRQ_TIME_MARKER1     17
#define SATCAN_IRQ_TIME_MARKER2     18
#define SATCAN_IRQ_TIME_MARKER3     19
#define SATCAN_IRQ_TIME_MARKER4     20
#define SATCAN_IRQ_TIME_MARKER5     21
#define SATCAN_IRQ_EOD1             22
#define SATCAN_IRQ_EOD2             23
#define SATCAN_IRQ_TOD              24
#define SATCAN_IRQ_CRITICAL         25

/* IOC */
#define SATCAN_IOC_DMA_2K           1  /* Use DMA area for 2K messages */
#define SATCAN_IOC_DMA_8K           2  /* Use DMA area for 8K messages */
#define SATCAN_IOC_GET_REG          3  /* Provides direct read access to all core registers */
#define SATCAN_IOC_SET_REG          4  /* Provides direct write access to all core registers */
#define SATCAN_IOC_OR_REG           5  /* Provides direct read access to all core registers */
#define SATCAN_IOC_AND_REG          6  /* Provides direct write access to all core registers */
#define SATCAN_IOC_EN_TX1_DIS_TX2   7  /* Enable DMA TX channel 1, Disable DMA TX channel 2 */
#define SATCAN_IOC_EN_TX2_DIS_TX1   8  /* Enable DMA TX channel 2, Disable DMA TX channel 1 */
#define SATCAN_IOC_GET_DMA_MODE     9  /* Returns the current DMA mode */
#define SATCAN_IOC_SET_DMA_MODE     10 /* Sets the DMA mode  */
#define SATCAN_IOC_ACTIVATE_DMA     11 /* Directly activate DMA channel */
#define SATCAN_IOC_DEACTIVATE_DMA   12 /* Directly deactivate DMA channel */
#define SATCAN_IOC_DMA_STATUS       13 /* Returns status of directly activated DMA */
#define SATCAN_IOC_GET_DOFFSET      14 /* Get TX DMA offset */ 
#define SATCAN_IOC_SET_DOFFSET      15 /* Set TX DMA offset */
#define SATCAN_IOC_GET_TIMEOUT      16 /* Set TX DMA timeout */
#define SATCAN_IOC_SET_TIMEOUT      17 /* Get TX DMA timeout */


/* Values used to select core register with IOC_SET_REG/IOC_GET_REG */ 
#define SATCAN_SWRES        0  /* Software reset */
#define SATCAN_INT_EN       1  /* Interrupt enable */
#define SATCAN_FIFO         3  /* FIFO read */
#define SATCAN_FIFO_RES     4  /* FIFO reset */
#define SATCAN_TSTAMP       5  /* Current time stamp */
#define SATCAN_CMD0         6  /* Command register 0 */
#define SATCAN_CMD1         7  /* Command register 1 */
#define SATCAN_START_CTC    8  /* Start cycle time counter */
#define SATCAN_RAM_BASE     9  /* RAM offset address */
#define SATCAN_STOP_CTC     10 /* Stop cycle time counter / DPS active status */
#define SATCAN_DPS_ACT      10 /* Stop cycle time counter / DPS active status */
#define SATCAN_PLL_RST      11 /* DPLL reset */
#define SATCAN_PLL_CMD      12 /* DPLL command */
#define SATCAN_PLL_STAT     13 /* DPLL status */
#define SATCAN_PLL_OFF      14 /* DPLL offset */
#define SATCAN_DMA          15 /* DMA channel enable */
#define SATCAN_DMA_TX_1_CUR 16 /* DMA channel 1 TX current address */
#define SATCAN_DMA_TX_1_END 17 /* DMA channel 1 TX end address */
#define SATCAN_DMA_TX_2_CUR 18 /* DMA channel 2 TX current address */
#define SATCAN_DMA_TX_2_END 19 /* DMA channel 2 TX end address */
#define SATCAN_RX           20 /* CAN RX enable / Filter start ID */
#define SATCAN_FILTER_START 20 /* CAN RX enable / Filter start ID */
#define SATCAN_FILTER_SETUP 21 /* Filter setup / Filter stop ID */
#define SATCAN_FILTER_STOP  21 /* Filter setup / Filter stop ID */
#define SATCAN_WCTRL        32 /* Wrapper status/control register */
#define SATCAN_WIPEND       33 /* Wrapper interrupt pending register */
#define SATCAN_WIMASK       34 /* Wrapper interrupt mask register */
#define SATCAN_WAHBADDR     35 /* Wrapper AHB address register */


/* Values used to communicate DMA mode */
#define SATCAN_DMA_MODE_USER   0
#define SATCAN_DMA_MODE_SYSTEM 1

/* Values used to directly activate DMA channel */
#define SATCAN_DMA_ENABLE_TX1  1
#define SATCAN_DMA_ENABLE_TX2  2

#ifdef __cplusplus
}
#endif

#endif /* __SATCAN_H__ */
