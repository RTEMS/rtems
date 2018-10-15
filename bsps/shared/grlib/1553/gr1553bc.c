/*  GR1553B BC driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <string.h>
#include <rtems.h>
#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/gr1553b.h>
#include <grlib/gr1553bc.h>

#include <grlib/grlib_impl.h>

#define GR1553BC_WRITE_MEM(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BC_READ_MEM(adr) (*(volatile uint32_t *)(adr))

#define GR1553BC_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553BC_READ_REG(adr) (*(volatile uint32_t *)(adr))

/* Needed by list for data pinter and BD translation */
struct gr1553bc_priv {
	struct drvmgr_dev **pdev;
	struct gr1553b_regs *regs;
	struct gr1553bc_list *list;
	struct gr1553bc_list *alist;
	int started;
	SPIN_DECLARE(devlock);

	/* IRQ log management */
	void *irq_log_p;
	uint32_t *irq_log_base;
	uint32_t *irq_log_curr;
	uint32_t *irq_log_end;
	uint32_t *irq_log_base_hw;

	/* Standard IRQ handler function */
	bcirq_func_t irq_func;
	void *irq_data;
};


/*************** LIST HANDLING ROUTINES ***************/

/* This marks that the jump is a jump to next Minor.
 * It is important that it sets one of the two LSB
 * so that we can separate it from a JUMP-IRQ function,
 * function pointers must be aligned to 4bytes.
 *
 * This marker is used to optimize the INDICATION process,
 * from a descriptor pointer we can step to next Jump that
 * has this MARKER set, then we know that the MID is stored
 * there.
 *
 * The marker is limited to 1 byte.
 */
#define NEXT_MINOR_MARKER 0x01

/* To separate ASYNC list from SYNC list we mark them differently, but with
 * LSB always set. This can be used to get the list the descriptor is a part
 * of.
 */
#define NEXT_MINOR_MARKER_ASYNC 0x80

struct gr1553bc_list_cfg gr1553bc_def_cfg =
{
	.rt_timeout =
		{
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20, 20,
			20, 20, 20
		},
	.bc_timeout = 30,
	.tropt_irq_on_err = 0,
	.tropt_pause_on_err = 0,
	.async_list = 0,
};

int gr1553bc_list_alloc(struct gr1553bc_list **list, int max_major)
{
	size_t size;
	struct gr1553bc_list *l;

	size = sizeof(*l) + max_major * sizeof(void *);
	l = grlib_calloc(1, size);
	if ( l == NULL )
		return -1;

	l->major_cnt = max_major;
	*list = l;

	/* Set default options:
	 *  - RT timeout tolerance 20us
	 *  - Global transfer options used when generating transfer descriptors
	 *  - No BC device, note that this only works when no translation is
	 *    required
	 */
	if ( gr1553bc_list_config(l, &gr1553bc_def_cfg, NULL) ) {
		free(l);
		return -1;
	}

	return 0;
}

void gr1553bc_list_free(struct gr1553bc_list *list)
{
	gr1553bc_list_table_free(list);
	free(list);
}

int gr1553bc_list_config
	(
	struct gr1553bc_list *list,
	struct gr1553bc_list_cfg *cfg,
	void *bc
	)
{
	int timeout, i, tropts;

	/* RT Time Tolerances */
	for (i=0; i<31; i++) {
		/* 0=14us, 1=18us ... 0xf=74us
		 * round upwards: 15us will be 18us
		 */
		timeout = ((cfg->rt_timeout[i] + 1)  - 14) / 4;
		if ( (timeout > 0xf) || (timeout < 0) )
			return -1;
		list->rt_timeout[i] = timeout;
	}
	timeout = ((cfg->bc_timeout + 1) - 14) / 4;
	if ( timeout > 0xf )
		return -1;
	list->rt_timeout[i] = timeout;

	/* Transfer descriptor generation options */
	tropts = 0;
	if ( cfg->tropt_irq_on_err )
		tropts |= 1<<28;
	if ( cfg->tropt_pause_on_err )
		tropts |= 1<<26;
	list->tropts = tropts;

	list->async_list = cfg->async_list;
	list->bc = bc;

	return 0;
}

void gr1553bc_list_link_major(
	struct gr1553bc_major *major,
	struct gr1553bc_major *next
	)
{
	if ( major ) {
		major->next = next;
		if ( next ) {
			major->minors[major->cfg->minor_cnt-1]->next =
				next->minors[0];
		} else {
			major->minors[major->cfg->minor_cnt-1]->next = NULL;
		}
	}
}

int gr1553bc_list_set_major(
	struct gr1553bc_list *list,
	struct gr1553bc_major *major,
	int no)
{
	struct gr1553bc_major *prev, *next;

	if ( no >= list->major_cnt )
		return -1;

	list->majors[no] = major;

	/* Link previous Major frame with this one */
	if ( no > 0 ) {
		prev = list->majors[no-1];
	} else {
		/* First Major is linked with last major */
		prev = list->majors[list->major_cnt-1];
	}

	/* Link to next Major if not the last one and if there is
	 * a next major
	 */
	if ( no == list->major_cnt-1 ) {
		/* The last major, assume that it is connected with the first */
		next = list->majors[0];
	} else {
		next = list->majors[no+1];
	}

	/* Link previous frame to jump into this */
	gr1553bc_list_link_major(prev, major);

	/* Link This frame to jump into the next */
	gr1553bc_list_link_major(major, next);

	return 0;
}

/* Translate Descriptor address from CPU-address to Hardware Address */
static inline union gr1553bc_bd *gr1553bc_bd_cpu2hw
	(
	struct gr1553bc_list *list,
	union gr1553bc_bd *bd
	)
{
	return (union gr1553bc_bd *)(((unsigned int)bd - list->table_cpu) +
		list->table_hw);
}

/* Translate Descriptor address from HW-address to CPU Address */
static inline union gr1553bc_bd *gr1553bc_bd_hw2cpu
	(
	struct gr1553bc_list *list,
	union gr1553bc_bd *bd
	)
{
	return (union gr1553bc_bd *)(((unsigned int)bd - list->table_hw) +
		list->table_cpu);
}

int gr1553bc_minor_table_size(struct gr1553bc_minor *minor)
{
	struct gr1553bc_minor_cfg *mincfg = minor->cfg;
	int slot_cnt;

	/* SLOTS + JUMP */
	slot_cnt = mincfg->slot_cnt + 1;
	if ( mincfg->timeslot ) {
		/* time management requires 1 extra slot */
		slot_cnt++;
	}

	return slot_cnt * GR1553BC_BD_SIZE;
}

