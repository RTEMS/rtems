/*===============================================================*\
| Project: RTEMS generic MPC5200 BSP                              |
+-----------------------------------------------------------------+
| Partially based on the code references which are named below.   |
| Adaptions, modifications, enhancements and any recent parts of  |
| the code are:                                                   |
|                    Copyright (c) 2005                           |
|                    Embedded Brains GmbH                         |
|                    Obere Lagerstr. 30                           |
|                    D-82178 Puchheim                             |
|                    Germany                                      |
|                    rtems@embedded-brains.de                     |
+-----------------------------------------------------------------+
| The license and distribution terms for this file may be         |
| found in the file LICENSE in this distribution or at            |
|                                                                 |
| http://www.rtems.com/license/LICENSE.                           |
|                                                                 |
+-----------------------------------------------------------------+
| this file contains the PCMCIA IDE access functions              |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       pcmcia_ide.c                                        */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 PCMCIA IDE harddisk driver            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:                                                      */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   RTEMS MBX8xx PCMCIA IDE harddisc driver             */
/*   Module:       pcmcia_ide.c                                        */
/*   Project:      RTEMS 4.6.0pre1 / Mbx8xx BSP                        */
/*   Version                                                           */
/*   Date:         01/14/2003                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*                    Copyright (c) 2003 IMD                           */
/*      Ingenieurbuero fuer Microcomputertechnik Th. Doerfler          */
/*               <Thomas.Doerfler@imd-systems.de>                      */
/*                       all rights reserved                           */
/*                                                                     */
/*  this file contains the BSP layer for PCMCIA IDE access below the   */
/*  libchip IDE harddisc driver based on a board specific driver from  */
/*  Eugeny S. Mints, Oktet                                             */
/*                                                                     */
/*  The license and distribution terms for this file may be            */
/*  found in the file LICENSE in this distribution or at               */
/*  http://www.rtems.com/license/LICENSE.                         */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Partially based on the code references which are named above.     */
/*   Adaptions, modifications, enhancements and any recent parts of    */
/*   the code are under the right of                                   */
/*                                                                     */
/*         IPR Engineering, Dachauer Straße 38, D-80335 München        */
/*                        Copyright(C) 2003                            */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   IPR Engineering makes no representation or warranties with        */
/*   respect to the performance of this computer program, and          */
/*   specifically disclaims any responsibility for any damages,        */
/*   special or consequential, connected with the use of this program. */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Version history:  1.0                                             */
/*                                                                     */
/***********************************************************************/

#include <rtems.h>
#include <rtems/error.h>
#include <bsp.h>
#include <bsp/irq.h>
#include "../include/mpc5200.h"
#include "./pcmcia_ide.h"

#include <libchip/ide_ctrl.h>
#include <libchip/ide_ctrl_cfg.h>
#include <libchip/ide_ctrl_io.h>
#include <string.h>

#ifndef MIN
#define MIN(a,b) (((a)<(b))?(a):(b))
#endif
#define IDE_DMA_TEST            FALSE

/* DMA supported PIO mode is broken */
#define IDE_USE_INT             TRUE
#define IDE_READ_USE_DMA        FALSE
#define IDE_USE_READ_PIO_OPT    FALSE
#define IDE_WRITE_USE_DMA       FALSE
#define IDE_USE_WRITE_PIO_OPT   FALSE
#define IDE_USE_DMA (IDE_READ_USE_DMA || IDE_WRITE_USE_DMA)

#define IDE_USE_STATISTICS      TRUE

#if IDE_USE_DMA
#define PCMCIA_IDE_DMA_WR_BD_CNT 2
#define PCMCIA_IDE_DMA_RD_BD_CNT 2
#define PCMCIA_IDE_INTERRUPT_EVENT RTEMS_EVENT_2
/* Task number assignment */
#include "../bestcomm/bestcomm_glue.h"
#include "../bestcomm/bestcomm_api.h"
#include "../bestcomm/task_api/bestcomm_cntrl.h"
#include "../bestcomm/task_api/tasksetup_bdtable.h"

#define IDE_RX_TASK_NO TASK_GEN_DP_BD_0
#define IDE_TX_TASK_NO TASK_GEN_DP_BD_1
static TaskId pcmcia_ide_rxTaskId;	/* SDMA RX task ID */
static TaskId pcmcia_ide_txTaskId;	/* SDMA TX task ID */
#define PCMCIA_IDE_RD_SECTOR_SIZE 512   /* FIXME: make this better... */
#define PCMCIA_IDE_WR_SECTOR_SIZE 512   /* FIXME: make this better... */

