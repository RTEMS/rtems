/*-------------------------------------*/
/* prcb.h                              */
/* Last change :  11. 1.95              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _PRCB_H_
#define _PRCB_H_

#include "flttbl.h"
#include "cntrltbl.h"
#include "intrtbl.h"
#include "systbl.h"

  /* PRocess Control Block  
   */      
struct PRCB {                       
  FaultTblEntry * faultTbl;             /* Fault Table Base */   
  ControlTblEntry * controlTbl;         /* Control Table Base */    
  unsigned int arithConfig;             /* Arithmetic Control Register Image */ 
  unsigned int faultConfig;             /* Fault Configuration Word Image */
  InterruptTbl * interruptTbl;          /* Interrupt Table Base */ 
  SystemTbl * systemTbl;                /* System Procedure Table Base */   
  unsigned int reserved;                /* Reserved */  
  unsigned int * intStackPtr;           /* Interrupt Stack Pointer */
  unsigned int instCacheConfig;         /* Instruction Cache Config */ 
  unsigned int regCacheConfig;          /* Register Cache Config */ 
}; 
  /* Constants for Arithmetic Control Register.
   */
#define	INT_OVFL_ENABLE		0
#define	INT_OVFL_DISABLE	0x1000

#define	PRECISE_FLTS		0x8000 
#define	IMPRECISE_FLTS		0 
  /* Constants for Fault Configuration Word.
   */
#define	UNLGND_FAULT_ENABLE	0
#define	UNLGND_FAULT_DISABLE    0x40000000 
  /* Constants for Instruction Cache Configuration Word.
   */  
#define INST_CACHE_ENABLE	0
#define INST_CACHE_DISABLE      0x10000
  /* RAM-based Process Control Block.
   */
extern struct PRCB ram_prcb;
extern struct PRCB rom_prcb;
  /* Supervisor Stack. Is defined directly in rom.ld.
   */
extern unsigned int svrStackPtr[]; 
  /* Interrupt Stack. Is defined directly in rom.ld.
   */
extern unsigned int intStackPtr[]; 

#endif
/*-------------*/
/* End of file */
/*-------------*/

