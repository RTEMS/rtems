/*  This file contains the GRASCS RTEMS driver
 *
 *  COPYRIGHT (c) 2008.
 *  Cobham Gaisler AB.
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.org/license/LICENSE.
 */

#include <stdlib.h>
#include <bsp.h>
#include <grlib/ambapp.h>
#include <grlib/grascs.h>
#include <grlib/grlib.h>

#include <grlib/grlib_impl.h>

#ifndef GAISLER_ASCS
#define GAISLER_ASCS 0x043
#endif

#ifdef DEBUG
#define DBG(x...) printk(x)
#else
#define DBG(x...)
#endif

typedef struct {
  volatile unsigned int cmd;
  volatile unsigned int clk;
  volatile unsigned int sts;
  volatile unsigned int tcd;
  volatile unsigned int tmd;
} GRASCS_regs;

typedef struct {
  unsigned char tmconf;
  unsigned char usconf;
  unsigned char nslaves;
  unsigned char dbits;
  int clkfreq;
} GRASCS_caps;

typedef struct {
  GRASCS_regs *regs; /* Pointer to core registers */
  GRASCS_caps *caps; /* Pointer to capability struct */
  rtems_id tcsem1, tcsem2;
  rtems_id tmsem1, tmsem2;
  volatile char running;
  int tcptr;
  int tmptr;
  int tcwords;
  int tmwords;
} GRASCS_cfg;

static GRASCS_cfg *cfg = NULL;

/*------------------------------------*/
/* Start of internal helper functions */ 
/*------------------------------------*/

/* Function: ASCS_getaddr
   Arguments: base: Core's register base address
              irq: Core's irq
   Return values: 0 if successful, -1 if core is not found
   Description: Assigns core's register base address and
     irq to arguments. Uses AMBA plug and play to find the
     core.
*/
static int ASCS_get_addr(int *base, int *irq) {

  struct ambapp_apb_info core;
  
  if(ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_ASCS, &core) == 1) {
    *base = core.start;
    *irq = core.irq;
    DBG("ASCS_get_addr: Registerd ASCS core at 0x%x with irq %i\n",core.start, core.irq);
    return 0;
  }
  DBG("ASCS_get_addr: Failed to detect core\n");
  return -1;  
}

/* Function: ASCS_calc_clkreg
   Arguments: sysfreq: System clock frequency in kHz
              etrfreq: ETR frequency in Hz
   Return values: Value of core's CLK-register
   Description: Calculates value of core's CLK-register. See
     GRASCS IP core documentation for details.
*/
static int ASCS_calc_clkreg(int sysfreq, int etrfreq) {

  if(cfg->caps->usconf)
    return 1000000/etrfreq;
  else
    return sysfreq*1000/etrfreq;
}

/* Function: ASCS_get_sysfreq
   Arguments: - 
   Return values: System clock frequency in kHz, -1 if failed
   Description: Uses AMBA plug and play to lookup system frequency
*/
static int ASCS_get_sysfreq(void) {

  struct ambapp_apb_info gpt;
  struct gptimer_regs *tregs;
  int tmp;

  if(ambapp_find_apbslv(&ambapp_plb, VENDOR_GAISLER, GAISLER_GPTIMER, &gpt) == 1) {
    tregs = (struct gptimer_regs *) gpt.start;
    tmp = (tregs->scaler_reload + 1)*1000;
    DBG("ASCS_get_sysfreq: Detected system frequency %i kHz\n",tmp);
    if((tmp < GRASCS_MIN_SFREQ) || (tmp > GRASCS_MAX_SFREQ)) {
      DBG("ASCS_get_sysfreq: System frequency is invalid for ASCS core\n");
      return -1;
    }
    else
      return (tregs->scaler_reload + 1)*1000;
  }
  DBG("ASCS_get_sysfreq: Failed to detect system frequency\n");
  return -1;
}

