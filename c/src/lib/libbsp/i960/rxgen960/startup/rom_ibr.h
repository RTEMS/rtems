/*-------------------------------------*/
/* rom_ibr.h                           */
/* Last change :  23. 1.95             */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _ROM_IBR_H_
#define _ROM_IBR_H_

#include "flttbl.h"
#include "cntrltbl.h"
#include "intrtbl.h"
#include "systbl.h"

  /* Initial Boot Record. 
   */      
struct IBR {                       
  unsigned int busConfig[4];            /* Initial Bus Configuration */
  void (* start)(void);                 /* Start Point */
  struct PRCB * prcb;                   /* PRCB */
  unsigned int chckSum[6];              /* CheckSum */ 
}; 
  /* Check sum values (defined in *.ld).
   */
extern unsigned int rom_ibr_cksum[];

#endif
/*-------------*/
/* End of file */
/*-------------*/

