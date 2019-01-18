/*
 *  GRPCI2 DMA Driver
 *
 *  COPYRIGHT (c) 2017
 *  Cobham Gaisler AB
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <string.h>
#include <stddef.h>
#include <drvmgr/drvmgr.h>
#include <rtems.h>
#include <rtems/bspIo.h>  /* for printk */
#include <bsp.h>
#include <grlib/grpci2dma.h>

#include <grlib/grlib_impl.h>

/* This driver has been prepared for SMP operation
 */

/*#define STATIC*/
#define STATIC static

/*#define INLINE*/
#define INLINE inline

/*#define UNUSED*/
#define UNUSED __attribute__((unused))

/*#define DEBUG 1*/

#ifdef DEBUG
#define DBG(x...) printf(x)
#else
#define DBG(x...) 
#endif

#define BD_CHAN_EN (1<<BD_CHAN_EN_BIT)
#define BD_CHAN_ID (0x3<<BD_CHAN_ID_BIT)
#define BD_CHAN_TYPE (0x3<<BD_CHAN_TYPE_BIT)
#define BD_CHAN_TYPE_DMA (0x1<<BD_CHAN_TYPE_BIT)
#define BD_CHAN_BDCNT (0xffff<<BD_CHAN_BDCNT_BIT)
#define BD_CHAN_EN_BIT 31
#define BD_CHAN_ID_BIT 22
#define BD_CHAN_TYPE_BIT 20
#define BD_CHAN_BDCNT_BIT 0

#define BD_DATA_EN (0x1<<BD_DATA_EN_BIT)
#define BD_DATA_IE (0x1<<BD_DATA_IE_BIT)
#define BD_DATA_DR (0x1<<BD_DATA_DR_BIT)
#define BD_DATA_BE (0x1<<BD_DATA_BE_BIT)
#define BD_DATA_TYPE (0x3<<BD_DATA_TYPE_BIT)
#define BD_DATA_TYPE_DATA (0x0<<BD_DATA_TYPE_BIT)
#define BD_DATA_ER (0x1<<BD_DATA_ER_BIT)
#define BD_DATA_LEN (0xffff<<BD_DATA_LEN_BIT)
#define BD_DATA_EN_BIT 31
#define BD_DATA_IE_BIT 30
#define BD_DATA_DR_BIT 29
#define BD_DATA_BE_BIT 28
#define BD_DATA_TYPE_BIT 20
#define BD_DATA_ER_BIT 19
#define BD_DATA_LEN_BIT 0

#define DMACTRL_SAFE (0x1<<DMACTRL_SAFE_BIT)
#define DMACTRL_WCLEAR (0x1fff<<DMACTRL_ERR_BIT)
#define DMACTRL_ERR (0x1f<<DMACTRL_ERR_BIT)
#define DMACTRL_CHIRQ (0xff<<DMACTRL_CHIRQ_BIT)
#define DMACTRL_ERR (0x1f<<DMACTRL_ERR_BIT)
#define DMACTRL_NUMCH (0x7<<DMACTRL_NUMCH_BIT)
#define DMACTRL_DIS (0x1<<DMACTRL_DIS_BIT)
#define DMACTRL_IE (0x1<<DMACTRL_IE_BIT)
#define DMACTRL_ACT (0x1<<DMACTRL_ACT_BIT)
#define DMACTRL_EN (0x1<<DMACTRL_EN_BIT)

#define DMACTRL_SAFE_BIT 31
#define DMACTRL_CHIRQ_BIT 12
#define DMACTRL_ERR_BIT 7
#define DMACTRL_NUMCH_BIT 4
#define DMACTRL_DIS_BIT 2
#define DMACTRL_IE_BIT 1
#define DMACTRL_ACT_BIT 3
#define DMACTRL_EN_BIT 0

/* GRPCI2 DMA does not allow more than 8 DMA chans */
#define MAX_DMA_CHANS 8

/* GRPCI2 DMA does not allow transfer of more than 0x10000 words */
#define MAX_DMA_TRANSFER_SIZE (0x10000*4)

/* We use the following limits as default */ 
#define MAX_DMA_DATA 128

/* Memory and HW Registers Access routines. All 32-bit access routines */
#define BD_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
/*#define BD_READ(addr) (*(volatile unsigned int *)(addr))*/
#define BD_READ(addr) grlib_read_uncached32((unsigned long)(addr))
#define REG_WRITE(addr, val) (*(volatile unsigned int *)(addr) = (unsigned int)(val))
#define REG_READ(addr) (*(volatile unsigned int *)(addr))

/*
 * GRPCI2 DMA Channel descriptor
 */
struct grpci2_bd_chan {
	volatile unsigned int ctrl;	/* 0x00 DMA Control */
	volatile unsigned int nchan;	/* 0x04 Next DMA Channel Address */
	volatile unsigned int nbd;	/* 0x08 Next Data Descriptor in channel */
	volatile unsigned int res;	/* 0x0C Reserved */
};

/*
 * GRPCI2 DMA Data descriptor
 */
struct grpci2_bd_data {
	volatile unsigned int ctrl;	/* 0x00 DMA Data Control */
	volatile unsigned int pci_adr;	/* 0x04 PCI Start Address */
	volatile unsigned int ahb_adr;	/* 0x08 AHB Start address */
	volatile unsigned int next;	/* 0x0C Next Data Descriptor in channel */
};


/*
 * GRPCI2 DMA APB Register MAP
 */
struct grpci2dma_regs {
	volatile unsigned int dma_ctrl;		/* 0x00 */
	volatile unsigned int dma_bdbase;	/* 0x04 */
	volatile unsigned int dma_chact;	/* 0x08 */
};

#define DEVNAME_LEN 11
/*
 * GRPCI2 DMA Driver private data struture
 */
struct grpci2dma_priv {
	/* DMA control registers */
	struct grpci2dma_regs	*regs;
	char devname[DEVNAME_LEN];

	/* Channel info */
	struct {
		/* Channel pointer. Indicates the assigned channel 
		 * for a given cid (used as index). NULL if not assigned.
		 */
		struct grpci2_bd_chan * ptr;
		/* Is this channel allocated by the driver */
		int allocated;
		/* Last added data descriptor for each channel.
		 * This simplifies/speeds up adding data descriptors
		 * to the channel*/
		struct grpci2_bd_data * lastdata;
		/* Is this channel active */
		int active;
		/* Interrupt-code Handling 
		 * - isr: Holds the ISR for each channel
		 * - isr_arg: Holds the ISR arg for each channel
		 */
		grpci2dma_isr_t isr;
		void * isr_arg;

		/* DMA Channel Semaphore */
		rtems_id sem;
	} channel[MAX_DMA_CHANS];

	/* Indicates the number of channels. */
	int nchans;

	/* Indicates the number of active channels. */
	int nactive;

	/* Indicates if the number of DMA ISR that have been registered 
	 * into the GRPCI2 DRIVER */
	int isr_registered;

	/* Callback to register the DMA ISR into the GRPCI2 DRIVER */
	void (*isr_register)( void (*isr)(void*), void * arg);

	/* Spin-lock ISR protection */
	SPIN_DECLARE(devlock);
};

/* The GRPCI2 DMA semaphore */
rtems_id grpci2dma_sem;

/*
 * GRPCI2 DMA internal prototypes 
 */
/* -Descriptor linked-list functions*/
STATIC int grpci2dma_channel_list_add(struct grpci2_bd_chan * list, 
		struct grpci2_bd_chan * chan);
STATIC int grpci2dma_channel_list_remove(struct grpci2_bd_chan * chan);
STATIC int grpci2dma_data_list_add(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data, struct grpci2_bd_data * last_chan_data);
STATIC int grpci2dma_data_list_remove(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data);
STATIC int grpci2dma_channel_list_foreach(struct grpci2_bd_chan * chan, 
		int func( struct grpci2_bd_chan * chan), int maxindex);
STATIC int grpci2dma_data_list_foreach(struct grpci2_bd_data * data, 
		int func( struct grpci2_bd_data * data), int maxindex);