int gr1553bc_list_table_size(struct gr1553bc_list *list)
{
	struct gr1553bc_major *major;
	int i, j, minor_cnt, size;

	size = 0;
	for (i=0; i<list->major_cnt; i++) {
		major = list->majors[i];
		minor_cnt = major->cfg->minor_cnt;
		for (j=0; j<minor_cnt; j++) {
			/* 128-bit Alignment required by HW */
			size += (GR1553BC_BD_ALIGN -
				(size & (GR1553BC_BD_ALIGN-1))) &
				~(GR1553BC_BD_ALIGN-1);

			/* Size required by descriptors */
			size += gr1553bc_minor_table_size(major->minors[j]);
		}
	}

	return size;
}

int gr1553bc_list_table_alloc
	(
	struct gr1553bc_list *list,
	void *bdtab_custom
	)
{
	struct gr1553bc_major *major;
	int i, j, minor_cnt, size;
	unsigned int table;
	struct gr1553bc_priv *bcpriv = list->bc;
	int retval = 0;

	/* Free previous allocated descriptor table */
	gr1553bc_list_table_free(list);

	/* Remember user's settings for uninitialization */
	list->_table_custom = bdtab_custom;

	/* Get Size required for descriptors */
	size = gr1553bc_list_table_size(list);

	if ((unsigned int)bdtab_custom & 0x1) {
		/* Address given in Hardware accessible address, we
		 * convert it into CPU-accessible address.
		 */
		list->_table = (void*)((unsigned int)bdtab_custom & ~0x1);
		list->table_hw = (unsigned int)list->_table;
		drvmgr_translate_check(
			*bcpriv->pdev,
			DMAMEM_TO_CPU,
			(void *)list->table_hw,
			(void **)&list->table_cpu,
			size);
	} else {
		if (bdtab_custom == NULL) {
			/* Allocate descriptors */
			list->_table = grlib_malloc(size + (GR1553BC_BD_ALIGN-1));
			if ( list->_table == NULL ) {
				retval = -1;
				goto err;
			}
			/* 128-bit Alignment required by HW */
			list->table_cpu =
				(((unsigned int)list->_table + (GR1553BC_BD_ALIGN-1)) &
				~(GR1553BC_BD_ALIGN-1));
		} else {
			/* Custom address, given in CPU-accessible address */
			list->_table = bdtab_custom;
			list->table_cpu = (unsigned int)list->_table;
		}

		/* We got CPU accessible descriptor table address, now we
		 * translate that into an address that the Hardware can
		 * understand
		 */
		if (bcpriv) {
			drvmgr_translate_check(
				*bcpriv->pdev,
				CPUMEM_TO_DMA,
				(void *)list->table_cpu,
				(void **)&list->table_hw,
				size
				);
		} else {
			list->table_hw = list->table_cpu;
		}
	}

	/* Verify alignment */
	if (list->table_hw & (GR1553BC_BD_ALIGN-1)) {
		retval = -2;
		goto err;
	}

	/* Write End-Of-List all over the descriptor table here,
	 * For debugging/safety?
	 */

	/* Assign descriptors to all minor frames. The addresses is
	 * CPU-accessible addresses.
	 */
	table = list->table_cpu;
	for (i=0; i<list->major_cnt; i++) {
		major = list->majors[i];
		minor_cnt = major->cfg->minor_cnt;
		for (j=0; j<minor_cnt; j++) {
			/* 128-bit Alignment required by HW */
			table = (table + (GR1553BC_BD_ALIGN-1)) &
				~(GR1553BC_BD_ALIGN-1);
			major->minors[j]->bds = (union gr1553bc_bd *)table;

			/* Calc size required by descriptors */
			table += gr1553bc_minor_table_size(major->minors[j]);
		}
	}
err:
	if (retval) {
		if (list->_table_custom == NULL && list->_table) {
			free(list->_table);
		}
		list->table_hw = 0;
		list->table_cpu = 0;
		list->_table = NULL;
	}
	return retval;
}

void gr1553bc_list_table_free(struct gr1553bc_list *list)
{
	if ( (list->_table_custom == NULL) && list->_table ) {
		free(list->_table);
	}
	list->_table = NULL;
	list->_table_custom = NULL;
	list->table_cpu = 0;
	list->table_hw = 0;
}

/* Init descriptor table provided by each minor frame,
 * we link them together using unconditional JUMP.
 */
int gr1553bc_list_table_build(struct gr1553bc_list *list)
{
	struct gr1553bc_major *major;
	struct gr1553bc_minor *minor;
	struct gr1553bc_minor_cfg *mincfg;
	int i, j, k, minor_cnt, marker;
	union gr1553bc_bd *bds, *hwbd;

	marker = NEXT_MINOR_MARKER;
	if ( list->async_list )
		marker |= NEXT_MINOR_MARKER_ASYNC;

	/* Create Major linking */
	for (i=0; i<list->major_cnt; i++) {
		major = list->majors[i];
		minor_cnt = major->cfg->minor_cnt;
		for (j=0; j<minor_cnt; j++) {
			minor = major->minors[j];
			mincfg = minor->cfg;
			bds = minor->bds;

			/* BD[0..SLOTCNT-1] = message slots
			 * BD[SLOTCNT+0] = END
			 * BD[SLOTCNT+1] = JUMP
			 *
			 * or if no optional time slot handling:
			 *
			 * BD[0..SLOTCNT-1] = message slots
			 * BD[SLOTCNT] = JUMP
			 */

			/* BD[0..SLOTCNT-1] */
			for (k=0; k<mincfg->slot_cnt; k++) {
				gr1553bc_bd_tr_init(
					&bds[k].tr,
					GR1553BC_TR_DUMMY_0,
					GR1553BC_TR_DUMMY_1,
					0,
					0);
			}

			/* BD[SLOTCNT] (OPTIONAL)
			 * If a minor frame is configured to be executed in
			 * certain time (given a time slot), this descriptor
			 * sums up all unused time. The time slot is
			 * decremented when messages are inserted into the
			 * minor frame and increased when messages are removed.
			 */
			if ( mincfg->timeslot > 0 ) {
				gr1553bc_bd_tr_init(
					&bds[k].tr,
					GR1553BC_TR_DUMMY_0 | (mincfg->timeslot >> 2),
					GR1553BC_TR_DUMMY_1,
					0,
					0);
				k++;
			}

			/* Last descriptor is a jump to next minor frame, to a
			 * synchronization point. If chain ends here, the list
			 * is marked with a "end-of-list" marker.
			 *
			 */
			if ( minor->next ) {
				/* Translate CPU address of BD into HW address */
				hwbd = gr1553bc_bd_cpu2hw(
					list,
					&minor->next->bds[0]
					);
				gr1553bc_bd_init(
					&bds[k],
					0xf,
					GR1553BC_UNCOND_JMP,
					(uint32_t)hwbd,
					((GR1553BC_ID(i,j,k) << 8) | marker),
					0
					);
			} else {
				gr1553bc_bd_init(
					&bds[k],
					0xf,
					GR1553BC_TR_EOL,
					0,
					((GR1553BC_ID(i,j,k) << 8) | marker),
					0);
			}
		}
	}

	return 0;
}

