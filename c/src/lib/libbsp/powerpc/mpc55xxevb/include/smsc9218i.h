/**
 * @file
 *
 * @ingroup mpc55xx
 *
 * @brief SMSC - LAN9218i
 */

/*
 * Copyright (c) 2009-2012 embedded brains GmbH.  All rights reserved.
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

#include <bsp.h>

/**
 * @name Memory Map
 * @{
 */

typedef struct {
  uint32_t rx_fifo_data;
  uint32_t rx_fifo_data_aliases [7];
  uint32_t tx_fifo_data;
  uint32_t tx_fifo_data_aliases [7];
  uint32_t rx_fifo_status;
  uint32_t rx_fifo_status_peek;
  uint32_t tx_fifo_status;
  uint32_t tx_fifo_status_peek;
  uint32_t id_rev;
  uint32_t irq_cfg;
  uint32_t int_sts;
  uint32_t int_en;
  uint32_t reserved_0;
  uint32_t byte_test;
  uint32_t fifo_int;
  uint32_t rx_cfg;
  uint32_t tx_cfg;
  uint32_t hw_cfg;
  uint32_t rx_dp_ctl;
  uint32_t rx_fifo_inf;
  uint32_t tx_fifo_inf;
  uint32_t pmt_ctrl;
  uint32_t gpio_cfg;
  uint32_t gpt_cfg;
  uint32_t gpt_cnt;
  uint32_t reserved_1;
  uint32_t word_swap;
  uint32_t free_run;
  uint32_t rx_drop;
  uint32_t mac_csr_cmd;
  uint32_t mac_csr_data;
  uint32_t afc_cfg;
  uint32_t e2p_cmd;
  uint32_t e2p_data;
} smsc9218i_registers;

/*
 * SMSC9218 registers are accessed little-endian (address 0x3fff8000, A22 used
 * as END_SEL).
 */
#ifdef SMSC9218I_BIG_ENDIAN_SUPPORT
  volatile smsc9218i_registers *const smsc9218i =
    (volatile smsc9218i_registers *) 0x3fff8000;
  volatile smsc9218i_registers *const smsc9218i_dma =
    (volatile smsc9218i_registers *) 0x3fff8200;
#else
  volatile smsc9218i_registers *const smsc9218i =
    (volatile smsc9218i_registers *) 0x3fff8000;
  volatile smsc9218i_registers *const smsc9218i_dma =
    (volatile smsc9218i_registers *) 0x3fff8000;
#endif

/** @} */

#ifdef SMSC9218I_BIG_ENDIAN_SUPPORT
  #define SMSC9218I_BIT_POS(pos) (pos)
#else
  #define SMSC9218I_BIT_POS(pos) \
    ((pos) > 15 ? \
      ((pos) > 23 ? (pos) - 24 : (pos) - 8) \
        : ((pos) > 7 ? (pos) + 8 : (pos) + 24))
#endif

#define SMSC9218I_FLAG(pos) \
  (1U << SMSC9218I_BIT_POS(pos))

#define SMSC9218I_FIELD_8(val, pos) \
  (((val) & 0xff) << SMSC9218I_BIT_POS(pos))

#define SMSC9218I_GET_FIELD_8(reg, pos) \
  (((reg) >> SMSC9218I_BIT_POS(pos)) & 0xff)

#define SMSC9218I_FIELD_16(val, pos) \
  (SMSC9218I_FIELD_8((val) >> 8, (pos) + 8) \
    | SMSC9218I_FIELD_8((val), pos))

#define SMSC9218I_GET_FIELD_16(reg, pos) \
  ((SMSC9218I_GET_FIELD_8(reg, (pos) + 8) << 8) \
    | SMSC9218I_GET_FIELD_8(reg, pos))

#ifdef SMSC9218I_BIG_ENDIAN_SUPPORT
  #define SMSC9218I_SWAP(val) (val)
#else
  #define SMSC9218I_SWAP(val) \
    ((((val) >> 24) & 0xff) \
      | ((((val) >> 16) & 0xff) << 8) \
      | ((((val) >> 8) & 0xff) << 16) \
      | (((val) & 0xff) << 24))
#endif

/**
 * @name Receive Status
 * @{
 */