/* -DMA ctrl access functions */
STATIC INLINE int grpci2dma_ctrl_init(void);
STATIC INLINE int grpci2dma_ctrl_start(struct grpci2_bd_chan * chan);
STATIC INLINE int grpci2dma_ctrl_stop(void);
STATIC INLINE int grpci2dma_ctrl_resume(void);
STATIC INLINE unsigned int grpci2dma_ctrl_status(void);
STATIC INLINE unsigned int grpci2dma_ctrl_base(void);
STATIC INLINE unsigned int grpci2dma_ctrl_active(void);
STATIC INLINE int grpci2dma_ctrl_numch_set(int numch);
STATIC INLINE int grpci2dma_ctrl_interrupt_status(void);
STATIC INLINE int grpci2dma_ctrl_interrupt_enable(void);
STATIC INLINE int grpci2dma_ctrl_interrupt_disable(void);
STATIC INLINE int grpci2dma_ctrl_interrupt_clear(void);

/* -Descriptor access functions */
STATIC int grpci2dma_channel_bd_init(struct grpci2_bd_chan * chan);
STATIC int grpci2dma_data_bd_init(struct grpci2_bd_data * data, 
		uint32_t pci_adr, uint32_t ahb_adr, int dir, int endianness, 
		int size, struct grpci2_bd_data * next);
STATIC int grpci2dma_channel_bd_enable(struct grpci2_bd_chan * chan, 
		unsigned int options);
STATIC int grpci2dma_channel_bd_disable(struct grpci2_bd_chan * chan);
STATIC void grpci2dma_channel_bd_set_cid(struct grpci2_bd_chan * chan, 
		int cid);
STATIC int grpci2dma_channel_bd_get_cid(struct grpci2_bd_chan * chan);
STATIC int grpci2dma_data_bd_status(struct grpci2_bd_data *data);
STATIC int grpci2dma_data_bd_disable(struct grpci2_bd_data *desc);
STATIC int grpci2dma_data_bd_interrupt_enable(struct grpci2_bd_data * data);
STATIC struct grpci2_bd_data * grpci2dma_channel_bd_get_data(
		struct grpci2_bd_chan * chan);
STATIC void grpci2dma_channel_bd_set_data(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data);
STATIC struct grpci2_bd_chan * grpci2dma_channel_bd_get_next(
		struct grpci2_bd_chan * chan);
STATIC struct grpci2_bd_data * grpci2dma_data_bd_get_next(
		struct grpci2_bd_data * data);
STATIC void grpci2dma_channel_bd_set_next(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_chan * next);
STATIC void grpci2dma_data_bd_set_next(struct grpci2_bd_data * data, 
		struct grpci2_bd_data * next);

/* -Channel functions */
STATIC int grpci2dma_channel_open(struct grpci2_bd_chan * chan, int cid);
STATIC int grpci2dma_channel_free_id(void);
STATIC struct grpci2_bd_chan * grpci2dma_channel_get_active_list(void);
STATIC int grpci2dma_channel_start(int chan_no, int options);
STATIC int grpci2dma_channel_stop(int chan_no);
STATIC int grpci2dma_channel_push(int chan_no, void *dataptr, int index, 
		int ndata);
STATIC int grpci2dma_channel_close(int chan_no);
STATIC int grpci2dma_channel_isr_unregister(int chan_no);

/* -ISR functions*/
STATIC void grpci2dma_isr(void *arg);

/* -Init function called by GRPCI2*/
int grpci2dma_init(void * regs, 
		void isr_register( void (*isr)(void*), void * arg));


#ifdef DEBUG
STATIC int grpci2dma_channel_print(struct grpci2_bd_chan * chan);
STATIC int grpci2dma_data_print(struct grpci2_bd_data * data);
#endif

static struct grpci2dma_priv *grpci2dmapriv = NULL;

/* All data linked list must point to a disabled descriptor at the end.
 * We use this DISABLED_DESCRIPTOR as a list end for all channels.
 */
#define ALIGNED __attribute__((aligned(GRPCI2DMA_BD_DATA_ALIGN)))
static ALIGNED struct grpci2_bd_data disabled_data = {
	/*.ctrl=*/0,
	/*.pci_adr=*/0,
	/*.ahb_adr=*/0,
	/*.next=*/0
};
#define DISABLED_DESCRIPTOR (&disabled_data)

/*** START OF DESCRIPTOR LINKED-LIST HELPER FUNCTIONS ***/

/* This functions adds a channel descriptor to the DMA channel 
 * linked list. It assumes that someone has check the input 
 * parameters already.
 */
STATIC int grpci2dma_channel_list_add(struct grpci2_bd_chan * list, 
		struct grpci2_bd_chan * chan)
{
	DBG("Adding channel (0x%08x) to GRPCI2 DMA driver\n", (unsigned int) chan);

	/* Add channel to the linnked list */
	if (list == chan) {
		/* No previous channels. Finish. */
		return GRPCI2DMA_ERR_OK;
	} else {
		/* Get next chan from list */
		struct grpci2_bd_chan * nchan = grpci2dma_channel_bd_get_next(list);
		/* Close the circular linked list */
		grpci2dma_channel_bd_set_next(chan,nchan);
		/* Attach the new channel in the middle */
		grpci2dma_channel_bd_set_next(list, chan);
		return GRPCI2DMA_ERR_OK;
	}
}

/* This functions removes a channel descriptor from the DMA channel 
 * linked list. It assumes that someone has check the input 
 * parameters already.
 * It returns 0 if successfull. Otherwise,
 * it can return:
 * - ERROR: Different causes:
 *	  x Number of channels is corrupted.
 */
STATIC int grpci2dma_channel_list_remove(struct grpci2_bd_chan * chan)
{
	DBG("Removing channel (0x%08x) from GRPCI2 DMA driver\n",
			(unsigned int) chan);

	/* Remove channel from the linnked list */
	struct grpci2_bd_chan * nchan = grpci2dma_channel_bd_get_next(chan);
	if (nchan != chan){
		/* There are more channels */
		/* Since this is a circular linked list, we need to find last channel 
		 * and update the pointer to the next element */
		/* Use index to avoid having an infinite loop in case of corrupted 
		 * channels */
		struct grpci2_bd_chan * new_first_chan = nchan;
		struct grpci2_bd_chan * curr_chan;
		int i=1;
		while((nchan != chan) && (i<MAX_DMA_CHANS)){
			curr_chan = nchan;
			nchan = grpci2dma_channel_bd_get_next(curr_chan);
			i++;
		}
		if (nchan != chan) {
			DBG("Maximum DMA channels exceeded. Maybe corrupted?\n");
			return GRPCI2DMA_ERR_ERROR;
		} else {
			/* Update the pointer */
			grpci2dma_channel_bd_set_next(curr_chan, new_first_chan);
			return GRPCI2DMA_ERR_OK;
		}
	}else{
		/* There are no more channels */
		return GRPCI2DMA_ERR_OK;
	}
}

/* This functions adds a data descriptor to the channel's data 
 * linked list. The function assumes, that the data descriptor
 * points to either a DISABLED_DESCRIPTOR or to linked list of
 * data descriptors that ends with a DISABLED_DESCRIPTOR. 
 * It returns the number of active data descriptors
 * if successfull. Otherwise, it can return:
 * - ERROR: Different causes:
 *	  x Number of channels is corrupted.
 *	  x Last linked list element is not pointing to the first.
 */
STATIC int grpci2dma_data_list_add(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data, struct grpci2_bd_data * last_chan_data)
{
	DBG("Adding data (0x%08x) to channel (0x%08x)\n", 
			(unsigned int) data, (unsigned int) chan);

	/* Add data to the linnked list */
	/* 1st- Get current data */
	struct grpci2_bd_data * first_data = grpci2dma_channel_bd_get_data(chan);
	if (first_data == NULL) {
		/* Channel should always be pointing to a disabled descriptor */
		DBG("Channel not pointing to disabled descpriptor\n");
		return GRPCI2DMA_ERR_ERROR;
	} else if (first_data == DISABLED_DESCRIPTOR){
		/* No previous data. Assign this one and finish. */
		grpci2dma_channel_bd_set_data(chan, data);
		return GRPCI2DMA_ERR_OK;
	} else {
		/* Let's add the data to the last data pointer added to this channel */
		/* Attach the new data */
		grpci2dma_data_bd_set_next(last_chan_data, data);
		/* 2nd- Let's check again to make sure that the DMA did not finished
		 * while we were inserting the new data */
		first_data = grpci2dma_channel_bd_get_data(chan);
		if (first_data == DISABLED_DESCRIPTOR){
			grpci2dma_channel_bd_set_data(chan, data);
		}
		return GRPCI2DMA_ERR_OK;
	}
}

