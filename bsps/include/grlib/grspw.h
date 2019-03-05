/**
 * @file
 * @ingroup spw
 */

/*
 *  COPYRIGHT (c) 2007.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GRSPW_H__
#define __GRSPW_H__

/**
 * @defgroup spw SpaceWire
 *
 * @ingroup RTEMSBSPsSharedGRLIB
 *
 * @brief GRSPW Device Driver
 *
 * @{
 */

#include <grlib/ambapp.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SPW_LINKERR_EVENT RTEMS_EVENT_0

typedef struct {
   unsigned int rxsize;
   unsigned int txdsize;
   unsigned int txhsize;
} spw_ioctl_packetsize;

#define GRSPW_PKTSEND_OPTION_HDR_CRC  0x1
#define GRSPW_PKTSEND_OPTION_DATA_CRC 0x2
#define GRSPW_PKTSEND_OPTION_NOCRCLEN(len) ((len & 0xf) << 8)
#define GRSPW_PKTSEND_OPTION_NOCRCLEN_MASK 0xf00

typedef struct {
   unsigned int hlen;
   char *hdr;
   unsigned int dlen;
   char *data;
   unsigned int sent;
	 unsigned int options;
} spw_ioctl_pkt_send;

typedef struct {
   unsigned int tx_link_err;
   unsigned int rx_rmap_header_crc_err;
   unsigned int rx_rmap_data_crc_err;
   unsigned int rx_eep_err;
   unsigned int rx_truncated;
   unsigned int parity_err;
   unsigned int escape_err;
   unsigned int credit_err;
   unsigned int write_sync_err;
   unsigned int disconnect_err;
   unsigned int early_ep;
   unsigned int invalid_address;
   unsigned int packets_sent;
   unsigned int packets_received;
} spw_stats;

typedef struct {
   unsigned int nodeaddr;
   unsigned int destkey;
   unsigned int clkdiv; /* Note: contain both CLKDIVSTART and CLKDIVRUN, but IOCTL_SET_CLKDIV* commands are split into two */
   unsigned int rxmaxlen;
   unsigned int timer;
   unsigned int disconnect;
   unsigned int promiscuous;
   unsigned int rmapen;
   unsigned int rmapbufdis;
   unsigned int linkdisabled;
   unsigned int linkstart;

   unsigned int check_rmap_err; /* check incoming packets for rmap errors */
   unsigned int rm_prot_id; /* remove protocol id from incoming packets */
   unsigned int tx_blocking; /* use blocking tx */
   unsigned int tx_block_on_full; /* block when all tx_buffers are used */
   unsigned int rx_blocking; /* block when no data is available */
   unsigned int disable_err; /* disable link automatically when link error is detected */
   unsigned int link_err_irq; /* generate an interrupt when link error occurs */
   rtems_id event_id; /* task id that should receive link err irq event */

   unsigned int is_rmap;
   unsigned int is_rxunaligned;
   unsigned int is_rmapcrc;

   unsigned int nodemask;
	 unsigned int keep_source; /* copy source address to user-buffer in read() operations
                              * Note that rm_prot_id has no effect when keep_source is
                              * set.
                              */
	 unsigned int rtimeout; /* Read timeout if != 0 */
} spw_config;

#define SPACEWIRE_IOCTRL_SET_NODEADDR        1
#define SPACEWIRE_IOCTRL_SET_RXBLOCK         2
#define SPACEWIRE_IOCTRL_SET_DESTKEY         4
#define SPACEWIRE_IOCTRL_SET_CLKDIV          5
#define SPACEWIRE_IOCTRL_SET_TIMER           6
#define SPACEWIRE_IOCTRL_SET_DISCONNECT      7
#define SPACEWIRE_IOCTRL_SET_PROMISCUOUS     8
#define SPACEWIRE_IOCTRL_SET_RMAPEN          9
#define SPACEWIRE_IOCTRL_SET_RMAPBUFDIS      10
#define SPACEWIRE_IOCTRL_SET_CHECK_RMAP      11
#define SPACEWIRE_IOCTRL_SET_RM_PROT_ID      12
#define SPACEWIRE_IOCTRL_SET_TXBLOCK         14
#define SPACEWIRE_IOCTRL_SET_DISABLE_ERR     15
#define SPACEWIRE_IOCTRL_SET_LINK_ERR_IRQ    16
#define SPACEWIRE_IOCTRL_SET_EVENT_ID        17
#define SPACEWIRE_IOCTRL_SET_PACKETSIZE      20
#define SPACEWIRE_IOCTRL_GET_LINK_STATUS     23
#define SPACEWIRE_IOCTRL_GET_CONFIG          25
#define SPACEWIRE_IOCTRL_GET_STATISTICS      26
#define SPACEWIRE_IOCTRL_CLR_STATISTICS      27
#define SPACEWIRE_IOCTRL_SEND                28
#define SPACEWIRE_IOCTRL_LINKDISABLE         29
#define SPACEWIRE_IOCTRL_LINKSTART           30
#define SPACEWIRE_IOCTRL_SET_TXBLOCK_ON_FULL 31
#define SPACEWIRE_IOCTRL_SET_COREFREQ        32
#define SPACEWIRE_IOCTRL_SET_CLKDIVSTART     33
#define SPACEWIRE_IOCTRL_SET_NODEMASK        34
#define SPACEWIRE_IOCTRL_SET_KEEP_SOURCE     35
#define SPACEWIRE_IOCTRL_SET_TCODE_CTRL      36
#define SPACEWIRE_IOCTRL_SET_TCODE           37
#define SPACEWIRE_IOCTRL_GET_TCODE           38
#define SPACEWIRE_IOCTRL_SET_READ_TIMEOUT    39

#define SPACEWIRE_IOCTRL_START               64
#define SPACEWIRE_IOCTRL_STOP                65

/* Defines what register bits that will be touched 
 * for SPACEWIRE_IOCTRL_SET_TCODE_CTRL
 */
#define SPACEWIRE_TCODE_CTRL_IE_MSK          0x001
#define SPACEWIRE_TCODE_CTRL_TT_MSK          0x004
#define SPACEWIRE_TCODE_CTRL_TR_MSK          0x008

/* Defines what register bits that should be set
 * for SPACEWIRE_IOCTRL_SET_TCODE_CTRL
 */
#define SPACEWIRE_TCODE_CTRL_IE              0x100
#define SPACEWIRE_TCODE_CTRL_TT              0x400
#define SPACEWIRE_TCODE_CTRL_TR              0x800

/* SPACEWIRE_IOCTRL_SET_TCODE argument mask */
#define SPACEWIRE_TCODE_TCODE                0x0ff
#define SPACEWIRE_TCODE_SET                  0x100 /* Set Timecode register */
#define SPACEWIRE_TCODE_TX                   0x400

void grspw_register_drv (void);

void grspw_print(int options);

/* Global GRSPW Function pointer called upon timecode receive interrupt */
extern void (*grspw_timecode_callback)
    (void *pDev, void *regs, int minor, unsigned int tc);


#ifdef __cplusplus
}
#endif

/** @} */

#endif /* __GRSPW_H__ */
