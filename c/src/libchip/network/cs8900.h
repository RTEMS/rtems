/*
  ------------------------------------------------------------------------

  Copyright Cybertec Pty Ltd, 2000
  All rights reserved Cybertec Pty Ltd, 2000

  Port to the DIMM PC copyright (c) 2004 Angelo Fraietta
    This project has been assisted by the Commonwealth Government
    through the Australia Council, its arts funding and advisory body.

  COPYRIGHT (c) 1989-1998.
  On-Line Applications Research Corporation (OAR).

  The license and distribution terms for this file may be
  found in the file LICENSE in this distribution or at
  http://www.rtems.com/license/LICENSE.

  ------------------------------------------------------------------------

  CS8900 RTEMS driver.

  This is a generic driver that requires a BSP backend. The BSP backend
  provides the glue to the specific bus for the target hardware. It has
  been tested with Coldfire processors, and the PC. These targets have
  completely different bus, byte order and interrupt structures.

  An example BSP backend is provided in the pci386 BSP.

  The BSP provides the following functions:

    cs8900_io_set_reg
    cs8900_io_get_reg
    cs8900_mem_set_reg
    cs8900_mem_get_reg
    cs8900_put_data_block
    cs8900_get_data_block
    cs8900_tx_load
    cs8900_attach_interrupt
    cs8900_detach_interrupt

  The header file provides documentation for these functions. There
  are four types of functions.

  The I/O set/get functions access the CS8900 I/O registers via the
  I/O Mode. For example on a PC with an ISA bus you would use the
  IA32 in/out port instructions. The cs8900_device structure passed
  to these functions provide these functions with the I/O base
  address. The BSP must provide these functions.

  The Memory set/get functions access the CS8900 internal registers
  and frame buffers directly from a 4K byte block of host memory.
  Memory mode provides a faster access to the CS8900. The cs8900_device
  structure passed to these functions provides the memory base
  address. The BSP needs to provide these functions but they do not
  need to be implemented if the mem_base field is set to 0. The
  driver will use I/O mode only.

  The Block transfer functions are used to read or write a block
  of memory from the CS8900. This saves the driver making a number
  of small calls. The BSP driver must know if I/O or Memory mode
  can be used.

  The final group of functions is to handle interrupts. The BSP
  must take care of save and restoring any interrupt state
  information.

  The BSP declares a 'cs8900_device' structure for each device being
  attached to the networking stack. It also creates a
  'struct rtems_bsdnet_ifconfig' which is used to attach the interface
  to the networking stack. The following code declares the BSD config:

    static cs8900_device cs8900;

    static struct rtems_bsdnet_ifconfig cs8900_ifconfig =
     {
       "cs0",
       cs8900_driver_attach,
       NULL,
       NULL,
       NULL,
       NULL,
       0,
       0,
       0,
       0,
       0,
       0,
       0,
       0
     };

   The device linked to the BSD config structure with:

     cs8900_ifconfig.drv_ctrl = &cs8900;

   If you have a specific hardware address you should point the BSD
   config structure to that address. If you do not the driver will read
   the MAC address from the CS8900. This assumes the CS8900 has read
   the address from an external EEPROM or has been setup by a BIOS or
   boot monitor. For EEPROM less you need to supply the MAC address.

   Set the I/O and Memory base addresses. If the Memory base address
   is 0 the driver will use I/O mode only. A typical initialisation
   looks like:

     printf ("RTEMS BSD Network initialisation.\n");
     rtems_bsdnet_initialize_network ();

     #define ETHERNET_IO_BASE   0x300
     #define ETHERNET_MEM_BASE  0
     #define ETHERNET_IRQ_LEVEL 0

     cs8900_device *cs = &cs8900;

     memset (cs, 0, sizeof (cs8900_device));

     cs->dev = 0;
     cs->io_base = ETHERNET_IO_BASE;
     cs->mem_base = ETHERNET_MEM_BASE;
     cs->irq_level = ETHERNET_IRQ_LEVEL;
     cs->rx_queue_size = 30;

     cs8900_ifconfig.drv_ctrl = &cs8900;

     printf ("CS8900 initialisation\n");

     rtems_bsdnet_attach (&cs8900_ifconfig);

     flags = IFF_UP;
     if (rtems_bsdnet_ifconfig (cs8900_ifconfig.name,
                                SIOCSIFFLAGS,
                                &flags) < 0)
     {
       printf ("error: can't bring up %s: %s\n",
               cs8900_ifconfig.name, strerror (errno));
       return;
     }

     rtems_bsdnet_do_bootp_and_rootfs ();

   The IRQ level is the one documented in the CS8900 datasheet and below
   in the CS8900 device structure. You need to map your target IRQ to the
   CS8900 in the BSP driver.

 */

