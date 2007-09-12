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
| this file contains the irq controller handler                   |
\*===============================================================*/
/***********************************************************************/
/*                                                                     */
/*   Module:       irq.c                                               */
/*   Date:         07/17/2003                                          */
/*   Purpose:      RTEMS MPC5x00 CPU main interrupt handler & routines */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Description:  This file contains the implementation of the        */
/*                 functions described in irq.h                        */
/*                                                                     */
/*---------------------------------------------------------------------*/
/*                                                                     */
/*   Code                                                              */
/*   References:   MPC8260ads main interrupt handler & routines        */
/*   Module:       irc.c                                               */
/*   Project:      RTEMS 4.6.0pre1 / MCF8260ads BSP                    */
/*   Version       1.2                                                 */
/*   Date:         04/18/2002                                          */
/*                                                                     */
/*   Author(s) / Copyright(s):                                         */
/*                                                                     */
/*   Copyright (C) 1998, 1999 valette@crf.canon.fr                     */
/*                                                                     */
/*   Modified for mpc8260 Andy Dachs <a.dachs@sstl.co.uk>              */
/*   Surrey Satellite Technology Limited, 2000                         */
/*   Nested exception handlers not working yet.                        */
/*                                                                     */
/*   The license and distribution terms for this file may be           */
/*   found in found in the file LICENSE in this distribution or at     */
/*   http://www.rtems.com/license/LICENSE.                        */
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

#include <bsp.h>
#include <rtems.h>
#include "../irq/irq.h"
#include <rtems/score/apiext.h>
#include <rtems/bspIo.h>
#include <libcpu/raw_exception.h>
#include "../vectors/vectors.h"
#include "../include/mpc5200.h"


extern uint32_t irqMaskTable[];

/*
 * default handler connected on each irq after bsp initialization
 */
static rtems_irq_connect_data	default_rtems_entry;

/*
 * location used to store initial tables used for interrupt
 * management.
 */
static rtems_irq_global_settings* 	internal_config;
static rtems_irq_connect_data*		rtems_hdl_tbl;

/*
 * bit in the SIU mask registers (PPC bit numbering) that should
 * be set to enable the relevant interrupt, mask of 32 is for unused entries
 *
 */
const static unsigned int SIU_MaskBit[BSP_SIU_IRQ_NUMBER] =
  {
  0, 1, 2, 3,       /* smart_comm, psc1, psc2, psc3            */
  4, 5, 6, 7,       /* irda/psc6, eth, usb, ata                */
  8, 9, 10, 11,     /* pci_ctrl, pci_sc_rx, pci_sc_tx, psc4    */
  12, 13, 14, 15,   /* psc5,spi_modf, spi_spif, i2c1           */
  16, 17, 18, 19,   /* i2c, can1, can2, ir_rx                  */
  20, 21, 15, 16,   /* ir_rx, xlb_arb, slice_tim2, irq1,       */
  17, 18, 19, 20,   /* irq2, irq3, lo_int, rtc_pint            */
  21, 22, 23, 24,   /* rtc_sint, gpio_std, gpio_wkup, tmr0     */
  25, 26, 27, 28,   /* tmr1, tmr2, tmr3, tmr4                  */
  29, 30, 31, 32,	/* tmr5, tmr6, tmr7, res                   */
  32, 32, 32        /* res, res, res                           */
  };

/*
 * Check if symbolic IRQ name is a Processor IRQ
 */
static inline int is_processor_irq(const rtems_irq_symbolic_name irqLine)
  {

  return (((int)irqLine <= BSP_PROCESSOR_IRQ_MAX_OFFSET) & ((int)irqLine >= BSP_PROCESSOR_IRQ_LOWEST_OFFSET));

  }

/*
 * Check for SIU IRQ and return base index
 */
static inline int is_siu_irq(const rtems_irq_symbolic_name irqLine)
  {

  return (((int)irqLine <= BSP_SIU_IRQ_MAX_OFFSET) && ((int)irqLine >= BSP_SIU_IRQ_LOWEST_OFFSET));

  }


/*
 * Check for SIU IRQ and return base index
 */