/* Function: ASCS_irqhandler
   Arguments: v: not used
   Return values: -
   Description: Determines the source of the interrupt, clears the
                appropriate bits in the core's STS register and releases
		the associated semaphore
*/
static rtems_isr ASCS_irqhandler(void *v) {

  if(cfg->regs->sts & GRASCS_STS_TCDONE) {
    /* Clear TC done bit */
    cfg->regs->sts |= GRASCS_STS_TCDONE;    

    if(--cfg->tcwords == 0)
      /* No more TCs to perform right now */
      rtems_semaphore_release(cfg->tcsem2);
    else {
      /* Block not sent yet, start next TC */
      if(cfg->caps->dbits == 8) {
	cfg->tcptr++;
	cfg->regs->tcd = *((unsigned char*)cfg->tcptr);
      }
      else if(cfg->caps->dbits == 16) {
	cfg->tcptr += 2;
	cfg->regs->tcd = *((unsigned short int*)cfg->tcptr);
      }
      else {
	cfg->tcptr += 4;
	cfg->regs->tcd = *((unsigned int*)cfg->tcptr);
      }
    }
  }

  if(cfg->regs->sts & GRASCS_STS_TMDONE) {
    /* Clear TM done bit */
    cfg->regs->sts |= GRASCS_STS_TMDONE;

    /* Store received data */
    if(cfg->caps->dbits == 8) {
      *((unsigned char*)cfg->tmptr) = (unsigned char)(cfg->regs->tmd & 0xFF);
      cfg->tmptr++;
    }
    else if(cfg->caps->dbits == 16) {
      *((unsigned short int*)cfg->tmptr) = (unsigned short int)(cfg->regs->tmd & 0xFFFF);
      cfg->tmptr += 2;
    }
    else {
      *((unsigned int*)cfg->tmptr) = cfg->regs->tmd;
      cfg->tmptr += 4;
    }
    
    if(--cfg->tmwords == 0)
      /* No more TMs to perform right now */
      rtems_semaphore_release(cfg->tmsem2);
    else
      /* Block not received yet, start next TM */
      cfg->regs->cmd |= GRASCS_CMD_SENDTM;      
  }
}

/*---------------------------*/
/* Start of driver interface */
/*---------------------------*/