#if !defined(_CS8900_H_)
#define _CS8900_H_

#include <rtems.h>
#include <rtems/error.h>
#include <rtems/rtems_bsdnet.h>

#include <sys/param.h>
#include <sys/mbuf.h>
#include <sys/socket.h>
#include <sys/sockio.h>

#include <net/if.h>

#include <netinet/in.h>
#include <netinet/if_ether.h>

/* #include <target.h> what does this provide? joel to chris */

#define ET_MINLEN 60

/*
 * CS8900 device register definitions
 */

/*
 * Crystal ESIA product id.
 */

#define CS8900_ESIA_ID             (0x630e)

/*
 * IO Registers.
 */

#define CS8900_IO_RX_TX_DATA_PORT0 (0x0000)
#define CS8900_IO_TX_TX_DATA_PORT1 (0x0002)
#define CS8900_IO_TxCMD            (0x0004)
#define CS8900_IO_TxLength         (0x0006)
#define CS8900_IO_ISQ              (0x0008)
#define CS8900_IO_PACKET_PAGE_PTR  (0x000a)
#define CS8900_IO_PP_DATA_PORT0    (0x000c)
#define CS8900_IO_PP_DATA_PORT1    (0x000e)

/*
 * Packet Page Registers.
 */

/*
 * Bus Interface Registers.
 */

#define CS8900_PP_PROD_ID          (0x0000)
#define CS8900_PP_IO_BASE          (0x0020)
#define CS8900_PP_INT              (0x0022)
#define CS8900_PP_DMA_CHANNEL      (0x0024)
#define CS8900_PP_DMA_SOF          (0x0026)
#define CS8900_PP_DMA_FRM_CNT      (0x0028)
#define CS8900_PP_DMA_RX_BCNT      (0x002a)
#define CS8900_PP_MEM_BASE         (0x002c)
#define CS8900_PP_BPROM_BASE       (0x0030)
#define CS8900_PP_BPROM_AMASK      (0x0034)
#define CS8900_PP_EEPROM_CMD       (0x0040)
#define CS8900_PP_EEPROM_DATA      (0x0042)
#define CS8900_PP_RX_FRAME_BCNT    (0x0050)

/*
 * Configuration and Control Registers.
 */

#define CS8900_PP_RxCFG            (0x0102)
#define CS8900_PP_RxCTL            (0x0104)
#define CS8900_PP_TxCFG            (0x0106)
#define CS8900_PP_TxCMD_READ       (0x0108)
#define CS8900_PP_BufCFG           (0x010a)
#define CS8900_PP_LineCFG          (0x0112)
#define CS8900_PP_SelfCTL          (0x0114)
#define CS8900_PP_BusCTL           (0x0116)
#define CS8900_PP_TestCTL          (0x0118)

/*
 * Status and Event Registers.
 */

#define CS8900_PP_ISQ              (0x0120)
#define CS8900_PP_RxEvent          (0x0124)
#define CS8900_PP_TxEvent          (0x0128)
#define CS8900_PP_BufEvent         (0x012c)
#define CS8900_PP_RxMISS           (0x0130)
#define CS8900_PP_TxCol            (0x0132)
#define CS8900_PP_LineST           (0x0134)
#define CS8900_PP_SelfST           (0x0136)
#define CS8900_PP_BusST            (0x0138)
#define CS8900_PP_TDR              (0x013c)

/*
 * Initiate Transmit Registers.
 */

#define CS8900_PP_TxCMD            (0x0144)
#define CS8900_PP_TxLength         (0x0146)

/*
 * Address Filter Registers.
 */

#define CS8900_PP_LAF              (0x0150)
#define CS8900_PP_IA               (0x0158)

/*
 * Frame Location.
 */