static inline int get_siu_irq_base_index(const rtems_irq_symbolic_name irqLine)
  {

  if(irqLine <= BSP_PER_IRQ_MAX_OFFSET)
    return BSP_PER_IRQ_LOWEST_OFFSET;
  else
    if(irqLine <= BSP_MAIN_IRQ_MAX_OFFSET)
      return BSP_MAIN_IRQ_LOWEST_OFFSET;
    else
      if(irqLine <= BSP_CRIT_IRQ_MAX_OFFSET)
        return BSP_CRIT_IRQ_LOWEST_OFFSET;
      else
        return -1;

  }


static inline void BSP_enable_per_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {
  uint8_t lo_hi_ind = 0, prio_index_offset;
  uint32_t *reg;
  rtems_irq_prio *irqPrioTable = internal_config->irqPrioTbl;
  volatile uint32_t per_pri_1,main_pri_1, crit_pri_main_mask, per_mask;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_PER_IRQ_LOWEST_OFFSET) % 8;

  /* set interrupt priorities */
  if(irqPrioTable[irqLine] <= 15)
    {

    /* set peripheral int priority */
	reg = (uint32_t *)(&(mpc5200.per_pri_1));

    /* choose proper register */
    reg += (irqLine >> 3);

    /* set priority as given in priority table */
    *reg |= (irqPrioTable[irqLine] << (28 - (prio_index_offset<< 2)));

    /* test msb (hash-bit) and set LO_/HI_int indicator */
    if((lo_hi_ind = (irqPrioTable[irqLine] >> 3)))
      {

      /* set critical HI_int priority */
      reg = (uint32_t *)(&(mpc5200.crit_pri_main_mask));
      *reg |= (irqPrioTable[BSP_SIU_IRQ_HI_INT] << 26);

	  /*
	   * critical interrupt handling for the 603le core is not
	   * yet supported, routing of critical interrupts is forced
	   * to core_int (bit 31 / CEb)
	   */
	   mpc5200.ext_en_type |= 1;

      }
    else
      {

      if(irqPrioTable[irqLine] <= 15)
	  	{

	    /* set main LO_int priority */
	    reg = (uint32_t *)(&(mpc5200.main_pri_1));
	    *reg |= (irqPrioTable[BSP_SIU_IRQ_LO_INT] << 16);

	  	}

      }

    }

  /* if LO_int ind., enable (unmask) main interrupt */
  if(!lo_hi_ind)
    {

	mpc5200.crit_pri_main_mask &= ~(0x80000000 >> SIU_MaskBit[BSP_SIU_IRQ_LO_INT]);

    }


  /* enable (unmask) peripheral interrupt */
  mpc5200.per_mask &= ~(0x80000000 >> SIU_MaskBit[irqLine]);

  main_pri_1         = mpc5200.main_pri_1;
  crit_pri_main_mask = mpc5200.crit_pri_main_mask;
  per_pri_1          = mpc5200.per_pri_1;
  per_mask           = mpc5200.per_mask;


  }


static inline void BSP_enable_main_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {

  uint8_t prio_index_offset;
  uint32_t *reg;
  rtems_irq_prio *irqPrioTable = internal_config->irqPrioTbl;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_MAIN_IRQ_LOWEST_OFFSET) % 8;

  /* set main interrupt priority */
  if(irqPrioTable[irqLine] <= 15)
    {

    /* set main int priority */
	reg = (uint32_t *)(&(mpc5200.main_pri_1));

	/* choose proper register */
    reg += (irqLine >> 3);

    /* set priority as given in priority table */
    *reg |= (irqPrioTable[irqLine] << (28 - (prio_index_offset << 2)));

    if((irqLine >= BSP_SIU_IRQ_IRQ1) && (irqLine <= BSP_SIU_IRQ_IRQ3))
      {

      /* enable external irq-pin */
	  mpc5200.ext_en_type |= (0x80000000 >> (20 + prio_index_offset));

      }

    }

  /* enable (unmask) main interrupt */
  mpc5200.crit_pri_main_mask &= ~(0x80000000 >> SIU_MaskBit[irqLine]);

  }


