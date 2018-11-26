/*  GR1553B RT driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR1553RT_H__
#define __GR1553RT_H__

/* CONFIG OPTION: Maximum number of LIST IDs supported.
 *                There are two lists per RT subaddress, one for RX one
 *                for TX.
 */
#define RTLISTID_MAX 64

/* CONFIG OPTION: Maximum number of Interrupt handlers per device supported 
 * max is 256 supported, and minimum is 1.
 */
#define RTISR_MAX 64

/* CONFIG OPTION: Maximum number of transfer (RX/TX) descriptors supported.
 *
 * Set this option to zero to allow flexible number of descriptors,
 * requires dynamically allocation of driver structures.
 */
/*#define RTBD_MAX 4096*/
#define RTBD_MAX 0

#ifdef __cplusplus
extern "C" {
#endif

/* Register GR1553B driver needed by RT driver */
extern void gr1553rt_register(void);

struct gr1553rt_list;

/* Descriptor read/written by hardware.
 *
 * Must be aligned to 16 byte boundary 
 */
struct gr1553rt_bd {
	volatile unsigned int	ctrl;	/* 0x00 Control/Status word */
	volatile unsigned int	dptr;	/* 0x04 Data Pointer */
	volatile unsigned int	next;	/* 0x08 Next Descriptor in list */
	volatile unsigned int	unused;	/* 0x0C UNUSED BY HARDWARE */
};

/* Sub address table entry, the hardware access */
struct gr1553rt_sa {
	volatile unsigned int ctrl;	/* 0x00 SUBADDRESS CONTROL WORD */
	volatile unsigned int txptr;	/* 0x04 TRANSMIT BD POINTER */
	volatile unsigned int rxptr;	/* 0x08 RECEIVE BD POINTER */
	volatile unsigned int unused;	/* 0x0C UNUSED BY HARDWARE */
};

/* Configuration of a RT-SubAddress-List */
struct gr1553rt_list_cfg {
	unsigned int bd_cnt;		/* Number of hw-descriptors in list */
};

/* A TX or RX subaddress descriptor list */
struct gr1553rt_list {
	short listid;			/* ID/NUMBER of List. -1 unassigned */
	short subadr;			/* SubAddress. -1 when not scheduled */
	void *rt;			/* Scheduled on Device */
	struct gr1553rt_list_cfg *cfg;	/* List configuration */
	int bd_cnt;			/* Number of Descriptors */

	/* !!Must be last in data structure!! 
	 * !!Array must at least be of length bd_cnt!!
	 */
	unsigned short bds[0];		/* Array of BDIDs */
};

/* GR1553B-RT Driver configuration options used when calling gr1553rt_config().
 *
 * Note that if not custom addresses are given the driver will dynamically
 *      allocate memory for buffers.
 * Note that if custom addresses with the LSB set, the address will be
 *      interpreted as a address accessible by hardware, and translated
 *      into an address used by CPU.
 */
struct gr1553rt_cfg {
	unsigned char rtaddress;	/* RT Address 0..30 */

	/*** MODE CODE CONFIG ***/
	unsigned int modecode;		/* Mode codes enable/disable/IRQ/EV-Log.
					 * Each modecode has a 2-bit cfg field.
					 * See Mode Code Control Register in
					 * hardware manual.
					 */

	/*** TIME CONFIG ***/
	unsigned short time_res;	/* Time tag resolution in us */

	/*** SUBADDRESS TABLE CONFIG ***/
	void *satab_buffer;		/* Optional Custom buffer. Must be 
					 * At least 16*32 bytes, and be aligned
					 * to 10-bit (1KB) boundary. Set to NULL
					 * to make driver allocate buffer.
					 */

	/*** EVENT LOG CONFIG ***/
	void *evlog_buffer;		/* Optional Custom buffer */
	int evlog_size;			/* Length, must be a multiple of 2.
					 * If set to ZERO event log is disabled
					 */

	/*** TRANSFER DESCRIPTOR CONFIG ***/
	int bd_count;			/* Number of transfer descriptors shared
					 * by all RX/TX sub-addresses */
	void *bd_buffer;		/* Optional Custom descriptor area.
					 * Must hold bd_count*32 bytes.
					 * If NULL, descriptors will be 	
					 * allocated dynamically. */
};

/* GR1553B-RT status indication, copied from the RT registers and stored
 * here. Used when calling the gr1553rt_status() function.
 */
struct gr1553rt_status {
	unsigned int status;		/* RT Status word */
	unsigned int bus_status;	/* BUS Status */
	unsigned short synctime;	/* Time Tag of last sync with data */
	unsigned short syncword;	/* Data of last mode code synchronize
					 * with data. */
	unsigned short time_res;	/* Time resolution (set by config) */
	unsigned short time;		/* Current Time Tag */
};

/* ISR callback definition for ERRORs detected in the GR1553B-RT interrupt
 * handler.
 *
 * \param err    Inidicate Error type. The IRQ flag register bit mask:
 *                 Bit 9  - RT DMA ERROR
 *                 Bit 10 - RT Table access error
 * \param data   Custom data assigned by user
 */
typedef void (*gr1553rt_irqerr_t)(int err, void *data);

/* ISR callback definition for modecodes that are configured to generate
 * an IRQ. The callback is called from within the GR1553B-RT interrupt
 * handler.
 *
 * \param mcode  Mode code that caused this IRQ
 * \param entry  The raw Eventlog Entry
 * \param data   Custom data assigned by user
 */
typedef void (*gr1553rt_irqmc_t)(int mcode, unsigned int entry, void *data);

/* Transfer ISR callback definition. Called from GR1553B-RT interrupt handler
 * when an interrupt has been generated and a event logged due to a 1553
 * transfer to this RT.
 *
 * \param list     List (Subaddress/TransferType) that caused IRQ
 * \param entry    The raw Eventlog Entry
 * \param bd_next  Next Descriptor-entry index in the list (Subaddress/tr-type)
 *                 This can be used to process all descriptors upto entry_next.
 * \param data     Custom data assigned by user
 */
typedef void (*gr1553rt_irq_t)(
	struct gr1553rt_list *list, 
	unsigned int entry,
	int bd_next,
	void *data
	);

/* Configure a list according to configuration. Assign the list
 * to a device, however not to a RT sub address yet. The rt
 * is stored within list.
 *
 * \param rt       RT Device driver identification, stored within list.
 * \param list     The list to configure
 * \param cfg      Configuration for list. Pointer to configuration is
 *                 stored within list for later use.
 */
extern int gr1553rt_list_init
	(
	void *rt,
	struct gr1553rt_list **plist,
	struct gr1553rt_list_cfg *cfg
	);

/* Assign an Error Interrupt handler. Before the handler is called the 
 * RT hardware is stopped/disabled. The handler is optional, if not assigned
 * the ISR will still stop the RT upon error.
 *
 * Errors detected by the interrupt handler:
 *  - DMA error
 *  - Subaddress table access error
 *
 * \param func     ISR called when an error causes an interrupt.
 * \param data     Custom data given as an argument when calling ISR
 */
extern int gr1553rt_irq_err
	(
	void *rt,
	gr1553rt_irqerr_t func,
	void *data
	);

/* Assign a ModeCode Interrupt handler callback. Called when a 1553 modecode
 * transfer is logged and cause an IRQ. The modecode IRQ generation is
 * configured from "struct gr1553rt_cfg" when calling gr1553rt_config().
 *
 * \param func     ISR called when a modecode causes an interrupt.
 * \param data     Custom data given as an argument when calling ISR
 */
extern int gr1553rt_irq_mc
	(
	void *rt,
	gr1553rt_irqmc_t func,
	void *data
	);

/* Assign transfer interrupt handler callback. Called when a RX or TX
 * transfer is logged and cause an interrupt, the function is called
 * from the GR1553B-RT driver's ISR, in interrupt context.
 *
 * The callback can be installed per subaddress and transfer type.
 * Subaddress 0 and 31 are not valid (gr1553rt_irq_mc() for modecodes).
 *
 * \param subadr   Select subaddress (1-30)
 * \param tx       1=TX subaddress, 0=RX subaddress
 * \param func     ISR called when subaddress of spcified transfer type
 *                 causes an interrupt.
 * \param data     Custom data given as an argument when calling ISR
 */
extern int gr1553rt_irq_sa
	(
	void *rt,
	int subadr,
	int tx,
	gr1553rt_irq_t func,
	void *data
	);

#define GR1553RT_BD_FLAGS_IRQEN (1<<30)
/* Initialize a descriptor entry in a list. This is typically done
 * prior to scheduling the list.
 *
 * \param entry_no  Entry number in list (descriptor index in list)
 * \param flags     Enable IRQ when descriptor is accessed by setting 
 *                  argument GR1553RT_BD_FLAGS_IRQEN. Enabling IRQ on a
 *                  descriptor basis will override SA-table IRQ config.
 * \param dptr      Data Pointer to RX or TX operation. The LSB indicate
 *                  if the address must be translated into Hardware address
 *                  - this is useful when a buffer close to CPU is used
 *                  as a data pointer and the RT core is located over PCI.
 * \param next      Next Entry in list. Set to 0xffff for end of list. Set
 *                  0xfffe for next entry in list, wrap around to entry
 *                  zero if entry_no is last descriptor in list (circular).
 */
extern int gr1553rt_bd_init(
	struct gr1553rt_list *list,
	unsigned short entry_no,
	unsigned int flags,
	uint16_t *dptr,
	unsigned short next
	);

/* Manipulate/Read Control/Status and Data Pointer words of a buffer descriptor.
 * If status is zero, the control/status word is accessed. If dptr is non-zero
 * the data pointer word is accessed.
 *
 * \param list       The list that the descriptor is located at
 *
 * \param entry_no   The descriptor number accessed
 *
 * \param status     IN/OUT. If zero no effect. If pointer is non-zero the
 *                   value pointed to:
 *                   IN: Written to Control/Status
 *                   OUT: the value of the Control/Status word before writing.
 *
 * \param dptr       IN/OUT. If zero no effect. If pointer is non-zero, the
 *                   value pointed to:
 *                   IN: non-zero: Descriptor data pointer will be updated with
 *                       this value. Note that the LSB indicate if the address
 *                       must be translated into hardware-aware address.
 *                   OUT: The old data pointer is stored here.
 */
extern int gr1553rt_bd_update(
	struct gr1553rt_list *list,
	int entry_no,
	unsigned int *status,
	uint16_t **dptr
	);

/* Get the next/current descriptor processed of a RT sub-address.
 *
 * \param subadr  RT Subaddress
 * \param txeno   Pointer to where TX descriptor number is stored.
 * \param rxeno   Pointer to where RX descriptor number is stored.
 */
extern int gr1553rt_indication(void *rt, int subadr, int *txeno, int *rxeno);

/* Take a GR1553RT hardware device identified by minor.
 * A pointer is returned that is used internally by the GR1553RT
 * driver, it is used as an input parameter 'rt' to all other
 * functions that manipulate the hardware.
 *
 * This function initializes the RT hardware to a stopped/disable level.
 */
extern void *gr1553rt_open(int minor);

/* Close and stop/disable the RT hardware. */
extern void gr1553rt_close(void *rt);

/* Configure the RT. The RT device must be configured once before
 * started. A started RT device can not be configured.
 *
 * \param rt    The RT to configure
 * \param cfg   Configuration parameters
 */
extern int gr1553rt_config(void *rt, struct gr1553rt_cfg *cfg);

/* Schedule a RX or TX list on a sub address. If a list has already been
 * schduled on the subaddress and on the same transfer type (RX/TX), the 
 * old list is replaced with the list given here.
 *
 * \param subadr   Subaddress to schedule list on
 * \param tx       Subaddress transfer type: 1=TX, 0=RX
 * \param list     Preconfigued RT list scheduled
 */
extern void gr1553rt_sa_schedule(
	void *rt,
	int subadr,
	int tx,
	struct gr1553rt_list *list
	);

/* Set SubAdress options. One may for example Enable or Disable a sub 
 * address RX and/or TX. See hardware manual for SA-Table configuration
 * options.
 *
 * \param subadr   SubAddress to configure
 * \param mask     Bit mask of option-bits written to subaddress config
 * \param options  The new options written to subaddress config.
 *
 */
extern void gr1553rt_sa_setopts(
	void *rt,
	int subadr,
	unsigned int mask,
	unsigned int options
	);

/* Get The Subaddress and transfer type of a scheduled list. Normally the
 * application knows which subaddress the list is for.
 *
 * \param list     List to lookup information for
 * \param subadr   Pointer to where the subaddress is stored
 * \param tx       Transfer type is stored here. 1=TX, 0=RX.
 */
extern void gr1553rt_list_sa(
	struct gr1553rt_list *list,
	int *subadr,
	int *tx
	);

/* Start RT Communication
 *
 * Interrupts will be enabled. The RT enabled and the "RT-run-time" 
 * part of the API will be opened for the user and parts that need the
 * RT to be stopped are no longer available. After the RT has been
 * started the configuration function can not be called.
 */
extern int gr1553rt_start(void *rt);

/* Get Status of the RT core. See data structure gr1553rt_status for more
 * information about the result. It can be used to read out:
 *  - time information
 *  - sync information
 *  - bus & RT status
 *
 * \param status   Pointer to where the status words will be stored. They
 *                 are stored according to the gr1553rt_status data structure.
 */
extern void gr1553rt_status(void *rt, struct gr1553rt_status *status);

/* Stop RT communication. Only possible to stop an already started RT device.
 * Interrupts are disabled and the RT Enable bit cleared.
 */
extern void gr1553rt_stop(void *rt);

/* Set RT vector and/or bit word.
 *
 *  - Vector Word is used in response to "Transmit vector word" BC commands
 *  - Bit Word is used in response to "Transmit bit word" BC commands
 *
 *
 * \param mask     Bit-Mask, bits that are 1 will result in that bit in the
 *                 words register being overwritten with the value of words
 * \param words    Bits 31..16: Bit Word. Bits 15..0: Vector Word.
 *
 * Operation:
 *  hw_words = (hw_words & ~mask) | (words & mask)
 */
extern void gr1553rt_set_vecword(
	void *rt,
	unsigned int mask,
	unsigned int words
	);

/* Set selectable bits of the "Bus Status Register". The bits written
 * is determined by the "mask" bit-mask. Operation:
 *
 * bus_status = (bus_status & ~mask) | (sts & mask)
 * 
 */
extern void gr1553rt_set_bussts(void *rt, unsigned int mask, unsigned int sts);

/* Read up to MAX number of entries in eventlog log. 
 *
 * \param dst   Destination address for event log entries
 * \param max   Maximal number of event log entries that an be stored into dst
 *
 * Return
 *  negative   Failure
 *  zero       No entries available at the moment
 *  positive   Number of entries copied into dst
 */
extern int gr1553rt_evlog_read(void *rt, unsigned int *dst, int max);

#ifdef __cplusplus
}
#endif

#endif
