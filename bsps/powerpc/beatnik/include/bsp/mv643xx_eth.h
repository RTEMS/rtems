#ifndef MV643XX_LOWLEVEL_DRIVER_H
#define MV643XX_LOWLEVEL_DRIVER_H

struct mveth_private;

struct mveth_private *
BSP_mve_create(
	int		 unit,
	rtems_id tid,
	void     (*isr)(void*isr_arg),
	void     *isr_arg,
	void (*cleanup_txbuf)(void *user_buf, void *closure, int error_on_tx_occurred), 
	void *cleanup_txbuf_arg,
	void *(*alloc_rxbuf)(int *p_size, uintptr_t *p_data_addr),
	void (*consume_rxbuf)(void *user_buf, void *closure, int len),
	void *consume_rxbuf_arg,
	int		rx_ring_size,
	int		tx_ring_size,
	int		irq_mask
);

/* Enable/disable promiscuous mode */
void
BSP_mve_promisc_set(struct mveth_private *mp, int promisc);

/* Clear multicast filters                        */
void
BSP_mve_mcast_filter_clear(struct mveth_private *mp);
void
BSP_mve_mcast_filter_accept_all(struct mveth_private *mp);
void
BSP_mve_mcast_filter_accept_add(struct mveth_private *mp, unsigned char *enaddr);
void
BSP_mve_mcast_filter_accept_del(struct mveth_private *mp, unsigned char *enaddr);
/* Stop hardware and clean out the rings */
void
BSP_mve_stop_hw(struct mveth_private *mp);

/* MAIN RX-TX ROUTINES
 *
 * BSP_mve_swipe_tx():  descriptor scavenger; releases mbufs
 * BSP_mve_send_buf():  xfer mbufs from IF to chip
 * BSP_mve_swipe_rx():  enqueue received mbufs to interface
 *                    allocate new ones and yield them to the
 *                    chip.
 */

/* clean up the TX ring freeing up buffers */
int
BSP_mve_swipe_tx(struct mveth_private *mp);

/* Enqueue a mbuf chain or a raw data buffer for transmission;
 * RETURN: #bytes sent or -1 if there are not enough descriptors
 *         -2 is returned if the caller should attempt to
 *         repackage the chain into a smaller one.
 *
 * Comments: software cache-flushing incurs a penalty if the
 *           packet cannot be queued since it is flushed anyways.
 *           The algorithm is slightly more efficient in the normal
 *			 case, though.
 */

typedef struct MveEthBufIter {
	void  *hdl;    /* opaque handle for the iterator implementor */
	void  *data;   /* data to be sent                            */
	size_t len;    /* data size (in octets)                      */
	void  *uptr;   /* user-pointer to go into the descriptor;
                      note: cleanup_txbuf is only called for desriptors
	                        where this field is non-NULL (for historical
	                        reasons)                             */
} MveEthBufIter;

typedef MveEthBufIter *(*MveEthBufIterNext)(const MveEthBufIter*);

int
BSP_mve_send_buf_chain(struct mveth_private *mp, MveEthBufIterNext next, MveEthBufIter *it);

int
BSP_mve_send_buf_raw(
	struct mveth_private *mp,
	void                 *head_p,
	int                   h_len,
	void                 *data_p,
    int                   d_len);

/* send received buffers upwards and replace them
 * with freshly allocated ones;
 * ASSUMPTION:	buffer length NEVER changes and is set
 *				when the ring is initialized.
 * TS 20060727: not sure if this assumption is still necessary - I believe it isn't.
 */
int
BSP_mve_swipe_rx(struct mveth_private *mp);

rtems_id
BSP_mve_get_tid(struct mveth_private *mp);
int
BSP_mve_detach(struct mveth_private *mp);

/* Fire up the low-level driver
 *
 * - make sure hardware is halted
 * - enable cache snooping
 * - clear address filters
 * - clear mib counters
 * - reset phy
 * - initialize (or reinitialize) descriptor rings
 * - check that the firmware has set up a reasonable mac address.
 * - generate unicast filter entry for our mac address
 * - write register config values to the chip
 * - start hardware (serial port and SDMA)
 */

void
BSP_mve_init_hw(struct mveth_private *mp, int promisc, unsigned char *enaddr);

#define MV643XX_MEDIA_FD   (1<<0)
#define MV643XX_MEDIA_10   (1<<8)
#define MV643XX_MEDIA_100  (2<<8)
#define MV643XX_MEDIA_1000 (3<<8)
#define MV643XX_MEDIA_SPEED_MSK (0xff00)
void
BSP_mve_update_serial_port(struct mveth_private *mp, int media);

/* read ethernet address from hw to buffer */
void
BSP_mve_read_eaddr(struct mveth_private *mp, unsigned char *oeaddr);

void
BSP_mve_enable_irqs(struct mveth_private *mp);
void
BSP_mve_disable_irqs(struct mveth_private *mp);

#define MV643XX_ETH_IRQ_RX_DONE						(1<<2)
#define MV643XX_ETH_EXT_IRQ_TX_DONE					(1<<0)
#define MV643XX_ETH_EXT_IRQ_LINK_CHG				(1<<16)
uint32_t
BSP_mve_ack_irqs(struct mveth_private *mp);

void
BSP_mve_enable_irq_mask(struct mveth_private *mp, uint32_t mask);

uint32_t
BSP_mve_disable_irq_mask(struct mveth_private *mp, uint32_t mask);

uint32_t
BSP_mve_ack_irq_mask(struct mveth_private *mp, uint32_t mask);

void
BSP_mve_dump_stats(struct mveth_private *mp, FILE *f);

unsigned
BSP_mve_mii_read(struct mveth_private *mp, unsigned addr);

unsigned
BSP_mve_mii_write(struct mveth_private *mp, unsigned addr, unsigned v);

unsigned
BSP_mve_mii_read_early(int port, unsigned addr);

unsigned
BSP_mve_mii_write_early(int port, unsigned addr, unsigned v);

#endif