/* This functions removes a data descriptor from the channel's data 
 * linked list. Note that in a normal execution, the DMA will remove 
 * the data descriptors from the linked list, so there is no need to
 * use this function. It returns 0 if successfull. Otherwise, 
 * it can return:
 * - WRONGPTR: The chan (or data) pointer is either NULL or not aligned to
 *	 0x10.
 * - STOPDMA: The DMA is running, cannot add channels while DMA is running.
 * - TOOMANY: The max number of data is reached.
 * - ERROR: Different causes:
 *	  x There are no free channel id numbers.
 *	  x Number of channels is corrupted.
 *	  x Last linked list element is not pointing to the first.
 */
UNUSED STATIC int grpci2dma_data_list_remove(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data)
{
	DBG("Removing data (0x%08x) from channel (0x%08x)\n", 
			(unsigned int) data, (unsigned int) chan);

	/* Remove data from the linked list */
	/* 1st- Get current DMA data */
	struct grpci2_bd_data * first_data = grpci2dma_channel_bd_get_data(chan);
	if (first_data == NULL) {
		/* Channel should always be pointing to a disabled descriptor */
		DBG("Channel not pointing to disabled descpriptor\n");
		return GRPCI2DMA_ERR_ERROR;
	} else if (first_data == DISABLED_DESCRIPTOR){
		/* No previous data. Cannot detach */
		DBG("No data to detach.\n");
		return GRPCI2DMA_ERR_NOTFOUND;
	} else {
		/* 2nd- Already available data, let's find the data */
		if (first_data == data) {
			/* 3rd- It is the first one. */
			struct grpci2_bd_data *current = first_data;
			struct grpci2_bd_data *next = grpci2dma_data_bd_get_next(current);
			if (next != DISABLED_DESCRIPTOR){
				/* There are more data */
				/* Set channel next data descriptor to data*/
				grpci2dma_channel_bd_set_data(chan, next);
				/* Update the removed data */
				grpci2dma_data_bd_set_next(data, DISABLED_DESCRIPTOR);
				return GRPCI2DMA_ERR_OK;
			}else{
				/* No more data */
				/* Clear DMA NBD */
				grpci2dma_channel_bd_set_data(chan, DISABLED_DESCRIPTOR);
				/* Update the removed data */
				grpci2dma_data_bd_set_next(data, DISABLED_DESCRIPTOR);
				return GRPCI2DMA_ERR_OK;
			}
		} else {
			/* It is not the first data. Let's find it */
			struct grpci2_bd_data * current = first_data;
			struct grpci2_bd_data * next = grpci2dma_data_bd_get_next(current);
			while( (next != data) && (next != DISABLED_DESCRIPTOR) && 
					(next != NULL)){
				current = next;
				next = grpci2dma_data_bd_get_next(current);
			}
			if (next != data) {
				DBG("Maximum DMA data exceeded. Maybe corrupted?\n");
				return GRPCI2DMA_ERR_NOTFOUND;
			} else {
				/* Detach the data */
				next = grpci2dma_data_bd_get_next(data);
				grpci2dma_data_bd_set_next(current, next);
				/* Update the removed data */
				grpci2dma_data_bd_set_next(data, DISABLED_DESCRIPTOR);
				return GRPCI2DMA_ERR_OK;
			}
		}
	}
}

/* Iterate through all channel starting in FIRST_CHAN up to MAXINDEX 
 * and execute FUNC*/
UNUSED STATIC int grpci2dma_channel_list_foreach(
		struct grpci2_bd_chan * first_chan, 
		int func( struct grpci2_bd_chan * chan), int maxindex)
{
	if (maxindex <= 0) return 0;
	if (first_chan == NULL) {
		/* No previous channels */
		return 0;
	} else {
		/* Available channels */
		/* Iterate through next channels */
		/* Use index to avoid having an infinite loop in case of corrupted
		 * channels */
		int i=0;
		int ret;
		struct grpci2_bd_chan * curr_chan = first_chan;
		struct grpci2_bd_chan * nchan;
		do{
			if (curr_chan == NULL) return GRPCI2DMA_ERR_WRONGPTR;
			ret = func(curr_chan);
			if (ret < 0){
				/* error */
				return ret;
			}
			nchan = grpci2dma_channel_bd_get_next(curr_chan);
			curr_chan = nchan;
			i++;
		}while((curr_chan != first_chan) && (i < maxindex));
	}
	return 0;
}

/* Iterate through all data starting in FIRST_DATA up to MAXINDEX 
 * and execute FUNC*/
STATIC int grpci2dma_data_list_foreach(struct grpci2_bd_data * first_data, 
		int func( struct grpci2_bd_data * data), int maxindex)
{
	if (maxindex <= 0) return 0;
	if (first_data == NULL) return GRPCI2DMA_ERR_WRONGPTR;
	/* Available data */
	/* Iterate through next data */
	/* Use index to avoid having an infinite loop in case of corrupted 
	 * channels */
	int i=0;
	int ret;
	struct grpci2_bd_data * curr_data = first_data;
	struct grpci2_bd_data * ndata;
	while((curr_data != DISABLED_DESCRIPTOR) && (i < maxindex)){
		if (curr_data == NULL) return GRPCI2DMA_ERR_WRONGPTR;
		ret = func(curr_data);
		if (ret < 0){
			/* error */
			return ret;
		}
		ndata = grpci2dma_data_bd_get_next(curr_data);
		curr_data = ndata;
		i++;
	}
	return 0;
}


/*** END OF DESCRIPTOR LINKED-LIST HELPER FUNCTIONS ***/

/*** START OF DMACTRL ACCESS FUNCTIONS ***/

/* Initialize the DMA Ctrl*/
STATIC INLINE int grpci2dma_ctrl_init()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Clear DMA Control: clear IRQ and ERR status */
	REG_WRITE(&priv->regs->dma_ctrl, 0|DMACTRL_SAFE|DMACTRL_CHIRQ|DMACTRL_ERR);

	/* Clear DMA BASE */
	REG_WRITE(&priv->regs->dma_bdbase, 0);

	/* Clear DMA Chan */
	REG_WRITE(&priv->regs->dma_chact, 0);

	return 0;
}


/* Stop the DMA */
STATIC INLINE int grpci2dma_ctrl_stop( void )
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Stop DMA */
	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	REG_WRITE(&priv->regs->dma_ctrl, (ctrl & ~(DMACTRL_WCLEAR | DMACTRL_EN)) |
			DMACTRL_DIS);

	return 0;
}

/* Start the DMA */
STATIC INLINE int grpci2dma_ctrl_start( struct grpci2_bd_chan * chan)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Set BDBASE to linked list of chans */
	REG_WRITE(&priv->regs->dma_bdbase, (unsigned int) chan);

	/* Start DMA */
	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	REG_WRITE(&priv->regs->dma_ctrl, (ctrl & ~(DMACTRL_WCLEAR | DMACTRL_DIS)) |
			DMACTRL_EN);

	return 0;
}

/* Resume the DMA */
STATIC INLINE int grpci2dma_ctrl_resume( void )
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Resume DMA */
	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	REG_WRITE(&priv->regs->dma_ctrl, (ctrl & ~(DMACTRL_WCLEAR | DMACTRL_DIS)) |
			DMACTRL_EN);

	return 0;
}

/* Interrupt status*/
STATIC INLINE int grpci2dma_ctrl_interrupt_status(void)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	return (ctrl & DMACTRL_IE);
}

/* Enable interrupts */
STATIC INLINE int grpci2dma_ctrl_interrupt_enable(void)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	if (ctrl & DMACTRL_IE){
		/* Nothing to do. Already enabled */
		return 0;
	}

	/* Clear pending CHIRQ and errors */
	ctrl = ctrl | (DMACTRL_CHIRQ | DMACTRL_ERR);

	/* Enable interrupts */
	ctrl = ctrl | DMACTRL_IE;

	REG_WRITE(&priv->regs->dma_ctrl, ctrl );
	return 0;
}