static inline void BSP_enable_crit_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {

  uint8_t prio_index_offset;
  uint32_t *reg;
  rtems_irq_prio *irqPrioTable = internal_config->irqPrioTbl;

  prio_index_offset = irqLine - BSP_CRIT_IRQ_LOWEST_OFFSET;

  /*
   * critical interrupt handling for the 603Le core is not
   * yet supported, routing of critical interrupts is forced
   * to core_int (bit 31 / CEb)
   */
  mpc5200.ext_en_type |= 1;


  /* set critical interrupt priorities */
  if(irqPrioTable[irqLine] <= 3)
    {

    /* choose proper register */
    reg = (uint32_t *)(&(mpc5200.crit_pri_main_mask));

    /* set priority as given in priority table */
    *reg |= (irqPrioTable[irqLine] << (30 - (prio_index_offset << 1)));

    /* external irq0-pin */
    if(irqLine == BSP_SIU_IRQ_IRQ1)
      {

      /* enable external irq-pin */
  	  mpc5200.ext_en_type |= (0x80000000 >> (20 + prio_index_offset));

      }

    }

  }


static inline void BSP_disable_per_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {

  uint8_t prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_PER_IRQ_LOWEST_OFFSET) % 8;

  /* disable (mask) peripheral interrupt */
  mpc5200.per_mask |= (0x80000000 >> SIU_MaskBit[irqLine]);

  /* reset priority to lowest level (reset value) */
  reg = (uint32_t *)(&(mpc5200.per_pri_1));
  reg += (irqLine >> 3);
  *reg &= ~(15 << (28 - (prio_index_offset << 2)));

  }


static inline void BSP_disable_main_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {

  uint8_t prio_index_offset;
  uint32_t *reg;

  /* calculate the index offset of priority value bit field */
  prio_index_offset = (irqLine - BSP_MAIN_IRQ_LOWEST_OFFSET) % 8;

  /* disable (mask) main interrupt */
  mpc5200.crit_pri_main_mask |= (0x80000000 >> SIU_MaskBit[irqLine]);

  if((irqLine >= BSP_SIU_IRQ_IRQ1) && (irqLine <= BSP_SIU_IRQ_IRQ3))
    {

    /* disable external irq-pin */
    mpc5200.ext_en_type &= ~(0x80000000 >> (20 + prio_index_offset));

    }

  /* reset priority to lowest level (reset value) */
  reg = (uint32_t *)(&(mpc5200.main_pri_1));
  reg  += (irqLine >> 3);
  *reg &= ~(15 << (28 - (prio_index_offset << 2)));

  }


static inline void BSP_disable_crit_irq_at_siu(const rtems_irq_symbolic_name irqLine)
  {

  uint8_t prio_index_offset;
  uint32_t *reg;

  prio_index_offset = irqLine - BSP_CRIT_IRQ_LOWEST_OFFSET;

  /* reset critical int priority to lowest level (reset value) */
  reg = (uint32_t *)(&(mpc5200.crit_pri_main_mask));
  *reg &= ~(3 << (30 - (prio_index_offset << 1)));

  if(irqLine == BSP_SIU_IRQ_IRQ1)
    {

    /* disable external irq0-pin */
    mpc5200.ext_en_type &= ~(0x80000000 >> (20 + prio_index_offset));

    }

  }


/*
 * ------------------------ RTEMS Irq helper functions ----------------
 */


/*
 * This function check that the value given for the irq line
 * is valid.
 */
static int isValidInterrupt(int irq)
  {
  if ( (irq < BSP_LOWEST_OFFSET) || (irq > BSP_MAX_OFFSET) )
    return 0;
  return 1;
  }


 /*
  * This function enables a given siu interrupt
  */