#define SMSC9218I_RX_STS_FILTER_FAIL SMSC9218I_FLAG(30)
#define SMSC9218I_RX_STS_GET_LENGTH(reg) (SMSC9218I_GET_FIELD_16(reg, 16) & 0x3fff)
#define SMSC9218I_RX_STS_ERROR SMSC9218I_FLAG(15)
#define SMSC9218I_RX_STS_BROADCAST SMSC9218I_FLAG(13)
#define SMSC9218I_RX_STS_ERROR_LENGTH SMSC9218I_FLAG(12)
#define SMSC9218I_RX_STS_ERROR_RUNT_FRAME SMSC9218I_FLAG(11)
#define SMSC9218I_RX_STS_MULTICAST SMSC9218I_FLAG(10)
#define SMSC9218I_RX_STS_ERROR_TOO_LONG SMSC9218I_FLAG(7)
#define SMSC9218I_RX_STS_ERROR_COLLISION SMSC9218I_FLAG(6)
#define SMSC9218I_RX_STS_TYPE SMSC9218I_FLAG(5)
#define SMSC9218I_RX_STS_WATCHDOG SMSC9218I_FLAG(4)
#define SMSC9218I_RX_STS_ERROR_MII SMSC9218I_FLAG(3)
#define SMSC9218I_RX_STS_DRIBBLING_BIT SMSC9218I_FLAG(2)
#define SMSC9218I_RX_STS_ERROR_CRC SMSC9218I_FLAG(1)

/** @} */

/**
 * @name Transmit Status
 * @{
 */

#define SMSC9218I_TX_STS_GET_TAG(reg) SMSC9218I_GET_FIELD_16(reg, 16)
#define SMSC9218I_TX_STS_ERROR SMSC9218I_FLAG(15)
#define SMSC9218I_TX_STS_ERROR_LOSS_OF_CARRIER SMSC9218I_FLAG(11)
#define SMSC9218I_TX_STS_ERROR_NO_CARRIER SMSC9218I_FLAG(10)
#define SMSC9218I_TX_STS_ERROR_LATE_COLLISION SMSC9218I_FLAG(9)
#define SMSC9218I_TX_STS_ERROR_EXCESSIVE_COLLISIONS SMSC9218I_FLAG(8)
#define SMSC9218I_TX_STS_ERROR_EXCESSIVE_DEFERRAL SMSC9218I_FLAG(2)
#define SMSC9218I_TX_STS_ERROR_DEFERRED SMSC9218I_FLAG(0)

/** @} */

/**
 * @name Transmit Command A
 * @{
 */

#define SMSC9218I_TX_A_IOC SMSC9218I_FLAG(31)
#define SMSC9218I_TX_A_END_ALIGN_4 0
#define SMSC9218I_TX_A_END_ALIGN_16 SMSC9218I_FLAG(24)
#define SMSC9218I_TX_A_END_ALIGN_32 SMSC9218I_FLAG(25)
#define SMSC9218I_TX_A_DOFF(val) SMSC9218I_FIELD_8(val, 16)
#define SMSC9218I_TX_A_FIRST SMSC9218I_FLAG(13)
#define SMSC9218I_TX_A_LAST SMSC9218I_FLAG(12)
#define SMSC9218I_TX_A_FRAGMENT_LENGTH(val) SMSC9218I_FIELD_16(val, 0)

/** @} */

/**
 * @name Transmit Command B
 * @{
 */

#define SMSC9218I_TX_B_TAG(val) SMSC9218I_FIELD_16(val, 16)
#define SMSC9218I_TX_B_GET_TAG(reg) SMSC9218I_GET_FIELD_16(reg, 16)
#define SMSC9218I_TX_B_DISABLE_CRC SMSC9218I_FLAG(13)
#define SMSC9218I_TX_B_DISABLE_PAD SMSC9218I_FLAG(12)
#define SMSC9218I_TX_B_FRAME_LENGTH(val) SMSC9218I_FIELD_16(val, 0)

/** @} */

/**
 * @name Chip ID and Revision
 * @{
 */

