/*===============================================================*\
| Project: RTEMS generic MCF548x BSP                              |
+-----------------------------------------------------------------+
|                    Copyright (c) 2004-2009                      |
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
| this file contains glue functions to the Freescale MC_DMA API   |
\*===============================================================*/
#include <rtems.h>
#include <rtems/error.h>
#include <bsp.h>
#include <mcf548x/mcf548x.h>
#include <mcf548x/MCD_dma.h>

#define MCDMA_INT_ENABLE(reg,chan)   (reg &= ~(1 << (chan)))
#define MCDMA_INT_DISABLE(reg,chan)  (reg |=  (1 << (chan)))
#define	MCDMA_CLEAR_IEVENT(reg,chan) (reg  =  (1 << (chan)))

#define MCDMA_INT_BIT_IMPL           0x0000FFFF /* implemented IRQ sources (bitmask for IPEND... */
#define MCDMA_IRQ_VECTOR    (48+64)
#define MCDMA_IRQ_LEVEL     (2)
#define MCDMA_IRQ_PRIORITY  (2)
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_enable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   enable interrupt for given task number                                  |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_channo                              /* task number to enable    */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  rtems_interrupt_level level;
  if (0 != ((1UL<<mcdma_channo) & MCDMA_INT_BIT_IMPL)) {
    rtems_interrupt_disable(level);
    /*
     * valid task number
     * enable interrupt in mcdma mask
     */
    MCDMA_INT_ENABLE(MCF548X_DMA_DIMR,mcdma_channo);
    rtems_interrupt_enable(level);
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_disable
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   disable interrupt for given task number                                 |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_channo                              /* task number to disable   */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  rtems_interrupt_level level;
  if (0 != ((1UL<<mcdma_channo) & MCDMA_INT_BIT_IMPL)) {
    rtems_interrupt_disable(level);
    /*
     * valid task number
     * disable interrupt in mcdma mask
     */
    MCDMA_INT_DISABLE(MCF548X_DMA_DIMR,mcdma_channo);
    rtems_interrupt_enable(level);
  }
}

typedef struct {
  void (*the_handler)(void *param);
  void *the_param;
} mcdma_glue_irq_handlers_t;

mcdma_glue_irq_handlers_t mcdma_glue_irq_handlers[32];

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_irq_install
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   install given function as mcdma interrupt handler                    |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 int mcdma_channo,                          /* task number for handler  */
 void (*the_handler)(void *),               /* function to call         */
 void *the_param
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  if (0 != ((1UL<<mcdma_channo) & MCDMA_INT_BIT_IMPL)) {
    /*
     * valid task number
     * install handler
     */
    mcdma_glue_irq_handlers[mcdma_channo].the_handler = the_handler;
    mcdma_glue_irq_handlers[mcdma_channo].the_param   = the_param;
  }
}

/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
rtems_isr mcdma_glue_irq_dispatcher
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   general mcdma interrupt handler/dispatcher                              |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 rtems_vector_number v                   /* irq specific handle (not used) */

)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  uint32_t pending;
  int curr_channo;

  pending = MCF548X_DMA_DIPR & ~MCF548X_DMA_DIMR;
  curr_channo = 0;
  while (pending != 0) {
    if ((pending & (1UL<<curr_channo)) != 0) {
      if (mcdma_glue_irq_handlers[curr_channo].the_handler == NULL) {
	/*
	 * This should never happen. we have a pending IRQ but no handler
	 * let's clear this pending bit
	 */
	MCDMA_CLEAR_IEVENT(MCF548X_DMA_DIPR,curr_channo);
      }
      else {
	/*
	 * call proper handler
	 */
	mcdma_glue_irq_handlers[curr_channo].the_handler
	  (mcdma_glue_irq_handlers[curr_channo].the_param);
      }
      /*
       * clear this bit in our pending copy
       * and go to next bit
       */
      pending &= ~(1<<curr_channo);
    }
    curr_channo++;
  }
}

static bool mcdma_glue_is_initialized = false;
/*=========================================================================*\
| Function:                                                                 |
\*-------------------------------------------------------------------------*/
void mcdma_glue_init
(
/*-------------------------------------------------------------------------*\
| Purpose:                                                                  |
|   initialize the mcdma module (if not yet done):                          |
|   - load code                                                             |
|   - initialize registers                                                  |
|   - initialize bus arbiter                                                |
|   - initialize interrupt control                                          |
+---------------------------------------------------------------------------+
| Input Parameters:                                                         |
\*-------------------------------------------------------------------------*/
 void *sram_base         /* base address for SRAM, to be used for DMA task */
)
/*-------------------------------------------------------------------------*\
| Return Value:                                                             |
|    none                                                                   |
\*=========================================================================*/
{
  rtems_isr_entry old_handler;
  if (!mcdma_glue_is_initialized) {
    mcdma_glue_is_initialized = true;

    MCD_initDma((dmaRegs *)&MCF548X_DMA_TASKBAR,
		sram_base,
		MCD_TT_FLAGS_DEF);

    /*
     * initialize interrupt dispatcher
     */
    if(rtems_interrupt_catch(mcdma_glue_irq_dispatcher,
			     MCDMA_IRQ_VECTOR,
			     &old_handler)) {
      rtems_panic ("Can't attach MFC548x MCDma interrupt handler\n");
    }
    MCF548X_INTC_ICRn(MCDMA_IRQ_VECTOR - 64) =
      MCF548X_INTC_ICRn_IL(MCDMA_IRQ_LEVEL) |
      MCF548X_INTC_ICRn_IP(MCDMA_IRQ_PRIORITY);

    MCF548X_INTC_IMRH &= ~(1 << (MCDMA_IRQ_VECTOR % 32));
  }
}