#define CS8900_PP_RxStatus         (0x0400)
#define CS8900_PP_RxLength         (0x0402)
#define CS8900_PP_RxFrameLoc       (0x0404)
#define CS8900_PP_TxFrameLoc       (0x0a00)

/*
 * Bit Definitions of Registers.
 */

/*
 * IO Packet Page Pointer.
 */

#define CS8900_PPP_AUTO_INCREMENT             (0x8000)

/*
 * Reg 3. Receiver Configuration.
 */

#define CS8900_RX_CONFIG_SKIP_1               (1 << 6)
#define CS8900_RX_CONFIG_STREAM_ENABLE        (1 << 7)
#define CS8900_RX_CONFIG_RX_OK                (1 << 8)
#define CS8900_RX_CONFIG_RX_DMA               (1 << 9)
#define CS8900_RX_CONFIG_RX_AUTO_DMA          (1 << 10)
#define CS8900_RX_CONFIG_BUFFER_CRC           (1 << 11)
#define CS8900_RX_CONFIG_CRC_ERROR            (1 << 12)
#define CS8900_RX_CONFIG_RUNT                 (1 << 13)
#define CS8900_RX_CONFIG_EXTRA_DATA           (1 << 14)

/*
 * Reg 4. Receiver Event.
 */

#define CS8900_RX_EVENT_HASH_IA_MATCH         (1 << 6)
#define CS8900_RX_EVENT_DRIBBLE_BITS          (1 << 7)
#define CS8900_RX_EVENT_RX_OK                 (1 << 8)
#define CS8900_RX_EVENT_HASHED                (1 << 9)
#define CS8900_RX_EVENT_IA                    (1 << 10)
#define CS8900_RX_EVENT_BROADCAST             (1 << 11)
#define CS8900_RX_EVENT_CRC_ERROR             (1 << 12)
#define CS8900_RX_EVENT_RUNT                  (1 << 13)
#define CS8900_RX_EVENT_EXTRA_DATA            (1 << 14)

/*
 * Reg 5. Receiver Control.
 */

#define CS8900_RX_CTRL_HASH_IA_MATCH          (1 << 6)
#define CS8900_RX_CTRL_PROMISCUOUS            (1 << 7)
#define CS8900_RX_CTRL_RX_OK                  (1 << 8)
#define CS8900_RX_CTRL_MULTICAST              (1 << 9)
#define CS8900_RX_CTRL_INDIVIDUAL             (1 << 10)
#define CS8900_RX_CTRL_BROADCAST              (1 << 11)
#define CS8900_RX_CTRL_CRC_ERROR              (1 << 12)
#define CS8900_RX_CTRL_RUNT                   (1 << 13)
#define CS8900_RX_CTRL_EXTRA_DATA             (1 << 14)

/*
 * Reg 7. Transmit Configuration.
 */

#define CS8900_TX_CONFIG_LOSS_OF_CARRIER      (1 << 6)
#define CS8900_TX_CONFIG_SQ_ERROR             (1 << 7)
#define CS8900_TX_CONFIG_TX_OK                (1 << 8)
#define CS8900_TX_CONFIG_OUT_OF_WINDOW        (1 << 9)
#define CS8900_TX_CONFIG_JABBER               (1 << 10)
#define CS8900_TX_CONFIG_ANY_COLLISION        (1 << 11)
#define CS8900_TX_CONFIG_16_COLLISION         (1 << 15)

/*
 * Reg 8. Transmit Event.
 */

#define CS8900_TX_EVENT_LOSS_OF_CARRIER       (1 << 6)
#define CS8900_TX_EVENT_SQ_ERROR              (1 << 7)
#define CS8900_TX_EVENT_TX_OK                 (1 << 8)
#define CS8900_TX_EVENT_OUT_OF_WINDOW         (1 << 9)
#define CS8900_TX_EVENT_JABBER                (1 << 10)
#define CS8900_TX_EVENT_16_COLLISIONS         (1 << 15)

/*
 * Reg 9. Transmit Command Status.
 */