bool mpc5200_dma_task_started[2] = {false,false};
#endif /* IDE_USE_DMA */

#if IDE_USE_STATISTICS
uint32_t mpc5200_pcmciaide_write_block_call_cnt  = 0;
uint32_t mpc5200_pcmciaide_write_block_block_cnt = 0;
uint32_t mpc5200_pcmciaide_read_block_call_cnt   = 0;
uint32_t mpc5200_pcmciaide_read_block_block_cnt  = 0;
#endif

extern volatile uint32_t * mpc5200_ata_drive_regs[];
extern uint32_t ata_pio_timings[2][6];

void mpc5200_pcmciaide_dma_blockop(
  bool, int, uint16_t, rtems_blkdev_sg_buffer *, uint32_t *, uint32_t *);
/*
 * support functions for PCMCIA IDE IF
 */
bool mpc5200_pcmciaide_probe(int minor)
  {
  bool ide_card_plugged = false; /* assume: we don't have a card plugged in */
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[GPT2]);

  #ifdef BSP_TYPE_DP2
    /* Deactivate RESET signal */
    rtems_interrupt_level level;
    rtems_interrupt_disable(level);
    mpc5200.gpiowe |= GPIO_W_PIN_PSC1_4;
    mpc5200.gpiowod &= ~GPIO_W_PIN_PSC1_4;
    mpc5200.gpiowdd |= GPIO_W_PIN_PSC1_4;
    mpc5200.gpiowdo |= GPIO_W_PIN_PSC1_4;
    rtems_interrupt_enable(level);
    /* FIXME */
    volatile int i = 0;
    while (++i < 20000000);
  #endif

  /* enable card detection on GPT2 */
  gpt->emsel = (GPT_EMSEL_GPIO_IN | GPT_EMSEL_TIMER_MS_GPIO);

#if defined (BRS5L)
  /* Check for card detection (-CD0) */
  if((gpt->status) & GPT_STATUS_PIN)
    ide_card_plugged = false;
  else
#endif
    ide_card_plugged = true;

  return ide_card_plugged;

  }

#define DMA1_T0(val) BSP_BFLD32(COUNT_VAL(val), 0, 7)
#define DMA1_TD(val) BSP_BFLD32(COUNT_VAL(val), 8, 15)
#define DMA1_TK(val) BSP_BFLD32(COUNT_VAL(val), 16, 23)
#define DMA1_TM(val) BSP_BFLD32(COUNT_VAL(val), 24, 31)

#define DMA2_TH(val) BSP_BFLD32(COUNT_VAL(val), 0, 7)
#define DMA2_TJ(val) BSP_BFLD32(COUNT_VAL(val), 8, 15)
#define DMA2_TN(val) BSP_BFLD32(COUNT_VAL(val), 16, 23)

rtems_status_code mpc5200_pcmciaide_config_io_speed(int minor, uint16_t modes_avail)
  {
  uint8_t pio_t0, pio_t2_8, pio_t2_16, pio_t4, pio_t1, pio_ta;

  if((modes_avail & ATA_MODES_PIO4) != 0)
    {

    pio_t0    = ata_pio_timings[PIO_4][T0];
    pio_t2_8  = ata_pio_timings[PIO_4][T2_8];
    pio_t2_16 = ata_pio_timings[PIO_4][T2_16];
    pio_t4    = ata_pio_timings[PIO_4][T4];
    pio_t1    = ata_pio_timings[PIO_4][T1];
    pio_ta    = ata_pio_timings[PIO_4][TA];

    }
  else
    {

    pio_t0    = ata_pio_timings[PIO_3][T0];
    pio_t2_8  = ata_pio_timings[PIO_3][T2_8];
    pio_t2_16 = ata_pio_timings[PIO_3][T2_16];
    pio_t4    = ata_pio_timings[PIO_3][T4];
    pio_t1    = ata_pio_timings[PIO_3][T1];
    pio_ta    = ata_pio_timings[PIO_3][TA];

    }

  /* set timings according according to selected ATA mode */
  mpc5200.ata_pio1 = ATA_PIO_TIMING_1(pio_t0, pio_t2_8, pio_t2_16);
  mpc5200.ata_pio2 = ATA_PIO_TIMING_2(pio_t4, pio_t1, pio_ta);

  mpc5200.ata_dma1 = DMA1_T0(120) | DMA1_TD(70) | DMA1_TK(25) | DMA1_TM(25);
  mpc5200.ata_dma2 = DMA2_TH(10) | DMA2_TJ(5) | DMA2_TN(10);

  return RTEMS_SUCCESSFUL;

  }



