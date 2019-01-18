/*
 * This file contains the RTEMS GRSLINK SLINK master driver
 *
 * COPYRIGHT (c) 2009.
 * Cobham Gaisler AB.
 *
 * The license and distribution terms for this file may be
 * found in the file LICENSE in this distribution or at
 * http://www.rtems.org/license/LICENSE.
 *
 * Comments concerning current driver implementation:
 *
 * The SLINK specification says that there are three IO cards that are capable 
 * of transmitting data. But these IO cards can have the address range 0 to 3, 
 * and an 'For information only' comment explains that the current 
 * implementation has receive buffers for ".. x 4 (IO cards)".
 * Because of this the driver has four queues, one for each IO card 0 - 3. 
 * When the addressing convention used for the IO cards is known, the number of
 * queues may be lowered to three.
 *
 */

#include <stdlib.h>

#include <bsp.h>
#include <grlib/grslink.h>
#include <grlib/ambapp.h>
#include <grlib/grlib.h>

#include <grlib/grlib_impl.h>

#ifndef GAISLER_SLINK
#define GAISLER_SLINK 0x02F
#endif

/* Enable debug output? */
/* #define DEBUG */

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...) 
#endif

/* Bits and fields in SLINK transmit word */
#define SLINK_RW (1 << 23)
#define SLINK_CHAN_POS 16

/* Local types */
typedef struct {
	volatile unsigned int clockscale;
	volatile unsigned int ctrl;
	volatile unsigned int nullwrd;
	volatile unsigned int sts;
	volatile unsigned int msk;
	volatile unsigned int abase;
	volatile unsigned int bbase;
	volatile unsigned int td;
	volatile unsigned int rd;
} SLINK_regs;

typedef struct {
	char readstat;         /* Status of READ operation */
	char seqstat;          /* Status of SEQUENCE operation */
	unsigned char scnt;    /* Number of SEQUENCE words transferred */
} SLINK_status;

typedef struct {
	int size;
	unsigned int *buf;
	unsigned int *first;
	unsigned int *last;
	unsigned int *max;
	int full;
} SLINK_queue;

typedef struct {
	SLINK_regs    *reg;     /* Pointer to core registers */
	SLINK_status  *status;  /* Driver status information */
	void          (*slink_irq_handler)(int); /* Handler for INTERRUPT */
	void          (*slink_seq_change)(int); /* Callback on SEQUENCE change */
	int           rword;    /* Placeholder for READ response */
	rtems_id      read_sem; /* Semaphore for blocking SLINK_read */
	SLINK_queue   *queues;  /* Receive queues */
#ifdef SLINK_COLLECT_STATISTICS
	SLINK_stats   *stats;   /* Core statistics, optional */
#endif
} SLINK_cfg;


static SLINK_cfg *cfg = NULL;

/**** SLINK driver queues for unsolicited and INTERRUPT requests ****/

/* Function: SLINK_createqueues
 * Arguments: size: Number of elements in each queue
 * Returns: 0 on success, -1 on failure
 * Description: Creates SLINK_NUMQUEUES queues, one for each IO card 
 * that can send data. The pointers to the queues is saved in the driver 
 * config structure.
 */
static int SLINK_createqueues(int size)
{
	SLINK_queue *q;
	int i, j;

	if ((q = grlib_malloc(SLINK_NUMQUEUES*sizeof(*q))) == NULL)
		goto slink_qiniterr1;
		
	for (i = 0; i < SLINK_NUMQUEUES; i++) {
		q[i].size = size;
		if ((q[i].buf = grlib_malloc(size*sizeof(int))) == NULL)
			goto slink_qiniterr2;
		q[i].first = q[i].last = q[i].buf;
		q[i].max = q[i].buf + (size-1);
		q[i].full = 0;
	}

       	cfg->queues = q;

	return 0;
	
 slink_qiniterr2:
	for (j = 0; j < i; j++)
		free(q[i].buf);
	free(q);
 slink_qiniterr1:
	return -1;
}

/*
 * Function: SLINK_destroyqueues
 * Arguments: None
 * Returns: Nothing
 * Description: Frees the memory occupied by the queues in cfg->queues
 */
/*
  static void SLINK_destroyqueues(void)
  {
        int i;
	
	for(i = 0; i < SLINK_NUMQUEUES; i++)
		free(cfg->queues[i].buf);

	free(cfg->queues);
}
*/

/*
 * Function: SLINK_enqueue
 * Arguments: Received SLINK word
 * Returns: Nothing
 * Description: 
 */