/* Function: ASCS_init
   Arguments: -
   Return values: 0 if successful, -1 if unsuccessful
   Description: Initializes the ASCS core
*/
int ASCS_init(void) {

  int base, irq, tmp;

  DBG("ASCS_init: Starting initialization of ASCS core\n");
  
  /* Allocate memory for config, status and capability struct */
  if((cfg = grlib_malloc(sizeof(*cfg))) == NULL) {
    DBG("ASCS_init: Could not allocate memory for cfg struc\n");
    return -1;
  }
  
  if((cfg->caps = grlib_calloc(1,sizeof(*cfg->caps))) == NULL) {
    DBG("ASCS_init: Could not allocate memory for caps struc\n");
    goto init_error1;
  }
    
  /* Create semaphores for blocking ASCS_TC/TM functions */
  if(rtems_semaphore_create(rtems_build_name('A','S','C','0'),1,
			    (RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|
			     RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|
			     RTEMS_NO_PRIORITY_CEILING), 0,
			    &cfg->tcsem1) != RTEMS_SUCCESSFUL) {
    DBG("ASCS_init: Failed to create semaphore ASC0\n");
    goto init_error2;
  }
  if(rtems_semaphore_create(rtems_build_name('A','S','C','1'),1,
			    (RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|
			     RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|
			     RTEMS_NO_PRIORITY_CEILING), 0,
			    &cfg->tmsem1) != RTEMS_SUCCESSFUL) {
    DBG("ASCS_init: Failed to create semaphore ASC1\n");
    goto init_error2;
  }
  /* Create semaphores for waiting on ASCS_TC/TM interrupt */
  if(rtems_semaphore_create(rtems_build_name('A','S','C','2'),0,
			    (RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|
			     RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|
			     RTEMS_NO_PRIORITY_CEILING), 0,
			    &cfg->tcsem2) != RTEMS_SUCCESSFUL) {
    DBG("ASCS_init: Failed to create semaphore ASC2\n");
    goto init_error2;
  }
  if(rtems_semaphore_create(rtems_build_name('A','S','C','3'),0,
			    (RTEMS_FIFO|RTEMS_SIMPLE_BINARY_SEMAPHORE|
			     RTEMS_NO_INHERIT_PRIORITY|RTEMS_LOCAL|
			     RTEMS_NO_PRIORITY_CEILING), 0,
			    &cfg->tmsem2) != RTEMS_SUCCESSFUL) {
    DBG("ASCS_init: Failed to create semaphore ASC3\n");
    goto init_error2;
  }

  /* Set pointer to core registers */
  if(ASCS_get_addr(&base, &irq) == -1)
    goto init_error2;

  cfg->regs = (GRASCS_regs*)base;
  
  /* Read core capabilities */
  tmp = cfg->regs->sts;
  cfg->caps->dbits = ((tmp >> GRASCS_STS_DBITS_BITS) & 0x1F) + 1;
  cfg->caps->nslaves = ((tmp >> GRASCS_STS_NSLAVES_BITS) & 0xF) + 1;
  cfg->caps->tmconf = (tmp >> GRASCS_STS_TMCONF_BITS) & 0x1;
  cfg->caps->usconf = (tmp >> GRASCS_STS_USCONF_BITS) & 0x1;

  /* Reset and configure core */
  cfg->running = 0;
  cfg->regs->cmd |= GRASCS_CMD_RESET;
  if((tmp = ASCS_get_sysfreq()) == -1)
    goto init_error2;
  cfg->caps->clkfreq = tmp;
  while(ASCS_iface_status())
    ;  
  cfg->regs->clk = ASCS_calc_clkreg(tmp, GRASCS_DEFAULT_ETRFREQ);
  cfg->regs->cmd = GRASCS_CMD_US1C;
  cfg->regs->cmd |= (tmp/1000 << GRASCS_CMD_US1_BITS) | GRASCS_CMD_US1C |
    GRASCS_CMD_TCDONE | GRASCS_CMD_TMDONE;

  /* Register interrupt routine */
  rtems_interrupt_handler_install(irq, "grascs",
		  RTEMS_INTERRUPT_SHARED,
		  ASCS_irqhandler, NULL);
  
  return 0;

 init_error2:
  free(cfg->caps);
 init_error1:
  free(cfg);
  return -1;
}

/* Function: ASCS_input_select
   Arguments: slave: The number of the slave that is active,
              numbered from 0-15
   Return values: 0 if successful, -GRASCS_ERROR_CAPFAULT if slave value
                  is negative or too big, -GRASCS_ERROR_TRANSACTIVE if
		  a TM is active.
   Description: Sets the slave_sel bits in the core's CMD register.
                they are used to choose which slave the core listens
		to when performing a TM. The bits can't be set
		during a TM, and the function will in such a case fail.
*/
int ASCS_input_select(int slave) {

  if((slave < 0) || (slave > cfg->caps->nslaves)) {
    /* Slave number is negative or too big */
    DBG("ASCS_input_select: Wrong slave number\n");
    return -GRASCS_ERROR_CAPFAULT;
  }
    
  if(rtems_semaphore_obtain(cfg->tmsem1,RTEMS_NO_WAIT,RTEMS_NO_TIMEOUT) !=
     RTEMS_SUCCESSFUL) {
    /* Can't change active slave during a TM */
    DBG("ASCS_input_select: Transaction active\n");
    return -GRASCS_ERROR_TRANSACTIVE;
  }
  
  cfg->regs->cmd = ((cfg->regs->cmd &= ~GRASCS_CMD_SLAVESEL) |
		    (slave << GRASCS_CMD_SLAVESEL_BITS));

  rtems_semaphore_release(cfg->tmsem1);
  return 0;
}