void gr1553bc_bd_init(
	union gr1553bc_bd *bd,
	unsigned int flags,
	uint32_t word0,
	uint32_t word1,
	uint32_t word2,
	uint32_t word3
	)
{
	struct gr1553bc_bd_raw *raw = &bd->raw;

	if ( flags & 0x1 ) {
		if ( (flags & KEEP_TIMESLOT) &&
		     ((word0 & GR1553BC_BD_TYPE) == 0) ) {
			/* Don't touch timeslot previously allocated */
			word0 &= ~GR1553BC_TR_TIME;
			word0 |= GR1553BC_READ_MEM(&raw->words[0]) &
					GR1553BC_TR_TIME;
		}
		GR1553BC_WRITE_MEM(&raw->words[0], word0);
	}
	if ( flags & 0x2 )
		GR1553BC_WRITE_MEM(&raw->words[1], word1);
	if ( flags & 0x4 )
		GR1553BC_WRITE_MEM(&raw->words[2], word2);
	if ( flags & 0x8 )
		GR1553BC_WRITE_MEM(&raw->words[3], word3);
}

/* Alloc a Major frame according to the configuration structure */
int gr1553bc_major_alloc_skel
	(
	struct gr1553bc_major **major,
	struct gr1553bc_major_cfg *cfg
	)
{
	struct gr1553bc_major *maj;
	struct gr1553bc_minor *minor;
	size_t size;
	int i;

	if ( (cfg == NULL) || (major == NULL) || (cfg->minor_cnt <= 0) )
		return -1;

	/* Allocate Major Frame description, but no descriptors */
	size = sizeof(*maj) + cfg->minor_cnt *
		(sizeof(*minor) + sizeof(void *));
	maj = grlib_malloc(size);
	if ( maj == NULL )
		return -1;

	maj->cfg = cfg;
	maj->next = NULL;

	/* Create links between minor frames, and from minor frames
	 * to configuration structure.
	 */
	minor = (struct gr1553bc_minor *)&maj->minors[cfg->minor_cnt];
	for (i=0; i<cfg->minor_cnt; i++, minor++) {
		maj->minors[i] = minor;
		minor->next = minor + 1;
		minor->cfg = &cfg->minor_cfgs[i];
		minor->alloc = 0;
		minor->bds = NULL;
	}
	/* last Minor should point to next Major frame's first minor,
	 * we do that somewhere else.
	 */
	(minor - 1)->next = NULL;

	*major = maj;

	return 0;
}

struct gr1553bc_major *gr1553bc_major_from_id
	(
	struct gr1553bc_list *list,
	int mid
	)
{
	int major_no;

	/* Find Minor Frame from MID */
	major_no = GR1553BC_MAJID_FROM_ID(mid);

	if ( major_no >= list->major_cnt )
		return NULL;
	return list->majors[major_no];
}

struct gr1553bc_minor *gr1553bc_minor_from_id
	(
	struct gr1553bc_list *list,
	int mid
	)
{
	int minor_no;
	struct gr1553bc_major *major;

	/* Get Major from ID */
	major = gr1553bc_major_from_id(list, mid);
	if ( major == NULL )
		return NULL;

	/* Find Minor Frame from MID */
	minor_no = GR1553BC_MINID_FROM_ID(mid);

	if ( minor_no >= major->cfg->minor_cnt )
		return NULL;
	return major->minors[minor_no];
}

union gr1553bc_bd *gr1553bc_slot_bd
	(
	struct gr1553bc_list *list,
	int mid
	)
{
	struct gr1553bc_minor *minor;
	int slot_no;

	/*** look up BD ***/

	/* Get minor */
	minor = gr1553bc_minor_from_id(list, mid);
	if ( minor == NULL )
		return NULL;

	/* Get Slot */
	slot_no = GR1553BC_SLOTID_FROM_ID(mid);
	if ( slot_no >= 0xff )
		slot_no = 0;

	/* Get BD address */
	return &minor->bds[slot_no];
}

static int gr1553bc_minor_first_avail(struct gr1553bc_minor *minor)
{
	int slot_num;
	uint32_t alloc;

	alloc = minor->alloc;
	if ( alloc == 0xffffffff ) {
		/* No free */
		return -1;
	}
	slot_num = 0;
	while ( alloc & 1 ) {
		alloc = alloc >> 1;
		slot_num++;
	}
	if ( slot_num >= minor->cfg->slot_cnt ) {
		/* no free */
		return -1;
	}
	return slot_num;
}

int gr1553bc_slot_alloc(
	struct gr1553bc_list *list,
	int *mid,
	int timeslot,
	union gr1553bc_bd **bd
	)
{
	struct gr1553bc_minor *minor = gr1553bc_minor_from_id(list, *mid);

	return gr1553bc_slot_alloc2(minor, mid, timeslot, bd);
}

/* Same as gr1553bc_slot_alloc but identifies a minor instead of list.
 * The major/minor part of MID is ignored.
 */