/* Disable interrupts */
STATIC INLINE int grpci2dma_ctrl_interrupt_disable(void)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	if ((ctrl & DMACTRL_IE) == 0){
		/* Nothing to do. Already disabled */
		return 0;
	}

	/* Clear pending CHIRQ and errors */
	ctrl = ctrl | (DMACTRL_CHIRQ | DMACTRL_ERR);

	/* Disable interrupts */
	ctrl = ctrl & ~(DMACTRL_IE);

	REG_WRITE(&priv->regs->dma_ctrl, ctrl );
	return 0;
}

/* Clear interrupts */
STATIC INLINE int grpci2dma_ctrl_interrupt_clear(void)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);
	REG_WRITE(&priv->regs->dma_ctrl, (ctrl | DMACTRL_ERR | DMACTRL_CHIRQ));
	return 0;
}

STATIC INLINE unsigned int grpci2dma_ctrl_status()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Read DMA */
	return (REG_READ(&priv->regs->dma_ctrl));
}

STATIC INLINE unsigned int grpci2dma_ctrl_base()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Read DMA */
	return (REG_READ(&priv->regs->dma_bdbase));
}

UNUSED STATIC INLINE unsigned int grpci2dma_ctrl_active()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Read DMA */
	return (REG_READ(&priv->regs->dma_chact));
}

/* Set the DMA CTRL register NUMCH field */
STATIC INLINE int grpci2dma_ctrl_numch_set(int numch)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	unsigned int ctrl = REG_READ(&priv->regs->dma_ctrl);

	/* Clear old value */
	ctrl = (ctrl & ~(DMACTRL_NUMCH));

	/* Put new value */
	ctrl = (ctrl | ( (numch << DMACTRL_NUMCH_BIT) & DMACTRL_NUMCH));

	REG_WRITE(&priv->regs->dma_ctrl, ctrl & ~(DMACTRL_WCLEAR));
	return 0;
}

/*** END OF DMACTRL ACCESS FUNCTIONS ***/

/*** START OF DESCRIPTOR ACCESS FUNCTIONS ***/

STATIC int grpci2dma_data_bd_init(struct grpci2_bd_data * data, 
		uint32_t pci_adr, uint32_t ahb_adr, int dir, int endianness, int size,
		struct grpci2_bd_data * next)
{
	BD_WRITE(&data->ctrl, 0 |
			(BD_DATA_EN) |
			(BD_DATA_TYPE_DATA) |
			(dir == GRPCI2DMA_AHBTOPCI? BD_DATA_DR:0) |
			(endianness == GRPCI2DMA_LITTLEENDIAN? BD_DATA_BE:0) |
			( (size << BD_DATA_LEN_BIT) & BD_DATA_LEN )
			);
	BD_WRITE(&data->pci_adr, pci_adr);
	BD_WRITE(&data->ahb_adr, ahb_adr);
	BD_WRITE(&data->next, (unsigned int) next);
	return 0;
}

STATIC int grpci2dma_channel_bd_init(struct grpci2_bd_chan * chan)
{
	BD_WRITE(&chan->ctrl, 0 | BD_CHAN_TYPE_DMA | BD_CHAN_EN);
	BD_WRITE(&chan->nchan, (unsigned int) chan);
	BD_WRITE(&chan->nbd, (unsigned int) DISABLED_DESCRIPTOR);
	return 0;
}

/* Enable a channel with options.
 * options include:
 * - options & 0xFFFF: Maximum data descriptor count before 
 *	 moving to next DMA channel.
 */
STATIC int grpci2dma_channel_bd_enable(struct grpci2_bd_chan * chan, 
		unsigned int options)
{
	unsigned int ctrl = BD_READ(&chan->ctrl);
	ctrl = (ctrl & ~(BD_CHAN_BDCNT));
	BD_WRITE(&chan->ctrl, (ctrl | BD_CHAN_EN | 
				( (options << BD_CHAN_BDCNT_BIT) & BD_CHAN_BDCNT)));
	return 0;
}

/* Disable channel.
 */
STATIC int grpci2dma_channel_bd_disable(struct grpci2_bd_chan * chan)
{
	unsigned int ctrl = BD_READ(&chan->ctrl);
	BD_WRITE(&chan->ctrl, (ctrl & ~(BD_CHAN_EN)));
	return 0;
}

/* Get the CID of a channel.
 */
UNUSED STATIC int grpci2dma_channel_bd_get_cid(struct grpci2_bd_chan * chan)
{
	/* Get cid from chan */
	unsigned ctrl = BD_READ(&chan->ctrl);
	unsigned cid  = (ctrl & (BD_CHAN_ID)) >> BD_CHAN_ID_BIT;
	return cid;
}

/* Set the CID of a channel. */
STATIC void grpci2dma_channel_bd_set_cid(struct grpci2_bd_chan * chan, int cid)
{
	/* Set cid from chan */
	unsigned ctrl = BD_READ(&chan->ctrl);
	ctrl = (ctrl & ~(BD_CHAN_ID)) | ((cid << BD_CHAN_ID_BIT) & BD_CHAN_ID);
	BD_WRITE(&chan->ctrl,ctrl);
	return;
}

/* Disable data descriptor*/
UNUSED STATIC int grpci2dma_data_bd_disable(struct grpci2_bd_data *desc)
{
	BD_WRITE(&desc->ctrl,0);
	return 0;
}

/* Return status of data descriptor*/
STATIC int grpci2dma_data_bd_status(struct grpci2_bd_data *desc)
{
	int status = BD_READ(&desc->ctrl);
	if (status & BD_DATA_ER) {
		return GRPCI2DMA_BD_STATUS_ERR;
	}else if (status & BD_DATA_EN) {
		return GRPCI2DMA_BD_STATUS_ENABLED;
	}else {
		return GRPCI2DMA_BD_STATUS_DISABLED;
	}
	return GRPCI2DMA_BD_STATUS_ERR;
}

/* Enable interrupts in data descriptor*/
STATIC int grpci2dma_data_bd_interrupt_enable(struct grpci2_bd_data * data)
{
	unsigned int ctrl = BD_READ(&data->ctrl);
	BD_WRITE(&data->ctrl, ctrl | BD_DATA_IE);
	return 0;
}

/* Get data descriptor */
STATIC struct grpci2_bd_data * grpci2dma_channel_bd_get_data(
		struct grpci2_bd_chan * chan)
{
	return	(struct grpci2_bd_data *) BD_READ(&chan->nbd);
}

/* Set data descriptorl */
STATIC void grpci2dma_channel_bd_set_data(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_data * data)
{
	BD_WRITE(&chan->nbd, (unsigned int) data);
}

/* Get next channel */
STATIC struct grpci2_bd_chan * grpci2dma_channel_bd_get_next(
		struct grpci2_bd_chan * chan)
{
	return	(struct grpci2_bd_chan *) BD_READ(&chan->nchan);
}

/* Get next data */
STATIC struct grpci2_bd_data * grpci2dma_data_bd_get_next(
		struct grpci2_bd_data * data)
{
	return	(struct grpci2_bd_data *) BD_READ(&data->next);
}

/* Set next channel */
STATIC void grpci2dma_channel_bd_set_next(struct grpci2_bd_chan * chan, 
		struct grpci2_bd_chan * next)
{
	BD_WRITE(&chan->nchan,(unsigned int) next);
}

/* Set next data */
STATIC void grpci2dma_data_bd_set_next(struct grpci2_bd_data * data, 
		struct grpci2_bd_data * next)
{
	BD_WRITE(&data->next,(unsigned int) next);
}

/*** END OF DESCRIPTOR ACCESS FUNCTIONS ***/

/*** START OF CHANNEL FUNCTIONS ***/