#define SMSC9218I_ID_REV_GET_ID(reg) SMSC9218I_GET_FIELD_16(reg, 16)
#define SMSC9218I_ID_REV_GET_REV(reg) SMSC9218I_GET_FIELD_16(reg, 0)
#define SMSC9218I_ID_REV_ID_CHIP_118 0x0118U
#define SMSC9218I_ID_REV_ID_CHIP_218 0x118aU

/** @} */

/**
 * @name Interrupt Configuration
 * @{
 */

#define SMSC9218I_IRQ_CFG_INT_DEAS(val) SMSC9218I_FIELD_8(val, 24)
#define SMSC9218I_IRQ_CFG_GET_INT_DEAS(reg) SMSC9218I_GET_FIELD_8(reg, 24)
#define SMSC9218I_IRQ_CFG_INT_DEAS_CLR SMSC9218I_FLAG(14)
#define SMSC9218I_IRQ_CFG_INT_DEAS_STS SMSC9218I_FLAG(13)
#define SMSC9218I_IRQ_CFG_IRQ_INT SMSC9218I_FLAG(12)
#define SMSC9218I_IRQ_CFG_IRQ_EN SMSC9218I_FLAG(8)
#define SMSC9218I_IRQ_CFG_IRQ_POL SMSC9218I_FLAG(4)
#define SMSC9218I_IRQ_CFG_IRQ_TYPE SMSC9218I_FLAG(0)

/** @} */

/**
 * @name Interrupt Enable and Status
 * @{
 */

#define SMSC9218I_INT_SW SMSC9218I_FLAG(31)
#define SMSC9218I_INT_TXSTOP SMSC9218I_FLAG(25)
#define SMSC9218I_INT_RXSTOP SMSC9218I_FLAG(24)
#define SMSC9218I_INT_RXDFH SMSC9218I_FLAG(23)
#define SMSC9218I_INT_TIOC SMSC9218I_FLAG(21)
#define SMSC9218I_INT_RXD SMSC9218I_FLAG(20)
#define SMSC9218I_INT_GPT SMSC9218I_FLAG(19)
#define SMSC9218I_INT_PHY SMSC9218I_FLAG(18)
#define SMSC9218I_INT_PME SMSC9218I_FLAG(17)
#define SMSC9218I_INT_TXSO SMSC9218I_FLAG(16)
#define SMSC9218I_INT_RWT SMSC9218I_FLAG(15)
#define SMSC9218I_INT_RXE SMSC9218I_FLAG(14)
#define SMSC9218I_INT_TXE SMSC9218I_FLAG(13)
#define SMSC9218I_INT_TDFO SMSC9218I_FLAG(10)
#define SMSC9218I_INT_TDFA SMSC9218I_FLAG(9)
#define SMSC9218I_INT_TSFF SMSC9218I_FLAG(8)
#define SMSC9218I_INT_TSFL SMSC9218I_FLAG(7)
#define SMSC9218I_INT_RSFF SMSC9218I_FLAG(4)
#define SMSC9218I_INT_RSFL SMSC9218I_FLAG(3)
#define SMSC9218I_INT_GPIO2 SMSC9218I_FLAG(2)
#define SMSC9218I_INT_GPIO1 SMSC9218I_FLAG(1)
#define SMSC9218I_INT_GPIO0 SMSC9218I_FLAG(0)

/** @} */

/**
 * @name Byte Order Testing
 * @{
 */

#define SMSC9218I_BYTE_TEST SMSC9218I_SWAP(0x87654321U)

/** @} */

/**
 * @name FIFO Level Interrupts
 * @{
 */

#define SMSC9218I_FIFO_INT_TDAL(val) SMSC9218I_FIELD_8(val, 24)
#define SMSC9218I_FIFO_INT_GET_TDAL(reg) SMSC9218I_GET_FIELD_8(reg, 24)
#define SMSC9218I_FIFO_INT_TSL(val) SMSC9218I_FIELD_8(val, 16)
#define SMSC9218I_FIFO_INT_GET_TSL(reg) SMSC9218I_GET_FIELD_8(reg, 16)
#define SMSC9218I_FIFO_INT_RSL(val) SMSC9218I_FIELD_8(val, 0)
#define SMSC9218I_FIFO_INT_GET_RSL(reg) SMSC9218I_GET_FIELD_8(reg, 0)

/** @} */

/**
 * @name Receive Configuration
 * @{
 */