int gr1553bc_slot_alloc2(
	struct gr1553bc_minor *minor,
	int *mid,
	int timeslot,
	union gr1553bc_bd **bd
	)
{
	int slot_no;
	uint32_t set0;
	int timefree;
	struct gr1553bc_bd_tr *trbd;
	struct gr1553bc_minor_cfg *mincfg;

	if ( minor == NULL )
		return -1;

	mincfg = minor->cfg;

	/* Find first free slot if not a certain slot is requested */
	slot_no = GR1553BC_SLOTID_FROM_ID(*mid);
	if ( slot_no == 0xff ) {
		slot_no = gr1553bc_minor_first_avail(minor);
		if ( slot_no < 0 )
			return -1;
	} else {
		/* Allocate a certain slot, check that it is free */
		if ( slot_no >= mincfg->slot_cnt )
			return -1;
		if ( (1<<slot_no) & minor->alloc )
			return -1;
	}

	/* Ok, we got our slot. Lets allocate time for slot if requested by user
	 * and time management is enabled for this Minor Frame.
	 */
	if ( timeslot > 0 ) {
		/* Make timeslot on a 4us boundary (time resolution of core) */
		timeslot = (timeslot + 0x3) >> 2;

		if ( mincfg->timeslot ) {
			/* Subtract requested time from free time */
			trbd = &minor->bds[mincfg->slot_cnt].tr;
			set0 = GR1553BC_READ_MEM(&trbd->settings[0]);
			timefree = set0 & GR1553BC_TR_TIME;
			if ( timefree < timeslot ) {
				/* Not enough time left to schedule slot in minor */
				return -1;
			}
			/* Store back the time left */
			timefree -= timeslot;
			set0 = (set0 & ~GR1553BC_TR_TIME) | timefree;
			GR1553BC_WRITE_MEM(&trbd->settings[0], set0);
			/* Note: at the moment the minor frame can be executed faster
			 *       than expected, we hurry up writing requested
			 *       descriptor.
			 */
		}
	}

	/* Make the allocated descriptor be an empty slot with the
	 * timeslot requested.
	 */
	trbd = &minor->bds[slot_no].tr;
	gr1553bc_bd_tr_init(
		trbd,
		GR1553BC_TR_DUMMY_0 | timeslot,
		GR1553BC_TR_DUMMY_1,
		0,
		0);

	/* Allocate slot */
	minor->alloc |= 1<<slot_no;

	if ( bd )
		*bd = (union gr1553bc_bd *)trbd;
	*mid = GR1553BC_ID_SET_SLOT(*mid, slot_no);

	return 0;
}

/* Return time slot freed (if time is managed by driver), negative on error */
int gr1553bc_slot_free(struct gr1553bc_list *list, int mid)
{
	struct gr1553bc_minor *minor = gr1553bc_minor_from_id(list, mid);

	return gr1553bc_slot_free2(minor, mid);
}

/* Return time slot freed (if time is managed by driver), negative on error */
int gr1553bc_slot_free2(struct gr1553bc_minor *minor, int mid)
{
	union gr1553bc_bd *bd;
	struct gr1553bc_bd_tr *endbd;
	struct gr1553bc_minor_cfg *mincfg;
	int slot_no, timeslot, timefree;
	uint32_t word0, set0;

	if ( minor == NULL )
		return -1;

	slot_no = GR1553BC_SLOTID_FROM_ID(mid);

	if ( (minor->alloc & (1<<slot_no)) == 0 )
		return -1;

	bd = &minor->bds[slot_no];

	/* If the driver handles time for this minor frame, return
	 * time if previuosly requested.
	 */
	timeslot = 0;
	mincfg = minor->cfg;
	if ( mincfg->timeslot > 0 ) {
		/* Find out if message slot had time allocated */
		word0 = GR1553BC_READ_MEM(&bd->raw.words[0]);
		if ( word0 & GR1553BC_BD_TYPE ) {
			/* Condition ==> no time slot allocated */
		} else {
			/* Transfer descriptor, may have time slot */
			timeslot = word0 & GR1553BC_TR_TIME;
			if ( timeslot > 0 ) {
				/* Return previously allocated time to END
				 * TIME descriptor.
				 */
				endbd = &minor->bds[mincfg->slot_cnt].tr;
				set0 = GR1553BC_READ_MEM(&endbd->settings[0]);
				timefree = set0 & GR1553BC_TR_TIME;
				timefree += timeslot;
				set0 = (set0 & ~GR1553BC_TR_TIME) | timefree;
				GR1553BC_WRITE_MEM(&endbd->settings[0], set0);
				/* Note: at the moment the minor frame can be
				 *       executed slower than expected, the
				 *       timeslot is at two locations.
				 */
			}
		}
	}

	/* Make slot an empty message */
	gr1553bc_bd_tr_init(
		&bd->tr,
		GR1553BC_TR_DUMMY_0,
		GR1553BC_TR_DUMMY_1,
		0,
		0);

	/* unallocate descriptor */
	minor->alloc &= ~(1<<slot_no);

	/* Return time freed in microseconds */
	return timeslot << 2;
}

int gr1553bc_list_freetime(struct gr1553bc_list *list, int mid)
{
	struct gr1553bc_minor *minor = gr1553bc_minor_from_id(list, mid);

	return gr1553bc_minor_freetime(minor);
}

int gr1553bc_minor_freetime(struct gr1553bc_minor *minor)
{
	struct gr1553bc_bd_tr *endbd;
	struct gr1553bc_minor_cfg *mincfg;
	int timefree;
	uint32_t set0;

	if ( minor == NULL )
		return -1;

	/* If the driver handles time for this minor frame, return
	 * time if previuosly requested.
	 */
	timefree = 0;
	mincfg = minor->cfg;
	if ( mincfg->timeslot > 0 ) {
		/* Return previously allocated time to END
		 * TIME descriptor.
		 */
		endbd = &minor->bds[mincfg->slot_cnt].tr;
		set0 = GR1553BC_READ_MEM(&endbd->settings[0]);
		timefree = (set0 & GR1553BC_TR_TIME) << 2;
	}

	/* Return time freed */
	return timefree;
}

int gr1553bc_slot_raw
	(
	struct gr1553bc_list *list,
	int mid,
	unsigned int flags,
	uint32_t word0,
	uint32_t word1,
	uint32_t word2,
	uint32_t word3
	)
{
	struct gr1553bc_minor *minor;
	union gr1553bc_bd *bd;
	int slot_no;

	minor = gr1553bc_minor_from_id(list, mid);
	if ( minor == NULL )
		return -1;

	/* Get Slot */
	slot_no = GR1553BC_SLOTID_FROM_ID(mid);
	if ( slot_no >= minor->cfg->slot_cnt ) {
		return -1;
	}

	/* Get descriptor */
	bd = &minor->bds[slot_no];

	/* Build empty descriptor. */
	gr1553bc_bd_init(
		bd,
		flags,
		word0,
		word1,
		word2,
		word3);

	return 0;
}

/* Create unconditional IRQ customly defined location
 * The IRQ is disabled, enable it with gr1553bc_slot_irq_enable().
 */