static void SLINK_enqueue(unsigned int slink_wrd)
{
	SLINK_queue *ioq = cfg->queues + SLINK_WRD_CARDNUM(slink_wrd); 

	if (!ioq->full && SLINK_WRD_CARDNUM(slink_wrd) < SLINK_NUMQUEUES) {
		*ioq->last = slink_wrd;
		ioq->last = (ioq->last >= ioq->max) ? ioq->buf : ioq->last+1;
		ioq->full = ioq->last == ioq->first;
		return;
	}
#ifdef SLINK_COLLECT_STATISTICS
	cfg->stats->lostwords++;
#endif
}

/**** SLINK driver helper functions ****/

/*
 * Function: SLINK_getaddr
 * Arguments: amba_conf 
 *            base: assigned to base of core registers
 *            irq: assigned to core irq lines
 * Returns: Base address and IRQ via arguments, 0 if core is found, else -1
 * Description: See above.
 */
static int SLINK_getaddr(int *base, int *irq)
{	
	struct ambapp_apb_info c;

	if (ambapp_find_apbslv(&ambapp_plb,VENDOR_GAISLER,GAISLER_SLINK,&c) == 1) {
		*base = c.start;
		*irq = c.irq;
		return 0;
	}
	return -1;
}

/* Function: SLINK_calcscaler
 * Arguments: sysfreq: System frequency in Hz
 * Returns: Clock scaler register value
 * Description: Calculates value for SLINK clock scaler register to attain
 * a SLINK bus frequency as close to 6 MHz as possible. Please see the IP core 
 * documentation for a description of how clock scaling is implemented. 
 */
static int SLINK_calcscaler(int sysfreq)
{
	int fact = sysfreq / SLINK_FREQ_HZ;
	return ((fact/2-1) << 16) | (fact % 2 ? fact/2 : fact/2-1);  
}


/*
 * Function: SLINK_getsysfreq
 * Arguments: None
 * Returns: System frequency in Hz, or 0 if system timer is not found.
 * Description: Looks at the timer to determine system frequency. Makes use
 * of AMBA Plug'n'Play. 
 */
static int SLINK_getsysfreq(void)
{
	struct ambapp_apb_info t;
	struct gptimer_regs *tregs;
		
	if (ambapp_find_apbslv(&ambapp_plb,VENDOR_GAISLER,GAISLER_GPTIMER,&t)==1) {
		tregs = (struct gptimer_regs *)t.start;
		DBG("SLINK_getsysfreq returning %d\n", 
		    (tregs->scaler_reload+1)*1000*1000);
		return (tregs->scaler_reload+1)*1000*1000;
	}
	return 0;
}

/*
 * Function: SLINK_interrupt_handler
 * Arguments: v: not used
 * Returns: Nothing
 * Description: Interrupt handles checks RNE, SEQUENCE and error status
 * bits. Reads word from receive queue and distinguishes between INTERRUPT,
 * READ responses and SLAVE-WORD-SEND. When an INTERRUPT transfer is detected
 * the handler calls the user specified slink_irq_handler with the received
 * word. READ responses are saved and given to SLINK_read via a private
 * variable. SLAVE-WORD-SEND transfers are placed in the IO card's receive
 * queue.
 */