#define SMSC9218I_RX_CFG_END_ALIGN_4 0
#define SMSC9218I_RX_CFG_END_ALIGN_16 SMSC9218I_FLAG(30)
#define SMSC9218I_RX_CFG_END_ALIGN_32 SMSC9218I_FLAG(31)
#define SMSC9218I_RX_CFG_DMA_CNT(val) SMSC9218I_FIELD_8(val, 24)
#define SMSC9218I_RX_CFG_GET_DMA_CNT(reg) SMSC9218I_GET_FIELD_8(reg, 24)
#define SMSC9218I_RX_CFG_DUMP SMSC9218I_FLAG(15)
#define SMSC9218I_RX_CFG_DOFF(val) SMSC9218I_FIELD_8(val, 8)
#define SMSC9218I_RX_CFG_GET_DOFF(reg) SMSC9218I_GET_FIELD_8(reg, 8)

/** @} */

/**
 * @name Transmit Configuration
 * @{
 */

#define SMSC9218I_TX_CFG_SDUMP SMSC9218I_FLAG(15)
#define SMSC9218I_TX_CFG_DDUMP SMSC9218I_FLAG(14)
#define SMSC9218I_TX_CFG_SAO SMSC9218I_FLAG(2)
#define SMSC9218I_TX_CFG_ON SMSC9218I_FLAG(1)
#define SMSC9218I_TX_CFG_STOP SMSC9218I_FLAG(0)

/** @} */

/**
 * @name Hardware Configuration
 * @{
 */

#define SMSC9218I_HW_CFG_LED_3 SMSC9218I_FLAG(30)
#define SMSC9218I_HW_CFG_LED_2 SMSC9218I_FLAG(29)
#define SMSC9218I_HW_CFG_LED_1 SMSC9218I_FLAG(28)
#define SMSC9218I_HW_CFG_AMDIX SMSC9218I_FLAG(24)
#define SMSC9218I_HW_CFG_MBO SMSC9218I_FLAG(20)
#define SMSC9218I_HW_CFG_TX_FIF_SZ(val) SMSC9218I_FIELD_8(val, 16)
#define SMSC9218I_HW_CFG_GET_TX_FIF_SZ(reg) SMSC9218I_GET_FIELD_8(reg, 16)
#define SMSC9218I_HW_CFG_BITMD_32 SMSC9218I_FLAG(2)
#define SMSC9218I_HW_CFG_SRST_TO SMSC9218I_FLAG(1)
#define SMSC9218I_HW_CFG_SRST SMSC9218I_FLAG(0)

/** @} */

/**
 * @name Receive Datapath Control
 * @{
 */

#define SMSC9218I_RX_DP_CTRL_FFWD SMSC9218I_FLAG(31)

/** @} */

/**
 * @name Receive FIFO Information
 * @{
 */

#define SMSC9218I_RX_FIFO_INF_GET_SUSED(reg) SMSC9218I_GET_FIELD_8(reg, 16)
#define SMSC9218I_RX_FIFO_INF_GET_DUSED(reg) SMSC9218I_GET_FIELD_16(reg, 0)

/** @} */

/**
 * @name Transmit FIFO Information
 * @{
 */

#define SMSC9218I_TX_FIFO_INF_GET_SUSED(reg) SMSC9218I_GET_FIELD_8(reg, 16)
#define SMSC9218I_TX_FIFO_INF_GET_FREE(reg) SMSC9218I_GET_FIELD_16(reg, 0)

/** @} */

/**
 * @name Power Management Control
 * @{
 */

#define SMSC9218I_PMT_CTRL_PM_MODE_D0 0
#define SMSC9218I_PMT_CTRL_PM_MODE_D1 SMSC9218I_FLAG(12)
#define SMSC9218I_PMT_CTRL_PM_MODE_D2 SMSC9218I_FLAG(13)
#define SMSC9218I_PMT_CTRL_PHY_RST SMSC9218I_FLAG(10)
#define SMSC9218I_PMT_CTRL_WOL_EN SMSC9218I_FLAG(9)
#define SMSC9218I_PMT_CTRL_ED_EN SMSC9218I_FLAG(8)
#define SMSC9218I_PMT_CTRL_PME_TYPE_PUPU SMSC9218I_FLAG(6)
#define SMSC9218I_PMT_CTRL_WUPS_NO 0
#define SMSC9218I_PMT_CTRL_WUPS_ENERGY SMSC9218I_FLAG(4)
#define SMSC9218I_PMT_CTRL_WUPS_MAGIC SMSC9218I_FLAG(5)
#define SMSC9218I_PMT_CTRL_PME_IND SMSC9218I_FLAG(3)
#define SMSC9218I_PMT_CTRL_PME_POL SMSC9218I_FLAG(2)
#define SMSC9218I_PMT_CTRL_PME_EN SMSC9218I_FLAG(1)
#define SMSC9218I_PMT_CTRL_READY SMSC9218I_FLAG(0)