int gr1553bc_slot_irq_prepare
	(
	struct gr1553bc_list *list,
	int mid,
	bcirq_func_t func,
	void *data
	)
{
	union gr1553bc_bd *bd;
	int slot_no, to_mid;

	/* Build unconditional IRQ descriptor. The padding is used
	 * for identifying the MINOR frame and function and custom data.
	 *
	 * The IRQ is disabled at first, a unconditional jump to next
	 * descriptor in table.
	 */

	/* Get BD address of jump destination */
	slot_no = GR1553BC_SLOTID_FROM_ID(mid);
	to_mid = GR1553BC_ID_SET_SLOT(mid, slot_no + 1);
	bd = gr1553bc_slot_bd(list, to_mid);
	if ( bd == NULL )
		return -1;
	bd = gr1553bc_bd_cpu2hw(list, bd);

	return gr1553bc_slot_raw(
		list,
		mid,
		0xF,
		GR1553BC_UNCOND_JMP,
		(uint32_t)bd,
		(uint32_t)func,
		(uint32_t)data
		);
}

/* Enable previously prepared unconditional IRQ */
int gr1553bc_slot_irq_enable(struct gr1553bc_list *list, int mid)
{
	/* Leave word1..3 untouched:
	 *  1. Unconditional Jump address
	 *  2. Function
	 *  3. Custom Data
	 *
	 * Since only one bit is changed in word0 (Condition word),
	 * no hardware/software races will exist ==> it is safe
	 * to enable/disable IRQ at any time independent of where
	 * hardware is in table.
	 */
	return gr1553bc_slot_raw(
		list,
		mid,
		0x1,	/* change only WORD0 */
		GR1553BC_UNCOND_IRQ,
		0,
		0,
		0);
}

/* Disable unconditional IRQ point, changed to unconditional JUMP
 * to descriptor following.
 * After disabling it it can be enabled again, or freed.
 */
int gr1553bc_slot_irq_disable(struct gr1553bc_list *list, int mid)
{
	return gr1553bc_slot_raw(
		list,
		mid,
		0x1,	/* change only WORD0, JUMP address already set */
		GR1553BC_UNCOND_JMP,
		0,
		0,
		0);
}

int gr1553bc_slot_empty(struct gr1553bc_list *list, int mid)
{
	return gr1553bc_slot_raw(
		list,
		mid,
		0xF | KEEP_TIMESLOT,
		GR1553BC_TR_DUMMY_0,
		GR1553BC_TR_DUMMY_1,
		0,
		0);
}

int gr1553bc_slot_exttrig(struct gr1553bc_list *list, int mid)
{
	return gr1553bc_slot_raw(
		list,
		mid,
		0xF | KEEP_TIMESLOT,
		GR1553BC_TR_DUMMY_0 | GR1553BC_TR_EXTTRIG,
		GR1553BC_TR_DUMMY_1,
		0,
		0);
}

int gr1553bc_slot_jump
	(
	struct gr1553bc_list *list,
	int mid,
	uint32_t condition,
	int to_mid
	)
{
	union gr1553bc_bd *bd;

	/* Get BD address */
	bd = gr1553bc_slot_bd(list, to_mid);
	if ( bd == NULL )
		return -1;
	/* Convert into an address that the HW understand */
	bd = gr1553bc_bd_cpu2hw(list, bd);

	return gr1553bc_slot_raw(
		list,
		mid,
		0xF,
		condition,
		(uint32_t)bd,
		0,
		0);
}

int gr1553bc_slot_transfer(
	struct gr1553bc_list *list,
	int mid,
	int options,
	int tt,
	uint16_t *dptr)
{
	uint32_t set0, set1;
	union gr1553bc_bd *bd;
	int rx_rtadr, tx_rtadr, timeout;

	/* Get BD address */
	bd = gr1553bc_slot_bd(list, mid);
	if ( bd == NULL )
		return -1;

	/* Translate Data pointer from CPU-local to 1553-core accessible
	 * address if user wants that. This may be useful for AMBA-over-PCI
	 * cores.
	 */
	if ( (unsigned int)dptr & 0x1 ) {
		struct gr1553bc_priv *bcpriv = list->bc;

		drvmgr_translate(
			*bcpriv->pdev,
			CPUMEM_TO_DMA,
			(void *)((unsigned int)dptr & ~0x1),
			(void **)&dptr);
	}

	/* It is assumed that the descriptor has already been initialized
	 * as a empty slot (Dummy bit set), so to avoid races the dummy
	 * bit is cleared last.
	 *
	 * If we knew that the write would do a burst (for example over SpW)
	 * it would be safe to write in order.
	 */

	/* Preserve timeslot */
	set0 = GR1553BC_READ_MEM(&bd->tr.settings[0]);
	set0 &= GR1553BC_TR_TIME;
	set0 |= options & 0x61f00000;
	set0 |= list->tropts; /* Global options */

	/* Set transfer type, bus and let RT tolerance table descide
	 * responce tolerance.
	 *
	 * If a destination address is specified the longest timeout
	 * tolerance is taken.
	 */
	rx_rtadr = (tt >> 22) & 0x1f;
	tx_rtadr = (tt >> 12) & 0x1f;
	if ( (tx_rtadr != 0x1f) &&
	     (list->rt_timeout[rx_rtadr] < list->rt_timeout[tx_rtadr]) ) {
		timeout = list->rt_timeout[tx_rtadr];
	} else {
		timeout = list->rt_timeout[rx_rtadr];
	}
	set1 = ((timeout & 0xf) << 27) | (tt & 0x27ffffff) | ((options & 0x3)<<30);

	GR1553BC_WRITE_MEM(&bd->tr.settings[0], set0);
	GR1553BC_WRITE_MEM(&bd->tr.dptr, (uint32_t)dptr);
	/* Write UNUSED BIT, when cleared it Indicates that BC has written it */
	GR1553BC_WRITE_MEM(&bd->tr.status, 0x80000000);
	GR1553BC_WRITE_MEM(&bd->tr.settings[1], set1);

	return 0;
}