/* Function: ASCS_etr_select
   Arguments: src: The source of the ETR signal, valid values are
                   0-GRASCS_MAX_TMS (0 = internal source, 1-GRASCS_MAX_TMS =
		   external time markers 1-GRASCS_MAX_TMS).
	      freq: ETR frequency in Hz. Valid values are
	            GRASCS_MIN_ETRFREQ-GRASCS_MAX_ETRFREQ
   Return values: 0 if successful, -GRASCS_ERROR_CAPFAULT if src or freq values
                  are invalid, -GRASCS_ERROR_STARTSTOP if synchronization interface
		  isn't stopped.
   Description: Changes the source for the ETR signal. The frequency of source signal
                is assumed to be the same as the frequency of the freq input
*/
int ASCS_etr_select(int etr, int freq) {
  
  if((etr < 0) || (etr > GRASCS_MAX_TMS) || ((cfg->caps->tmconf == 0) && (etr > 0)) ||
     (freq < GRASCS_MIN_ETRFREQ) || (freq > GRASCS_MAX_ETRFREQ)) {
    /* ETR source value or frequency is invalid */
    DBG("ASCS_etr_select: Wrong etr src number or wrong frequency\n");
    return -GRASCS_ERROR_CAPFAULT;
  }
  
  if(cfg->regs->sts & GRASCS_STS_ERUNNING) {
    /* Synchronization interface is running */
    DBG("ASCS_etr_select: Synch interface is running\n");
    return -GRASCS_ERROR_STARTSTOP;
  }
  
  cfg->regs->clk = ASCS_calc_clkreg(cfg->caps->clkfreq,freq);
  cfg->regs->cmd = ((cfg->regs->cmd &= ~GRASCS_CMD_ETRCTRL) |
		    (etr << GRASCS_CMD_ETRCTRL_BITS));

  return 0;
}

/* Function: ASCS_start
   Arguments: -
   Return values: -
   Description: Enables the serial interface. 
*/
void ASCS_start(void) {
  
  /* Set register and internal status to running */
  cfg->regs->cmd |= GRASCS_CMD_STARTSTOP;
  cfg->running = 1;
}

/* Function: ASCS_stop
   Arguments: -
   Return values: -
   Description: Disables the serial interface. This function will
                block until possible calls to TC_send(_block) and
		TM_recv(_block) has returned in order to be sure
		that started transactions will be performed.
*/
void ASCS_stop(void) {

  /* Set internal status to stopped */
  cfg->running = 0;

  /* Obtain semaphores to avoid possible situation where a
     TC_send(_block) or TM_recv(_block) is aborted and driver is
     waiting forever for an interrupt */
  rtems_semaphore_obtain(cfg->tcsem1,RTEMS_WAIT,RTEMS_NO_TIMEOUT);
  rtems_semaphore_obtain(cfg->tmsem1,RTEMS_WAIT,RTEMS_NO_TIMEOUT);

  /* Change actual register value */
  cfg->regs->cmd &= ~GRASCS_CMD_STARTSTOP;

  /* Release the semaphores */
  rtems_semaphore_release(cfg->tcsem1);
  rtems_semaphore_release(cfg->tmsem1);
}

/* Function: ASCS_iface_status
   Arguments: - 
   Return values: 0 if both serial interface and synch interface is stopped,
                  1 if serial interface is running buth synch interface is
		  stopped, 2 if serial interface is stopped but synch interface
		  is running, 3 if both serial and synch interface is running
   Description: Reads the core's STS register and reports the status of the
                serial and synch interfaces
*/
int ASCS_iface_status(void) {

  return ((cfg->regs->sts & 0x3) & (0x2 | cfg->running));
}