static rtems_isr SLINK_interrupt_handler(void *v)
{
	unsigned int sts;
	unsigned int wrd;

	/* Read all words from Receive queue */
	while ((sts = cfg->reg->sts) & SLINK_S_RNE) {

		/* Read first word in receive queue */
		wrd = cfg->reg->rd;
	
		/* Check channel value to determine action */
		switch (SLINK_WRD_CHAN(wrd)) {
		case 0: /* Interrupt */
			cfg->slink_irq_handler(wrd);
#ifdef SLINK_COLLECT_STATISTICS
			cfg->stats->interrupts++;
#endif
			break;
		case 3: /* Read response, if no active READ, fall-through */
			if (cfg->status->readstat == SLINK_ACTIVE) {
				rtems_semaphore_release(cfg->read_sem);
				cfg->status->readstat = SLINK_COMPLETED;
				cfg->rword = wrd;
				break;
			}
		default: /* Unsolicited request */
			SLINK_enqueue(wrd);
			break;
		}
	}

	/* Check sequence operation */
	if (sts & SLINK_S_SC) {
		/* SEQUENCE completed */
		cfg->status->seqstat = SLINK_COMPLETED;
		if (cfg->slink_seq_change)
			cfg->slink_seq_change(SLINK_COMPLETED);
#ifdef SLINK_COLLECT_STATISTICS
		cfg->stats->seqcomp++;
#endif
	} else if (sts & SLINK_S_SA) {
		/* SEQUENCE aborted */
		cfg->status->seqstat = SLINK_ABORTED;
		cfg->status->scnt = (sts >> SLINK_S_SI_POS);
		if (cfg->slink_seq_change)
			cfg->slink_seq_change(SLINK_ABORTED);
	}

	/* Check error conditions */
	if (sts & SLINK_S_PERR) {
		/* 
		   Parity error detected, set seqstat if there is an ongoing 
		   sequence so that the calling application can decide if the
		   sequence should be aborted 
		*/
		if (cfg->status->seqstat == SLINK_ACTIVE) {
			cfg->status->seqstat = SLINK_PARERR;
			if (cfg->slink_seq_change)
				cfg->slink_seq_change(SLINK_PARERR);
		}
		/* Abort READ operation */
		if (cfg->status->readstat == SLINK_ACTIVE) {
			cfg->status->readstat = SLINK_PARERR;
			rtems_semaphore_release(cfg->read_sem);
		}
#ifdef SLINK_COLLECT_STATISTICS
		cfg->stats->parerr++;
#endif
	} 
	if (sts & SLINK_S_AERR) {
		/* AMBA error response, sequence aborted */
		cfg->status->seqstat = SLINK_AMBAERR;
		cfg->status->scnt = sts >> SLINK_S_SI_POS;
		if (cfg->slink_seq_change)
			cfg->slink_seq_change(SLINK_AMBAERR);
	}
	if (sts & SLINK_S_ROV) {
		/* Receive overflow, abort any ongoing READ */ 
		if (cfg->status->readstat == SLINK_ACTIVE) {
			cfg->status->readstat = SLINK_ROV;
			rtems_semaphore_release(cfg->read_sem);
		}
#ifdef SLINK_COLLECT_STATISICS
		cfg->status->recov++;
#endif
	}

	/* Clear processed bits */
	cfg->reg->sts = sts;
}

/**** SLINK driver interface starts here ****/

/* Function: SLINK_init
 * Arguments: nullwrd: NULL word
 *            parity: Even (0) or Odd (1) parity
 *            interrupt_trans_handler: Function that handles interrupt requests
 *            sequence_callback: Callback on SEQUENCE status changes
 *            qsize: Size of each receive queue
 * Returns: 0 on success, -1 on failure
 * Description: Initializes the SLINK core
 */
int SLINK_init(unsigned int nullwrd, int parity, int qsize,
	       void (*interrupt_trans_handler)(int),
	       void (*sequence_callback)(int))
{
	int base;
	int irq;
	rtems_status_code st;

	/* Allocate private config structure */
	if (cfg == NULL && (cfg = grlib_malloc(sizeof(*cfg))) == NULL) {
		DBG("SLINK_init: Could not allocate cfg structure\n");
		goto slink_initerr1;
	}
	
	/* Create simple binary semaphore for blocking SLINK_read */
	st = rtems_semaphore_create(rtems_build_name('S', 'L', 'R', '0'), 0, 
				    (RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|
				     RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|
				     RTEMS_NO_PRIORITY_CEILING), 0,
				    &cfg->read_sem);
	if (st != RTEMS_SUCCESSFUL) {
		DBG("SLINK_init: Could not create semaphore\n");
		goto slink_initerr1;
	}
  
	/* Initialize pointer to SLINK core registers and get IRQ line */
	if (SLINK_getaddr(&base, &irq) == -1) {
		DBG("SLINK_init: Could not find core\n");
		goto slink_initerr2;
	}
	cfg->reg = (SLINK_regs*)base;

	/* Allocate status structure and initialize members */
	if ((cfg->status = grlib_calloc(1, sizeof(*cfg->status))) == NULL) {
		DBG("SLINK_init: Could not allocate status structure\n");
		goto slink_initerr2;
	}
	cfg->status->seqstat = SLINK_COMPLETED;
	cfg->status->readstat = SLINK_COMPLETED;

#ifdef SLINK_COLLECT_STATISTICS
	/* Allocate statistics structure and initialize members */
	if ((cfg->stats = grlib_calloc(1, sizeof(*cfg->stats))) == NULL) {
		DBG("SLINK_init: Could not allocate statistics structure\n");
		goto slink_initerr3;
	}
#endif

	/* Allocate and initialize queues */
	if (SLINK_createqueues(qsize) == -1) {
		DBG("SLINK_init: Could not create queues\n");
		goto slink_initerr3;
	}

	/* Configure core registers */
	cfg->reg->clockscale = SLINK_calcscaler(SLINK_getsysfreq());
	cfg->reg->ctrl = parity ? SLINK_C_PAR : 0;
	cfg->reg->nullwrd = nullwrd;
	cfg->reg->msk = (SLINK_M_PERRE | SLINK_M_AERRE | SLINK_M_ROVE |
			 SLINK_M_RNEE | SLINK_M_SAE | SLINK_M_SCE);

	/* Set-up INTERRUPT transfer handling */
	cfg->slink_irq_handler = interrupt_trans_handler;

	/* Save SEQUENCE callback */
	cfg->slink_seq_change = sequence_callback;

	/* Set-up IRQ handling */
	rtems_interrupt_handler_install(irq, "slink",
			RTEMS_INTERRUPT_SHARED,
			SLINK_interrupt_handler, NULL);
	
	return 0;

 slink_initerr3:
	free(cfg->status);
 slink_initerr2:
	free(cfg);
 slink_initerr1:
	return -1;
}