int BSP_irq_enable_at_siu(const rtems_irq_symbolic_name irqLine)
  {
  int base_index;

  if(is_siu_irq(irqLine))
    {


    if((base_index = get_siu_irq_base_index(irqLine)) != -1)
      {

      switch(base_index)
        {

        case BSP_PER_IRQ_LOWEST_OFFSET:
          BSP_enable_per_irq_at_siu(irqLine);
          break;

	    case BSP_MAIN_IRQ_LOWEST_OFFSET:
          BSP_enable_main_irq_at_siu(irqLine);
          break;

	    case BSP_CRIT_IRQ_LOWEST_OFFSET:
	      BSP_enable_crit_irq_at_siu(irqLine);
	      break;

	    default:
          printk("No valid base index\n");
          break;

        }

      }

    }

  return 0;

  }

/*
 * This function disables a given siu interrupt
 */
int BSP_irq_disable_at_siu(const rtems_irq_symbolic_name irqLine)
  {
  int base_index;

  if ( (base_index = get_siu_irq_base_index(irqLine)) == -1)
    return 1;

  switch(base_index)
    {

    case BSP_PER_IRQ_LOWEST_OFFSET:
      BSP_disable_per_irq_at_siu(irqLine);

      break;

	case BSP_MAIN_IRQ_LOWEST_OFFSET:
      BSP_disable_main_irq_at_siu(irqLine);
      break;

	case BSP_CRIT_IRQ_LOWEST_OFFSET:
	  BSP_disable_crit_irq_at_siu(irqLine);
	  break;

    default:
      printk("No valid base index\n");
      break;

    }

  return 0;
  }


/*
 * ------------------------ RTEMS Single Irq Handler Mngt Routines ----------------
 */

 /*
  * This function removes the default entry and installs a device interrupt handler
  */
int BSP_install_rtems_irq_handler (const rtems_irq_connect_data* irq)
  {
  rtems_interrupt_level level;

  if(!isValidInterrupt(irq->name))
    {

    printk("not a valid interrupt\n");
    return 0;

    }

  /*
   * Check if default handler is actually connected. If not issue an error.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if (rtems_hdl_tbl[irq->name].hdl != default_rtems_entry.hdl)
    {

    printk( "Default handler not there\n" );
    return 0;

    }

  rtems_interrupt_disable(level);

  /*
   * store the data provided by user
   */
  rtems_hdl_tbl[irq->name] = *irq;

  if(is_siu_irq(irq->name))
    {

    /*
     * Enable interrupt at siu level
     */
    BSP_irq_enable_at_siu(irq->name);

    }
  else
    {

    if(is_processor_irq(irq->name))
      {

      /*
       * Should Enable exception at processor level but not needed.  Will restore
       * EE flags at the end of the routine anyway.
       */


      }
    else
      {

	  printk("not a valid interrupt\n");
	  return 0;

	  }

    }


    /*
     * Enable interrupt on device
     */
  irq->on(irq);

  rtems_interrupt_enable(level);

  return 1;

  }


 /*
  * This function procures the current interrupt handler
  */
int BSP_get_current_rtems_irq_handler (rtems_irq_connect_data* irq)
  {

  if(!isValidInterrupt(irq->name))
    {

    return 0;

    }

  *irq = rtems_hdl_tbl[irq->name];
  return 1;

  }


 /*
  * This function removes a device interrupt handler and restores the default entry
  */
int BSP_remove_rtems_irq_handler (const rtems_irq_connect_data* irq)
  {
  rtems_interrupt_level level;

  if(!isValidInterrupt(irq->name))
    {

    return 0;

    }

  /*
   * Check if default handler is actually connected. If not issue an error.
   * RATIONALE : to always have the same transition by forcing the user
   * to get the previous handler before accepting to disconnect.
   */
  if(rtems_hdl_tbl[irq->name].hdl != irq->hdl)
    {

    return 0;

    }

  rtems_interrupt_disable(level);

  if(is_siu_irq(irq->name))
    {

    /*
     * disable interrupt at PIC level
     */
    BSP_irq_disable_at_siu(irq->name);

    }

  if(is_processor_irq(irq->name))
    {
    /*
     * disable exception at processor level
     */
    }

  /*
   * Disable interrupt on device
   */
  irq->off(irq);

  /*
   * restore the default irq value
   */
  rtems_hdl_tbl[irq->name] = default_rtems_entry;

  rtems_interrupt_enable(level);

  return 1;

  }