#define CS8900_TX_CMD_STATUS_TX_START_5       (0 << 6)
#define CS8900_TX_CMD_STATUS_TX_START_381     (1 << 6)
#define CS8900_TX_CMD_STATUS_TX_START_1021    (2 << 6)
#define CS8900_TX_CMD_STATUS_TX_START_ENTIRE  (3 << 6)
#define CS8900_TX_CMD_STATUS_FORCE            (1 << 8)
#define CS8900_TX_CMD_STATUS_ONE_COLLISION    (1 << 9)
#define CS8900_TX_CMD_STATUS_INHIBIT_CRC      (1 << 12)
#define CS8900_TX_CMD_STATUS_TX_PAD_DISABLED  (1 << 13)

/*
 * Reg B. Buffer Configuration.
 */

#define CS8900_BUFFER_CONFIG_SW_INT           (1 << 6)
#define CS8900_BUFFER_CONFIG_RX_DMA_DONE      (1 << 7)
#define CS8900_BUFFER_CONFIG_RDY_FOR_TX       (1 << 8)
#define CS8900_BUFFER_CONFIG_TX_UNDERRUN      (1 << 9)
#define CS8900_BUFFER_CONFIG_RX_MISSED        (1 << 10)
#define CS8900_BUFFER_CONFIG_RX_128_BYTES     (1 << 11)
#define CS8900_BUFFER_CONFIG_TX_COL_OVF       (1 << 12)
#define CS8900_BUFFER_CONFIG_RX_MISSED_OVF    (1 << 13)
#define CS8900_BUFFER_CONFIG_RX_DEST_MATCH    (1 << 15)

/*
 * Reg C. Buffer Event.
 */

#define CS8900_BUFFER_EVENT_SW_INT            (1 << 6)
#define CS8900_BUFFER_EVENT_RX_DMA_DONE       (1 << 7)
#define CS8900_BUFFER_EVENT_RDY_FOR_TX        (1 << 8)
#define CS8900_BUFFER_EVENT_TX_UNDERRUN       (1 << 9)
#define CS8900_BUFFER_EVENT_RX_MISSED         (1 << 10)
#define CS8900_BUFFER_EVENT_RX_128_BYTES      (1 << 11)
#define CS8900_BUFFER_EVENT_RX_DEST_MATCH     (1 << 15)

/*
 * Reg 13. Line Control.
 */

#define CS8900_LINE_CTRL_RX_ON               (1 << 6)
#define CS8900_LINE_CTRL_TX_ON               (1 << 7)
#define CS8900_LINE_CTRL_AUI                 (1 << 8)
#define CS8900_LINE_CTRL_10BASET             (0 << 9)
#define CS8900_LINE_CTRL_AUTO_AUI_10BASET    (1 << 9)
#define CS8900_LINE_CTRL_MOD_BACKOFF         (1 << 11)
#define CS8900_LINE_CTRL_POLARITY_DISABLED   (1 << 12)
#define CS8900_LINE_CTRL_2_PART_DEF_DISABLED (1 << 13)
#define CS8900_LINE_CTRL_LO_RX_SQUELCH       (1 << 14)

/*
 * Reg 14. Line Status.
 */

#define CS8900_LINE_STATUS_LINK_OK           (1 << 7)
#define CS8900_LINE_STATUS_AUI               (1 << 8)
#define CS8900_LINE_STATUS_10_BASE_T         (1 << 9)
#define CS8900_LINE_STATUS_POLARITY_OK       (1 << 12)
#define CS8900_LINE_STATUS_CRS               (1 << 14)

/*
 * Reg 15. Self Control.
 */

#define CS8900_SELF_CTRL_RESET              (1 << 6)
#define CS8900_SELF_CTRL_SW_SUSPEND         (1 << 8)
#define CS8900_SELF_CTRL_HW_SLEEP           (1 << 9)
#define CS8900_SELF_CTRL_HW_STANDBY         (1 << 10)
#define CS8900_SELF_CTRL_HC0E               (1 << 12)
#define CS8900_SELF_CTRL_HC1E               (1 << 13)
#define CS8900_SELF_CTRL_HCB0               (1 << 14)
#define CS8900_SELF_CTRL_HCB1               (1 << 15)

/*
 * Reg 16. Self Status.
 */