/* Function: SLINK_start
 * Description: Enables the core
 */
void SLINK_start(void)
{   
	if (cfg != NULL)
		cfg->reg->ctrl |= SLINK_C_SLE;
}

/* Function: SLINK_stop
 * Description: Disables the core
 */
void SLINK_stop(void)
{
	if (cfg != NULL)
		cfg->reg->ctrl &= ~SLINK_C_SLE;
}

/*
 * Function: SLINK_read
 * Arguments: data: Payload of data word
 *            channel: -
 *            reply: Reply from IO card
 * Returns: 0 on success
 *          -(SLINK_PARERR, SLINK_ROV) on error or -SLINK_QFULL if transmit queue
 *          is full and software should try again.
 * Description: Reads one word and returns the response in *reply unless there
 *              is an error. This function blocks until the READ operation is
 *              completed or aborted.
 */
int SLINK_read(int data, int channel, int *reply)
{	
	DBG("SLINK_read: called..");

	if (cfg->reg->sts & SLINK_S_TNF) {
		cfg->status->readstat = SLINK_ACTIVE;
		cfg->reg->td = SLINK_RW | channel << SLINK_CHAN_POS | data;
	} else {
		DBG("queue FULL\n");
		return -SLINK_QFULL; /* Transmit queue full */
	}

	/* Block until the operation has completed or has been aborted */
	rtems_semaphore_obtain(cfg->read_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT);

	if (cfg->status->readstat == SLINK_COMPLETED) {
		*reply = cfg->rword; 
#ifdef SLINK_COLLECT_STATISTICS
		cfg->stats->reads++;
#endif       
		DBG("returning 0\n");
		return 0;
	} else {
		DBG("returning error code\n");
		return -cfg->status->readstat;
	}
}

/*
 * Function: SLINK_write
 * Arguments: data: Payload of SLINK data word
 *            channel: Channel value (bits 22 downto 16) of receive 
 *                     register word
 * Returns: 0 if command was placed in transmit queue
 *          -SLINK_QFULL if transmit queue was full (software should retry)
 * Description: See above.
 */
int SLINK_write(int data, int channel)
{ 
	if (cfg->reg->sts & SLINK_S_TNF) {
		cfg->reg->td = channel << SLINK_CHAN_POS | data;
#ifdef SLINK_COLLECT_STATISTICS
		cfg->stats->writes++;
#endif
		return 0;
	}

	return -SLINK_QFULL;	
}

/*
 * Function: SLINK_sequence
 * Arguments: a: Array containing sequence commands
 *            b: Array where SEQUENCE responses will be stored
 *            n: Number of commands in a array
 *            channel: Sequence Channel Number
 *            reconly: Set to 1 if the SEQUENCE operation is receive only
 * Returns: 0 if SEQUENCE could be started (SUCCESS)
 *          -1 if SEQUNCE was not started due to ongoing SEQUENCE
 */
int SLINK_seqstart(int *a, int *b, int n, int channel, int reconly)
{  
	/* Only start a new SEQUENCE of the former SEQUENCE has completed */
	if (cfg->status->seqstat == SLINK_ACTIVE || 
	    cfg->status->seqstat == SLINK_PARERR)
		return -1;

	/* Tell core about arrays */
	cfg->reg->abase = (int)a;
	cfg->reg->bbase = (int)b;
	
	/* As far as software is concerned the sequence is now active */
	cfg->status->seqstat = SLINK_ACTIVE;

	/* Enable SEQUENCE operation with SCN = channel and SLEN = n-1 */
        if (reconly == 1) {
           cfg->reg->ctrl = (((n-1) << SLINK_C_SLEN_POS) | SLINK_C_SRO |
                             (channel << SLINK_C_SCN_POS) |
                             SLINK_C_SE | (cfg->reg->ctrl & 0xC000000F));
        } else {
           cfg->reg->ctrl = (((n-1) << SLINK_C_SLEN_POS) |
                             (channel << SLINK_C_SCN_POS) |
                             SLINK_C_SE | (cfg->reg->ctrl & 0xC000000F));
        }

#ifdef SLINK_COLLECT_STATISTICS
	cfg->stats->sequences++;
#endif

	return 0;
}