/*
 * ------------------------ RTEMS Global Irq Handler Mngt Routines ----------------
 */

/*
 * This function set up interrupt management dependent on the given configuration
 */
int BSP_rtems_irq_mngt_set(rtems_irq_global_settings* config)
  {
  int                   i;
  rtems_interrupt_level level;

 /*
  * Store various code accelerators
  */
  internal_config 	= config;
  default_rtems_entry 	= config->defaultEntry;
  rtems_hdl_tbl 	= config->irqHdlTbl;

  rtems_interrupt_disable(level);

  /*
   * start with SIU IRQs
   */
  for (i=BSP_SIU_IRQ_LOWEST_OFFSET; i < BSP_SIU_IRQ_LOWEST_OFFSET + BSP_SIU_IRQ_NUMBER ; i++)
    {

    if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl)
      {

	  BSP_irq_enable_at_siu(i);
	  rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);

      }
    else
      {

	  rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);
	  BSP_irq_disable_at_siu(i);

      }

    }

  /*
   * finish with Processor exceptions handled like IRQs
   */
  for (i=BSP_PROCESSOR_IRQ_LOWEST_OFFSET; i < BSP_PROCESSOR_IRQ_LOWEST_OFFSET + BSP_PROCESSOR_IRQ_NUMBER; i++)
    {

    if (rtems_hdl_tbl[i].hdl != default_rtems_entry.hdl)
      {

	  rtems_hdl_tbl[i].on(&rtems_hdl_tbl[i]);

      }
    else
      {

	  rtems_hdl_tbl[i].off(&rtems_hdl_tbl[i]);

      }

    }

    rtems_interrupt_enable(level);
    return 1;

  }


int BSP_rtems_irq_mngt_get(rtems_irq_global_settings** config)
  {

  *config = internal_config;
  return 0;

  }


/*
 * High level IRQ handler called from shared_raw_irq_code_entry
 */