/* Function: ASCS_TC_send
   Arguments: word: Pointer to a word that should be sent
   Return values: 0 on success
                  -GRASCS_ERROR_STARTSTOP if serial interface is stopped,
		  -GRASCS_ERROR_TRANSACTIVE if another TC is in progress.
   Description: Start a TC and sends the data that word points to.
*/
int ASCS_TC_send(int *word) {

  int retval;

  if(rtems_semaphore_obtain(cfg->tcsem1,RTEMS_NO_WAIT,RTEMS_NO_TIMEOUT) !=
     RTEMS_SUCCESSFUL) {
    /* Can't start a TC_send if another TC_send of TC_send_block is
       in progress */
    DBG("ASCS_TC_send: Could not obtain semaphore, transcation probably in progress\n");
    return -GRASCS_ERROR_TRANSACTIVE;
  }
    
  if(!cfg->running) {
    /* Can't start a TC if serial interface isn't started */
    DBG("ASCS_TC_send: Serial interface is not started\n");
    retval = -GRASCS_ERROR_STARTSTOP;
  }
  else {
    /* Start the transfer */
    cfg->tcwords = 1;
    if(cfg->caps->dbits == 8)
      cfg->regs->tcd = *((unsigned char*)word);
    else if(cfg->caps->dbits == 16)
      cfg->regs->tcd = *((unsigned short int*)((int)word & ~1));
    else
      cfg->regs->tcd = *((unsigned int*)((int)word & ~3));
    
    /* Wait until transfer is complete */
    rtems_semaphore_obtain(cfg->tcsem2,RTEMS_WAIT,RTEMS_NO_TIMEOUT);
    retval = 0;
  }

  rtems_semaphore_release(cfg->tcsem1);
  
  return retval;
}

/* Function: ASCS_TC_send_block
   Arguments: block: Pointer to the start of a datablock that
                     should be sent.
	      ntrans: Number of transfers needed to transfer
	              the block.
   Return values: 0 if successfull, -GRASCS_ERROR_STARTSTOP if TC
                  couldn't be started because serial interface is
                  stopped, -GRASCS_ERROR_TRANSACTIVE if TC couldn't
		  be started because another TC isn't done yet.
   Description: Starts ntrans TCs and sends the data that starts at the
                address that block points to. The size of each
		transaction will vary depending on whether the core is
		configured for 8, 16, or 32 bits data transfers.
*/
int ASCS_TC_send_block(int *block, int ntrans) {

  int retval;

  if(rtems_semaphore_obtain(cfg->tcsem1,RTEMS_NO_WAIT,RTEMS_NO_TIMEOUT) !=
     RTEMS_SUCCESSFUL) {
    /* Can't start a TC_send_block if another TC_send of TC_send_block is
       in progress */
    DBG("ASCS_TC_send_block: Could not obtain semaphore, transcation probably in progress\n");
    return -GRASCS_ERROR_TRANSACTIVE;
  }

  if(!cfg->running) {
    /* Can't start a TC if serial interface isn't started */
    DBG("ASCS_TC_send_block: Serial interface is not started\n");
    retval = -GRASCS_ERROR_STARTSTOP;
  }
  else {
    /* Start the first transfer */
    cfg->tcwords = ntrans;
    if(cfg->caps->dbits == 8) {
      cfg->tcptr = (int)block;
      cfg->regs->tcd = *((unsigned char*)cfg->tcptr);
    }
    else if(cfg->caps->dbits == 16) {
      cfg->tcptr = (int)block & ~1;
      cfg->regs->tcd = *((unsigned short int*)cfg->tcptr);
    }
    else {
      cfg->tcptr = (int)block & ~3;
      cfg->regs->tcd = *((unsigned int*)cfg->tcptr);
    }
        
    /* Wait until all transfers are complete */
    rtems_semaphore_obtain(cfg->tcsem2,RTEMS_WAIT,RTEMS_NO_TIMEOUT);
    retval = 0;
  }
  
  rtems_semaphore_release(cfg->tcsem1);

  return retval;
}

/* Function: ASCS_TC_sync_start
   Arguments: -
   Return values: -
   Description: Starts synchronization interface. Might
     be delayed if a TM is in progress. SW can poll
     ASCS_iface_status() to find out when synch interface is
     started. First ETR pulse can be delay up to one ETR
     period depending on the source of the ETR and
     activity on the TM line.
*/
void ASCS_TC_sync_start(void) {
  
  cfg->regs->cmd |= GRASCS_CMD_ESTARTSTOP;
}

