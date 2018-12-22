/*  GR1553B RT driver
 *
 *  COPYRIGHT (c) 2010.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <rtems.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <grlib/gr1553b.h>
#include <grlib/gr1553rt.h>

#include <drvmgr/drvmgr.h>
#include <grlib/ambapp_bus.h>

#include <grlib/grlib_impl.h>

#define GR1553RT_WRITE_MEM(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553RT_READ_MEM(adr) (*(volatile uint32_t *)(adr))

#define GR1553RT_WRITE_REG(adr, val) *(volatile uint32_t *)(adr) = (uint32_t)(val)
#define GR1553RT_READ_REG(adr) (*(volatile uint32_t *)(adr))

/* Software representation of one hardware descriptor */
struct gr1553rt_sw_bd {
	unsigned short this_next;/* Next entry or this entry. 0xffff: no next */
	unsigned char listid;	/* ListID of List the descriptor is attached */
	char unused;
} __attribute__((packed));

/* Software description of a subaddress */
struct gr1553rt_subadr {
	/* RX LIST */
	unsigned char rxlistid;
	/* TX LIST */
	unsigned char txlistid;
};

struct gr1553rt_irqerr {
	gr1553rt_irqerr_t func;
	void *data;
};

struct gr1553rt_irqmc {
	gr1553rt_irqmc_t func;
	void *data;
};

struct gr1553rt_irq {
	gr1553rt_irq_t func;
	void *data;
};

struct gr1553rt_priv {
	/* Pointer to Hardware registers */
	struct gr1553b_regs *regs;

	/* Software State */
	int started;
	struct gr1553rt_cfg cfg;
	SPIN_DECLARE(devlock);

	/* Handle to GR1553B RT device layer */
	struct drvmgr_dev **pdev;

	/* Each Index represents one RT Subaddress. 31 = Broadcast */
	struct gr1553rt_subadr subadrs[32];

	/* Pointer to array of Software's description of a hardware
	 * descriptor.
	 */
#if (RTBD_MAX == 0)
	struct gr1553rt_sw_bd *swbds;
#else
	struct gr1553rt_sw_bd swbds[RTBD_MAX];
#endif

	/* List of Free descriptors */
	unsigned short swbd_free;
	int swbd_free_cnt;

	/* Hardware SubAddress descriptors given for CPU and Hardware */
	void *satab_buffer;
	struct gr1553rt_sa *sas_cpu;	/* Translated for CPU */
	struct gr1553rt_sa *sas_hw;	/* Translated for Hardware */

	/* Hardware descriptors address given for CPU and hardware */
	void *bd_buffer;
	int bds_cnt;			/* Number of descriptors */
	struct gr1553rt_bd *bds_cpu;	/* Translated for CPU */
	struct gr1553rt_bd *bds_hw;	/* Translated for Hardware */


	/* Event Log buffer in */
	void *evlog_buffer;
	unsigned int *evlog_cpu_next;	/* Next LOG entry to be handled */
	unsigned int *evlog_cpu_base;	/* First Entry in LOG */
	unsigned int *evlog_cpu_end;	/* Last+1 Entry in LOG */
	unsigned int *evlog_hw_base;	/* Translated for Hardware */

	/* Each Index represents a LIST ID */
	struct gr1553rt_list *lists[RTLISTID_MAX];

	/* IRQ handlers, one per SUBADDRESS */
	struct gr1553rt_irq irq_rx[32];
	struct gr1553rt_irq irq_tx[32];

	/* ISR called when an ERROR IRQ is received */
	struct gr1553rt_irqerr irq_err;

	/* ISR called when an Mode Code is received */
	struct gr1553rt_irqmc irq_mc;
};

void gr1553rt_sw_init(struct gr1553rt_priv *priv);
void gr1553rt_sw_free(struct gr1553rt_priv *priv);
void gr1553rt_isr(void *data);

/* Assign and ID to the list. An LIST ID is needed before scheduling list
 * on an RT subaddress.
 *
 * Only 64 lists can be registered at a time on the same device.
 */
static int gr1553rt_list_reg(struct gr1553rt_list *list)
{
	struct gr1553rt_priv *priv = list->rt;
	int i;

	/* Find first free list ID */
	for ( i=0; i<RTLISTID_MAX; i++) {
		if ( priv->lists[i] == NULL ) {
			priv->lists[i] = list;
			list->listid = i;
			return i;
		}
	}

	/* No available LIST IDs */
	list->listid = -1;

	return -1;
}