#define CS8900_SELF_STATUS_3_3_V            (1 << 6)
#define CS8900_SELF_STATUS_INITD            (1 << 7)
#define CS8900_SELF_STATUS_SIBUST           (1 << 8)
#define CS8900_SELF_STATUS_EEPROM_PRESENT   (1 << 9)
#define CS8900_SELF_STATUS_EEPROM_OK        (1 << 10)
#define CS8900_SELF_STATUS_EL_PRESENT       (1 << 11)
#define CS8900_SELF_STATUS_EE_SIZE          (1 << 12)

/*
 * Reg 17. Bus Control.
 */

#define CS8900_BUS_CTRL_RESET_RX_DMA        (1 << 6)
#define CS8900_BUS_CTRL_USE_SA              (1 << 9)
#define CS8900_BUS_CTRL_MEMORY_ENABLE       (1 << 10)
#define CS8900_BUS_CTRL_DMA_BURST           (1 << 11)
#define CS8900_BUS_CTRL_IOCHRDYE            (1 << 12)
#define CS8900_BUS_CTRL_RX_DMA_SIZE         (1 << 13)
#define CS8900_BUS_CTRL_ENABLE_INT          (1 << 15)

/*
 * Reg 18. Bus Status.
 */

#define CS8900_BUS_STATUS_TX_BID_ERROR      (1 << 7)
#define CS8900_BUS_STATUS_RDY_FOR_TX_NOW    (1 << 8)

/*
 * Trace for debugging the isq processing. Define to 1 to enable.
 */
#define CS8900_TRACE      0
#define CS8900_TRACE_SIZE (400)

/*
 * The default receive queue size. If the BSP sets this field to
 * 0 this default is used.
 */
#define CS8900_RX_QUEUE_SIZE (30)

/*
 * Stats, more for debugging than anything else.
 */

typedef struct
{
  unsigned long rx_packets;     /* total packets received         */
  unsigned long tx_packets;     /* total packets transmitted      */
  unsigned long rx_bytes;       /* total bytes received           */
  unsigned long tx_bytes;       /* total bytes transmitted        */
  unsigned long rx_interrupts;  /* total number of rx interrupts  */
  unsigned long tx_interrupts;  /* total number of tx interrupts  */

  /* detailed rx errors: */
  unsigned long rx_dropped;     /* no mbufs in queue              */
  unsigned long rx_no_mbufs;    /* no mbufs                       */
  unsigned long rx_no_clusters; /* no clusters                    */
  unsigned long rx_oversize_errors;
  unsigned long rx_crc_errors;    /* recved pkt with crc error    */
  unsigned long rx_runt_errors;
  unsigned long rx_missed_errors; /* receiver missed packet       */

  /* detailed tx errors */
  unsigned long tx_ok;
  unsigned long tx_collisions;
  unsigned long tx_bid_errors;
  unsigned long tx_wait_for_rdy4tx;
  unsigned long tx_rdy4tx;
  unsigned long tx_underrun_errors;
  unsigned long tx_dropped;
  unsigned long tx_resends;

  /* interrupt watch dog */
  unsigned long int_swint_req;
  unsigned long int_swint_res;
  unsigned long int_lockup;

  unsigned long interrupts;

} eth_statistics;

/*
 * CS8900 device structure
 */

typedef struct
{
  /*
   * Device number.
   */

  int dev;

  /*
   * Memory base addresses. Making mem_base 0 forces the
   * driver to perform only I/O space accesses.
   */

  unsigned long  io_base;
  unsigned long  mem_base;

  /*
   * The IRQ level as defined in the datasheet for the CS8900.
   *
   *      ISA BUS   Pin    Value
   *       IRQ10   INTRQ0    0
   *       IRQ11   INTRQ1    1
   *       IRQ12   INTRQ2    2
   *       IRQ5    INTRQ3    3
   */

  int            irq_level;

  /*
   * The MAC address.
   */

  unsigned char mac_address[6];

  /*
   * The bsdnet information structure.
   */

  struct arpcom  arpcom;

  /*
   * Driver state and resources.
   */

  int            accept_bcast;
  int            tx_active;

  rtems_id       rx_task;
  rtems_id       tx_task;

  /*
   * The queues. FIXME : these should be changed to be mbuf lists.
   */

  struct mbuf    *rx_ready_head;
  struct mbuf    *rx_ready_tail;
  int            rx_ready_len;

  struct mbuf    *rx_loaded_head;
  struct mbuf    *rx_loaded_tail;
  int            rx_loaded_len;

  /*
   * Number of mbufs queued for the interrupt handler to
   * loop reading.
   */

  int            rx_queue_size;

#if CS8900_TRACE
  unsigned short trace_key[CS8900_TRACE_SIZE];
  unsigned long  trace_var[CS8900_TRACE_SIZE];
  unsigned long  trace_time[CS8900_TRACE_SIZE];
  int            trace_in;
#endif

  /**
   * Standard(!) ethernet statistics
   */

  eth_statistics eth_stats;

} cs8900_device;