STATIC int grpci2dma_channel_open(struct grpci2_bd_chan * chan, int cid)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int allocated = 0;

	/* Get pointer */
	if (chan == NULL) {
		/* User does not provide channel, let's create it */
		chan = grpci2dma_channel_new(1);
		allocated = 1;
	}else{
		/* Make sure the pointer is not already on the linked list */
		int i;
		for (i=0; i<MAX_DMA_CHANS; i++){
			if (priv->channel[i].ptr == chan){
				return GRPCI2DMA_ERR_WRONGPTR;
			}
		}
	}

	DBG("Opening channel %d (0x%08x)\n", cid, (unsigned int) chan);

	/* Init channel descriptor */
	grpci2dma_channel_bd_init(chan);

	/* Assign cid to chan */
	priv->channel[cid].ptr = chan;
	grpci2dma_channel_bd_set_cid(chan, cid);

	/* Increase number of channels */
	priv->nchans++;

	DBG("number of channels: %d\n", priv->nchans);

	/* Initialize channel data */
	priv->channel[cid].allocated = allocated;
	priv->channel[cid].active = 0;

	/* Initialize record of last added data */
	priv->channel[cid].lastdata = DISABLED_DESCRIPTOR;

	return cid;
}

/* Get first free CID.
 */
STATIC int grpci2dma_channel_free_id()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;

	/* Find the first free CID */
	int i;
	for (i=0; i<MAX_DMA_CHANS; i++){
		if (priv->channel[i].ptr == NULL){
			return i;
		}
	}
	return GRPCI2DMA_ERR_TOOMANY;
}

/* Get the active channel circular linked list */
STATIC struct grpci2_bd_chan * grpci2dma_channel_get_active_list()
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int i;
	/* Just get the first non NULL associated cid */
	for (i=0; i< MAX_DMA_CHANS; i++){
		if ((priv->channel[i].ptr != NULL) && (priv->channel[i].active)){
			return priv->channel[i].ptr;
		}
	}
	return NULL;
}

/* Start a channel */
STATIC int grpci2dma_channel_start(int chan_no, int options)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_chan *chan;
	SPIN_IRQFLAGS(irqflags);

	/* Get chan pointer */
	chan = priv->channel[chan_no].ptr;

	/* Check if channel is active */
	if (priv->channel[chan_no].active){
		/* nothing to do */
		return GRPCI2DMA_ERR_OK;
	}

	/* Get the max descriptor count */
	unsigned int desccnt;
	if (options == 0){
		/* Default */
		desccnt = 0xffff;
	}else{
		desccnt = options & 0xffff;
	}

	/* Start the channel by enabling it.
	 * HWNOTE: In GRPCI2 this bit does not work as it is supposed.
	 * So we better add/remove the channel from the active linked 
	 * list. */
	grpci2dma_channel_bd_enable(chan, desccnt);
	priv->channel[chan_no].active = 1;
	priv->nactive++;
	/* Get active linked list */
	struct grpci2_bd_chan * list = grpci2dma_channel_get_active_list();
	if (list == NULL){
		/* No previous channels. New list */
		list = chan;
	}
	/* Add channel from the linked list */
	if (grpci2dma_channel_list_add(list, chan) < 0){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Increase NUMCH in DMA ctrl */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	grpci2dma_ctrl_numch_set( (priv->nactive? priv->nactive -1:0));

	/* Check if DMA is active */
	if (!grpci2dma_active()){
		/* Start DMA */
		grpci2dma_ctrl_start(chan);
	}
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	DBG("Channel %d started (0x%08x)\n", chan_no, (unsigned int) chan);

	return GRPCI2DMA_ERR_OK;
}

/* Stop a channel */
STATIC int grpci2dma_channel_stop(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_chan *chan;
	SPIN_IRQFLAGS(irqflags);
	int resume;

	/* Get chan pointer */
	chan = priv->channel[chan_no].ptr;

	/* Check if channel is active */
	if (!priv->channel[chan_no].active){
		/* nothing to do */
		return GRPCI2DMA_ERR_OK;
	}

	/* First remove channel from the linked list */
	if (grpci2dma_channel_list_remove(chan) < 0){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Update driver struct */
	priv->channel[chan_no].active = 0;
	priv->nactive--;

	/* Check if DMA is active and it the removed
	 * channel is the active */
	resume = 0;
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	if (grpci2dma_active() && (grpci2dma_ctrl_active() == (unsigned int)chan)){
		/* We need to stop the DMA */
		grpci2dma_ctrl_stop();
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		/* Wait until DMA stops */
		while (grpci2dma_active()){}
		/* We need to check later to resume the DMA */
		resume = 1;
	}else{
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	}


	/* Now either the DMA is stopped, or it is processing 
	 * a different channel and the removed channel is no
	 * longer in the linked list */

	/* Now is safe to update the removed channel */
	grpci2dma_channel_bd_set_next(chan, chan);

	/* Stop the channel by disabling it. 
	 * HWNOTE: In GRPCI2 this bit does not work as it is supposed.
	 * So we better remove the channel from the active linked 
	 * list. */
	grpci2dma_channel_bd_disable(chan);

	/* Point channel to disabled descriptor */
	grpci2dma_channel_bd_set_data(chan, DISABLED_DESCRIPTOR);

	DBG("Channel %d stoped (0x%08x)\n", chan_no, (unsigned int) chan);

	/* Decrease NUMCH in DMA ctrl */
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	grpci2dma_ctrl_numch_set( (priv->nactive? priv->nactive -1:0));

	/* Reactivate DMA only if we stopped */
	if (resume){
		/* We have two options, either we stopped when the active
		 * channel was still the active one, or we stopped when
		 * the active channel was a different one */
		if (grpci2dma_ctrl_active() == (unsigned int) chan){
			/* In this case, we need to start the DMA with
			 * any active channel on the list */
			int i;
			for (i=0; i<MAX_DMA_CHANS; i++){
				if (priv->channel[i].active){
					grpci2dma_ctrl_start(priv->channel[i].ptr);
					break;
				}
			}
		}else{
			/* In this case, we need to resume the DMA operation */
			/* HWNOTE: The GRPCI2 core does not update the channel next 
			 * data descriptor if we stopped a channel. This means that
			 * we need to resume the DMA from the descriptor is was,
			 * by only setting the enable bit, and not changing the 
			 * base register */
			grpci2dma_ctrl_resume();
		}
	}
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	return GRPCI2DMA_ERR_OK;
}

STATIC int grpci2dma_channel_push(int chan_no, void *dataptr, int index,
		int ndata)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_chan * chan;
	struct grpci2_bd_data * data = dataptr;

	/* Get channel */
	chan = priv->channel[chan_no].ptr;

	DBG("Pushing %d data (starting at 0x%08x) to channel %d (0x%08x)\n",
			ndata, (unsigned int) &data[index], chan_no, (unsigned int) chan);

	/* Get last added data */
	struct grpci2_bd_data * last_added = priv->channel[chan_no].lastdata;

	/* Add data to channel */
	grpci2dma_data_list_add(chan, &data[index], last_added);

	/* Update last added */
	priv->channel[chan_no].lastdata = &data[index + ndata-1];

	return GRPCI2DMA_ERR_OK;
}

STATIC int grpci2dma_channel_close(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_chan * chan;

	/* Get channel */
	chan = priv->channel[chan_no].ptr;

	DBG("Closing channel %d (0x%08x)\n", chan_no, (unsigned int) chan);

	/* Stop channel */
	if (grpci2dma_channel_stop(chan_no) != GRPCI2DMA_ERR_OK ){
		DBG("Cannot stop channel!.\n");
		return GRPCI2DMA_ERR_STOPDMA;
	}

	/* Unregister channel ISR */
	grpci2dma_channel_isr_unregister(chan_no);

	/* Free the cid */
	priv->channel[chan_no].ptr = NULL;

	/* Remove the ISR */
	priv->channel[chan_no].isr = NULL;
	priv->channel[chan_no].isr_arg = NULL;

	/* Deallocate channel if needed */
	if (priv->channel[chan_no].allocated){
		grpci2dma_channel_delete((void *)chan);
	}

	/* Decrease number of channels */
	priv->nchans--;

	DBG("number of channels: %d\n", priv->nchans);

	/* Everything OK */
	return GRPCI2DMA_ERR_OK;
}