#if 0 /* unused for now */
/* Unregister List from device */
static void gr1553rt_list_unreg(struct gr1553rt_list *list)
{
	struct gr1553rt_priv *priv = list->rt;

	priv->lists[list->listid] = NULL;
	list->listid = -1;
}
#endif

static int gr1553rt_bdid(void *rt, struct gr1553rt_sw_bd *bd)
{
	struct gr1553rt_priv *priv = rt;
	
	unsigned short index;

	/* Get Index of Software BD */
	index = ((unsigned int)bd - (unsigned int)&priv->swbds[0]) / 
		sizeof(struct gr1553rt_sw_bd);

	return index;
}

static void gr1553rt_bd_alloc_init(void *rt, int count)
{
	struct gr1553rt_priv *priv = rt;
	int i;

	for (i=0; i<count-1; i++) {
		priv->swbds[i].this_next = i+1;
	}
	priv->swbds[count-1].this_next = 0xffff;
	priv->swbd_free = 0;
	priv->swbd_free_cnt = count;
}

/* Allocate a Chain of descriptors */
static int gr1553rt_bd_alloc(void *rt, struct gr1553rt_sw_bd **bd, int cnt)
{
	struct gr1553rt_priv *priv = rt;
	struct gr1553rt_sw_bd *curr;
	int i;

	if ((priv->swbd_free_cnt < cnt) || (cnt <= 0)) {
		*bd = NULL;
		return -1;
	}

	*bd = &priv->swbds[priv->swbd_free];
	for (i=0; i<cnt; i++) {
		if ( i == 0) {
			curr = &priv->swbds[priv->swbd_free];
		} else {
			curr = &priv->swbds[curr->this_next];
		}
		if ( curr->this_next == 0xffff ) {
			*bd = NULL;
			return -1;
		}
	}
	priv->swbd_free = curr->this_next;
	priv->swbd_free_cnt -= cnt;
	curr->this_next = 0xffff; /* Mark end of chain on last entry */

	return 0;
}

#if 0 /* unused for now */
static void gr1553rt_bd_free(void *rt, struct gr1553rt_sw_bd *bd)
{
	struct gr1553rt_priv *priv = rt;
	unsigned short index;

	/* Get Index of Software BD */
	index = gr1553rt_bdid(priv, bd);

	/* Insert first in list */
	bd->this_next = priv->swbd_free;
	priv->swbd_free = index;
	priv->swbd_free_cnt++;
}
#endif

int gr1553rt_list_init
	(
	void *rt,
	struct gr1553rt_list **plist,
	struct gr1553rt_list_cfg *cfg
	)
{
	struct gr1553rt_priv *priv = rt;
	size_t size;
	int i;
	struct gr1553rt_sw_bd *swbd;
	unsigned short index;
	struct gr1553rt_list *list;

	/* The user may provide a pre allocated LIST, or
	 * let the driver handle allocation by using malloc()
	 *
	 * If the IN/OUT plist argument points to NULL a list
	 * dynamically allocated here.
	 */
	list = *plist;
	if ( list == NULL ) {
		/* Dynamically allocate LIST */
		size = sizeof(*list) +
			(cfg->bd_cnt * sizeof(list->bds[0]));
		list = grlib_malloc(size);
		if ( list == NULL )
			return -1;
		*plist = list;
	}

	list->rt = rt;
	list->subadr = -1;
	list->listid = gr1553rt_list_reg(list);
	if ( list->listid == -1 )
		return -2; /* Too many lists */
	list->cfg = cfg;
	list->bd_cnt = cfg->bd_cnt;

	/* Allocate all BDs needed by list */
	if ( gr1553rt_bd_alloc(rt, &swbd, list->bd_cnt) ) {
		return -3; /* Too few descriptors */
	}

	/* Get ID/INDEX of Software BDs */
	index = gr1553rt_bdid(rt, swbd);
	list->bds[0] = index;
	for (i=1; i<list->bd_cnt; i++) {
		list->bds[i] = priv->swbds[list->bds[i-1]].this_next;
	}

	/* Now that the next pointer has fullfilled it's job and not
	 * needed anymore, we use it as list entry pointer instead.
	 * The this_next pointer is a list entry number.
	 */
	for (i=0; i<list->bd_cnt; i++) {
		priv->swbds[list->bds[i]].this_next = i;
	}

	return 0;
}