/** @} */

/**
 * @name General Purpose IO Configuration
 * @{
 */

#define SMSC9218I_GPIO_CFG_LED3 SMSC9218I_FLAG(30)
#define SMSC9218I_GPIO_CFG_LED2 SMSC9218I_FLAG(29)
#define SMSC9218I_GPIO_CFG_LED1 SMSC9218I_FLAG(28)
#define SMSC9218I_GPIO_CFG_GPIO2_INT_POL SMSC9218I_FLAG(26)
#define SMSC9218I_GPIO_CFG_GPIO1_INT_POL SMSC9218I_FLAG(25)
#define SMSC9218I_GPIO_CFG_GPIO0_INT_POL SMSC9218I_FLAG(24)
#define SMSC9218I_GPIO_CFG_GPIOBUF2 SMSC9218I_FLAG(18)
#define SMSC9218I_GPIO_CFG_GPIOBUF1 SMSC9218I_FLAG(17)
#define SMSC9218I_GPIO_CFG_GPIOBUF0 SMSC9218I_FLAG(16)
#define SMSC9218I_GPIO_CFG_GPIODIR2 SMSC9218I_FLAG(10)
#define SMSC9218I_GPIO_CFG_GPIODIR1 SMSC9218I_FLAG(9)
#define SMSC9218I_GPIO_CFG_GPIODIR0 SMSC9218I_FLAG(8)
#define SMSC9218I_GPIO_CFG_GPO4 SMSC9218I_FLAG(4)
#define SMSC9218I_GPIO_CFG_GPO3 SMSC9218I_FLAG(3)
#define SMSC9218I_GPIO_CFG_GPIO0 SMSC9218I_FLAG(0)
#define SMSC9218I_GPIO_CFG_GPIO2 SMSC9218I_FLAG(2)
#define SMSC9218I_GPIO_CFG_GPIO1 SMSC9218I_FLAG(1)

/** @} */

/**
 * @name General Purpose Timer Configuration
 * @{
 */

#define SMSC9218I_GPT_CFG_TIMER_EN SMSC9218I_FLAG(29)
#define SMSC9218I_GPT_CFG_LOAD(val) SMSC9218I_FIELD_16(val, 0)
#define SMSC9218I_GPT_CFG_GET_LOAD(reg) SMSC9218I_GET_FIELD_16(reg, 0)

/** @} */

/**
 * @name General Purpose Timer Count
 * @{
 */

#define SMSC9218I_GPT_CNT_GET_CNT SMSC9218I_GET_FIELD_16(reg, 0)

/** @} */

/**
 * @name Word Swap
 * @{
 */

#define SMSC9218I_ENDIAN_BIG 0xffffffffU

/** @} */

/**
 * @name Free Run Counter
 * @{
 */

#define SMSC9218I_FREE_RUN_GET(reg) SMSC9218I_SWAP(reg)

/** @} */

/**
 * @name Receiver Dropped Frames Counter
 * @{
 */

#define SMSC9218I_RX_DROP_GET(reg) SMSC9218I_SWAP(reg)

/** @} */

/**
 * @name EEPROM Command Register
 * @{
 */

#define SMSC9218I_E2P_CMD_EPC_BUSY SMSC9218I_FLAG(31)

/** @} */

/**
 * @name MAC Control and Status Synchronizer Command
 * @{
 */