/* Function: ASCS_TC_sync_stop
   Arguments: -
   Return values: -
   Description: Stops the synchronization interface. Might
     be delayed for 1 us if a ETR pulse is being generated. SW
     can determine when synch interface has stopped by polling
     ASCS_iface_status().
*/
void ASCS_TC_sync_stop(void) {
  
  cfg->regs->cmd &= ~GRASCS_CMD_ESTARTSTOP;
}

/* Function: ASCS_TM_recv
   Arguments: word: Pointer to where the received word should be
                    placed
   Return values: 0 if successful, -GRASCS_ERROR_STARTSTOP if serial
                  interface isn't started, -GRASCS_ERROR_TRANSACTIVE
		  if another TM is in progress
   Description: Starts a TM and stores the incoming data in word.
*/
int ASCS_TM_recv(int *word) {

  int retval;

  if(rtems_semaphore_obtain(cfg->tmsem1,RTEMS_NO_WAIT,RTEMS_NO_TIMEOUT) !=
     RTEMS_SUCCESSFUL) {
    /* Can't start a TM_recv if another TM_recv of TM_recv_block is
       in progress */
    DBG("ASCS_TM_recv: Could not obtain semaphore, transaction probably in progress\n");
    return -GRASCS_ERROR_TRANSACTIVE;
  }

  if(!cfg->running) {
    /* Can't start a TM if serial interface isn't started */
    DBG("ASCS_TM_recv: Serial interface is not started\n");
    retval = -GRASCS_ERROR_STARTSTOP;
  }
  else {
    /* Start transfer */
    cfg->tmwords = 1;
    cfg->tmptr = (int)word;
    cfg->regs->cmd |= GRASCS_CMD_SENDTM;
    
    /* Wait until transfer finishes */
    rtems_semaphore_obtain(cfg->tmsem2,RTEMS_WAIT,RTEMS_NO_TIMEOUT);
    retval = 0;
  }

  rtems_semaphore_release(cfg->tmsem1);

  return retval;
}

/* Function: ASCS_TM_recv_block
   Arguments: block: Pointer to where the received datablock
                     should be stored.
	      ntrans: Number of transfers needed to transfer
	              the block.
   Return values: 0 if successful, -GRASCS_ERROR_STARTSTOP if serial
                  interface isn't started, -GRASCS_ERROR_TRANSACTIVE if
		  a performed TM hasn't been processed yet
   Description: Starts ntrans TMs and stores the data at the address
                that block points to. The size of each transaction
		will vary depending on whether the core is
		configured for 8, 16, or 32 bits data transfers.
*/
int ASCS_TM_recv_block(int *block, int ntrans) {

  int retval;

  if(rtems_semaphore_obtain(cfg->tmsem1,RTEMS_NO_WAIT,RTEMS_NO_TIMEOUT) !=
     RTEMS_SUCCESSFUL) {
    /* Can't start a TM_recv_block if another TM_recv of TM_recv_block is
       in progress */
    DBG("ASCS_TM_recv_block: Could not obtain semaphore, transaction probably in progress\n");
    return -GRASCS_ERROR_TRANSACTIVE;
  }

  if(!cfg->running) {
    /* Can't start a TM if serial interface isn't started */
    DBG("ASCS_TM_recv_block: Serial interface is not started\n");
    retval = -GRASCS_ERROR_STARTSTOP;
  }
  else {
    /* Start transfer */
    cfg->tmwords = ntrans;
    cfg->tmptr = (int)block;
    cfg->regs->cmd |= GRASCS_CMD_SENDTM;
    
    /* Wait until transfer finishes */
    rtems_semaphore_obtain(cfg->tmsem2,RTEMS_WAIT,RTEMS_NO_TIMEOUT);
    retval = 0;
  }

  rtems_semaphore_release(cfg->tmsem1);

  return retval;
}