/* Register channel ISR */
STATIC int grpci2dma_channel_isr_unregister(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	SPIN_IRQFLAGS(irqflags);

	/* Unregister channel ISR */
	priv->channel[chan_no].isr = NULL;
	priv->channel[chan_no].isr_arg = NULL;

	/* Unregister DMA ISR in GRPCI2 if needed */
	priv->isr_registered--;
	if(priv->isr_registered == 0){
		/* Disable DMA Interrupts */
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		grpci2dma_ctrl_interrupt_disable();
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
		(priv->isr_register)( NULL, NULL);
	}

	/* Everything OK */
	return GRPCI2DMA_ERR_OK;
}

/*** END OF CHANNEL FUNCTIONS ***/

/*** START OF ISR FUNCTIONS ***/

/* PCI DMA Interrupt handler, called when there is a PCI DMA interrupt */
STATIC void grpci2dma_isr(void *arg)
{
	struct grpci2dma_priv *priv = arg;
	SPIN_ISR_IRQFLAGS(irqflags);
	unsigned int ctrl = grpci2dma_ctrl_status();
	/* Clear Interrupts */
	SPIN_LOCK(&priv->devlock, irqflags);
	grpci2dma_ctrl_interrupt_clear();
	SPIN_UNLOCK(&priv->devlock, irqflags);
	unsigned int sts = (ctrl & DMACTRL_CHIRQ) >> DMACTRL_CHIRQ_BIT;
	unsigned int errsts = (ctrl & DMACTRL_ERR);

	/* Error interrupt */
	if(errsts){
		/* Find which channels had the error.
		 * The GRPCI2DMA core does not indicate which channel
		 * had the error, so we need to get 1st the base descriptor register
		 * and see if it a channel. If is not a channel, then the active 
		 * channel register tells us which channel is.
		 * After having the channel we need to find out which channel was. */
		struct grpci2_bd_chan * chan =
			(struct grpci2_bd_chan *) grpci2dma_ctrl_base();
		/* Check if the base is a channel descriptor */
		if ((BD_READ(&chan->ctrl) & BD_CHAN_TYPE) != BD_CHAN_TYPE_DMA){
			/* Is not a channel, so the channel is in the channel active 
			 * register */
			chan = (struct grpci2_bd_chan *) grpci2dma_ctrl_active();
		}
		int i;
		for (i=0; i<MAX_DMA_CHANS; i++){
			if (chan == priv->channel[i].ptr){
				/* Found */
				if (priv->channel[i].isr != NULL){
					(priv->channel[i].isr)(priv->channel[i].isr_arg,i,errsts);
				}else{
					printk("Unhandled GRPCI2 DMA error interrupt, sts:0x%02x\n", errsts);
				}
				break;
			}
		}
		if (i == MAX_DMA_CHANS){
			printk("Unhandled GRPCI2 DMA error interrupt , sts:0x%02x\n", errsts);
		}
	}

	/* Normal packet interrupt */
	int cid=0;
	/* Find which channels have interrupts */
	while(sts){
		/* Find if current channel has an interrupt*/
		if(sts & 0x1){
			/* Find if current channel has an ISR */
			if (priv->channel[cid].isr != NULL){
				(priv->channel[cid].isr)(
						priv->channel[cid].isr_arg, cid, errsts);
			}else{
				printk("Unhandled GRPCI2 DMA interrupt in channel %d, sts:0x%02x\n", cid, 0);
			}
		}
		/* Next channel */
		sts = sts >> 1;
		cid++;
	}
}

/*** END OF ISR FUNCTIONS ***/

/*** START OF DEBUG HELPERS ***/
#ifdef DEBUG
STATIC int grpci2dma_channel_print(struct grpci2_bd_chan * chan)
{
	printf("  GRPCI2 DMA channel descriptor\n");
	printf("	  0x%08x  DMA channel control					  0x%08x\n", (unsigned int) chan, chan->ctrl);
	printf("	  31	 en				   0x%01x		  Channel descriptor enable.\n", (chan->ctrl >> 31) & (0x1));
	printf("	  24:22  cid			   0x%01x		  Channel ID.\n", (chan->ctrl >> 22) & (0x7));
	printf("	  21:20  type			   0x%01x		  Descriptor type. 01=DMA channel descriptor.\n", (chan->ctrl >> 20) & (0x3)); 
	printf("	  15:0	 dlen			0x%04x		   Data descriptor count.\n", (chan->ctrl >> 0) & (0xffff));
	printf("\n");
	printf("	  0x%08x  Next DMA channel						  0x%08x\n", (unsigned int) &(chan->nchan), chan->nchan);
	printf("	  31:0	 nc			0x%08x		   Next DMA channel.\n", chan->nchan);
	printf("\n");
	printf("	  0x%08x  Next data descriptor					  0x%08x\n" , (unsigned int) &(chan->nbd), chan->nbd);
	printf("	  31:0	 nd			0x%08x		   Next data descriptor.\n", chan->nbd);
	printf("\n");
	return 0;
}

STATIC int grpci2dma_data_print(struct grpci2_bd_data * data)
{
	printf("  GRPCI2 DMA data descriptor\n");
	printf("	  0x%08x  DMA data control						  0x%08x\n", (unsigned int) data, data->ctrl);
	printf("	  31	 en				   0x%01x		  Data descriptor enable.\n" , (data->ctrl >> 31) & (0x1));
	printf("	  30	 ie				   0x%01x		  Interrupt generation enable.\n" , (data->ctrl >> 30) & (0x1));
	printf("	  29	 dr				   0x%01x		  Tranfer direction.\n" , (data->ctrl >> 29) & (0x1));
	printf("	  28	 be				   0x%01x		  Bus endianess.\n" , (data->ctrl >> 28) & (0x1));
	printf("	  21:20  type			   0x%01x		  Descriptor type. 00=DMA data descriptor.\n" , (data->ctrl >> 20) & (0x3));
	printf("	  19	 er				   0x%01x		  Error status.\n" , (data->ctrl >> 19) & (0x1));
	printf("	  15:0	 len			0x%04x		   Transfer lenght (in words) - 1.\n" ,  (data->ctrl >> 0) & (0xffff));
	printf("\n");
	printf("	  0x%08x  32-bit PCI start address				  0x%08x\n" , (unsigned int) &(data->pci_adr), data->pci_adr);
	printf("	  31:0	 pa			0x%08x		   PCI address.\n" , data->pci_adr);
	printf("\n");
	printf("	  0x%08x  32-bit AHB start address				  0x%08x\n" , (unsigned int) &(data->ahb_adr), data->ahb_adr);
	printf("	  31:0	 aa			0x%08x		   AHB address.\n" , data->ahb_adr);
	printf("\n");
	printf("	  0x%08x  Next data descriptor					  0x%08x\n" , (unsigned int) &(data->next), data->next);
	printf("	  31:0	 nd			0x%08x		   Next data descriptor.\n" , data->next);
	printf("\n");
	return 0;
}
#endif
/*** END OF DEBUG HELPERS ***/

/*** START OF MEMORY ALLOCATION FUNCTIONS ***/

void * grpci2dma_channel_new(int number)
{
	/* Allocate memory */
	unsigned int * orig_ptr = (unsigned int *) grlib_malloc(
			(GRPCI2DMA_BD_CHAN_SIZE)*number + GRPCI2DMA_BD_CHAN_ALIGN);
	if (orig_ptr == NULL) return NULL;

	/* Get the aligned pointer */
	unsigned int aligned_ptr = (
		((unsigned int) orig_ptr + GRPCI2DMA_BD_CHAN_ALIGN) &
		 ~(GRPCI2DMA_BD_CHAN_ALIGN - 1));

	/* Save the original pointer just before the aligned pointer */
	unsigned int ** tmp_ptr =
		(unsigned int **) (aligned_ptr - sizeof(orig_ptr));
	*tmp_ptr= orig_ptr;

	/* Return aligned pointer */
	return (void *) aligned_ptr;
}

void grpci2dma_channel_delete(void * chan)
{
	/* Recover orignal pointer placed just before the aligned pointer */
	unsigned int * orig_ptr;
	unsigned int ** tmp_ptr =  (unsigned int **) (chan - sizeof(orig_ptr));
	orig_ptr = *tmp_ptr;

	/* Deallocate memory */
	free(orig_ptr);
}