void C_dispatch_irq_handler (CPU_Interrupt_frame *frame, unsigned int excNum)
  {
  register unsigned int irq;
  register unsigned int msr;
  register unsigned int new_msr;
  register unsigned int pmce;
  register unsigned int crit_pri_main_mask, per_mask;

  switch(excNum)
    {


    /*
     * Handle decrementer interrupt
     */
    case ASM_DEC_VECTOR:

      /* call the module specific handler and pass the specific handler */
      rtems_hdl_tbl[BSP_DECREMENTER].hdl(0);

      return;

    case ASM_SYSMGMT_VECTOR:

      /* get the content of main interrupt status register */
	  pmce =  mpc5200.pmce;

	  /* main interrupts may be routed to SMI, see bit SMI/INT select bit in main int. priorities */
	  while(CHK_MSE_STICKY(pmce))
	    {

        /* check for main interrupt sources (hirarchical order) -> LO_int indicates peripheral sources */
        if(CHK_MSE_STICKY(pmce))
          {

          /* get source of main interrupt */
		  irq = MSE_SOURCE(pmce);

	      switch(irq)
            {

            /* irq1-3, RTC, GPIO, TMR0-7 detected (attention: slice timer 2 is always routed to SMI) */
            case 0: /* slice timer 2 */
            case 1:
            case 2:
            case 3:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:

              /* add proper offset for main interrupts in the siu handler array */
			  irq += BSP_MAIN_IRQ_LOWEST_OFFSET;

              /* save original mask and disable all lower priorized main interrupts*/
			  crit_pri_main_mask =  mpc5200.crit_pri_main_mask;
		      mpc5200.crit_pri_main_mask |= irqMaskTable[irq];

              /* enable interrupt nesting */
			  _CPU_MSR_GET(msr);
			  new_msr = msr | MSR_EE;
			  _CPU_MSR_SET(new_msr);

              /* call the module specific handler and pass the specific handler */
              rtems_hdl_tbl[irq].hdl(0);

              /* disable interrupt nesting */
              _CPU_MSR_SET(msr);

              /* restore original interrupt mask */
              mpc5200.crit_pri_main_mask = crit_pri_main_mask;

              break;

            /* peripheral LO_int interrupt source detected */
            case 4:

              /* check for valid peripheral interrupt source */
			  if(CHK_PSE_STICKY(pmce))
			    {

                /* get source of peripheral interrupt */
	            irq = PSE_SOURCE(pmce);

	            /* add proper offset for peripheral interrupts in the siu handler array */
	            irq += BSP_PER_IRQ_LOWEST_OFFSET;

	            /* save original mask and disable all lower priorized main interrupts */
                per_mask =  mpc5200.per_mask;
                mpc5200.per_mask |= irqMaskTable[irq];

                /* enable interrupt nesting */
				_CPU_MSR_GET(msr);
				new_msr = msr | MSR_EE;
				_CPU_MSR_SET(new_msr);

                /* call the module specific handler and pass the specific handler */
	            rtems_hdl_tbl[irq].hdl(0);


	            /* disable interrupt nesting */
				_CPU_MSR_SET(msr);

				/* restore original interrupt mask */
                mpc5200.per_mask = per_mask;

                /* force re-evaluation of peripheral interrupts */
	            CLR_PSE_STICKY(mpc5200.pmce);

			    }
			  /* this case may not occur: no valid peripheral interrupt source */
	          else
	            {

			    printk("No valid peripheral LO_int interrupt source\n");

			    }

              break;

            /* error: unknown interrupt source */
	    	default:
	    	  printk("Unknown peripheral LO_int interrupt source\n");
	    	  break;

	        }

	      /* force re-evaluation of main interrupts */
          CLR_MSE_STICKY(mpc5200.pmce);

          }

        /* get the content of main interrupt status register */
		pmce =  mpc5200.pmce;

	    }

      break;

    case ASM_EXT_VECTOR:

      /* get the content of main interrupt status register */
	  pmce =  mpc5200.pmce;

      /* critical interrupts may be routed to the core_int dependent on premature initialization, see bit 31 (CEbsH) */
      while((CHK_CE_SHADOW(pmce) && CHK_CSE_STICKY(pmce)) || CHK_MSE_STICKY(pmce) || CHK_PSE_STICKY(pmce) )
        {

        /* first: check for critical interrupt sources (hirarchical order) -> HI_int indicates peripheral sources */
        if(CHK_CE_SHADOW(pmce) && CHK_CSE_STICKY(pmce))
          {

          /* get source of critical interrupt */
	      irq = CSE_SOURCE(pmce);

	      switch(irq)
            {
            /* irq0, slice timer 1 or ccs wakeup detected */
            case 0:
            case 1:
            case 3:

              /* add proper offset for critical interrupts in the siu handler array */
			  irq += BSP_CRIT_IRQ_LOWEST_OFFSET;

              /* call the module specific handler and pass the specific handler */
              rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);

              break;

            /* peripheral HI_int interrupt source detected */
            case 2:

              /* check for valid peripheral interrupt source */
			  if(CHK_PSE_STICKY(pmce))
			    {

                /* get source of peripheral interrupt */
                irq = PSE_SOURCE(pmce);

                /* add proper offset for peripheral interrupts in the siu handler array */
                irq += BSP_PER_IRQ_LOWEST_OFFSET;

                /* save original mask and disable all lower priorized main interrupts */
                per_mask =  mpc5200.per_mask;
                mpc5200.per_mask |= irqMaskTable[irq];

                /* enable interrupt nesting */
                _CPU_MSR_GET(msr);
                new_msr = msr | MSR_EE;
                _CPU_MSR_SET(new_msr);

                /* call the module specific handler and pass the specific handler */
                rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);


                /* disable interrupt nesting */
                _CPU_MSR_SET(msr);

                /* restore original interrupt mask */
                mpc5200.per_mask = per_mask;

                /* force re-evaluation of peripheral interrupts */
                CLR_PSE_STICKY(mpc5200.pmce);

			    }
			  /* this case may not occur: no valid peripheral interrupt source */
	          else
	            {

			    printk("No valid peripheral HI_int interrupt source\n");

			    }

              break;

            /* error: unknown interrupt source */
	    	default:
	    	  printk("Unknown HI_int interrupt source\n");
	    	  break;

	        }

	      /* force re-evaluation of critical interrupts */
	      CLR_CSE_STICKY(mpc5200.pmce);

          }

        /* second: check for main interrupt sources (hirarchical order) -> LO_int indicates peripheral sources */
        if(CHK_MSE_STICKY(pmce))
          {

          /* get source of main interrupt */
		  irq = MSE_SOURCE(pmce);

	      switch(irq)
            {

            /* irq1-3, RTC, GPIO, TMR0-7 detected (attention: slice timer 2 is always routed to SMI) */
            case 1:
            case 2:
            case 3:
            case 5:
            case 6:
            case 7:
            case 8:
            case 9:
            case 10:
            case 11:
            case 12:
            case 13:
            case 14:
            case 15:
            case 16:

              /* add proper offset for main interrupts in the siu handler array */
			  irq += BSP_MAIN_IRQ_LOWEST_OFFSET;

              /* save original mask and disable all lower priorized main interrupts*/
			  crit_pri_main_mask =  mpc5200.crit_pri_main_mask;
		      mpc5200.crit_pri_main_mask |= irqMaskTable[irq];

              /* enable interrupt nesting */
			  _CPU_MSR_GET(msr);
			  new_msr = msr | MSR_EE;
			  _CPU_MSR_SET(new_msr);

              /* call the module specific handler and pass the specific handler */
              rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);

              /* disable interrupt nesting */
              _CPU_MSR_SET(msr);

              /* restore original interrupt mask */
              mpc5200.crit_pri_main_mask = crit_pri_main_mask;

              break;

            /* peripheral LO_int interrupt source detected */
            case 4:

              /* check for valid peripheral interrupt source */
			  if(CHK_PSE_STICKY(pmce))
			    {

                /* get source of peripheral interrupt */
	            irq = PSE_SOURCE(pmce);

	            /* add proper offset for peripheral interrupts in the siu handler array */
	            irq += BSP_PER_IRQ_LOWEST_OFFSET;

	            /* save original mask and disable all lower priorized main interrupts */
                per_mask =  mpc5200.per_mask;
                mpc5200.per_mask |= irqMaskTable[irq];

                /* enable interrupt nesting */
				_CPU_MSR_GET(msr);
				new_msr = msr | MSR_EE;
				_CPU_MSR_SET(new_msr);

                /* call the module specific handler and pass the specific handler */
	            rtems_hdl_tbl[irq].hdl(rtems_hdl_tbl[irq].handle);


	            /* disable interrupt nesting */
				_CPU_MSR_SET(msr);

				/* restore original interrupt mask */
                mpc5200.per_mask = per_mask;

                /* force re-evaluation of peripheral interrupts */
	            CLR_PSE_STICKY(mpc5200.pmce);

			    }
			  /* this case may not occur: no valid peripheral interrupt source */
	          else
	            {

			    printk("No valid peripheral LO_int interrupt source\n");

			    }

              break;

            /* error: unknown interrupt source */
	    	default:
	    	  printk("Unknown peripheral LO_int interrupt source\n");
	    	  break;

	        }

	      /* force re-evaluation of main interrupts */
          CLR_MSE_STICKY(mpc5200.pmce);

          }


        /* get the content of main interrupt status register */
		pmce =  mpc5200.pmce;

	    }

      break;

    default:
      printk("Unknown processor exception\n");
	  break;

    } /* end of switch(excNum) */

  return;

  }


void _ThreadProcessSignalsFromIrq (BSP_Exception_frame* ctx)
  {
  /*
   * Process pending signals that have not already been
   * processed by _Thread_Displatch. This happens quite
   * unfrequently : the ISR must have posted an action
   * to the current running thread.
   */
  if ( _Thread_Do_post_task_switch_extension ||
       _Thread_Executing->do_post_task_switch_extension )
    {

    _Thread_Executing->do_post_task_switch_extension = FALSE;
    _API_extensions_Run_postswitch();

    }
  /*
   * I plan to process other thread related events here.
   * This will include DEBUG session requested from keyboard...
   */
}