int gr1553bc_slot_update
	(
	struct gr1553bc_list *list,
	int mid,
	uint16_t *dptr,
	unsigned int *stat
	)
{
	union gr1553bc_bd *bd;
	unsigned int status;
	unsigned int dataptr = (unsigned int)dptr;

	/* Get BD address */
	bd = gr1553bc_slot_bd(list, mid);
	if ( bd == NULL )
		return -1;

	/* Write new Data Pointer if needed */
	if ( dataptr ) {
		struct gr1553bc_priv *bcpriv = list->bc;

		/* Translate Data pointer from CPU-local to 1553-core accessible
		 * address if user wants that. This may be useful for AMBA-over-PCI
		 * cores.
		 */
		if ( dataptr & 0x1 ) {
			drvmgr_translate(
				*bcpriv->pdev,
				CPUMEM_TO_DMA,
				(void *)(dataptr & ~0x1),
				(void **)&dptr
				);
		}

		/* Update Data Pointer */
		GR1553BC_WRITE_MEM(&bd->tr.dptr, dataptr);
	}

	/* Get status of transfer descriptor */
	if ( stat ) {
		status = *stat;
		*stat = GR1553BC_READ_MEM(&bd->tr.status);
		if ( status ) {
			/* Clear status fields user selects, then
			 * or bit31 if user wants that. The bit31
			 * may be used to indicate if the BC has
			 * performed the access.
			 */
			status = (*stat & (status & 0xffffff)) |
				 (status & (1<<31));
			GR1553BC_WRITE_MEM(&bd->tr.status, status);
		}
	}

	return 0;
}

int gr1553bc_slot_dummy(
	struct gr1553bc_list *list,
	int mid,
	unsigned int *dummy)
{
	union gr1553bc_bd *bd;
	unsigned int set1, new_set1;

	/* Get BD address */
	bd = gr1553bc_slot_bd(list, mid);
	if ( bd == NULL )
		return -1;
	/* Update the Dummy Bit */
	set1 = GR1553BC_READ_MEM(&bd->tr.settings[1]);
	new_set1 = (set1 & ~GR1553BC_TR_DUMMY_1) | (*dummy & GR1553BC_TR_DUMMY_1);
	GR1553BC_WRITE_MEM(&bd->tr.settings[1], new_set1);

	*dummy = set1;

	return 0;
}

/* Find MID from Descriptor pointer */
int gr1553bc_mid_from_bd(
	union gr1553bc_bd *bd,
	int *mid,
	int *async
	)
{
	int i, bdmid, slot_no;
	uint32_t word0, word2;

	/* Find Jump to next Minor Frame or End-Of-List,
	 * at those locations we have stored a MID
	 *
	 * GR1553BC_SLOT_MAX+2 = Worst case, BD is max distance from jump
	 *                       descriptor. 2=END and Jump descriptors.
	 */
	for (i=0; i<GR1553BC_SLOT_MAX+2; i++) {
		word0 = GR1553BC_READ_MEM(&bd->raw.words[0]);
		if ( word0 & GR1553BC_BD_TYPE ) {
			if ( word0 == GR1553BC_UNCOND_JMP ) {
				/* May be a unconditional IRQ set by user. In
				 * that case the function is stored in WORD3,
				 * functions must be aligned to 4 byte boudary.
				 */
				word2 = GR1553BC_READ_MEM(&bd->raw.words[2]);
				if ( word2 & NEXT_MINOR_MARKER ) {
					goto found_mid;
				}
			} else if ( word0 == GR1553BC_TR_EOL ) {
				/* End-Of-List, does contain a MID */
				word2 = GR1553BC_READ_MEM(&bd->raw.words[2]);
				goto found_mid;
			}
		}
		bd++;
	}

	return -1;

found_mid:
	/* Get MID of JUMP descriptor */
	bdmid = word2 >> 8;
	/* Subtract distance from JUMP descriptor to find MID
	 * of requested BD.
	 */
	slot_no = GR1553BC_SLOTID_FROM_ID(bdmid);
	slot_no -= i;
	bdmid = GR1553BC_ID_SET_SLOT(bdmid, slot_no);

	if ( mid )
		*mid = bdmid;

	/* Determine which list BD belongs to: async or sync */
	if ( async )
		*async = word2 & NEXT_MINOR_MARKER_ASYNC;

	return 0;
}

/*************** END OF LIST HANDLING ROUTINES ***************/

/*************** DEVICE HANDLING ROUTINES ***************/

void gr1553bc_device_init(struct gr1553bc_priv *priv);
void gr1553bc_device_uninit(struct gr1553bc_priv *priv);
void gr1553bc_isr(void *data);

/*** GR1553BC driver ***/

void gr1553bc_register(void)
{
	/* The BC driver rely on the GR1553B Driver */
	gr1553_register();
}

static void gr1553bc_isr_std(union gr1553bc_bd *bd, void *data)
{
	/* Do nothing */
}

/* Take a GR1553BC hardware device identified by minor.
 * A pointer is returned that is used internally by the GR1553BC
 * driver, it is used as an input paramter 'bc' to all other
 * functions that manipulate the hardware.
 */
void *gr1553bc_open(int minor)
{
	struct drvmgr_dev **pdev = NULL;
	struct gr1553bc_priv *priv = NULL;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;
	void *irq_log_p = NULL;

	/* Allocate requested device */
	pdev = gr1553_bc_open(minor);
	if ( pdev == NULL )
		goto fail;

	irq_log_p = grlib_malloc(GR1553BC_IRQLOG_SIZE*2);
	if ( irq_log_p == NULL )
		goto fail;

	priv = grlib_calloc(1, sizeof(*priv));
	if ( priv == NULL )
		goto fail;

	/* Init BC device */
	priv->pdev = pdev;
	(*pdev)->priv = priv;
	priv->irq_log_p = irq_log_p;
	priv->started = 0;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)(*pdev)->businfo;
	pnpinfo = &ambadev->info;
	priv->regs = (struct gr1553b_regs *)pnpinfo->apb_slv->start;

	SPIN_INIT(&priv->devlock, "gr1553bc");

	gr1553bc_device_init(priv);

	/* Register ISR handler (unmask at IRQ controller) */
	if ( drvmgr_interrupt_register(*priv->pdev, 0, "gr1553bc",
	     gr1553bc_isr, priv) ) {
		goto fail;
	}

	return priv;

fail:
	if ( pdev )
		gr1553_bc_close(pdev);
	if ( irq_log_p )
		free(irq_log_p);
	if ( priv )
		free(priv);
	return NULL;
}

void gr1553bc_close(void *bc)
{
	struct gr1553bc_priv *priv = bc;

	/* Stop Hardware */
	gr1553bc_stop(bc, 0x3);

	gr1553bc_device_uninit(priv);

	/* Remove interrupt handler (mask IRQ at IRQ controller) */
	drvmgr_interrupt_unregister(*priv->pdev, 0, gr1553bc_isr, priv);

	/* Free device */
	gr1553_bc_close(priv->pdev);
	SPIN_FREE(&priv->devlock);
	free(priv->irq_log_p);
	free(priv);
}