void mpc5200_pcmciaide_read_reg(int minor, int reg, uint16_t *value)
  {
  volatile uint32_t *ata_reg = mpc5200_ata_drive_regs[reg];

  if(reg == IDE_REGISTER_DATA_WORD)
    *value = *(volatile uint16_t *)(ata_reg);
  else
    *value = *(volatile uint8_t  *)(ata_reg);
  }


void mpc5200_pcmciaide_write_reg(int minor, int reg, uint16_t value)
  {
  volatile uint32_t *ata_reg = mpc5200_ata_drive_regs[reg];

  if(reg == IDE_REGISTER_DATA_WORD)
    *(volatile uint16_t *)(ata_reg) = value;
  else
    *(volatile uint8_t  *)(ata_reg) = value;
  }

#if IDE_USE_DMA


uint32_t pcmcia_ide_rxInterrupts;
uint32_t pcmcia_ide_txInterrupts;
volatile rtems_id pcmcia_ide_hdl_task = 0;
/*
 * MPC5200 BestComm interrupt handlers
 */
static void pcmcia_ide_recv_dmairq_hdl(rtems_irq_hdl_param unused)
{
  SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend,IDE_RX_TASK_NO);

/*Disable receive ints*/
  bestcomm_glue_irq_disable(IDE_RX_TASK_NO);

  pcmcia_ide_rxInterrupts++; 		/* Rx int has occurred */

  if (pcmcia_ide_hdl_task != 0) {
    rtems_event_send(pcmcia_ide_hdl_task,PCMCIA_IDE_INTERRUPT_EVENT);
  }
}

static void pcmcia_ide_xmit_dmairq_hdl(rtems_irq_hdl_param unused)
{

  SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend,IDE_TX_TASK_NO);

  /*Disable transmit ints*/
  bestcomm_glue_irq_disable(IDE_TX_TASK_NO);

  pcmcia_ide_txInterrupts++; 		/* Tx int has occurred */

  if (pcmcia_ide_hdl_task != 0) {
    rtems_event_send(pcmcia_ide_hdl_task,PCMCIA_IDE_INTERRUPT_EVENT);
  }
}


void mpc5200_pcmciaide_dma_init(int minor)
{
  TaskSetupParamSet_t	rxParam;	/* RX task setup parameters	*/
  TaskSetupParamSet_t	txParam;	/* TX task setup parameters	*/
  /*
   * Init Bestcomm system
   */
  bestcomm_glue_init();
  /*
   * Setup the SDMA RX task.
   */
  rxParam.NumBD        = PCMCIA_IDE_DMA_RD_BD_CNT;
  rxParam.Size.MaxBuf  = PCMCIA_IDE_RD_SECTOR_SIZE;
  rxParam.Initiator    = INITIATOR_ALWAYS;
  rxParam.StartAddrSrc =
    (uint32)mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD];
  rxParam.IncrSrc      = 0;
  rxParam.SzSrc	       = sizeof(uint16_t);
  rxParam.StartAddrDst = (uint32)NULL;
  rxParam.IncrDst      = sizeof(uint16_t);
  rxParam.SzDst	       = sizeof(uint16_t);  /* XXX: set this to 32 bit? */

  pcmcia_ide_rxTaskId  = TaskSetup(IDE_RX_TASK_NO,&rxParam );

  /*
   * Setup the TX task.
   */
  txParam.NumBD        = PCMCIA_IDE_DMA_WR_BD_CNT;
  txParam.Size.MaxBuf  = PCMCIA_IDE_WR_SECTOR_SIZE;
  txParam.Initiator    = INITIATOR_ALWAYS;
  txParam.StartAddrSrc = (uint32)NULL;
  txParam.IncrSrc      = sizeof(uint16_t);
  txParam.SzSrc        = sizeof(uint16_t); /* do not set this to 32 bit! */
  txParam.StartAddrDst =
    (uint32)mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD];
  txParam.IncrDst      = 0;
  txParam.SzDst        = sizeof(uint16_t);

  pcmcia_ide_txTaskId  = TaskSetup( IDE_TX_TASK_NO, &txParam );
  /*
   * FIXME: Init BD rings
   */
  /*
   * Enable the SmartDMA transmit/receive task.
   * do not enable interrupts to CPU
   */
  /*
   * connect interrupt handlers
   */
  bestcomm_glue_irq_install(IDE_TX_TASK_NO,pcmcia_ide_xmit_dmairq_hdl,NULL);
  bestcomm_glue_irq_install(IDE_RX_TASK_NO,pcmcia_ide_recv_dmairq_hdl,NULL);
}
#endif /* IDE_USE_DMA */