#define SMSC9218I_MAC_CSR_CMD_BUSY SMSC9218I_FLAG(31)
#define SMSC9218I_MAC_CSR_CMD_READ SMSC9218I_FLAG(30)
#define SMSC9218I_MAC_CSR_CMD_ADDR(val) SMSC9218I_FIELD_8(val, 0)
#define SMSC9218I_MAC_CSR_CMD_GET_ADDR(reg) SMSC9218I_GET_FIELD_8(reg, 0)

/** @} */

/**
 * @name MAC Control Register
 * @{
 */

#define SMSC9218I_MAC_CR 0x00000001U
#define SMSC9218I_MAC_CR_RXALL 0x80000000U
#define SMSC9218I_MAC_CR_HBDIS 0x10000000U
#define SMSC9218I_MAC_CR_RCVOWN 0x00800000U
#define SMSC9218I_MAC_CR_LOOPBK 0x00200000U
#define SMSC9218I_MAC_CR_FDPX 0x00100000U
#define SMSC9218I_MAC_CR_MCPAS 0x00080000U
#define SMSC9218I_MAC_CR_PRMS 0x00040000U
#define SMSC9218I_MAC_CR_INVFILT 0x00020000U
#define SMSC9218I_MAC_CR_PASSBAD 0x00010000U
#define SMSC9218I_MAC_CR_HFILT 0x00008000U
#define SMSC9218I_MAC_CR_HPFILT 0x00002000U
#define SMSC9218I_MAC_CR_LCOLL 0x00001000U
#define SMSC9218I_MAC_CR_BCAST 0x00000800U
#define SMSC9218I_MAC_CR_DISRTY 0x00000400U
#define SMSC9218I_MAC_CR_PADSTR 0x00000100U
#define SMSC9218I_MAC_CR_BOLMT_MASK 0x000000c0U
#define SMSC9218I_MAC_CR_BOLMT_10 0x00000000U
#define SMSC9218I_MAC_CR_BOLMT_8 0x00000040U
#define SMSC9218I_MAC_CR_BOLMT_4 0x00000080U
#define SMSC9218I_MAC_CR_BOLMT_1 0x000000c0U
#define SMSC9218I_MAC_CR_DFCHK 0x00000020U
#define SMSC9218I_MAC_CR_TXEN 0x00000008U
#define SMSC9218I_MAC_CR_RXEN 0x00000004U

/** @} */

/**
 * @name MAC Address High
 * @{
 */

#define SMSC9218I_MAC_ADDRH 0x00000002U
#define SMSC9218I_MAC_ADDRH_MASK 0x0000ffffU

/** @} */

/**
 * @name MAC Address Low
 * @{
 */

#define SMSC9218I_MAC_ADDRL 0x00000003U
#define SMSC9218I_MAC_ADDRL_MASK 0xffffffffU

/** @} */

/**
 * @name Multicast Hash Table High
 * @{
 */

#define SMSC9218I_MAC_HASHH 0x00000004U
#define SMSC9218I_MAC_HASHH_MASK 0xffffffffU

/** @} */

/**
 * @name Multicast Hash Table Low
 * @{
 */

#define SMSC9218I_MAC_HASHL 0x00000005U
#define SMSC9218I_MAC_HASHL_MASK 0xffffffffU

/** @} */

/**
 * @name MII Access
 * @{
 */

#define SMSC9218I_MAC_MII_ACC 0x00000006U
#define SMSC9218I_MAC_MII_ACC_PHY_DEFAULT (1U << 11)
#define SMSC9218I_MAC_MII_ACC_WRITE (1U << 1)
#define SMSC9218I_MAC_MII_ACC_BUSY (1U << 0)
#define SMSC9218I_MAC_MII_ACC_ADDR(addr) ((addr) << 6)

/** @} */

/**
 * @name MII Data
 * @{
 */

#define SMSC9218I_MAC_MII_DATA 0x00000007U

/** @} */

/**
 * @name Flow Control
 * @{
 */

#define SMSC9218I_MAC_FLOW 0x00000008U
#define SMSC9218I_MAC_FLOW_FCPT_MASK 0xffff0000U
#define SMSC9218I_MAC_FLOW_FCPASS 0x00000004U
#define SMSC9218I_MAC_FLOW_FCEN 0x00000002U
#define SMSC9218I_MAC_FLOW_FCBSY 0x00000001U

/** @} */

/**
 * @name VLAN1 Tag
 * @{
 */