int gr1553rt_bd_init(
	struct gr1553rt_list *list,
	unsigned short entry_no,
	unsigned int flags,
	uint16_t *dptr,
	unsigned short next
	)
{
	struct gr1553rt_priv *priv;
	unsigned short bdid;
	struct gr1553rt_bd *bd;
	unsigned int nextbd, dataptr;
	SPIN_IRQFLAGS(irqflags);

	if ( entry_no >= list->bd_cnt )
		return -1;

	/* Find Descriptor */
	bdid = list->bds[entry_no];
	priv = list->rt;
	bd = &priv->bds_cpu[bdid];

	if ( next == 0xfffe ) {
		next = entry_no + 1;
		if ( next >= list->bd_cnt )
			next = 0;
	}

	/* Find next descriptor in address space that the
	 * Hardware understand.
	 */
	if ( next >= 0xffff ) {
		nextbd = 0x3; /* End of list */
	} else if ( next >= list->bd_cnt ) {
		return -1;
	} else {
		bdid = list->bds[next];
		nextbd = (unsigned int)&priv->bds_hw[bdid];
	}

	dataptr = (unsigned int)dptr;
	if ( dataptr & 1 ) {
		/* Translate address from CPU-local into remote */
		dataptr &= ~1;
		drvmgr_translate(
			*priv->pdev,
			CPUMEM_TO_DMA,
			(void *)dataptr,
			(void **)&dataptr
			);
	}

	/* Init BD */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	bd->ctrl = flags & GR1553RT_BD_FLAGS_IRQEN;
	bd->dptr = (unsigned int)dptr;
	bd->next = nextbd;
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;	
}