void mpc5200_pcmciaide_dma_blockop(bool is_write,
				   int minor,
				   uint16_t block_size,
				   rtems_blkdev_sg_buffer *bufs,
				   uint32_t *cbuf,
				   uint32_t *pos)

{
#if IDE_USE_DMA
  /*
   * Nameing:
   * - a block is one unit of data on disk (multiple sectors)
   * - a buffer is a contignuous chunk of data in memory
   * a block on disk may be filled with data from several buffers
   */
  uint32_t buf_idx,bufs_from_dma, bufs_to_dma,bufs_total;
  uint32_t bds_free;
  uint32_t llength;
  rtems_status_code rc = RTEMS_SUCCESSFUL;
  rtems_event_set events;
  BDIdx nxt_bd_idx;
  bool use_irq = (_System_state_Current == SYSTEM_STATE_UP);
  /*
   * determine number of blocks
   */
  llength = 0;
  buf_idx = 0;
  bufs += *cbuf; /* *cbuf is the index of the next buffer to send in this transaction */
  while (llength < block_size) {
    llength += bufs[buf_idx++].length;
  }
  bufs_from_dma = 0;
  bufs_to_dma   = 0;
  bufs_total    = buf_idx;
  /*
   * here all BDs should be unused
   */
  bds_free = is_write ? PCMCIA_IDE_DMA_WR_BD_CNT : PCMCIA_IDE_DMA_RD_BD_CNT;
  /*
   * repeat, until all bufs are transferred
   */
  while ((rc == RTEMS_SUCCESSFUL) &&
	 (bufs_from_dma < bufs_total)) {

    while ((rc == RTEMS_SUCCESSFUL) &&
	   (bufs_to_dma < bufs_total) &&
	   (bds_free > 0)) {
      /*
       * fill in BD, set interrupt if needed
       */
    SDMA_CLEAR_IEVENT(&mpc5200.sdma.IntPend,(is_write
	                                  ? IDE_TX_TASK_NO
	                                  : IDE_RX_TASK_NO));
      if (is_write) {
	TaskBDAssign(pcmcia_ide_txTaskId ,
		     (void *)bufs[bufs_to_dma].buffer,
		     (void *)mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD],
		     bufs[bufs_to_dma].length,
		     0/* flags */);
#if IDE_USE_STATISTICS
	mpc5200_pcmciaide_write_block_block_cnt++;
#endif
      }
      else {
	TaskBDAssign(pcmcia_ide_rxTaskId ,
		     (void *)mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD],
		     (void *)bufs[bufs_to_dma].buffer,
		     bufs[bufs_to_dma].length,
		     0/* flags */);
#if IDE_USE_STATISTICS
	mpc5200_pcmciaide_read_block_block_cnt++;
#endif
      }
      bufs_to_dma ++;
      bds_free    --;
    }
    if (is_write) {
      TaskStart( pcmcia_ide_txTaskId, TASK_AUTOSTART_DISABLE,
		 pcmcia_ide_txTaskId, TASK_INTERRUPT_DISABLE );
    }
    else {
      TaskStart( pcmcia_ide_rxTaskId, TASK_AUTOSTART_DISABLE,
		 pcmcia_ide_rxTaskId, TASK_INTERRUPT_DISABLE );
    }
    if (use_irq) {

      /*
       * enable interrupts, wait for interrupt event
       */
      pcmcia_ide_hdl_task = rtems_task_self();
      bestcomm_glue_irq_enable((is_write
				? IDE_TX_TASK_NO
				: IDE_RX_TASK_NO));

      rtems_event_receive(PCMCIA_IDE_INTERRUPT_EVENT,
			  RTEMS_WAIT | RTEMS_EVENT_ANY,
			  RTEMS_NO_TIMEOUT, &events);

      pcmcia_ide_hdl_task = 0;
    }
    else {
      /*
       * HACK: just wait some time...
       */
      /*
       * FIXME: poll, until SDMA is finished
       */
      volatile int32_t i;
      for (i = 0;i < 10000;i++) {};
    }

    do {
	  nxt_bd_idx = TaskBDRelease(is_write
	   				             ? pcmcia_ide_txTaskId
					             : pcmcia_ide_rxTaskId);
      if ((nxt_bd_idx != TASK_ERR_BD_RING_EMPTY) &&
          (nxt_bd_idx != TASK_ERR_BD_BUSY)) {
        (*cbuf)++;
        (*pos) += bufs[bufs_from_dma].length;
        bufs_from_dma++;
        bds_free++;
	  }
    } while ((nxt_bd_idx != TASK_ERR_BD_RING_EMPTY) &&
             (nxt_bd_idx != TASK_ERR_BD_BUSY)       &&
  	         (bufs_from_dma < bufs_to_dma));
  }
