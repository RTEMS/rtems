/**
 * @file
 *
 * @brief API header for the DWMAC 10/100/1000 Network Interface Controllers
 *
 * DWMAC 10/100/1000 on-chip Ethernet controllers are a Synopsys IP Core.
 */

/*
 * Copyright (c) 2013 embedded brains GmbH.  All rights reserved.
 *
 *  embedded brains GmbH
 *  Dornierstr. 4
 *  82178 Puchheim
 *  Germany
 *  <rtems@embedded-brains.de>
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 */

#ifndef DWMAC_H_
#define DWMAC_H_

#include <stddef.h>
#include <stdint.h>
#include <rtems/rtems_bsdnet.h>

#ifdef __cplusplus
extern "C" {
#endif /* __cplusplus */

/** @brief DWMAC user configuration structure.
 *
 * Gives the user the possibility to overwrite some configuration data by
 * setting the drv_ctrl pointer of the @ref rtems_bsdnet_ifconfig structure to a
 * object with this type.
 */
typedef struct {
  int phy_addr;
} dwmac_ifconfig_drv_ctrl;

/** @brief PHY event.
 *
 * Data type to be used for PHY events and event sets.
 */
typedef uint8_t dwmac_phy_event;

/** @brief PHY event Jabber. */
extern const dwmac_phy_event PHY_EVENT_JABBER;

/** @brief PHY event Receive Error. */
extern const dwmac_phy_event PHY_EVENT_RECEIVE_ERROR;

/** @brief PHY event Page Receive. */
extern const dwmac_phy_event PHY_EVENT_PAGE_RECEIVE;

/** @brief PHY event Parallel Detect Fault. */
extern const dwmac_phy_event PHY_EVENT_PARALLEL_DETECT_FAULT;

/** @brief PHY event Link Partner Acknowledge. */
extern const dwmac_phy_event PHY_EVENT_LINK_PARTNER_ACK;

/** @brief PHY event Link Down. */
extern const dwmac_phy_event PHY_EVENT_LINK_DOWN;

/** @brief PHY event Remote Fault. */
extern const dwmac_phy_event PHY_EVENT_REMOTE_FAULT;

/** @brief PHY event Link Up. */
extern const dwmac_phy_event PHY_EVENT_LINK_UP;

/** @brief NIC enable
 *
 * Enables (e.g. powers up) the network interface card.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_nic_enable)( void *arg );

/** @brief NIC disable.
 *
 * Disables (e.g. powers down) the network interface card.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_nic_disable)( void *arg );

/** @brief PHY enable.
 *
 * Enables (e.g. powers up) the network PHY.
 * @param[in,out] arg    The void pointer argument passed to the attach_detach
 *                       method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_phy_enable)( void *arg );

/** @brief PHY disable.
 *
 * Disables (e.g. powers down) the network PHY.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_phy_disable)( void *arg );

/** @brief PHY event enable.
 *
 * Enables generation of events for an event set.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @param[in] event_set   Set of events. For these events shall get generated
 *                        upon PHY status change.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_phy_event_enable)(
  void                 *arg,
  const dwmac_phy_event event_set );

/** @brief Clear phy event status.
 *
 * Clears all PHY event statuses.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_phy_event_status_clear)( void *arg );

/**
 * @brief Get the PHY event status.
 *
 * Reports status on PHY events (e.g. PHY interrupts).
 * @param[in,out] arg       The void pointer argument passed to the 
 *                          attach_detach method.
 * @param[out]    event_set Pointer to a buffer for a set of events for which a
 *                          PHY status change was detected.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 * @see dwmac_phy_event for events.
 */
typedef int
(*dwmac_if_phy_events_status_get)(
  void            *arg,
  dwmac_phy_event *event_set );

/**
 * @brief Start the network PHY.
 *
 * Do anything necessary to start the network PHY (start generating
 * events, ...).
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwcmac_if_phy_start)( void *arg );

/**
 * @brief Stop the network PHY.
 *
 * Do anything necessary to stop the network PHY (stop generating events, ...).
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwcmac_if_phy_stop)( void *arg );

/**
 * @brief Allocate nocache RAM.
 *
 * Allocate uncached RAM.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @param[out]    memory  Pointer of a buffer to write the address of the
 *                        allocated memory to.
 * @param[in]     size    Number of bytes to be allocated
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_mem_alloc_nocache)(
  void        *arg,
  void       **memory,
  const size_t size );

/**
 * @brief Free nocache RAM.
 *
 * Release uncached RAM.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @param[in]     memory  Pointer to the memory to be freed.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_mem_free_nocache)(
  void *arg,
  void *memory );

/**
 * @brief Bus setup.
 *
 * Callback method for setting up the system bus for the network driver.
 * @param[in,out] arg     The void pointer argument passed to the attach_detach
 *                        method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach for the attach_detach method.
 */
typedef int
(*dwmac_if_bus_setup)( void *arg );

/**
 * @brief Callback methods.
 *
 * The address of an instance of such a callback struct must get passed
 * to the attach_detach method of the network driver.
 * Via these callback methods, those parts of the network driver which
 * are micro controller specific or PCB specific will get handled.
 * @see dwmac_network_if_attach_detach() for the drivers attach_detach method.
 */
typedef struct {
  /** @brief Enable the network interface controller. */
  dwmac_if_nic_enable nic_enable;

  /** @brief Disable the network interface controller. */
  dwmac_if_nic_disable nic_disable;

  /** @brief Enable (power up, ... ) the network PHY. */
  dwmac_if_phy_enable phy_enable;

  /** @brief Disable (power down, ... ) the network PHY. */
  dwmac_if_phy_disable phy_disable;

  /** @brief Enable a set of PHY events for generation upon status change. */
  dwmac_if_phy_event_enable phy_event_enable;

  /** @brief Clear the event status (e.g. interrupt staus) of the PHY. */
  dwmac_if_phy_event_status_clear phy_event_clear;

  /** @brief Get set of tripped events from PHY. */
  dwmac_if_phy_events_status_get phy_events_get;

  /** @brief Start the phy (start generating events, ...). */
  dwcmac_if_phy_start phy_start;

  /** @brief Stop the phy (stop generating events, ...). */
  dwcmac_if_phy_stop phy_stop;

  /** @brief Allocate uncached memory. */
  dwmac_if_mem_alloc_nocache mem_alloc_nocache;

  /** @brief Free uncached memory. */
  dwmac_if_mem_free_nocache mem_free_nocache;

  /** @brief Setup handling for bus upon device startup. */
  dwmac_if_bus_setup bus_setup;
} dwmac_callback_cfg;

/** @brief Initializer for callback methods.
 *
 * Initializes a struct which contains pointers to the callback methods
 * required by the driver.
 * @see dwmac_callback_cfg for the struct.
 * @param[in] nic_enable              Callback method for for enabling the
 *                                    network interface controller.
 * @param[in] nic_disable             Callback method for disabling the
 *                                    network interface controller.
 * @param[in] phy_enable              Callback method for enabling the
 *                                    network PHY.
 * @param[in] phy_disable             Callback method for disabling the
 *                                    network PHY.
 * @param[in] phy_event_enable        Callback method for enabling PHY status
 *                                    changes for event generation.
 * @param[in] phy_event_clear         Callback method for
 *                                    clearing/acknowledging PHY events.
 * @param[in] phy_events_get          Callback method for reading the status of
 *                                    PHY events.
 * @param[in] phy_start               Callback method for starting event
 *                                    generation by the network PHY.
 * @param[in] phy_stop                Callback method for stoping event
 *                                    generation by the network PHY.
 * @param[in] mem_alloc_nocache       Callback method for allocating uncached
 *                                    RAM.
 * @param[in] mem_free_nocache        Callback method for releasing uncached
 *                                    RAM.
 * @param[in] bus_setup               Callback method for setting up the system
 *                                    bus.
 * @returns An initialized struct of pointers to callback methods.
 * @see dwmac_if_nic_enable for the NIC enable methods.
 * @see dwmac_if_nic_disable for NIC disable methods.
 * @see dwmac_if_phy_enable for PHY enable methods.
 * @see dwmac_if_phy_disable for PHY disable methods.
 * @see dwmac_if_phy_event_enable for PHY envent enable methods.
 * @see dwmac_if_phy_event_status_clear for PHY event status clear methods.
 * @see dwmac_if_phy_events_status_get for PHY event status get methods.
 * @see dwcmac_if_phy_start for PHY start methods.
 * @see dwcmac_if_phy_stop for PHY stop methods.
 * @see dwmac_if_mem_alloc_nocache for nocache mem alloc methods.
 * @see dwmac_if_mem_free_nocache for nocache mem free methods.
 * @see dwmac_if_bus_setup for bus setup methods.
 */
#define DWMAC_CALLBACK_CFG_INITIALIZER( \
    nic_enable, \
    nic_disable, \
    phy_enable, \
    phy_disable, \
    phy_event_enable, \
    phy_event_clear, \
    phy_events_get, \
    phy_start, \
    phy_stop, \
    mem_alloc_nocache, \
    mem_free_nocache, \
    bus_setup \
    ) \
  { \
    nic_enable, \
    nic_disable, \
    phy_enable, \
    phy_disable, \
    phy_event_enable, \
    phy_event_clear, \
    phy_events_get, \
    phy_start, \
    phy_stop, \
    mem_alloc_nocache, \
    mem_free_nocache, \
    bus_setup \
  }