#define SMSC9218I_MAC_VLAN1 0x00000009U

/** @} */

/**
 * @name VLAN2 Tag
 * @{
 */

#define SMSC9218I_MAC_VLAN2 0x0000000aU

/** @} */

/**
 * @name Wake-up Frame Filter
 * @{
 */

#define SMSC9218I_MAC_WUFF 0x0000000bU

/** @} */

/**
 * @name Wake-up Control and Status
 * @{
 */

#define SMSC9218I_MAC_WUCSR 0x0000000cU
#define SMSC9218I_MAC_WUCSR_GUE 0x00000200U
#define SMSC9218I_MAC_WUCSR_WUFR 0x00000040U
#define SMSC9218I_MAC_WUCSR_MPR 0x00000020U
#define SMSC9218I_MAC_WUCSR_WUEN 0x00000004U
#define SMSC9218I_MAC_WUCSR_MPEN 0x00000002U

/** @} */

/**
 * @name PHY Identifier 1
 * @{
 */

#define SMSC9218I_PHY_ID1_LAN9118 0x7

/** @} */

/**
 * @name PHY Identifier 2
 * @{
 */

#define SMSC9218I_PHY_ID2_LAN9218 0xc0c3

/** @} */

/**
 * @name Mode Control and Status
 * @{
 */

#define SMSC9218I_PHY_MCSR 0x00000011U
#define SMSC9218I_PHY_MCSR_EDPWRDOWN 0x00002000U
#define SMSC9218I_PHY_MCSR_ENERGYON 0x00000002U

/** @} */

/**
 * @name Special Modes
 * @{
 */

#define SMSC9218I_PHY_SPMODES 0x00000012U

/** @} */

/**
 * @name Special Control and Status Indications
 * @{
 */

#define SMSC9218I_PHY_CSIR 0x0000001bU
#define SMSC9218I_PHY_CSIR_SQEOFF 0x00000800U
#define SMSC9218I_PHY_CSIR_FEFIEN 0x00000020U
#define SMSC9218I_PHY_CSIR_XPOL 0x00000010U

/** @} */

/**
 * @name Interrupt Source Flag
 * @{
 */

#define SMSC9218I_PHY_ISR 0x0000001dU
#define SMSC9218I_PHY_ISR_INT7 0x00000080U
#define SMSC9218I_PHY_ISR_INT6 0x00000040U
#define SMSC9218I_PHY_ISR_INT5 0x00000020U
#define SMSC9218I_PHY_ISR_INT4 0x00000010U
#define SMSC9218I_PHY_ISR_INT3 0x00000008U
#define SMSC9218I_PHY_ISR_INT2 0x00000004U
#define SMSC9218I_PHY_ISR_INT1 0x00000002U

/** @} */

/**
 * @name Interrupt Mask
 * @{
 */

#define SMSC9218I_PHY_IMR 0x0000001eU
#define SMSC9218I_PHY_IMR_INT7 0x00000080U
#define SMSC9218I_PHY_IMR_INT6 0x00000040U
#define SMSC9218I_PHY_IMR_INT5 0x00000020U
#define SMSC9218I_PHY_IMR_INT4 0x00000010U
#define SMSC9218I_PHY_IMR_INT3 0x00000008U
#define SMSC9218I_PHY_IMR_INT2 0x00000004U
#define SMSC9218I_PHY_IMR_INT1 0x00000002U

/** @} */

/**
 * @name PHY Special Control and Status
 * @{
 */

#define SMSC9218I_PHY_PHYSCSR 0x0000001fU
#define SMSC9218I_PHY_PHYSCSR_ANDONE 0x00001000U
#define SMSC9218I_PHY_PHYSCSR_4B5B_EN 0x00000040U
#define SMSC9218I_PHY_PHYSCSR_SPEED_MASK 0x0000001cU
#define SMSC9218I_PHY_PHYSCSR_SPEED_10HD 0x00000004U
#define SMSC9218I_PHY_PHYSCSR_SPEED_10FD 0x00000014U
#define SMSC9218I_PHY_PHYSCSR_SPEED_100HD 0x00000008U
#define SMSC9218I_PHY_PHYSCSR_SPEED_100FD 0x00000018U

/** @} */
