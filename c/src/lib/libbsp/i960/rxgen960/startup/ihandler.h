/*-------------------------------------*/
/* ihandler.h                          */
/* Last change : 12.10.94              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _IHANDLER_H_
#define _IHANDLER_H_

  /* Interrupt Handler for NMI.
   */
extern void nmiHandler(void);
  /* Interrupt Handlers for Dedicated Interrupts.
   */
extern void intr5Handler(void);
extern void intr6Handler(void);   

#endif
/*-------------*/
/* End of file */
/*-------------*/