#endif /* IDE_USE_DMA */
}


void mpc5200_pcmciaide_read_block(int minor, uint32_t block_size, rtems_blkdev_sg_buffer *bufs,
                                  uint32_t *cbuf, uint32_t *pos)
{

  volatile uint32_t *ata_reg=mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD];
  uint16_t  cnt = 0;
  uint16_t *lbuf = (uint16_t*)((uint8_t*)(bufs[(*cbuf)].buffer)+(*pos));
  uint32_t  llength = bufs[(*cbuf)].length;
  bool      use_dma;

#if IDE_USE_STATISTICS
      mpc5200_pcmciaide_read_block_call_cnt++;
#endif
#if IDE_READ_USE_DMA
  /*
   * FIXME: walk through buffer list. If any buffer has other size than default,
   * then do not use DMA
   * Is this needed?
   */
  use_dma = true;
  /* use_dma = false; */
#else
  use_dma = false;
#endif
  if (use_dma) {
    /*
     * FIXME: wait for DRQ ready
     * check, that once DRQ is ready, we really can send ALL data for this
     * type of transfer mode
     */
    while ((GET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG((volatile uint32_t)
						 (mpc5200.ata_dctr_dasr)) &
	    IDE_REGISTER_STATUS_DRQ) == 0);
    /*
     * translate (part of) buffer list into DMA BDs
     * only last (available) DMA BD sends interrupt
     * DMA BDs may get ready as soon as possible
     */
    mpc5200_pcmciaide_dma_blockop(FALSE, /* read operation */
				  minor,
				  block_size,bufs,cbuf,pos);
  }
  else {
#if IDE_USE_READ_PIO_OPT
    while(cnt < block_size) {

      *lbuf++ = GET_UP_WORD_OF_MPC5200_ATA_DRIVE_REG(*(volatile uint32_t *)(ata_reg)); /* only 16 bit data port */
      cnt += 2;
      (*pos) += 2;

      if((*pos) == llength) {

	  (*pos) = 0;
	  (*cbuf)++;
	  lbuf = bufs[(*cbuf)].buffer;
	  llength = bufs[(*cbuf)].length;

      }
    }
#else

    while((GET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG((volatile uint32_t)(mpc5200.ata_dctr_dasr)) & IDE_REGISTER_STATUS_DRQ) && (cnt < block_size)) {

      *lbuf++ = *(volatile uint16_t *)(ata_reg); /* only 16 bit data port */
      cnt += 2;
      (*pos) += 2;

      if((*pos) == llength) {
	(*pos) = 0;
	(*cbuf)++;
	lbuf = bufs[(*cbuf)].buffer;
	llength = bufs[(*cbuf)].length;
      }
    }
#endif
  }
}

void mpc5200_pcmciaide_write_block(int minor, uint32_t block_size,
                                   rtems_blkdev_sg_buffer *bufs, uint32_t *cbuf,
                                   uint32_t *pos)