void * grpci2dma_data_new(int number)
{
	/* Allocate memory */
	unsigned int * orig_ptr = (unsigned int *) grlib_malloc(
			(GRPCI2DMA_BD_DATA_SIZE)*number + GRPCI2DMA_BD_DATA_ALIGN);
	if (orig_ptr == NULL) return NULL;

	/* Get the aligned pointer */
	unsigned int aligned_ptr = (
		((unsigned int) orig_ptr + GRPCI2DMA_BD_DATA_ALIGN) & 
		~(GRPCI2DMA_BD_DATA_ALIGN - 1));

	/* Save the original pointer before the aligned pointer */
	unsigned int ** tmp_ptr = 
		(unsigned int **) (aligned_ptr - sizeof(orig_ptr));
	*tmp_ptr= orig_ptr;

	/* Return aligned pointer */
	return (void *) aligned_ptr;
}

void grpci2dma_data_delete(void * data)
{
	/* Recover orignal pointer placed just before the aligned pointer */
	unsigned int * orig_ptr;
	unsigned int ** tmp_ptr =  (unsigned int **) (data - sizeof(orig_ptr));
	orig_ptr = *tmp_ptr;

	/* Deallocate memory */
	free(orig_ptr);
}

/*** END OF MEMORY ALLOCATION FUNCTIONS ***/

/*** START OF USER API ***/

/* Initialize GRPCI2 DMA: GRPCI2 DRIVER calls this
 * using a weak function definition */
int grpci2dma_init(
		void * regs, void isr_register( void (*isr)(void*), void * arg))
{
	struct grpci2dma_priv *priv;
	int i;

	DBG("Registering GRPCI2 DMA driver with arg: 0x%08x\n",
			(unsigned int) regs);

	/* We only allow one GRPCI2 DMA */
	if (grpci2dmapriv) {
		DBG("Driver only supports one PCI DMA core\n");
		return DRVMGR_FAIL;
	}

	/* Device Semaphore created with count = 1 */
	if (rtems_semaphore_create(rtems_build_name('G', 'P', '2', 'D'), 1,
		RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
		RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
		RTEMS_NO_PRIORITY_CEILING, 0, &grpci2dma_sem) != RTEMS_SUCCESSFUL)
		return -1;

	/* Allocate and init Memory for DMA */
	priv = grlib_calloc(1, sizeof(*priv));
	if (priv == NULL)
		return DRVMGR_NOMEM;

	priv->regs = regs;
	strncpy(&priv->devname[0], "grpci2dma0", DEVNAME_LEN);

	/* Initialize Spin-lock for GRPCI2dma Device. */
	SPIN_INIT(&priv->devlock, priv->devname);

	/* Channel Sempahores */
	for (i=0; i<MAX_DMA_CHANS; i++){
		/* set to NULL, they are created when openning channels */
		priv->channel[i].sem = RTEMS_ID_NONE;
	}

	/* Register device */
	grpci2dmapriv = priv;

	/* Initialize Ctrl regs */
	grpci2dma_ctrl_init();

	/* Install DMA ISR */
	priv->isr_register = isr_register;

	/* Startup actions:
	 * - stop DMA
	 */
	grpci2dma_ctrl_stop();

	return DRVMGR_OK;
}