/** @brief Ethernet MAC operations.
 *
 * Actually this is a mere wrapper which contains void ponters to the core
 * operations and DMA operations to be used by the driver (void pointer
 * for the purpose of information hiding).
 * There will be two instances of such a struct:
 * One for DWMAC 10/100 ethernet operations and one for DWMAC 1000 ethernet
 * operations.
 * The address of either of these must get passed to the initializer of the
 * driver configuration  for configuring the driver.
 * @see DWMAC_100_ETHERNET_MAC_OPS for DWMAC 10/100 ethernet operations.
 * @see DWMAC_1000_ETHERNET_MAC_OPS for DWMAC 1000 ethernet operations.
 * @see DWMAC_CFG_INITIALIZER driver configuration initializer.
 * @see DWMAC_ETHERNET_MAC_OPS_INITIALIZER for an initializer for the MAC 
 * operations
 */
typedef struct {
  const void *core;
  const void *dma;
} dwmac_ethernet_mac_ops;

/** @brief Ethernet MAC operations initializer.
 *
 * Initializes a structure of ethernet MAC operations.
 * @see dwmac_ethernet_mac_ops for the struct.
 * @param[in] core_ops_addr Address of the core operations to be used by the
 *                          driver.
 * @param[in] dma_ops_addr  Address of the DMA operations to be used by the 
 *                          driver.
 * @returns An initialized struct of ethernet mac operations.
 */