/* Return Current Minor frame number */
int gr1553bc_indication(void *bc, int async, int *mid)
{
	struct gr1553bc_priv *priv = bc;
	union gr1553bc_bd *bd;

	/* Get current descriptor pointer */
	if ( async ) {
		bd = (union gr1553bc_bd *)
			GR1553BC_READ_REG(&priv->regs->bc_aslot);
		bd = gr1553bc_bd_hw2cpu(priv->alist, bd);
	} else {
		bd = (union gr1553bc_bd *)
			GR1553BC_READ_REG(&priv->regs->bc_slot);
		bd = gr1553bc_bd_hw2cpu(priv->list, bd);
	}

	return gr1553bc_mid_from_bd(bd, mid, NULL);
}

/* Start major frame processing, wait for TimerManager tick or start directly */
int gr1553bc_start(void *bc, struct gr1553bc_list *list, struct gr1553bc_list *list_async)
{
	struct gr1553bc_priv *priv = bc;
	union gr1553bc_bd *bd = NULL, *bd_async = NULL;
	uint32_t ctrl, irqmask;
	SPIN_IRQFLAGS(irqflags);

	if ( (list == NULL) && (list_async == NULL) )
		return 0;

	/* Find first descriptor in list, the descriptor
	 * first to be executed.
	 */
	ctrl = GR1553BC_KEY;
	if ( list ) {
		bd = gr1553bc_slot_bd(list, GR1553BC_ID(0,0,0));
		if ( bd == NULL )
			return -1;
		bd = gr1553bc_bd_cpu2hw(list, bd);
		ctrl |= GR1553B_BC_ACT_SCSRT;
	}
	if ( list_async ) {
		bd_async = gr1553bc_slot_bd(list_async, GR1553BC_ID(0,0,0));
		if ( bd_async == NULL )
			return -1;
		bd_async = gr1553bc_bd_cpu2hw(list_async, bd_async);
		ctrl |= GR1553B_BC_ACT_ASSRT;
	}

	/* Do "hot-swapping" of lists */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	if ( list ) {
		priv->list = list;
		GR1553BC_WRITE_REG(&priv->regs->bc_bd, (uint32_t)bd);
	}
	if ( list_async ) {
		priv->alist = list_async;
		GR1553BC_WRITE_REG(&priv->regs->bc_abd, (uint32_t)bd_async);
	}

	/* If not enabled before, we enable it now. */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);

	/* Enable IRQ */
	if ( priv->started == 0 ) {
		priv->started = 1;
		irqmask = GR1553BC_READ_REG(&priv->regs->imask);
		irqmask |= GR1553B_IRQEN_BCEVE|GR1553B_IRQEN_BCDE|GR1553B_IRQEN_BCWKE;
		GR1553BC_WRITE_REG(&priv->regs->imask, irqmask);
	}

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

/* Pause GR1553 BC transfers */
int gr1553bc_pause(void *bc)
{
	struct gr1553bc_priv *priv = bc;
	uint32_t ctrl;
	SPIN_IRQFLAGS(irqflags);

	/* Do "hot-swapping" of lists */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	ctrl = GR1553BC_KEY | GR1553B_BC_ACT_SCSUS;
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

/* Restart GR1553 BC transfers, after being paused */
int gr1553bc_restart(void *bc)
{
	struct gr1553bc_priv *priv = bc;
	uint32_t ctrl;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	ctrl = GR1553BC_KEY | GR1553B_BC_ACT_SCSRT;
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

/* Stop BC transmission */
int gr1553bc_stop(void *bc, int options)
{
	struct gr1553bc_priv *priv = bc;
	uint32_t ctrl;
	SPIN_IRQFLAGS(irqflags);

	ctrl = GR1553BC_KEY;
	if ( options & 0x1 )
		ctrl |= GR1553B_BC_ACT_SCSTP;
	if ( options & 0x2 )
		ctrl |= GR1553B_BC_ACT_ASSTP;

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, ctrl);
	priv->started = 0;
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

/* Reset software and BC hardware into a known "unused/init" state */
void gr1553bc_device_init(struct gr1553bc_priv *priv)
{
/* RESET HARDWARE REGISTERS */
	/* Stop BC if not already stopped */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | 0x0204);

	/* Since RT can not be used at the same time as BC, we stop
	 * RT rx, it should already be stopped...
	 */
	GR1553BC_WRITE_REG(&priv->regs->rt_cfg, GR1553RT_KEY);

	/* Clear some registers */
	GR1553BC_WRITE_REG(&priv->regs->bc_bd, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_abd, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_timer, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_wake, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_irqptr, 0);
	GR1553BC_WRITE_REG(&priv->regs->bc_busmsk, 0);

/* PUT SOFTWARE INTO INITIAL STATE */
	priv->list = NULL;
	priv->alist = NULL;

	priv->irq_log_base = (uint32_t *)
		(((uint32_t)priv->irq_log_p + (GR1553BC_IRQLOG_SIZE-1)) &
		~(GR1553BC_IRQLOG_SIZE-1));
	/* Translate into a hardware accessible address */
	drvmgr_translate_check(
		*priv->pdev,
		CPUMEM_TO_DMA,
		(void *)priv->irq_log_base,
		(void **)&priv->irq_log_base_hw,
		GR1553BC_IRQLOG_SIZE);
	priv->irq_log_curr = priv->irq_log_base;
	priv->irq_log_end = &priv->irq_log_base[GR1553BC_IRQLOG_CNT-1];
	priv->irq_func = gr1553bc_isr_std;
	priv->irq_data = NULL;

	GR1553BC_WRITE_REG(&priv->regs->bc_irqptr,(uint32_t)priv->irq_log_base_hw);
}

void gr1553bc_device_uninit(struct gr1553bc_priv *priv)
{
	uint32_t irqmask;

	/* Stop BC if not already stopped */
	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | 0x0204);

	/* Since RT can not be used at the same time as BC, we stop
	 * RT rx, it should already be stopped...
	 */
	GR1553BC_WRITE_REG(&priv->regs->rt_cfg, GR1553RT_KEY);

	/* Turn off IRQ generation */
	irqmask=GR1553BC_READ_REG(&priv->regs->imask);
	irqmask&=~(GR1553B_IRQEN_BCEVE|GR1553B_IRQEN_BCDE|GR1553B_IRQEN_BCWKE);
	GR1553BC_WRITE_REG(&priv->regs->irq, irqmask);
}