/*
 * Link active returns the state of the PHY.
 *
 * @param cs Pointer to the device structure.
 */

int cs8900_link_active (cs8900_device *cs);

/**
 * The RTEMS network stack driver attach function that is loaded into the
 * the rtems_bsdnet_ifconfig struct. The network stack will call this
 * function when attaching the driver. The BSP must load the 'drv_ctrl'
 * field of the structure before calling the 'rtems_bsdnet_attach'
 * function.
 *
 * @param config The RTEMS BSD config structure.
 *
 * @param attaching True is the stack is attaching the interface.
 *
 * @retval int Set to 1 if the device has attached.
 */

int cs8900_driver_attach (struct rtems_bsdnet_ifconfig *config,
                          int                          attaching);

/**
 * The BSP specific interrupt wrapper calls this function when a device
 * interrupt occurs.
 *
 * @param v The RTEMS vector number that generated the interrupt.
 *
 * @param cs Pointer to the device structure passed to the interrupt
 *           catch function provided by the BSP.
 *
 * @retval rtems_isr The standard ISR return type.
 */

rtems_isr cs8900_interrupt (rtems_vector_number v, void *cs);

/**
 * Get the MAC address for the interface.
 *
 * @param cs Pointer to the device structure.
 *
 * @param mac_address Pointer to the memory to load the MAC address. This
 *                    is a 6 byte buffer so do not exceeed the bounds.
 */

void cs8900_get_mac_addr (cs8900_device *cs, unsigned char *mac_address);

/**
 * Catch the device interrupt. When the interrupt is called call the
 * function 'cs8900_interrupt'.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 */

void cs8900_attach_interrupt (cs8900_device *cs);

/**
 * Detach the device interrupt.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 */

void cs8900_detach_interrupt (cs8900_device *cs);

/**
 * Write to an IO space register.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param reg Register offset from the IO base.
 *
 * @param data The data to be written to the register.
 */

void cs8900_io_set_reg (cs8900_device *cs,
                        unsigned short reg, unsigned short data);

/**
 * Read an IO space register.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param reg Register offset from the IO base.
 *
 * @retval unsigned short The register data.
 */

unsigned short cs8900_io_get_reg (cs8900_device *cs, unsigned short reg);

/**
 * Write to a memory space register. Will only be called is the mem_base
 * field of the 'cs' struct is not 0.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param reg Register offset from the memory base.
 *
 * @param data The data to be written to the register.
 */

void cs8900_mem_set_reg (cs8900_device *cs,
                         unsigned long reg, unsigned short data);

/**
 * Read a memory space register. Will only be called is the mem_base
 * field of the 'cs' struct is not 0.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param reg Register offset from the IO base.
 *
 * @retval unsigned short The register data.
 */

unsigned short cs8900_mem_get_reg (cs8900_device *cs, unsigned long reg);

/**
 * Write a block of data to the interface. The BSP codes if this is an IO or
 * memory space write.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param len The length of data to write.
 *
 * @param data Pointer to the data to be written.
 */

void cs8900_put_data_block (cs8900_device *cs, int len, unsigned char *data);

/**
 * Read a block of data from the interface. The BSP codes if this is an IO or
 * memory space write. The read must not be longer than the MTU size.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param data Pointer to the buffer where the data is to be written.
 *
 * @retval unsigned short The number of bytes read from the device.
 */

unsigned short cs8900_get_data_block (cs8900_device *cs, unsigned char *data);

/**
 * Load a mbuf chain to the device ready for tranmission.
 *
 * BSP to provide this function.
 *
 * @param cs Pointer to the device structure.
 *
 * @param m Pointer to the head of an mbuf chain.
 */

void cs8900_tx_load (cs8900_device *cs, struct mbuf *m);

#endif