#define DWMAC_ETHERNET_MAC_OPS_INITIALIZER( \
    core_ops_addr, \
    dma_ops_addr \
    ) \
  { \
    core_ops_addr, \
    dma_ops_addr \
  }

/** @brief Descriptor operations.
 *
 * Actually this is a mere wrapper which contains a void pointer to a
 * descriptor operations struct which can be used by the driver (void pointer
 * for the purpose of information hiding).
 * There will be two instances of such a struct:
 * One for normal DMA descriptors and one for enhanced DMA descriptors.
 * The address of either of these must get passed to the configuration 
 * initializer for configuring the driver.
 * @see DWMAC_DESCRIPTOR_OPS_NORMAL for normal DMA descriptor operations.
 * @see DWMAC_DESCRIPTOR_OPS_ENHANCED for enhanced DMA descriptor operations.
 * @see DWMAC_CFG_INITIALIZER for the configuration initializer.
 * @see DWMAC_DESCRIPTOR_OPS_INITIALIZER for an initializer an initializer 
 * for descriptor operations.
 */
typedef struct {
  /** @brief Address of the descriptor operations to be used by the driver */
  const void *ops;
} dwmac_descriptor_ops;

/** @brief Initializer for descriptor operations.
 *
 * Initializes a struct which simply makes up a wrapper for DMA descriptor 
 * operations.
 * @param[in] desc_ops_addr Address of the descriptor operations.
 * @returns an initialized descriptor operations struct.
 * @see dwmac_descriptor_ops for the struct.
 */
