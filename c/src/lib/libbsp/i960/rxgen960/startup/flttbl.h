/*-------------------------------------*/
/* fltbl.h                             */
/* Last change :  3.11.94              */
/*-------------------------------------*/
/*
 *  $Id$
 */

#ifndef _FLTTBL_H_
#define _FLTTBL_H_

  /* FaultTable Entry.   
   */
typedef struct  { 
  void (* hndl)(void);                           /* Fault Handle */
  unsigned int type;                             /* Fault Table Type */
} FaultTblEntry;
  /* Fault Handler Type.
   */
#define LOCAL_FH	0
#define SYSTEM_FH	0x10

#define LOCAL_FW	0
#define SYSTEM_FW	0x027F   
  /* FaultTable Itself.
   */
extern FaultTblEntry faultTbl[];
  /* To initialize fault handling.
   */
extern void faultTblInit(void);   
  /* Fault handler. Invoked from low-level handler.
   */
extern void faultTblHandler(unsigned int * fp,
                            unsigned int * faultBuffer);
                            
#endif   
/*-------------*/
/* End of file */
/*-------------*/

