/*-------------------------------------*/
/* cntrltbl.h                          */
/* Last change : 11. 1.95              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _CNTRLTBL_H_
#define _CNTRLTBL_H_

  /* Control Table Entry.   
   */
typedef unsigned int ControlTblEntry;
  /* Control Table itself.
   */
extern ControlTblEntry controlTbl[];
extern ControlTblEntry rom_controlTbl[];

  /* Interrupt Registers Initial.
   */     
#define IPB0  		0
#define IPB1  		0 
#define DAB0  		0  
#define DAB1  		0  

#define I_DISABLE	(0x1<<10) 
#define I_ENABLE	0 

#define MSK_UNCHNG	0
#define MSK_CLEAR  (0x1<<11) 

#define VECTOR_CACHE   	(0x1<<13) 



  /* BreakPoint Control Register Initial.
   */
#define BPCON		0  
  /* Bus Controller Mode Comstants.
  */
#define CONF_TBL_VALID	0x1
#define PROTECT_RAM	0x2  
#define PROTECT_RAM_SUP	0x4



#endif   
/*-------------*/
/* End of file */
/*-------------*/

