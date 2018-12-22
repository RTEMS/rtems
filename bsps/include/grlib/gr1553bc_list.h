/*
 *  GR1553B BC driver, Descriptor LIST handling
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#ifndef __GR1553BC_LIST_H__
#define __GR1553BC_LIST_H__

/*!\file doc/gr1553bc_list.h
 * \brief GR1553B BC driver
 *
 * \section OVERVIEW
 *
 * The BC device driver can schedule synchronous and asynchronous lists
 * of descriptors. The list contains a descriptor table and a software
 * description to make some operations possible, for example translate
 * descriptor-address into descriptor-number.
 *
 * This is the LIST API. It provides functionality to create and manage
 * a BC descriptor list.
 *
 * A list is built up by the following build blocks:
 *  - Major Frame (Consists of N Minor Frames)
 *  - Minor Frame (Consists of up to 32 1553 Message Slots)
 *  - Message Slot (Transfer/Condition BC descriptor)
 *
 * The user can configure lists with different configuration of number of
 * Major Frames, Minor Frame and messages slots within a Minor Frame. The
 * List manages a strait descriptor table (may be changed) and a Frame/Slot
 * tree in order to easily find it's way through all descriptor created.
 *
 * Each Minor frame consist of up to 32 message slot and 2 message slots
 * for time management and descriptor find operations. The list can manage
 * time slots per minor frame, for example a minor frame may be programmed
 * to take 8ms and when the user allocate a message slot within that Minor
 * frame the time spcified will be subtracted from the 8ms, and when the
 * message slot is freed the time will be returned to the Minor frame again.
 *
 * A Major, Minor and Message Slots are identified using a MID (Message-ID).
 * The MID is a way for the user to avoid using pointers are talk with the
 * list API in an easier way. For example a condition Slot that should jump
 * to a transfer slot can be created by knowing "MID and Jump-To-MID". When
 * allocating a Slot (with or without time) the user may specify a certain
 * Slot or a Minor frame, when a Minor frame is given then the API will find
 * a free Slot as early in the Minor Frame as possible and return it to the
 * user.
 *
 * A MID can be created using the macros:
 *   GR1553BC_ID(major,minor,slot)      - ID of a SLOT
 *   GR1553BC_MINOR_ID(major,minor)     - ID of a MINOR (Slot=0xff)
 *   GR1553BC_MAJOR_ID(major)           - ID of a Major (Minor=0xff,Slot=0xff)
 *
 * The typical approach create lists is in the following order:
 *   -# gr1553bc_list_alloc(&list, MAJOR_CNT)
 *   -# gr1553bc_list_config(list, &listcfg)
 *   -# Create all Major Frames and Minor frame, for each major frame:
 *       a) gr1553bc_major_alloc_skel(&major, &major_minor_cfg)
 *       b) gr1553bc_list_set_major(list, &major, MAJOR_NUM)
 *   -# link end Major Frames together:
 *       a) gr1553bc_list_set_major(&major7, &major0)   // Connect Major frames
 *   -# gr1553bc_list_table_alloc()   (Allocate Descriptor Table)
 *   -# gr1553bc_list_table_build()   (Build Descriptor Table from Majors/Minors)
 *   -# Allocate and initialize Descriptors pre defined before starting:
 *      -## gr1553bc_slot_alloc(list, &MID, TIME_REQUIRED, ..)
 *      -## gr1553bc_slot_transfer(MID, ...)
 *   -# START BC HARDWARE BY SHCDULING ABOVE LIST
 *   -# Operate on List
 *
 *
 * \section bc_list_update Changing a scheduled BC list (during BC-runtime)
 *
 *  One can use the INDICATION service to avoid modifying
 *  a descriptor currently in use by the BC core. One can also in most cases
 *  do descriptor initialization in three steps: Init Descriptor as Dummy 
 *  with and allocated time (often done before starting/scheduling list),
 *  then modify transfer options and data-pointers, then clear the Dummy
 *  bit in one atomic data store. This approach will avoid potential races
 *  between software has hardware.
 *
 *
 * \section bc_memory_setup Custom Memory Setup
 *
 *  For designs where dynamically memory is not an option, or the driver
 *  is used on a AMBA-over-PCI bus (where malloc() does not work), the
 *  API allows the user to provide custom addresses for descriptor table
 *  and object descriptions (lists, major frames, minor frames). Custom
 *  descriptor table is probably the most interesting thing for most, it
 *  is setup with gr1553bc_list_table_alloc(list, CUSTOM_ADDRESS).
 *
 *  Object descriptions are normally allocated during initialization
 *  procedure by providing the API with a object configuration, for 
 *  example a Major Frame configuration enables the API to allocate
 *  the software description of a Major Frame with all it's Minor frames.
 *
 *
 * \section major Major Frame
 *
 *  Consists of multiple Minor frames. A Major frame may be connected/linked
 *  with another Major frame, this will result in a Jump Slot from last
 *  Minor frame in the first Major to the first Minor in the second Major.
 *
 *
 * \section minor Minor Frame
 *
 *  Consists of up to 32 Message Slots. The services are Time-Management and
 *  Slot allocation.
 *
 *  Time-Management is optional.
 *
 *  Time-Slot-Management can be enabled per Minor frame. A Minor frame can be
 *  assigned a time in microseconds. The BC will not continue to the next
 *  Minor frame until the time has passed. It is managed by adding an extra
 *  Dummy Message Slot with the total minor frame time. Each time a message
 *  Slot is allocated (with a certain time: Slot-Time) the Slot-Time will
 *  be decremented from the total time of the Minor frame. This way the
 *  sum of the Message Slot will always sum up to the total time of the
 *  Minor configuration. When a message slot is freed, the Dymmy Message
 *  Slot's Slot-Time is incremented with the freed Slot-Time.
 *
 *  A Message Slot can be allocated by identifying a specific free Slot
 *  by the MID (Message-ID) or by letting the API allocate the first free
 *  Slot in the Minor Frame (Set MID Slot-ID to 0xff to identify Minor
 *  Frame).
 *
 *
 * \section slot Message Slot
 *
 *  The GR1553B BC core supports two Slot (Descriptor) Types:
 *   - Transfer descriptor
 *   - Condition descriptor (Jump, unconditional-IRQ)
 *
 *  See the hardware manual for a detail description of a descriptor (Slot).
 *
 *  The BC Core is unaware of lists, it steps through executing each 
 *  descriptor as the encountered, Conditionals resulting in jumps may
 *  let us to create more complex arrangements of buffer descriptos (BDs)
 *  which we call list.
 *
 *  Transfer BDs (TBDs) may have a time slot assigned, the BC core will wait
 *  until the time has expired before executing the next descriptor. Time
 *  slots are handled by a Minor frame in the list.
 *
 *  A Message Slot is allocated using the gr1553bc_slot_alloc() function,
 *  and configured by calling one of the below functions:
 *   - gr1553bc_slot_irq_prepare   [unconditional IRQ slot]
 *   - gr1553bc_slot_jump          [unconditional jump]
 *   - gr1553bc_slot_exttrig       [Dummy transfer, wait for EXTERNAL-TRIGGER]
 *   - gr1553bc_slot_transfer      [Transfer descriptor]
 *   - gr1553bc_slot_empty         [Create Dummy Transfer descriptor]
 *   - gr1553bc_slot_raw           [Custom Descriptor handling]
 *
 *   - gr1553bc_slot_dummy         [Set existing Transfer descriptor to Dummy]
 *   - gr1553bc_slot_update        [Update DataPointer|Status of a TBD]
 *
 *
 * \section bc_IRQ Interrupt Handling
 *
 * There are different types of interrupts, Error IRQs or transfer IRQs. The
 * Error IRQs are handled by the driver can a callback function is called. 
 *
 * Transfer Descriptors can be programmed to generate interrupt, and
 * condition descriptors can be programmed to generate interrupt
 * unconditionaly (there exists more conditional types). When a Transfer
 * descriptor causes IRQ the general ISR callback of the BC driver is 
 * called to let the user handle the interrupt. When a condition descriptor
 * causes an IRQ a custom IRQ handler is called (if assigned).
 *
 * Transfers descriptor IRQ is enabled by configuring the descriptor.
 *
 * The API provides functions for placing unconditional IRQ points anywhere
 * in the list. The order:
 *   -# gr1553bc_slot_alloc(&MID, TIME=0, ..)
 *   -# gr1553bc_slot_irq_prepare(MID, funcISR, data)
 *   -# gr1553bc_slot_irq_enable(MID)
 * 
 * \verbatim
 *  void funcISR(*bd, *data)
 *  {
 *    // HANDLE ONE OR MULTIPLE DESCRIPTORS (MULTIPLE IN THIS EXAMPLE):
 *    int MID;
 *    gr1553bc_mid_from_bd(bd,&MID,NULL);
 *    printf("IRQ ON %06x\n", MID);
 *  }
 * \endverbatim
 *
 * \ingroup GR1553BC
 */