{


  volatile uint32_t *ata_reg = mpc5200_ata_drive_regs[IDE_REGISTER_DATA_WORD];
  uint16_t  cnt = 0;
  uint16_t *lbuf = (uint16_t *)((uint8_t *)(bufs[(*cbuf)].buffer) + (*pos));
  uint32_t  llength = bufs[(*cbuf)].length;
  bool use_dma;

#if IDE_USE_STATISTICS
  mpc5200_pcmciaide_write_block_call_cnt++;
#endif
#if IDE_WRITE_USE_DMA
  /*
   * FIXME: walk through buffer list. If any buffer has other size than default,
   * then do not use DMA
   * Is this needed?
   */
  use_dma = true;
#else
  use_dma = false;
#endif

  if (use_dma) {
    /*
     * wait for DRQ ready
     * FIXME: check, that once DRQ is ready, we really can send ALL data for this
     * type of transfer mode
     */
    while ((GET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG((volatile uint32_t)
						 (mpc5200.ata_dctr_dasr)) &
	    IDE_REGISTER_STATUS_DRQ) == 0);
    /*
     * translate (part of) buffer list into DMA BDs
     * only last (available) DMA BD sends interrupt
     * DMA BDs may get ready as soon as possible
     */
    mpc5200_pcmciaide_dma_blockop(true, /* write opeartion */
				  minor,
				  block_size,bufs,cbuf,pos);
  }
  else {
#if IDE_USE_WRITE_PIO_OPT
    while(cnt < block_size) {
      int32_t loop_cnt,loop_max;

#if IDE_USE_STATISTICS
      mpc5200_pcmciaide_write_block_block_cnt++;
#endif

      loop_max = llength - (*pos) ;
      if (loop_max > (block_size - cnt)) {
	loop_max = (block_size - cnt);
      }
      for (loop_cnt = loop_max/2;loop_cnt > 0;loop_cnt--) {
	*(volatile uint32_t *)(ata_reg) =
	  SET_UP_WORD_OF_MPC5200_ATA_DRIVE_REG(*lbuf++); /* only 16 bit data port */
      }
      cnt += loop_max;
      (*pos) += loop_max;

      if((*pos) == llength) {

	(*pos) = 0;
	(*cbuf)++;
	lbuf = bufs[(*cbuf)].buffer;
	llength = bufs[(*cbuf)].length;
      }
    }
#else
    while((GET_UP_BYTE_OF_MPC5200_ATA_DRIVE_REG((volatile uint32_t)(mpc5200.ata_dctr_dasr))
	   & IDE_REGISTER_STATUS_DRQ)
	  && (cnt < block_size)) {
      *(volatile uint16_t *)(ata_reg) = *lbuf++; /* only 16 bit data port */
      cnt += 2;
      (*pos) += 2;

      if((*pos) == llength) {
	(*pos) = 0;
	(*cbuf)++;
	lbuf = bufs[(*cbuf)].buffer;
	llength = bufs[(*cbuf)].length;
      }
    }
#endif
  }
}

int mpc5200_pcmciaide_control(int  minor, uint32_t cmd, void * arg)
  {
  return RTEMS_SUCCESSFUL;
  }

void mpc5200_pcmciaide_initialize(int minor)
  {
#if defined (BRS5L)
  struct mpc5200_gpt *gpt = (struct mpc5200_gpt *)(&mpc5200.gpt[GPT7]);

  /* invert ATA reset on GPT7 */
  gpt->emsel = (GPT_EMSEL_GPIO_OUT_HIGH | GPT_EMSEL_TIMER_MS_GPIO);
#endif
  /* reset ata host contr. and FIFO */
  mpc5200.ata_hcfg |= (ATA_HCFG_SMR | ATA_HCFG_FR);
  mpc5200.ata_hcfg &= ~(ATA_HCFG_SMR | ATA_HCFG_FR);

  /* for the first access set lowest performance transfer mode to PIO3 */
  mpc5200_pcmciaide_config_io_speed(minor, ATA_MODES_PIO3);

  /* enable PIO operations (PIO 3/4) */
  mpc5200.ata_hcfg |= ATA_HCFG_IORDY;

#ifdef IDE_USE_INT
  mpc5200.ata_hcfg |= ATA_HCFG_IE ;
#endif

#if IDE_USE_DMA
  mpc5200_pcmciaide_dma_init(minor);
#endif
  }


/*
 * The following table configures the functions used for IDE drivers
 * in this BSP.
 */
ide_ctrl_fns_t mpc5200_pcmciaide_ctrl_fns =
  {
  mpc5200_pcmciaide_probe,
  mpc5200_pcmciaide_initialize,
  mpc5200_pcmciaide_control,
  mpc5200_pcmciaide_read_reg,
  mpc5200_pcmciaide_write_reg,
  mpc5200_pcmciaide_read_block,
  mpc5200_pcmciaide_write_block,
  mpc5200_pcmciaide_config_io_speed
  };