/* Assign ISR Function to DMA IRQ */
int grpci2dma_isr_register(int chan_no, grpci2dma_isr_t dmaisr, void *data)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	SPIN_IRQFLAGS(irqflags);

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Check isr */
	if (dmaisr == NULL){
		/* No ISR */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Get chan pointer */
	if ((chan_no < 0 ) || (chan_no >= MAX_DMA_CHANS)) {
		/* Wrong channel id */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Register channel ISR */
	priv->channel[chan_no].isr_arg = data;
	priv->channel[chan_no].isr = dmaisr;

	/* Register DMA ISR in GRPCI2 if not done yet */
	if(priv->isr_registered == 0){
		(priv->isr_register)( grpci2dma_isr, (void *) priv);
		/* Enable DMA Interrupts */
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		grpci2dma_ctrl_interrupt_enable();
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	}
	priv->isr_registered++;

	/* Release channel sempahore */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	/* Release driver sempahore */
	rtems_semaphore_release(grpci2dma_sem);

	return GRPCI2DMA_ERR_OK;
}

/* Assign ISR Function to DMA IRQ */
int grpci2dma_isr_unregister(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Get chan pointer */
	if ((chan_no < 0 ) || (chan_no >= MAX_DMA_CHANS)) {
		/* Wrong channel id */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Get chan ISR */
	if (priv->channel[chan_no].isr == NULL){
		/* Nothing to do */
		return GRPCI2DMA_ERR_OK;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Unregister channel ISR */
	ret = grpci2dma_channel_isr_unregister(chan_no);

	/* Release channel sempahore */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	/* Release driver sempahore */
	rtems_semaphore_release(grpci2dma_sem);

	return ret;
}

int grpci2dma_open(void * chanptr)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int cid;
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Check alignment */
	if (((unsigned int ) chanptr) & (GRPCI2DMA_BD_CHAN_ALIGN-1)) {
		/* Channel is not properly aligned */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Get free channel id */
	cid = grpci2dma_channel_free_id();
	if (cid < 0 ){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_TOOMANY;
	}

	/* Open channel */
	ret = grpci2dma_channel_open((struct grpci2_bd_chan *) chanptr, cid);

	/* Create channel semaphore with count = 1 */
	if (ret >= 0){
		if (rtems_semaphore_create(
					rtems_build_name('P', 'D', '0', '0' + cid), 1,
					RTEMS_FIFO | RTEMS_SIMPLE_BINARY_SEMAPHORE | \
					RTEMS_NO_INHERIT_PRIORITY | RTEMS_LOCAL | \
					RTEMS_NO_PRIORITY_CEILING, 0, &priv->channel[cid].sem
					) != RTEMS_SUCCESSFUL) {
			priv->channel[cid].sem = RTEMS_ID_NONE;
			rtems_semaphore_release(grpci2dma_sem);
			return GRPCI2DMA_ERR_ERROR;
		}
	}

	/* Release driver semaphore */
	rtems_semaphore_release(grpci2dma_sem);

	/* Return channel id */
	return ret;
}

int grpci2dma_close(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Get chan pointer */
	if ((chan_no < 0) || (chan_no >= MAX_DMA_CHANS)){
		/* Wrong channel id */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Close channel */
	ret = grpci2dma_channel_close(chan_no);

	/* Release channel sempahore */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	/* Delete channel semaphore */
	if (ret == GRPCI2DMA_ERR_OK){
		if (rtems_semaphore_delete(priv->channel[chan_no].sem) 
				!= RTEMS_SUCCESSFUL){
			/* Release driver semaphore */
			rtems_semaphore_release(grpci2dma_sem);
			return GRPCI2DMA_ERR_ERROR;
		}
	}

	/* Release driver semaphore */
	rtems_semaphore_release(grpci2dma_sem);

	return ret;
}

/* Transfer_size =0 means maximum */
int grpci2dma_prepare(
		uint32_t pci_start, uint32_t ahb_start, int dir, int endianness,
		int size, void * dataptr, int index, int ndata, int transfer_size)
{
	struct grpci2_bd_data * data = dataptr;

	/* Check data pointer */
	if ((data == NULL) || 
			(((unsigned int ) data) & (GRPCI2DMA_BD_DATA_ALIGN-1))){
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check indexes */
	int maxdata = ndata - index;
	if ((maxdata < 1) || (index < 0)){
		/* No data descriptors to use */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check PCI transfer size */
	if ( (transfer_size < 0) || 
			(transfer_size > MAX_DMA_TRANSFER_SIZE) || 
			(transfer_size%4 != 0) ) {
		return GRPCI2DMA_ERR_WRONGPTR;
	}
	if (transfer_size == 0){
		transfer_size = MAX_DMA_TRANSFER_SIZE;
	}

	/* Check total size */
	if ( (size <=0) || (size % 4 != 0)){
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Calculate number of data descriptors needed */
	int words = size/4;
	int blocksize = transfer_size/4;
	int datacnt = words/blocksize + (words%blocksize != 0? 1: 0);
	/* Check that we can transfer the data */
	if (datacnt > maxdata) {
		return GRPCI2DMA_ERR_TOOMANY;
	}

	/* Prepare data descriptors */
	int i;
	uint32_t pci_adr;
	uint32_t ahb_adr;
	int remaining=words;
	int datasize;
	struct grpci2_bd_data * next;
	for (i=0; i<datacnt; i++){
		/* Get PCI and AHB start addresses */
		pci_adr = pci_start + i*blocksize;
		ahb_adr = ahb_start + i*blocksize;
		/* Get current data size */
		if (remaining >= blocksize){
			datasize = blocksize - 1;
			remaining -= blocksize;
		} else {
			datasize = remaining -1;
			remaining = 0;
		}
		/* Get linked list pointers */
		if (i == datacnt - 1){
			/* Last transfer */
			next = DISABLED_DESCRIPTOR;
		}else{
			next = &data[i+index+1];
		}
		/* Set Data descriptor */
		grpci2dma_data_bd_init(&data[i+index], pci_adr, ahb_adr, dir, endianness, datasize, next);
	}
	/* Return number of transfers used */
	return datacnt;
}

int grpci2dma_status(void *dataptr, int index, int ndata)
{
	struct grpci2_bd_data * data = dataptr;
	int i;

	/* Check data pointer */
	if ((data == NULL) || 
			(((unsigned int ) data) & (GRPCI2DMA_BD_DATA_ALIGN-1))){
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check maxdata */
	int maxdata = ndata - index;
	if ((maxdata < 1) || (index < 0)){
		/* No data descriptors to use */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check status of all packets in transfer */
	int status;
	for (i=0; i< maxdata; i++){
		status = grpci2dma_data_bd_status(&data[i+index]); 
		if (status == GRPCI2DMA_BD_STATUS_ERR){
			/* Error in one packet, means error in transfer */
			return status;
		} else if (status == GRPCI2DMA_BD_STATUS_ENABLED){
			/* If one packet is enabled, means transfer is not done */
			return status;
		}
	}

	/* If we reach here it means they are all disabled */
	return status;
}

int grpci2dma_print(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_chan * chan;

	if (!priv){
		/* DMA not initialized */
		DBG("DMA not initialized.\n");
		return GRPCI2DMA_ERR_NOINIT;
	}

	if ( (chan_no < 0) || (chan_no >= MAX_DMA_CHANS )){
		/* Wrong chan no*/
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	chan = priv->channel[chan_no].ptr;
	if (chan == NULL) {
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	#ifdef DEBUG
	/* Print channel state */
	grpci2dma_channel_print(chan);

	/* Get current DATA desc */
	struct grpci2_bd_data * first_data = (struct grpci2_bd_data *) BD_READ(&chan->nbd);

	/* Print data state */
	grpci2dma_data_list_foreach(first_data, grpci2dma_data_print, MAX_DMA_DATA);
	#endif
	return GRPCI2DMA_ERR_OK;
}

int grpci2dma_print_bd(void * dataptr)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	struct grpci2_bd_data * data = (struct grpci2_bd_data *) dataptr;

	if (!priv){
		/* DMA not initialized */
		DBG("DMA not initialized.\n");
		return GRPCI2DMA_ERR_NOINIT;
	}

	if ( data == NULL ){
		/* Wrong chan no*/
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	#ifdef DEBUG
	/* Print data state */
	grpci2dma_data_list_foreach(data, grpci2dma_data_print, MAX_DMA_DATA);
	#endif
	return GRPCI2DMA_ERR_OK;
}

int grpci2dma_interrupt_enable(
		void *dataptr, int index, int maxindex, int options)
{
	struct grpci2_bd_data * data = dataptr;
	struct grpci2dma_priv *priv = grpci2dmapriv;
	SPIN_IRQFLAGS(irqflags);

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Check data pointer */
	if ((data == NULL) || 
			(((unsigned int ) data) & (GRPCI2DMA_BD_DATA_ALIGN-1))){
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check index */
	if ((index < 0) || (maxindex < 1) || (index >= maxindex)){
		/* No data descriptors to use */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	if (options & GRPCI2DMA_OPTIONS_ALL){
		/* Enable all interrupts */
		if (grpci2dma_data_list_foreach(
					&data[index],
					grpci2dma_data_bd_interrupt_enable, maxindex -index)){
			return GRPCI2DMA_ERR_ERROR;
		}
	}else{
		/* Enable one packet interrupts */
		grpci2dma_data_bd_interrupt_enable(&data[index]);
	}

	/* Finally enable DMA interrupts if they are not already enabled */
	if (grpci2dma_ctrl_interrupt_status()==0){
		SPIN_LOCK_IRQ(&priv->devlock, irqflags);
		grpci2dma_ctrl_interrupt_enable();
		SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);
	}

	DBG("Interrupts enabled for data (0x%08x), index:%d, maxindex:%d, %s.\n", 
			(unsigned int) data, index, maxindex, 
			(options & GRPCI2DMA_OPTIONS_ALL)? "ALL":"ONE" );

	return GRPCI2DMA_ERR_OK;
}

int grpci2dma_push(int chan_no, void *dataptr, int index, int ndata)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	SPIN_IRQFLAGS(irqflags);
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	/* Check data pointer */
	if ((dataptr == NULL) || 
			(((unsigned int ) dataptr) & (GRPCI2DMA_BD_DATA_ALIGN-1))){
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check index */
	if ((ndata < 1) || (index < 0)){
		/* No data descriptors to use */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan_no */
	if ( (chan_no < 0) || (chan_no >= MAX_DMA_CHANS )){
		/* Wrong chan no*/
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* push data to channel */
	ret = grpci2dma_channel_push(chan_no, dataptr, index, ndata);

	if (ret != GRPCI2DMA_ERR_OK){
		/* Release channel lock */
		rtems_semaphore_release(priv->channel[chan_no].sem);
		return ret;
	}

	/* Start DMA if it is not active and channel is active*/
	SPIN_LOCK_IRQ(&priv->devlock, irqflags);
	if ((!grpci2dma_active()) && (priv->channel[chan_no].active)){
		grpci2dma_ctrl_start(priv->channel[chan_no].ptr);
	}
	SPIN_UNLOCK_IRQ(&priv->devlock, irqflags);

	/* Release channel lock */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	return ret;
}

/* Start the channel */
int grpci2dma_start(int chan_no, int options)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	if ((chan_no < 0 ) || (chan_no >= MAX_DMA_CHANS )) {
		/* Wrong channel id */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	if ( options < 0 ) {
		/* Wrong options */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Start the channel */
	ret = grpci2dma_channel_start(chan_no, options);

	/* Release channel lock */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	/* Release driver lock */
	rtems_semaphore_release(grpci2dma_sem);

	return ret;
}

/* Stop the channel, but don't stop ongoing transfers! */
int grpci2dma_stop(int chan_no)
{
	struct grpci2dma_priv *priv = grpci2dmapriv;
	int ret;

	if (!priv){
		/* DMA not initialized */
		return GRPCI2DMA_ERR_NOINIT;
	}

	if ((chan_no < 0 ) || (chan_no >= MAX_DMA_CHANS)) {
		/* Wrong channel id */
		return GRPCI2DMA_ERR_WRONGPTR;
	}

	/* Check chan is open */
	if (priv->channel[chan_no].ptr == NULL){
		/* No channel */
		return GRPCI2DMA_ERR_NOTFOUND;
	}

	/* Take driver lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(grpci2dma_sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Take channel lock - Wait until we get semaphore */
	if (rtems_semaphore_obtain(priv->channel[chan_no].sem, RTEMS_WAIT, RTEMS_NO_TIMEOUT)
		!= RTEMS_SUCCESSFUL){
		rtems_semaphore_release(grpci2dma_sem);
		return GRPCI2DMA_ERR_ERROR;
	}

	/* Stop the channel */
	ret = grpci2dma_channel_stop(chan_no);

	/* Release channel lock */
	rtems_semaphore_release(priv->channel[chan_no].sem);

	/* Release driver lock */
	rtems_semaphore_release(grpci2dma_sem);

	return ret;
}

int grpci2dma_active()
{
	return ((grpci2dma_ctrl_status()) & DMACTRL_ACT) >> DMACTRL_ACT_BIT;
}