#define DWMAC_DESCRIPTOR_OPS_INITIALIZER( \
    desc_ops_addr \
    ) \
  { \
    desc_ops_addr \
  }

/** @brief Ethernet MAC operations for DWMAC 1000.
 *
 * Pass the address of DWMAC_1000_ETHERNET_MAC_OPS to the configuration 
 * initializer if the driver is supposed to control a DWMAC 1000. 
 * @see DWMAC_CFG_INITIALIZER for the configuration initializer.
 */
extern const dwmac_ethernet_mac_ops DWMAC_1000_ETHERNET_MAC_OPS;

/** @brief Ethernet MAC operations for DWMAC 10/100.
 *
 * Pass the address of DWMAC_100_ETHERNET_MAC_OPS to the configuration 
 * initializer if the driver is supposed to control a DWMAC 10/100.
 * NOTE: Currently the DWMAC_100_ETHERNET_MAC_OPS are not yet implemented.
 * @see DWMAC_CFG_INITIALIZER for the configuration initializer.
 */
extern const dwmac_ethernet_mac_ops DWMAC_100_ETHERNET_MAC_OPS;

/** @brief DMA descriptor operations for normal descriptors.
 *
 * Pass the address of DWMAC_DESCRIPTOR_OPS_NORMAL to the configuration 
 * initializer if you intend to use the normal DMA descriptors.
 * NOTE: Currently the DWMAC_DESCRIPTOR_OPS_NORMAL are not yet implmented.
 * @see DWMAC_CFG_INITIALIZER for the configuration initializer.
 */
extern const dwmac_descriptor_ops   DWMAC_DESCRIPTOR_OPS_NORMAL;

/** @brief DMA descriptor operations for enhanced descriptors.
 *
 * Pass the address of DWMAC_DESCRIPTOR_OPS_ENHANCED to the configuration 
 * initializer if you intend to use the enhanced DMA descriptors. 
 * @see DWMAC_CFG_INITIALIZER for the configuration initializer.
 */
extern const dwmac_descriptor_ops DWMAC_DESCRIPTOR_OPS_ENHANCED;