/* Function: SLINK_seqabort
 * Description: This function aborts an ongoing SEQUENCE. Software can tell
 * when the SEQUENCE is aborted by polling SLINK_seqstat().
 */
void SLINK_seqabort(void)
{
	cfg->reg->ctrl = cfg->reg->ctrl | SLINK_C_AS;
}


/*
 * Function: SLINK_seqstatus
 * Returns: The current or status of the SEQUENCE operation:
 *          SLINK_COMPLETED, SLINK_ACTIVE, SLINK_PARERR, SLINK_AMBAERR,
 *          SLINK_ABORTED (these are defined in bsp/grslink.h)
 * Description: Meaning of returned values:
 *              SLINK_ABORTED: Aborted before all operations completed.
 *              SLINK_ACTIVE: The core is busy processing the SEQUENCE
 *              SLINK_AMBAERR: The last SEQUENCE was aborted by an AMBA ERROR
 *              SLINK_COMPLETED: All words were transferred in the last SEQUENCE
 *              SLINK_PARERR: Parity error detected. Software may want to abort
 *
 *              If the SEQUENCE was aborted SLINK_seqwrds() can be used to
 *              determine the number of completed operations.
 */
int SLINK_seqstatus(void)
{
	return cfg->status->seqstat;
}

/*
 * Function: SLINK_seqwrds
 * Returns: -1 for ongoing sequence
 *          0 if all words were transferred in the last sequence
 *          number of words if the last SEQUENCE did not complete 
 *          (SLINK_AMBAERR or SLINK_ABORTED is reported ny SLINK_seqstatus()) 
 */
int SLINK_seqwrds(void)
{
	switch (cfg->status->seqstat) {
	case SLINK_COMPLETED: return 0;
	case SLINK_ACTIVE | SLINK_PARERR: return -1;
	default: return cfg->status->scnt;
	}
}

/* 
 * Function: SLINK_hwstatus
 * Returns: The SLINK core's status register. The register values can be 
 *          interpreted with the help of macros defined in bsp/grslink.h.
 */
int SLINK_hwstatus(void)
{
	return cfg->reg->sts;
}

/*
 * Function: SLINK_queuestatus
 * Arguments: iocard: Queue which to check status for
 * Returns: Number of elements in queue or -1 on non-existent queue
 * Description: SLINK_queuestatus(queue) returns the number of elements in
 *              queue 'iocard'
 */
int SLINK_queuestatus(int iocard)
{	
	unsigned int first, last;
	SLINK_queue *ioq;
	
	if (iocard >= SLINK_NUMQUEUES)
		return -1;

	ioq = cfg->queues + iocard;

	if (ioq->full)
		return ioq->size;
	if (ioq->first == ioq->last)
		return 0;

	first = ((unsigned int)ioq->first)/sizeof(unsigned int);
	last = ((unsigned int)ioq->last)/sizeof(unsigned int);
	
	return first < last ? last - first : ioq->size - first + last; 
}

/*
 * Function: SLINK_dequeue
 * Arguments: iocard: IO card number
 *            elem: First element in IO card queue
 * Returns: 0 on success or -1 on empty or non-existent queue
 * Description: 
 */
int SLINK_dequeue(int iocard, int *elem)
{	
	if (iocard >= SLINK_NUMQUEUES)
		return -1;
	
	SLINK_queue *ioq = cfg->queues + iocard;
	
	if (ioq->last != ioq->first || ioq->full) {
		*elem = *ioq->first;
		ioq->first = (ioq->first >= ioq->max) ? ioq->buf : ioq->first+1;
		ioq->full = 0;
		return 0;
	}
	return -1;
}

/*
 * Function: SLINK_statistics
 * Returns: If the core has statistics colletion enabled this function returns
 * a pointer to a struct containing statistics information, otherwise NULL.
 */
SLINK_stats *SLINK_statistics(void)
{
#ifdef SLINK_COLLECT_STATISTICS
	return cfg->stats;
#else
	return NULL;
#endif
}