/* Interrupt handler */
void gr1553bc_isr(void *arg)
{
	struct gr1553bc_priv *priv = arg;
	uint32_t *curr, *pos, word0, word2;
	union gr1553bc_bd *bd;
	bcirq_func_t func;
	void *data;
	int handled, irq;
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Did core make IRQ */
	irq = GR1553BC_READ_REG(&priv->regs->irq);
	irq &= (GR1553B_IRQEN_BCEVE|GR1553B_IRQEN_BCDE|GR1553B_IRQEN_BCWKE);
	if ( irq == 0 )
		return; /* Shared IRQ: some one else may have caused the IRQ */

	/* Clear handled IRQs */
	GR1553BC_WRITE_REG(&priv->regs->irq, irq);

	/* DMA error. This IRQ does not affect the IRQ log.
	 * We let standard IRQ handle handle it.
	 */
	if ( irq & GR1553B_IRQEN_BCDE ) {
		priv->irq_func(NULL, priv->irq_data);
	}

	/* Get current posistion in hardware */
	pos = (uint32_t *)GR1553BC_READ_REG(&priv->regs->bc_irqptr);
	/* Converting into CPU address */
	pos = priv->irq_log_base +
		((unsigned int)pos - (unsigned int)priv->irq_log_base_hw)/4;

	/* Step in IRQ log until we reach the end. */
	handled = 0;
	curr = priv->irq_log_curr;
	while ( curr != pos ) {
		bd = (union gr1553bc_bd *)(GR1553BC_READ_MEM(curr) & ~1);
		GR1553BC_WRITE_MEM(curr, 0x2); /* Mark Handled */

		/* Convert Descriptor in IRQ log into CPU address. In order
		 * to convert we must know which list the descriptor belongs
		 * to, we compare the address of the bd to the ASYNC list
		 * descriptor table area.
		 */
		SPIN_LOCK(&priv->devlock, irqflags);
		if ( priv->alist && ((unsigned int)bd>=priv->alist->table_hw) &&
		     ((unsigned int)bd <
		     (priv->alist->table_hw + priv->alist->table_size))) {
		     	/* BD in async list */
			bd = gr1553bc_bd_hw2cpu(priv->alist, bd);
		} else if (priv->list &&
		           ((unsigned int)bd >= priv->list->table_hw) &&
			   ((unsigned int)bd <
			   (priv->list->table_hw + priv->list->table_size))) {
			/* BD in sync list */
			bd = gr1553bc_bd_hw2cpu(priv->list, bd);
		} else {
			/* error - unknown BD. Should not happen but could
			 * if user has switched list. Ignore IRQ entry and
			 * continue to next entry.
			 */
			bd = NULL;
		}

		/* Handle Descriptor that cased IRQ
		 *
		 * If someone have inserted an IRQ descriptor and tied
		 * that to a custom function we call that function, otherwise
		 * we let the standard IRQ handle handle it.
		 */
		if ( bd ) {
			word0 = GR1553BC_READ_MEM(&bd->raw.words[0]);
			word2 = GR1553BC_READ_MEM(&bd->raw.words[2]);
			SPIN_UNLOCK(&priv->devlock, irqflags);
			if ( word0 == GR1553BC_UNCOND_IRQ ) {
				if ( (word2 & 0x3) == 0 ) {
					func = (bcirq_func_t)(word2 & ~0x3);
					data = (void *)
					  GR1553BC_READ_MEM(&bd->raw.words[3]);
					func(bd, data);
					handled = 1;
				}
			}

			if ( handled == 0 ) {
				/* Let standard IRQ handle handle it */
				priv->irq_func(bd, priv->irq_data);
			} else {
				handled = 0;
			}
		} else {
			SPIN_UNLOCK(&priv->devlock, irqflags);
		}

		/* Increment to next entry in IRQ LOG */
		if ( curr == priv->irq_log_end )
			curr = priv->irq_log_base;
		else
			curr++;
	}
	priv->irq_log_curr = curr;
}

int gr1553bc_irq_setup
	(
	void *bc,
	bcirq_func_t func,
	void *data
	)
{
	struct gr1553bc_priv *priv = bc;

	if ( func == NULL )
		priv->irq_func = gr1553bc_isr_std;
	else
		priv->irq_func = func;
	priv->irq_data = data;

	return 0;
}

void gr1553bc_ext_trig(void *bc, int trig)
{
	struct gr1553bc_priv *priv = bc;
	unsigned int trigger;

	if ( trig )
		trigger = GR1553B_BC_ACT_SETT;
	else
		trigger = GR1553B_BC_ACT_CLRT;

	GR1553BC_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | trigger);
}

void gr1553bc_status(void *bc, struct gr1553bc_status *status)
{
	struct gr1553bc_priv *priv = bc;

	status->status = GR1553BC_READ_REG(&priv->regs->bc_stat);
	status->time = GR1553BC_READ_REG(&priv->regs->bc_timer);
}

/*** DEBUGGING HELP FUNCTIONS ***/

#include <stdio.h>

void gr1553bc_show_list(struct gr1553bc_list *list, int options)
{
	struct gr1553bc_major *major;
	struct gr1553bc_minor *minor;
	int i, j, minor_cnt, timefree;

	printf("LIST\n");
	printf("  major cnt: %d\n", list->major_cnt);
	for (i=0; i<32; i++) {
		printf("  RT[%d] timeout: %d\n", i, 14+(list->rt_timeout[i]*4));
	}

	for (i=0; i<list->major_cnt; i++) {
		major = list->majors[i];
		minor_cnt = major->cfg->minor_cnt;
		printf(" MAJOR[%d]\n", i);
		printf("   minor count: %d\n", minor_cnt);

		for (j=0; j<minor_cnt; j++) {
			minor = major->minors[j];

			printf("   MINOR[%d]\n", j);
			printf("     bd:        0x%08x (HW:0x%08x)\n",
				(unsigned int)&minor->bds[0],
				(unsigned int)gr1553bc_bd_cpu2hw(list,
							&minor->bds[0]));
			printf("     slot cnt:  %d\n", minor->cfg->slot_cnt);
			if ( minor->cfg->timeslot ) {
				timefree = gr1553bc_minor_freetime(minor);
				printf("     timefree:  %d\n", timefree);
				printf("     timetotal: %d\n",
					minor->cfg->timeslot);
			} else {
				printf("     no time mgr\n");
			}
		}
	}
}