int gr1553rt_bd_update(
	struct gr1553rt_list *list,
	int entry_no,
	unsigned int *status,
	uint16_t **dptr
	)
{
	struct gr1553rt_priv *priv;
	unsigned short bdid;
	struct gr1553rt_bd *bd;
	unsigned int tmp, dataptr;
	SPIN_IRQFLAGS(irqflags);

	if ( entry_no >= list->bd_cnt )
		return -1;

	/* Find Descriptor */
	bdid = list->bds[entry_no];
	priv = list->rt;
	bd = &priv->bds_cpu[bdid];

	/* Prepare translation if needed */
	if ( dptr && (dataptr=(unsigned int)*dptr) ) {
		if ( dataptr & 1 ) {
			/* Translate address from CPU-local into remote. May
			 * be used when RT core is accessed over the PCI bus.
			 */
			dataptr &= ~1;
			drvmgr_translate(
				*priv->pdev,
				CPUMEM_TO_DMA,
				(void *)dataptr,
				(void **)&dataptr
				);
		}
	}

	/* Get current values and then set new values in BD */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	/* READ/WRITE Status/Control word */
	if ( status ) {
		tmp = bd->ctrl;
		if ( *status ) {
			bd->ctrl = *status;
		}
		*status = tmp;
	}
	/* READ/WRITE Data-Pointer word */
	if ( dptr ) {
		tmp = bd->dptr;
		if ( dataptr ) {
			bd->dptr = dataptr;
		}
		*dptr = (uint16_t *)tmp;
	}
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

int gr1553rt_irq_err
	(
	void *rt,
	gr1553rt_irqerr_t func,
	void *data
	)
{
	struct gr1553rt_priv *priv = rt;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	priv->irq_err.func = func;
	priv->irq_err.data = data;
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

int gr1553rt_irq_mc
	(
	void *rt,
	gr1553rt_irqmc_t func,
	void *data
	)
{
	struct gr1553rt_priv *priv = rt;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	priv->irq_mc.func = func;
	priv->irq_mc.data = data;
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

int gr1553rt_irq_sa
	(
	void *rt,
	int subadr,
	int tx,
	gr1553rt_irq_t func,
	void *data
	)
{
	struct gr1553rt_priv *priv = rt;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	if ( tx ) {
		priv->irq_tx[subadr].func = func;
		priv->irq_tx[subadr].data = data;
	} else {
		priv->irq_rx[subadr].func = func;
		priv->irq_rx[subadr].data = data;
	}
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

/* GR1553-RT Interrupt Service Routine */
void gr1553rt_isr(void *data)
{
	struct gr1553rt_priv *priv = data;
	unsigned int firstirq, lastpos;
	int index;
	unsigned int *last, *curr, entry, hwbd;
	int type, samc, mcode, subadr;
	int listid;
	struct gr1553rt_irq *pisr, isr;
	struct gr1553rt_irqerr isrerr;
	struct gr1553rt_irqmc isrmc;
	unsigned int irq;
	SPIN_ISR_IRQFLAGS(irqflags);

	/* Ack IRQ before reading current write pointer, but after
	 * reading current IRQ pointer. This is because RT_EVIRQ
	 * may be updated after we ACK the IRQ source.
	 */
	irq = priv->regs->irq &
		(GR1553B_IRQ_RTTE|GR1553B_IRQ_RTD|GR1553B_IRQ_RTEV);
	if ( irq == 0 )
		return;

	firstirq = priv->regs->rt_evirq;
	priv->regs->irq = irq;
	lastpos = priv->regs->rt_evlog;

	/* Quit if nothing has been added to the log */
	if ( lastpos == firstirq )
		return;

	if ( irq & (GR1553B_IRQ_RTTE|GR1553B_IRQ_RTD) ) {
		/* copy func and arg while owning lock */
		SPIN_LOCK(&priv->devlock, irqflags);
		isrerr = priv->irq_err;
		SPIN_UNLOCK(&priv->devlock, irqflags);
		if ( isrerr.func ) {
			isrerr.func(irq, isrerr.data);
		}

		/* Stop Hardware and enter non-started mode. This will
		 * make all future calls to driver result in an error.
		 */
		gr1553rt_stop(priv);
	}

	/* Step between first log entry causing an IRQ to last
	 * entry. Each entry that has caused an IRQ will be handled
	 * by calling user-defined function.
	 *
	 * We convert hardware addresses into CPU accessable addresses
	 * first.
	 */
	index = (firstirq - (unsigned int)priv->evlog_hw_base) /
		sizeof(unsigned int);
	curr = priv->evlog_cpu_base + index;
	index = (lastpos - (unsigned int)priv->evlog_hw_base) /
		sizeof(unsigned int);
	last = priv->evlog_cpu_base + index;

	do {
		/* Process one entry */
		entry = *curr;

		if ( entry & 0x80000000 ) {
			/* Entry caused IRQ */
			type = (entry >> 29) & 0x3;
			samc = (entry >> 24) & 0x1f;
			if ( (type & 0x2) == 0 ) {
				/* Transmit/Receive Data */
				subadr = samc;
				if ( type ) {
					/* Receive */
					listid = priv->subadrs[subadr].rxlistid;
					hwbd = priv->sas_cpu[subadr].rxptr;
					pisr = &priv->irq_rx[subadr];
				} else {
					/* Transmit */
					listid = priv->subadrs[subadr].txlistid;
					hwbd = priv->sas_cpu[subadr].txptr;
					pisr = &priv->irq_tx[subadr];
				}

				index = ((unsigned int)hwbd - (unsigned int)
					priv->bds_hw)/sizeof(struct gr1553rt_bd);

				/* copy func and arg while owning lock */
				SPIN_LOCK(&priv->devlock, irqflags);
				isr = *pisr;
				SPIN_UNLOCK(&priv->devlock, irqflags);

				/* Call user ISR of RX/TX transfer */
				if ( isr.func ) {
					isr.func(
						priv->lists[listid],
						entry,
						priv->swbds[index].this_next,
						isr.data
						);
				}
			} else if ( type == 0x2) {
				/* Modecode */
				mcode = samc;

				/* copy func and arg while owning lock */
				SPIN_LOCK(&priv->devlock, irqflags);
				isrmc = priv->irq_mc;
				SPIN_UNLOCK(&priv->devlock, irqflags);

				/* Call user ISR of ModeCodes RX/TX */
				if ( isrmc.func ) {
					isrmc.func(
						mcode,
						entry,
						isrmc.data
						);
				}
			} else {
				/* ERROR OF SOME KIND, EVLOG OVERWRITTEN? */
				rtems_fatal_error_occurred(RTEMS_IO_ERROR);
			}
		}

		/* Calc next entry posistion */
		curr++;
		if ( curr == priv->evlog_cpu_end )
			curr = priv->evlog_cpu_base;

	} while ( curr != last );
}

int gr1553rt_indication(void *rt, int subadr, int *txeno, int *rxeno)
{
	struct gr1553rt_priv *priv = rt;
	struct gr1553rt_sa *sa;
	unsigned int bd, index;

	/*  Sub address valid */
	if ( (subadr < 0) || (subadr > 31) )
		return -1;

	/* Get SubAddress Descriptor address as accessed from CPU */
	sa = &priv->sas_cpu[subadr];

	/* Indication of TX descriptor? */
	if ( txeno ) {
		bd = sa->txptr;
		/* Get Index of Hardware BD */
		index = ((unsigned int)bd - (unsigned int)&priv->bds_hw[0]) /
			sizeof(struct gr1553rt_bd);
		*txeno = priv->swbds[index].this_next;
	}

	/* Indication of RX descriptor? */
	if ( rxeno ) {
		bd = sa->rxptr;
		/* Get Index of Hardware BD */
		index = ((unsigned int)bd - (unsigned int)&priv->bds_hw[0]) /
			sizeof(struct gr1553rt_bd);
		*rxeno = priv->swbds[index].this_next;
	}

	return 0;
}

void gr1553rt_hw_stop(struct gr1553rt_priv *priv);

void gr1553rt_register(void)
{
	/* The RT driver rely on the GR1553B Driver */
	gr1553_register();
}

void *gr1553rt_open(int minor)
{
	struct drvmgr_dev **pdev = NULL;
	struct gr1553rt_priv *priv = NULL;
	struct amba_dev_info *ambadev;
	struct ambapp_core *pnpinfo;

	/* Allocate requested device */
	pdev = gr1553_rt_open(minor);
	if ( pdev == NULL )
		goto fail;

	priv = grlib_calloc(1, sizeof(*priv));
	if ( priv == NULL )
		goto fail;

	/* Assign a device private to RT device */
	priv->pdev = pdev;
	(*pdev)->priv = priv;

	/* Get device information from AMBA PnP information */
	ambadev = (struct amba_dev_info *)(*pdev)->businfo;
	pnpinfo = &ambadev->info;
	priv->regs = (struct gr1553b_regs *)pnpinfo->apb_slv->start;

	SPIN_INIT(&priv->devlock, "gr1553rt");

	/* Start with default configuration */
	/*priv->cfg = gr1553rt_default_config;*/

	/* Unmask IRQs and so */
	gr1553rt_hw_stop(priv);

	/* Register ISR handler. hardware mask IRQ, so it is safe to unmask
	 * at IRQ controller.
	 */
	if (drvmgr_interrupt_register(*priv->pdev, 0, "gr1553rt", gr1553rt_isr, priv))
		goto fail;

	return priv;

fail:
	if ( pdev )
		gr1553_rt_close(pdev);
	if ( priv )
		free(priv);
	return NULL;
}

void gr1553rt_close(void *rt)
{
	struct gr1553rt_priv *priv = rt;

	if ( priv->started ) {
		gr1553rt_stop(priv);
	}

	/* Remove ISR handler */
	drvmgr_interrupt_unregister(*priv->pdev, 0, gr1553rt_isr, priv);

	/* Free dynamically allocated buffers if any */
	gr1553rt_sw_free(priv);
	SPIN_FREE(&priv->devlock);

	/* Return RT/BC device */
	gr1553_rt_close(priv->pdev);
}

/* Stop Hardware and disable IRQ */
void gr1553rt_hw_stop(struct gr1553rt_priv *priv)
{
	uint32_t irqmask;

	/* Disable RT */
	GR1553RT_WRITE_REG(&priv->regs->rt_cfg, GR1553RT_KEY);

	/* Stop BC if not already stopped: BC can not be used simultaneously
	 * as the RT anyway 
	 */
	GR1553RT_WRITE_REG(&priv->regs->bc_ctrl, GR1553BC_KEY | 0x0204);

	/* Turn off RT IRQ generation */
	irqmask=GR1553RT_READ_REG(&priv->regs->imask);
	irqmask&=~(GR1553B_IRQEN_RTEVE|GR1553B_IRQEN_RTDE);
	GR1553RT_WRITE_REG(&priv->regs->irq, irqmask);
}

/* Free dynamically allocated buffers, if any */
void gr1553rt_sw_free(struct gr1553rt_priv *priv)
{
	/* Event log */
	if ( (priv->cfg.evlog_buffer == NULL) && priv->evlog_buffer ) {
		free(priv->evlog_buffer);
		priv->evlog_buffer = NULL;
	}

	/* RX/TX Descriptors */
	if ( (priv->cfg.bd_buffer == NULL) && priv->bd_buffer ) {
		free(priv->bd_buffer);
		priv->bd_buffer = NULL;
	}

#if (RTBD_MAX == 0)
	if ( priv->swbds ) {
		free(priv->swbds);
		priv->swbds = NULL;
	}
#endif

	/* Sub address table */
	if ( (priv->cfg.satab_buffer == NULL) && priv->satab_buffer ) {
		free(priv->satab_buffer);
		priv->satab_buffer = NULL;
	}
}

/* Free dynamically allocated buffers, if any */
static int gr1553rt_sw_alloc(struct gr1553rt_priv *priv)
{
	int size;

	/* Allocate Event log */
	if ((unsigned int)priv->cfg.evlog_buffer & 1) {
		/* Translate Address from HARDWARE (REMOTE) to CPU-LOCAL */
		priv->evlog_hw_base = (unsigned int *)
			((unsigned int)priv->cfg.evlog_buffer & ~0x1);
		priv->evlog_buffer = priv->cfg.evlog_buffer;
		drvmgr_translate_check(
			*priv->pdev,
			DMAMEM_TO_CPU,
			(void *)priv->evlog_hw_base,
			(void **)&priv->evlog_cpu_base,
			priv->cfg.evlog_size
			);
	} else {
		if (priv->cfg.evlog_buffer == NULL) {
			priv->evlog_buffer = grlib_malloc(
				priv->cfg.evlog_size * 2);
			if (priv->evlog_buffer == NULL)
				return -1;
		} else {
			/* Addess already CPU-LOCAL */
			priv->evlog_buffer = priv->cfg.evlog_buffer;
		}
		/* Align to SIZE bytes boundary */
		priv->evlog_cpu_base = (unsigned int *)
			(((unsigned int)priv->evlog_buffer +
			(priv->cfg.evlog_size-1)) & ~(priv->cfg.evlog_size-1));

		drvmgr_translate_check(
			*priv->pdev,
			CPUMEM_TO_DMA,
			(void *)priv->evlog_cpu_base,
			(void **)&priv->evlog_hw_base,
			priv->cfg.evlog_size
			);
	}
	priv->evlog_cpu_end = priv->evlog_cpu_base +
				priv->cfg.evlog_size/sizeof(unsigned int *);

	/* Allocate Transfer Descriptors */
	priv->bds_cnt = priv->cfg.bd_count;
	size = priv->bds_cnt * sizeof(struct gr1553rt_bd);
	if ((unsigned int)priv->cfg.bd_buffer & 1) {
		/* Translate Address from HARDWARE (REMOTE) to CPU-LOCAL */
		priv->bds_hw = (struct gr1553rt_bd *)
			((unsigned int)priv->cfg.bd_buffer & ~0x1);
		priv->bd_buffer = priv->cfg.bd_buffer;
		drvmgr_translate_check(
			*priv->pdev,
			DMAMEM_TO_CPU,
			(void *)priv->bds_hw,
			(void **)&priv->bds_cpu,
			size
			);
	} else {
		if ( priv->cfg.bd_buffer == NULL ) {
			priv->bd_buffer = grlib_malloc(size + 0xf);
			if (priv->bd_buffer == NULL)
				return -1;
		} else {
			/* Addess already CPU-LOCAL */
			priv->bd_buffer	= priv->cfg.bd_buffer;
		}
		/* Align to 16 bytes boundary */
		priv->bds_cpu = (struct gr1553rt_bd *)
				(((unsigned int)priv->bd_buffer + 0xf) & ~0xf);

		/* Translate from CPU address to hardware address */
		drvmgr_translate_check(
			*priv->pdev,
			CPUMEM_TO_DMA,
			(void *)priv->bds_cpu,
			(void **)&priv->bds_hw,
			size
			);
	}

#if (RTBD_MAX == 0)
	/* Allocate software description of */
	priv->swbds = grlib_malloc(priv->cfg.bd_count * sizeof(*priv->swbds));
	if ( priv->swbds == NULL ) {
		return -1;
	}
#endif

	/* Allocate Sub address table */
	if ((unsigned int)priv->cfg.satab_buffer & 1) {
		/* Translate Address from HARDWARE (REMOTE) to CPU-LOCAL */
		priv->sas_hw = (struct gr1553rt_sa *)
			((unsigned int)priv->cfg.satab_buffer & ~0x1);
		priv->satab_buffer = priv->cfg.satab_buffer;
		drvmgr_translate_check(
			*priv->pdev,
			DMAMEM_TO_CPU,
			(void *)priv->sas_hw,
			(void **)&priv->sas_cpu,
			16 * 32);
	} else {
		if (priv->cfg.satab_buffer == NULL) {
			priv->satab_buffer = grlib_malloc((16 * 32) * 2);
			if (priv->satab_buffer == NULL)
				return -1;
		} else {
			/* Addess already CPU-LOCAL */
			priv->satab_buffer = priv->cfg.satab_buffer;
		}
		/* Align to 512 bytes boundary */
		priv->sas_cpu = (struct gr1553rt_sa *)
				(((unsigned int)priv->satab_buffer + 0x1ff) &
				~0x1ff);

		/* Translate Address from CPU-LOCAL to HARDWARE (REMOTE) */
		drvmgr_translate_check(
			*priv->pdev,
			CPUMEM_TO_DMA,
			(void *)priv->sas_cpu,
			(void **)&priv->sas_hw,
			16 * 32);
	}

	return 0;
}

void gr1553rt_sw_init(struct gr1553rt_priv *priv)
{
	int i;

	/* Clear Sub Address table */
	memset(priv->sas_cpu, 0, 512);

	/* Clear Transfer descriptors */
	memset(priv->bds_cpu, 0, priv->bds_cnt * 16);

	/* Clear the Event log */
	memset(priv->evlog_cpu_base, 0, priv->cfg.evlog_size);

	/* Init descriptor allocation algorithm */
	gr1553rt_bd_alloc_init(priv, priv->bds_cnt);

	/* Init table used to convert from sub address to list.
	 * Currently non assigned.
	 */
	for (i=0; i<32; i++) {
		priv->subadrs[i].rxlistid = 0xff;
		priv->subadrs[i].txlistid = 0xff;
	}

	/* Clear all previous IRQ handlers */
	for (i=0; i<32; i++) {
		priv->irq_rx[i].func = NULL;
		priv->irq_tx[i].data = NULL;
	}
	priv->irq_err.func = NULL;
	priv->irq_err.data = NULL;
	priv->irq_mc.func = NULL;
	priv->irq_mc.data = NULL;

	/* Clear LIST to LISTID table */
	for (i=0; i<RTLISTID_MAX; i++) {
		priv->lists[i] = NULL;
	}
}

int gr1553rt_config(void *rt, struct gr1553rt_cfg *cfg)
{
	struct gr1553rt_priv *priv = rt;

	if ( priv->started )
		return -1;

	/*** Free dynamically allocated buffers ***/

	gr1553rt_sw_free(priv);

	/*** Check new config ***/
	if ( cfg->rtaddress > 30 )
		return -1;
	if ( (cfg->evlog_size & (cfg->evlog_size-1)) != 0)
		return -1; /* SIZE: Not aligned to a power of 2 */
	if ( ((unsigned int)priv->cfg.evlog_buffer & (cfg->evlog_size-1)) != 0 )
		return -1; /* Buffer: Not aligned to size */
#if (RTBD_MAX > 0)
	if ( cfg->bd_count > RTBD_MAX )
		return -1;
#endif

	/*** Make new config current ***/
	priv->cfg = *cfg;

	/*** Adapt to new config ***/

	if ( gr1553rt_sw_alloc(priv) != 0 ) 
		return -1;

	gr1553rt_sw_init(priv);

	return 0;
}

int gr1553rt_start(void *rt)
{
	struct gr1553rt_priv *priv = rt;
	SPIN_IRQFLAGS(irqflags);

	if ( priv->started )
		return -1;

	/*** Initialize software Pointers and stuff ***/

	if ( !priv->satab_buffer || !priv->bd_buffer || !priv->evlog_buffer )
		return -2;

	priv->evlog_cpu_next = priv->evlog_cpu_base;

	/*** Initialize Registers ***/

	/* Subaddress table base */
	priv->regs->rt_tab = (unsigned int)priv->sas_hw;

	/* Mode code configuration */
	priv->regs->rt_mcctrl = priv->cfg.modecode;

	/* RT Time Tag resolution */
	priv->regs->rt_ttag = priv->cfg.time_res << 16;

	/* Event LOG base and size */
	priv->regs->rt_evsz = ~(priv->cfg.evlog_size - 1);
	priv->regs->rt_evlog = (unsigned int)priv->evlog_hw_base;
	priv->regs->rt_evirq = 0;

	/* Clear and old IRQ flag and Enable IRQ */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	priv->regs->irq = GR1553B_IRQ_RTEV|GR1553B_IRQ_RTD|GR1553B_IRQ_RTTE;
	priv->regs->imask |= GR1553B_IRQEN_RTEVE | GR1553B_IRQEN_RTDE |
			GR1553B_IRQEN_RTTEE;

	/* Enable and Set RT address */
	priv->regs->rt_cfg = GR1553RT_KEY |
			(priv->cfg.rtaddress << GR1553B_RT_CFG_RTADDR_BIT) |
			GR1553B_RT_CFG_RTEN;

	/* Tell software RT is started */
	priv->started = 1;
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return 0;
}

void gr1553rt_stop(void *rt)
{
	struct gr1553rt_priv *priv = rt;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);

	/* Stop Hardware */
	gr1553rt_hw_stop(priv);

	/* Software state */
	priv->started = 0;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

void gr1553rt_sa_schedule(
	void *rt,
	int subadr,
	int tx,
	struct gr1553rt_list *list
	)
{
	struct gr1553rt_priv *priv = rt;
	unsigned short bdid;
	struct gr1553rt_bd *bd;

	if ( !list || (list->listid == -1) )
		return;

	/* Get Hardware address of first descriptor in list */
	bdid = list->bds[0];
	if ( bdid == 0xffff )
		return;
	bd = &priv->bds_hw[bdid];

	list->subadr = subadr;

	/* Update Sub address table */
	if ( tx ) {
		list->subadr |= 0x100;
		priv->subadrs[subadr].txlistid = list->listid;
		priv->sas_cpu[subadr].txptr = (unsigned int)bd;
	} else {
		priv->subadrs[subadr].rxlistid = list->listid;
		priv->sas_cpu[subadr].rxptr = (unsigned int)bd;
	}
}

void gr1553rt_sa_setopts(
	void *rt,
	int subadr,
	unsigned int mask,
	unsigned int options
	)
{
	struct gr1553rt_priv *priv = rt;
	unsigned int ctrl;

	if ( (subadr > 31) || (priv->sas_cpu == NULL) )
		return;

	ctrl = priv->sas_cpu[subadr].ctrl;
	priv->sas_cpu[subadr].ctrl = (ctrl & ~mask) | options;
}

void gr1553rt_set_vecword(void *rt, unsigned int mask, unsigned int words)
{
	struct gr1553rt_priv *priv = rt;
	unsigned int vword;

	if ( mask == 0 )
		return;

	vword = priv->regs->rt_statw;

	priv->regs->rt_statw = (vword & ~mask) | (words & mask);
}

void gr1553rt_set_bussts(void *rt, unsigned int mask, unsigned int sts)
{
	struct gr1553rt_priv *priv = rt;
	unsigned int stat;

	stat = priv->regs->rt_stat2;
	priv->regs->rt_stat2 = (stat & ~mask) | (mask & sts);
}

void gr1553rt_status(void *rt, struct gr1553rt_status *status)
{
	struct gr1553rt_priv *priv = rt;
	struct gr1553b_regs *regs = priv->regs;
	unsigned int tmp;
	SPIN_IRQFLAGS(irqflags);

	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	status->status = regs->rt_stat;
	status->bus_status = regs->rt_stat2;

	tmp = regs->rt_sync;
	status->synctime = tmp >> 16;
	status->syncword = tmp & 0xffff;

	tmp = regs->rt_ttag;
	status->time_res = tmp >> 16;
	status->time = tmp & 0xffff;

	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
}

void gr1553rt_list_sa(struct gr1553rt_list *list, int *subadr, int *tx)
{
	int sa, trt;

	if ( list->subadr == -1 ) {
		sa = -1;
		trt = -1;
	} else {
		sa = list->subadr & 0xff;
		trt = (list->subadr & 0x100) >> 8;
	}

	if ( subadr )
		*subadr = sa;
	if ( tx )
		*tx = trt;
}

int gr1553rt_evlog_read(void *rt, unsigned int *dst, int max)
{
	struct gr1553rt_priv *priv = rt;
	int cnt, top, bot, left;
	unsigned int *hwpos;

	/* Get address of hardware's current working entry */
	hwpos = (unsigned int *)priv->regs->rt_evlog;

	/* Convert into CPU address */
	hwpos = (unsigned int *)
		((unsigned int)hwpos - (unsigned int)priv->evlog_hw_base +
		(unsigned int)priv->evlog_cpu_base);

	if ( priv->evlog_cpu_next == hwpos )
		return 0;

	if ( priv->evlog_cpu_next > hwpos ) {
		top = (unsigned int)priv->evlog_cpu_end -
			(unsigned int)priv->evlog_cpu_next;
		bot = (unsigned int)hwpos - (unsigned int)priv->evlog_cpu_base;
	} else {
		top = (unsigned int)hwpos - (unsigned int)priv->evlog_cpu_next;
		bot = 0;
	}
	top = top / 4;
	bot = bot / 4;

	left = max;
	if ( top > 0 ) {
		if ( top > left ) {
			cnt = left;
		} else {
			cnt = top;
		}
		memcpy(dst, priv->evlog_cpu_next, cnt*4);
		dst += cnt;
		left -= cnt;
	}

	if ( (bot > 0) && (left > 0) ) {
		if ( bot > left ) {
			cnt = left;
		} else {
			cnt = bot;
		}
		memcpy(dst, priv->evlog_cpu_base, cnt*4);
		left -= cnt;
	}

	cnt = max - left;
	priv->evlog_cpu_next += cnt;
	if ( priv->evlog_cpu_next >= priv->evlog_cpu_end ) {
		priv->evlog_cpu_next = (unsigned int *)
			((unsigned int)priv->evlog_cpu_base +
			((unsigned int)priv->evlog_cpu_next -
			 (unsigned int)priv->evlog_cpu_end ));
	}

	return max - left;
}