/** @brief Burst size. */
typedef enum {
  /** @brief Burst size = 1. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_1 = 0,

  /** @brief Burst size = 2. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_2 = 1,

  /** @brief Burst size = 4. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_4 = 3,

  /** @brief Burst size = 8. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_8 = 7,

  /** @brief Burst size = 16. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_16 = 15,

  /** @brief Burst size = 32. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_32 = 31,

  /** @brief Burst size = 64. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_64 = 63,

  /** @brief Burst size = 128. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_128 = 127,

  /** @brief Burst size = 256. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_LENGTH_256 = 255
} dwmac_dma_cfg_bus_mode_burst_length;

/** @brief Burst mode. */
typedef enum {
  /** @brief Single burst or incrment bursts. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_SINGLE_OR_INCR,

  /** @brief Fixed burst size. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_FIXED
} dwmac_dma_cfg_bus_mode_burst_mode;

/** @brief Mixed burst mode support. */
typedef enum {
  /** @brief Mixed burst mode is not supported. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_NOT_MIXED,
  /** @brief Mixed burst mode is supported. */
  DWMAC_DMA_CFG_BUS_MODE_BURST_MIXED
} dwmac_dma_cfg_bus_mode_burst_mixed;

/** @brief Burst length 4 support. */
typedef enum {
  /** @brief Bursts of length 4 are not supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_4_NOT_SUPPORTED,

  /** @brief Bursts of length 4 are supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_4_SUPPORTED
} dwmac_dma_cfg_axi_burst_length_4_support;

/** @brief Burst length 8 support. */
typedef enum {
  /** @brief Bursts of length 8 are not supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_8_NOT_SUPPORTED,

  /** @brief Bursts of length 8 are supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_8_SUPPORTED
} dwmac_dma_cfg_axi_burst_length_8_support;

/** @brief Burst length 16 support. */
typedef enum {
  /** @brief Bursts of length 16 are not supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_16_NOT_SUPPORTED,

  /** @brief Bursts of length 16 are supported. */
  DWMAC_DMA_CFG_AXI_BURST_LENGTH_16_SUPPORTED
} dwmac_dma_cfg_axi_burst_length_16_support;

/** @brief DMA Burst Boundary parameters. */
typedef enum {
  /** @brief Transfers do not cross 4 kB boundary. */
  DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_4_KB,

  /** @brief Transfers do not cross 1 kB boundary. */
  DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_1_KB
} dwmac_dma_cfg_axi_burst_boundary;

/**
 * @brief DMA configuration.
 *
 * Configuration data for the DMA of the network driver.
 * @see DWMAC_DMA_CFG_INITIALIZER for an inititializer.
 */
typedef union {
  uint16_t raw;
  struct {
    /** @brief Maximum number of beats to be transferred in one DMA transaction.
     *
     * This is the maximum value that is used in a single block Read or Write.
     * The DMA always attempts to burst as specified in bus_mode_burst_length
     * each time it starts a Burst transfer on the host bus.
     * Any non-permissible value results in undefined behavior.
     * The bus_mode_burst_length values have the following limitation:
     * The maximum number of possible beats (bus_mode_burst_length) is limited
     * by the size of the Tx FIFO and Rx FIFO in the MTL layer and the data bus
     * width on the DMA.
     * The FIFO has a constraint that the maximum beat supported is half the
     * depth of the FIFO, except when specified. 
     * @see dwmac_dma_cfg_bus_mode_burst_length for permissible values */
    uint16_t bus_mode_burst_length       : 6;

    /** @brief Controls whether the AXI Master performs fixed bursts or not.
     *
     * When set to DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_FIXED, the AXI interface
     * uses FIXED bursts during the start of the normal burst transfers.
     * When set to DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_SINGLE_OR_INCR, the AXI
     * interface uses SINGLE and INCR burst transfer operations. 
     * @see dwmac_dma_cfg_bus_mode_burst_mode for valid parameters. */
    uint16_t bus_mode_burst_mode         : 1;

    /** @brief Controls whether mixed bursts will be used or not.
     *
     * Mixed burst has no effect when if DWMAC_DMA_CFG_BUS_MODE_BURST_MODE_FIXED is set.
     * @see dwmac_dma_cfg_bus_mode_burst_mixed for valid parameters. */
    uint16_t bus_mode_burst_mixed        : 1;

    /** @brief Controls support of burst length 4.
     *
     * When set to DWMAC_DMA_CFG_AXI_BURST_LENGTH_4_SUPPORTED, the GMAC-AXI is
     * allowed to select a burst length of 4 on the AXI Master interface.
     * @see dwmac_dma_cfg_axi_burst_length_4_support for valid parameters see. */
    uint16_t axi_burst_length_4_support  : 1;

    /** @brief Controls support of burst length 8.
     *
     * When set to DWMAC_DMA_CFG_AXI_BURST_LENGTH_8_SUPPORTED, the GMAC-AXI is
     * allowed to select a burst length of 8 on the AXI Master interface.
     * @see dwmac_dma_cfg_axi_burst_length_8_support For valid parameters. */
    uint16_t axi_burst_length_8_support  : 1;

    /** @brief Controls support of burst length 16.
     *
     * When set to DWMAC_DMA_CFG_AXI_BURST_LENGTH_16_SUPPORTED fixed bust is
     * not selected, the GMAC-AXI is allowed to select a burst length of 16 on
     * the AXI Master interface.
     * @see dwmac_dma_cfg_axi_burst_length_16_support for valid parameters see. */
    uint16_t axi_burst_length_16_support : 1;

    /** @brief Select Burst Boundary.
     *
     * When set to DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_1_KB, the GMAC-AXI Master
     * performs burst transfers that do not cross 1 KB boundary.
     * When set to DWMAC_DMA_CFG_AXI_BURST_BOUNDARY_4_KB, the GMAC-AXI Master
     * performs burst transfers that do not cross 4 KB boundary.
     * @see dwmac_dma_cfg_axi_burst_boundary for valid parameters see. */
    uint16_t axi_burst_boundary          : 1;

    uint16_t unused                      : 4;
  };
} dwmac_dma_cfg;

/** @brief DMA Configuration initializer.
 *
 * Initializer for a DMA configuration struct.
 *
 * @param[in] bus_mode_burst_length       Number of bytes to be sent in one
 *                                        burst within a DMA transfer on the
 *                                        bus .
 * @param[in] bus_mode_burst_mode         Mode to be used for burst transfers.
 * @param[in] bus_mode_burst_mixed        Use mixed bursts or not. Fixed bursts
 *                                        have priority over mixed bursts.
 * @param[in] axi_burst_length_4_support  Support or don't support burst
 *                                        lengths of 4.
 * @param[in] axi_burst_length_8_support  Support or don't support burst
 *                                        lengths of 8.
 * @param[in] axi_burst_length_16_support Support or don't support burst
 *                                        lengths of 16.
 * @param[in] axi_burst_boundary          Select the burst boundary.
 * @returns An initialized struct of DMA configuration parameters.
 * @see dwmac_dma_cfg_bus_mode_burst_length for burst lengths.
 * @see dwmac_dma_cfg_bus_mode_burst_mode for burst modes
 * @see dwmac_dma_cfg_bus_mode_burst_mixed for burst mixing.
 * @see dwmac_dma_cfg_axi_burst_length_4_support for burst length 4 support.
 * @see dwmac_dma_cfg_axi_burst_length_8_support for burst length 8 support.
 * @see dwmac_dma_cfg_axi_burst_length_16_support for burst length 16 support.
 * @see dwmac_dma_cfg_axi_burst_boundary for burst boundaries.
 */
#define DWMAC_DMA_CFG_INITIALIZER( \
    bus_mode_burst_length, \
    bus_mode_burst_mode, \
    bus_mode_burst_mixed, \
    axi_burst_length_4_support, \
    axi_burst_length_8_support, \
    axi_burst_length_16_support, \
    axi_burst_boundary \
    ) \
  { \
    BSP_FLD16( bus_mode_burst_length, 0, 5 ) \
    | BSP_FLD16( bus_mode_burst_mode, 6, 6 ) \
    | BSP_FLD16( bus_mode_burst_mixed, 7, 7 ) \
    | BSP_FLD16( axi_burst_length_4_support, 8, 8 ) \
    | BSP_FLD16( axi_burst_length_8_support, 9, 9 ) \
    | BSP_FLD16( axi_burst_length_16_support, 10, 10 ) \
    | BSP_FLD16( axi_burst_boundary, 11, 11 ) \
  }

/**
 * @brief Driver configuration.
 *
 * Configuration data for the network driver.
 * See @see DWMAC_CFG_INITIALIZER for an initializer.
 */
typedef struct {
  /** @brief The clock to be used for the gmii interface in Hz. */
  const uint32_t GMII_CLK_RATE;

  /** @brief Start address of the MAC group registers. */
  volatile void *addr_gmac_regs;

  /** @brief Start address of the DMA group registers. */
  volatile void *addr_dma_regs;

  /** @brief Address of the PHY on the mdio bus (5 bit). */
  const uint8_t MDIO_BUS_ADDR;

  /** @brief Bytes per L1 cache line. */
  const uint8_t L1_CACHE_LINE_SIZE;

  /** @brief Interrupt vector number for EMAC IRQs. */
  const rtems_vector_number IRQ_EMAC;

  /** @brief Optional configuration for bus mode and axi bus mode. */
  const dwmac_dma_cfg *DMA_CFG;

  /** @brief Methods which must get provided to the by the micro controller. */
  const dwmac_callback_cfg CALLBACK;

  /** @brief Operations which are specific to the ethernet MAC. */
  const dwmac_ethernet_mac_ops *MAC_OPS;

  /** @brief DMA descriptor operations. */
  const dwmac_descriptor_ops *DESC_OPS;
} dwmac_cfg;

/**
 * @brief Configuration initializer.
 *
 * Initializes the configuration data to be passed to
 * the initialization method.
 * @see dwmac_network_if_attach_detach().
 *
 * @param[in] mdio_clk_rate           The clock to be used for the gmii
 *                                    interface in Hz.
 * @param[in] macgrp_regs_addr        Base address of the MAC group registers.
 * @param[in] dmagrp_regs_addr        Base address of the DMA group registers.
 * @param[in] mdio_bus_addr           Address of the network PHY on the
 *                                    mdio bus.
 * @param[in] l1_cache_line_size      Size of a cache line in the level 1 cache.
 * @param[in] irq_emac                Number of the EMAC interrupt.
 * @param[in] arch_has_prefetch       True if architecture supports.
 *                                    prefetching, false if not.
 * @param[in] dma_cfg_addr            Address of the optional DMA configuration.
 *                                    Set to NULL for defaults.
 * @param[in] nic_enable              Callback method for for enabling the
 *                                    network interface controller.
 * @param[in] nic_disable             Callback method for disabling the
 *                                    network interface controller.
 * @param[in] phy_enable              Callback method for enabling the
 *                                    network PHY.
 * @param[in] phy_disable             Callback method for disabling the
 *                                    network PHY.
 * @param[in] phy_event_enable        Callback method for enabling PHY status
 *                                    changes for event generation.
 * @param[in] phy_event_clear         Callback method for
 *                                    clearing/acknowledging PHY events.
 * @param[in] phy_events_get          Callback method for reading the status of
 *                                    PHY events.
 * @param[in] phy_start               Callback method for starting event
 *                                    generation by the network PHY.
 * @param[in] phy_stop                Callback method for stoping event
 *                                    generation by the network PHY.
 * @param[in] mem_alloc_nocache       Callback method for allocating uncached
 *                                    RAM.
 * @param[in] mem_free_nocache        Callback method for releasing uncached
 *                                    RAM.
 * @param[in] bus_setup               Callback method for setting up the system
 *                                    bus.
 * @param[in] ethernet_mac_ops_addr   Address of a struct encapsulating
 *                                    ethernet MAC operations for DWMAC 1000 or
 *                                    DWMAC 10/100.
 * @param[in] descriptor_ops_addr     Address of a struct encasulating DMA
 *                                    descriptor operations for either normal
 *                                    descriptors or enhanced descriptors.
 * @returns An initialized struct of configuration parameters. 
 * @see dwmac_cfg for the struct returned.
 * @see dwmac_dma_cfg for DMA configurations.
 * @see dwmac_if_nic_enable for NIC enable methods.
 * @see dwmac_if_nic_disable for NIC disable methods.
 * @see dwmac_if_phy_enable for PHY enable methods.
 * @see dwmac_if_phy_disable for PHY disable methods.
 * @see dwmac_if_phy_event_enable for PHY event enble methods.
 * @see dwmac_if_phy_event_status_clear for PHY status clear methods.
 * @see dwmac_if_phy_events_status_get for PHY status get mehods.
 * @see dwcmac_if_phy_start for PHY start methods.
 * @see dwcmac_if_phy_stop for PHY stop methods.
 * @see dwmac_if_mem_alloc_nocache for nocache memory allocate methods.
 * @see dwmac_if_mem_free_nocache for nocache memory release methods.
 * @see dwmac_if_bus_setup for bus setup methods.
 * @see DWMAC_1000_ETHERNET_MAC_OPS for DWMAC 1000 MAC operations.
 * @see DWMAC_100_ETHERNET_MAC_OPS for DWMAC 10/100 MAC operations.
 * @see DWMAC_DESCRIPTOR_OPS_NORMAL for normal DMA descriptor operations.
 * @see DWMAC_DESCRIPTOR_OPS_ENHANCED for enhanced DMA descriptor operations.
 */
#define DWMAC_CFG_INITIALIZER( \
    mdio_clk_rate, \
    macgrp_regs_addr, \
    dmagrp_regs_addr, \
    mdio_bus_addr, \
    l1_cache_line_size, \
    irq_emac, \
    dma_cfg_addr, \
    nic_enable, \
    nic_disable, \
    phy_enable, \
    phy_disable, \
    phy_event_enable, \
    phy_event_clear, \
    phy_events_get, \
    phy_start, \
    phy_stop, \
    mem_alloc_nocache, \
    mem_free_nocache, \
    bus_setup, \
    ethernet_mac_ops_addr, \
    descriptor_ops_addr \
    ) \
  { \
    mdio_clk_rate, \
    macgrp_regs_addr, \
    dmagrp_regs_addr, \
    mdio_bus_addr, \
    l1_cache_line_size, \
    irq_emac, \
    dma_cfg_addr, \
    DWMAC_CALLBACK_CFG_INITIALIZER( \
      nic_enable, \
      nic_disable, \
      phy_enable, \
      phy_disable, \
      phy_event_enable, \
      phy_event_clear, \
      phy_events_get, \
      phy_start, \
      phy_stop, \
      mem_alloc_nocache, \
      mem_free_nocache, \
      bus_setup \
      ), \
    ethernet_mac_ops_addr, \
    descriptor_ops_addr \
  }

/**
 * @brief Initialization method.
 *
 * Initializes the network driver and "links" it to the network stack.
 *
 * @param[in]  bsd_config     The BSD configuation passed to all
 *                            network_if_attach_detach() methods.
 * @param[in]  driver_config  Address of of a struct containing driver
 *                            specific configuration data.
 * @param[in]  arg            An optional argument which will get passed to all
 *                            callback methods.
 * @param[in]  attaching      1 for attching and 0 for detaching.
 *                            NOTE: Detaching is not supported!
 * @returns Address of the drivers context if successful or NULL if not
 *          successful.
 * @see dwmac_cfg for the driver configuration struct
 * @see DWMAC_CFG_INITIALIZER() for an initializer for the driver configuration
 */
void *dwmac_network_if_attach_detach(
  struct rtems_bsdnet_ifconfig *bsd_config,
  const dwmac_cfg              *driver_config,
  void                         *arg,
  int                           attaching );

/**
 * @brief Read from PHY
 *
 * Read a value from a register of the network PHY.
 *
 * @param[in,out] arg     Pointer returned from the attach_detach method.
 * @param[in]     phy_reg The PHY register to be read from.
 * @param[out]    val     Buffer address for the value to be read.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach() for the attach detach method.
 */
int dwmac_if_read_from_phy(
  void          *arg,
  const unsigned phy_reg,
  uint16_t      *val );

/**
 * @brief Write to PHY.
 *
 * Write a value to a register of the network PHY.
 *
 * @param[in,out] arg     Pointer returned from the attach_detach method.
 * @param[in]     phy_reg The PHY register to be written to.
 * @param[in]     val     The value to be written.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach() for the attach_detach method.
 */
int dwmac_if_write_to_phy(
  void          *arg,
  const unsigned phy_reg,
  const uint16_t val );

/**
 * @brief Handle PHY event.
 *
 * Handle an event from the network PHY.
 *
 * @param[in,out] arg Pointer returned from the attach_detach method.
 * @returns 0 on success, error code from errno.h on failure.
 * @see dwmac_network_if_attach_detach() for the attach_detach method.
 */
int dwmac_if_handle_phy_event( void *arg );

#ifdef __cplusplus
}
#endif /* __cplusplus */

#endif /* DWMAC_H_ */