#include <stdint.h>
#include "gr1553bc.h"

/**** CONFIGURATION OPTIONS ****/

/* Define GR1553BC_TIMESLOT to make driver take care of time
 * management of minor frames.
 */
#define GR1553BC_TIMESLOT

#define GR1553BC_MINOR_MAX 256
#define GR1553BC_SLOT_MAX 32

#ifdef __cplusplus
extern "C" {
#endif

struct gr1553bc_list;
struct gr1553bc_major;
struct gr1553bc_minor;
struct gr1553bc_minor_cfg;
struct gr1553bc_major_cfg;

struct gr1553bc_minor_cfg {
	int slot_cnt;
	int timeslot;		/* Total time of minor frame in us */
};

struct gr1553bc_major_cfg {
	int minor_cnt;				/* Number of Minor Frames */
	struct gr1553bc_minor_cfg minor_cfgs[1];
};

struct gr1553bc_list_cfg {
	unsigned char rt_timeout[31];	/* Number of us timeout tolerance per RT */
	unsigned char bc_timeout;	/* Number of us timeout tolerance of 
					 * broadcast transfers */
	int tropt_irq_on_err;		/* Generate IRQ on transfer error */
	int tropt_pause_on_err;		/* Pause list on transfer error */
	int async_list;			/* Set to non-zero if asyncronous list*/
};

/* Default Configuration */
extern struct gr1553bc_list_cfg gr1553bc_def_cfg;

/* Complete list of all major frames */
struct gr1553bc_list {
	void *_table_custom;		/* Config option given by user */
	void *_table;			/* address of allocated bd-table */
	unsigned int table_hw;		/* Descriptor table base HW-ADR */
	unsigned int table_cpu;		/* Descriptor table base CPU-ADR */
	int table_size;			/* Descriptor Table Size */
	void *bc;			/* BC HW, needed for adr translation */
	unsigned char rt_timeout[32];	/* Tolerance per RT, default 20us
					 * Note: 31 is for Broadcast */
	uint32_t tropts;		/* Transfer descriptor options:
					 *  On transfer error the following bits
					 *  do affect:
					 *  - bit28 1=Generate IRQ
					 *  - bit26 1=Pause transfer list
					 *  
					 */
	int async_list;			/* async list or not */
	int major_cnt;			/* Number of Major frames */
	struct gr1553bc_major *majors[1];	/* Var-Array of Major Pointers*/
};

/* Alloc a List with a maximum number of Major frames supported */
extern int gr1553bc_list_alloc(struct gr1553bc_list **list, int max_major);

/* Free List if allocated with gr1553bc_list_alloc() */
extern void gr1553bc_list_free(struct gr1553bc_list *list);

/* Configure Global List parameters 
 *
 * \param list    List to be configured and initialized.
 * \param cfg     List Configuration
 * \param bc      The BC hardware device description
 *                  (only needed for address translation)
 */
extern int gr1553bc_list_config
	(
	struct gr1553bc_list *list,
	struct gr1553bc_list_cfg *cfg,
	void *bc
	);

/* Link a 'major' Major frame with next major frame 
 * The links affected:
 *   - major->next
 *   - major->minor[LAST]->next
 */
extern void gr1553bc_list_link_major(
	struct gr1553bc_major *major,
	struct gr1553bc_major *next
	);

/* Link in a Major frame into a BC list.
 * Calls gr1553bc_list_link_major() to link major frame with major-1 and
 * major+1. If ending or starting major frame the frame is wrapped around.
 */
extern int gr1553bc_list_set_major(
	struct gr1553bc_list *list,
	struct gr1553bc_major *major,
	int no);

/* Calculate the size required in the descriptor table by one minor frame. */
extern int gr1553bc_minor_table_size(struct gr1553bc_minor *minor);

/* Calculate the size required for the descriptor table.
 */
extern int gr1553bc_list_table_size(struct gr1553bc_list *list);

/* Allocate an empty descriptor table from list description suitable for
 * the BC given by 'bc'.
 *
 * \param bdtab_custom   Custom Descriptor Allocation options:
 *                         ZERO: Dynamically allocated by Driver (CPU near RAM)
 *                         Non-Zero: Use provided address as BASE of BD-TABLE
 *                         Non-Zero with LSB set: Same as Non-Zero but address
 *                          is given as HW address (used with AMBA-over-PCI to
 *                          to specify RAM location on PCI board).
 */
extern int gr1553bc_list_table_alloc
	(
	struct gr1553bc_list *list,
	void *bdtab_custom
	);

/* Free descriptor table allocated with gr1553bc_list_table_alloc() */
extern void gr1553bc_list_table_free(struct gr1553bc_list *list);

/* Build an empty descriptor table from list description, 
 * the minor frames will be linked together.
 */
extern int gr1553bc_list_table_build(struct gr1553bc_list *list);

/* Major Frame */
struct gr1553bc_major {
	struct gr1553bc_major *next;		/* Next Major Frame */
	struct gr1553bc_major_cfg *cfg;		/* User Config of Major frame */
	struct gr1553bc_minor *minors[1];	/* Minor frames */
};

/* Minor Frame */
struct gr1553bc_minor {
	struct gr1553bc_minor *next;	/* Next Minor Frame */
	struct gr1553bc_minor_cfg *cfg;	/* User Config of Minor frame */
	uint32_t alloc;			/* Descripts allocated */

	/* Note: THIS POINTER MUST BE ALIGNED ON A 128-bit BOUNDARY */
	union gr1553bc_bd *bds;	/* Descriptors for this minor frame (CPU ADRS)*/
};

/* Alloc a Major/Minor frame skeleton according to the configuration structure. 
 * The descriptor table is not allocated.
 */
extern int gr1553bc_major_alloc_skel
	(
	struct gr1553bc_major **major,
	struct gr1553bc_major_cfg *cfg
	);

/* Unique Message/Descriptor ID. Can be used to identify a Major or Minor 
 * Frame, or a Slot.
 *
 * - If minor_num is 0xff, the ID identifies a Major Frame
 * - If slot_num is 0xff, the ID identifies a Minor Frame
 * - If non of the above is true, the ID identifies a specific Slot
 */
#define GR1553BC_ID(major_num, minor_num, slot_num) \
		((((major_num)<<16)&0xff0000) | (((minor_num)<<8)&0xff00) | \
		((slot_num) & 0xff))
#define GR1553BC_MINOR_ID(major_num, minor_num) \
		GR1553BC_ID(major_num, minor_num, 0xff)
#define GR1553BC_MAJOR_ID(major_num) \
		GR1553BC_ID(major_num, 0xff, 0xff)

#define GR1553BC_MAJID_FROM_ID(mid) (((mid) >> 16) & 0xff)
#define GR1553BC_MINID_FROM_ID(mid) (((mid) >> 8) & 0xff)
#define GR1553BC_SLOTID_FROM_ID(mid) ((mid) & 0xff)
#define GR1553BC_ID_SET_SLOT(mid, slot_num) (((mid) & ~0xff) | ((slot_num) & 0xff))

extern struct gr1553bc_major *gr1553bc_major_from_id
	(
	struct gr1553bc_list *list,
	int mid
	);

extern struct gr1553bc_minor *gr1553bc_minor_from_id
	(
	struct gr1553bc_list *list,
	int mid
	);

/* Get free time left of minor frame identified by MID 'mid' */
extern int gr1553bc_list_freetime(struct gr1553bc_list *list, int mid);

/* Get free time left of minor frame */
extern int gr1553bc_minor_freetime(struct gr1553bc_minor *minor);

/* Allocate a time slot on a minor frame, major/minor frame is identified 
 * by MID. The 'mid' is a input/ouput parameter, the resulting slot taken
 * will be placed in 'mid', a pointer to the allocated descriptor is stored
 * into bd.
 *
 * Major/Minor must be specified by MID, if slot is specified that slot will
 * be allocated, if slot is 0xff, then the first free slot is allocated.
 *
 * The function fails (return negative) if timeslot is longer than remaining
 * time in minor frame, if no more slots are available in minor frame, if 
 * MID points to a bad major/minor or major/minor/slot.
 */
extern int gr1553bc_slot_alloc(
	struct gr1553bc_list *list,
	int *mid,
	int timeslot,
	union gr1553bc_bd **bd
	);
/* Same as gr1553bc_slot_alloc but identifies a minor instead of list.
 * The major/minor part of MID is ignored.
 */
extern int gr1553bc_slot_alloc2(
	struct gr1553bc_minor *minor,
	int *mid,
	int timeslot,
	union gr1553bc_bd **bd
	);

/* Free message slot and the time associated with it. The time taken by the
 * message slot is added to the END TIME descriptor, if managed by the driver
 * for this minor frame. The descriptor will be 
 */
extern int gr1553bc_slot_free(struct gr1553bc_list *list, int mid);
extern int gr1553bc_slot_free2(struct gr1553bc_minor *minor, int mid);

/* Find MID from Descriptor pointer
 *
 * In the end of each minor frame is a unconditional jump 
 * to next minor frame descriptor. The hardware does not
 * use the last 8 bytes of conditional descriptors, in the
 * padding area a MID is stored so that we can lookup the
 * MID of a descriptor. This function finds the jump
 * descriptor and subtracs the offset from it.
 *
 * A faster way of looking up can be implemented if the
 * list is symertical, however in the current setup we
 * allow different numbers of slots in minor frames, and
 * different number of minor frames in a major frame.
 *
 * \param bd     IN: Descriptor to lookup MID of (CPU address of BD)
 * \param mid    OUT: Pointer to where Message-ID (Slot-ID) will be stored
 * \param async  OUT: Function will store non-zero value if BD belogs to 
 *                    async list.
 */
extern int gr1553bc_mid_from_bd(
	union gr1553bc_bd *bd,
	int *mid,
	int *async
	);

/********** TRANSFER DESCRIPTOR MANIPULATION **********/

/* Get pointer to descriptor entry from MID. */
extern union gr1553bc_bd *gr1553bc_slot_bd
	(
	struct gr1553bc_list *list,
	int mid
	);

/* IRQ function */
typedef void (*bcirq_func_t)(union gr1553bc_bd *bd, void *data);

/* Create unconditional IRQ customly defined location.
 * The IRQ is disabled, enable it with gr1553bc_slot_irq_enable().
 */
extern int gr1553bc_slot_irq_prepare
	(
	struct gr1553bc_list *list,
	int mid,
	bcirq_func_t func,
	void *data
	);

/* Enable previously prepared unconditional IRQ */
extern int gr1553bc_slot_irq_enable(struct gr1553bc_list *list, int mid);

/* Disable unconditional IRQ point, changed to unconditional JUMP
 * to descriptor following.
 * After disabling it it can be enabled again, or freed.
 */
extern int gr1553bc_slot_irq_disable(struct gr1553bc_list *list, int mid);

/* Create custom jump to descriptor, conditional or unconditional, see 
 * hardware manual for conditions.
 *
 * set conditional to GR1553BC_UNCOND_JMP for unconditional jump.
 */
extern int gr1553bc_slot_jump
	(
	struct gr1553bc_list *list,
	int mid,
	uint32_t condition,
	int to_mid
	);

/* Create a dummy transfer, paused until external trigger is set. The
 * Slot is will have the dummy bit set, no transfer will take place.
 */
extern int gr1553bc_slot_exttrig(struct gr1553bc_list *list, int mid);

/* Create a transfer on a previous allocated descriptor. It is assumed
 * that the descriptor has been initialized empty before calling this
 * function, this is to avoid races.
 *
 * The settings that are controlled on a global level (and not
 * by this function):
 *  - IRQ after transfer error
 *  - IRQ after transfer (not supported, insert separate IRQ slot after this)
 *  - Pause schedule after transfer error
 *  - Pause schedule after transfer (not supported)
 *  - slot time optional (set when MID allocated), otherwise 0
 *  - (OPTIONAL) Dummy Bit, set using slot_empty() or ..._TT_DUMMY
 *  - RT timeout tolerance (managed per RT)
 *
 *  Input Parameters:
 *  - Retry Mode			(options)
 *  - Number of retires			(options)
 *  - Bus selection (A or B)		(options)
 *  - dummy bit				(options)
 *  - transfer type			(tt)
 *  - rt src/dst address		(tt)
 *  - RT subaddress			(tt)
 *  - word count			(tt)
 *  - mode code				(tt)
 *  - data pointer			(dptr)
 *
 *
 * See macros defined in this header file for creating transfer types (tt)
 * and word count etc.
 *
 * See macros defined in this header file for creating the mask of options.
 *
 * Note that if bit0 (LSB) of dptr is set, then the address is translated into
 * hardware address, otherwise the dptr is assumed to be accessible from the
 * 1553 core. This is an option only for AMBA-over-PCI.
 */
extern int gr1553bc_slot_transfer(
	struct gr1553bc_list *list,
	int mid,
	int options,
	int tt,
	uint16_t *dptr);

/* Remove or set dummy bit of a transfer descriptor
 * Bit31 of *dummy is written to the dummy bit, the 
 * old descriptor value is stored into *dummy.
 */
extern int gr1553bc_slot_dummy(
	struct gr1553bc_list *list,
	int mid,
	unsigned int *dummy);

/* Make a slot empty (BC will not generate bus transfers), time slot 
 * allocated is untouched (if assigned).
 */
extern int gr1553bc_slot_empty(struct gr1553bc_list *list, int mid);

/* Transfer descriptor status and/or update Transfer descriptor data pointer.
 *
 * Read and/or write Status of a slot. Writing the status word may be
 * used by software to indicate that result has been handled, or bit 31
 * may be written 1 telling software that when it reaches 0, then BC
 * has executed the request.
 *
 * Operation:
 *  bd->status = *stat & (bd->status 0xffffff) | (*stat & 0x80000000);
 *  *stat = Value of bd->status before rewrite.
 *
 * Note that the status word is not written when *stat is zero.
 *
 * Note that if bit0 (LSB) of dptr is set, then the address is translated into
 * hardware address, otherwise the dptr is assumed to be accessible from the
 * 1553 core. This is an option only for AMBA-over-PCI.
 */
extern int gr1553bc_slot_update(
	struct gr1553bc_list *list,
	int mid,
	uint16_t *dptr,
	unsigned int *stat);

/* Modify a transfer descriptor in any way,
 * 
 * flags:
 *  bit[N=0..3]: 1 = set BD wordN according to argument wordN,
 *               0 = do not modify BD wordN
 */
extern int gr1553bc_slot_raw
	(
	struct gr1553bc_list *list,
	int mid,
	unsigned int flags,
	uint32_t word0,
	uint32_t word1,
	uint32_t word2,
	uint32_t word3
	);


/***** Macros to create BC Transfer Types (tt) for gr1553bc_slot_transfer() *****/

/* WRITE TO RT (BC-to-RT) */
#define GR1553BC_BC2RT(rtadr, subadr, word_count) \
		((rtadr<<11) | (subadr<<5) | (0x1f<<21) | (0<<10) | \
		((word_count>=32) ? 0 : word_count))

/* READ FROM RT (RT-to-BC) */
#define GR1553BC_RT2BC(rtadr, subadr, word_count) \
		((rtadr<<11) | (subadr<<5) | (0x1f<<21) | (1<<10) | \
		((word_count>=32) ? 0 : word_count))

/* RT(TX) WRITE TO RT(RX) (RT-to-RT) */
#define GR1553BC_RT2RT(tx_rtadr, tx_subadr, rx_rtadr, rx_subadr, word_count) \
		((rx_rtadr<<11) | (rx_subadr<<5) | \
		(tx_rtadr<<21) | (tx_subadr<<16) | \
		(0<<10) | \
		((word_count>=32) ? 0 : word_count))

/* Mode command without data. (BC-to-RT)
 * Mode code: 0,1,2,3,4,5,6,7 or 8.
 */
#define GR1553BC_MC_NODATA(rtadr, modecode) \
		((rtadr<<11) | (0x1f<<5) | (0x1f<<21) | \
		(modecode<<0) | (1<<10))

/* Mode command with 4 byte data (RT-to-BC)
 * Mode code: 16, 18 or 19.
 */
#define GR1553BC_MC_RT2BC(rtadr, modecode) \
		((rtadr<<11) | (0x1f<<5) | (0x1f<<21) | \
		(modecode<<0) | (1<<10))

/* Mode command with 4 byte data (BC-to-RT)
 * Mode code: 17, 20 or 21.
 */
#define GR1553BC_MC_BC2RT(rtadr, modecode) \
		((rtadr<<11) | (0x1f<<5) | (0x1f<<21) | \
		(modecode<<0) | (0<<10))

/* Broadcast to all RTs, to a specific subaddress (BC-to-RTs) */
#define GR1553BC_BC_BC2RT(subadr, word_count) \
		((0x1f<<11) | (subadr<<5) | (0x1f<<21) | \
		(0<<10) | \
		((word_count>=32) ? 0 : word_count))

/* Request RT to broadcast to all RTs, to a specific subaddress (RT-to-RTs) */
#define GR1553BC_BC_RT2RT(tx_rtadr, tx_subadr, rx_subadr, word_count) \
		((0x1f<<11) | (rx_subadr<<5) | \
		(tx_rtadr<<21) | (tx_subadr<<16) | \
		(0<<10) | \
		((word_count>=32) ? 0 : word_count))

/* Broadcast mode command without data (BC-to-RTs) 
 * Mode code: 1,3,4,5,6,7 or 8
 */
#define GR1553BC_BC_MC_NODATA(modecode) \
		((0x1f<<11) | (0x1f<<5) | (0x1f<<21) | \
		((modecode)<<0) | (1<<10))

/* Broadcast mode command with 4 byte data (BC-to-RTs)
 * Mode code: 17, 20 or 21
 */
#define GR1553BC_BC_MC_BC2RT(modecode) \
		((0x1f<<11) | (0x1f<<5) | (0x1f<<21) | \
		((modecode)<<0) | (0<<10))


/***** Macros to create BC options (options) for gr1553bc_slot_transfer() *****/

/* Dummy (BC does no bus trasactions) */
#define GR1553BC_OPT_DUMMY (1<<1)

/* Retry modes */
#define GR1553BC_RETRY_SAME	0x0	/* Retry on the same bus only */
#define GR1553BC_RETRY_ALTER	0x1	/* Retry alternating on both busses */
#define GR1553BC_RETRY_ATTEMPT	0x2	/* Many attepts first on original 
					 * bus then on other bus */
/* Number of retires supported */
#define GR1553BC_RETRY_CNT_MAX	6

/* Dummy bit: No transfer
 * Bus bit: 0=A, 1=B
 * Exttrig bit: Wait for external trigger (used for timesync)
 * Exclusive bit: 1=Don't allow other messages in this time slot.
 */
#define GR1553BC_OPTIONS(dummy, exttrig, exclusive, retrymode, nretry, bus) \
		((((exttrig) & 0x1) << 30) | (((exclusive) & 0x1) << 29) | \
		((retrymode) << 23) | ((nretry) << 20) | \
		((bus) & 1) | (((dummy) & 0x1) << 1))

#define GR1553BC_OPTIONS_BUSA GR1553BC_OPTIONS(0,0,0,GR1553BC_RETRY_SAME,0,0)
#define GR1553BC_OPTIONS_BUSB GR1553BC_OPTIONS(0,0,0,GR1553BC_RETRY_SAME,0,1)
#define GR1553BC_OPTIONS_BUSA_DUM GR1553BC_OPTIONS(1,0,0,GR1553BC_RETRY_SAME,0,0)
#define GR1553BC_OPTIONS_BUSB_DUM GR1553BC_OPTIONS(1,0,0,GR1553BC_RETRY_SAME,0,1)

/* Show parts of a list - this is for debugging only */
extern void gr1553bc_show_list(struct gr1553bc_list *list, int options);

#ifdef __cplusplus
}
#endif

#endif /* __GR1553BC_LIST_H__ */
