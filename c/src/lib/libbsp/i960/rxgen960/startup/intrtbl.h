/*-------------------------------------*/
/* intrtbl.h                           */
/* Last change : 12.10.94              */
/*-------------------------------------*/
#ifndef _INTRTBL_H_
#define _INTRTBL_H_

  /* Interrupt Handler.
   */
typedef void (* IntrHndl)(void);
  /* Interrupt Table.
   */
typedef struct {
  unsigned int pendPrty;		/* Pending Priorities */
  unsigned int pendIntr[8];		/* Pending Interrupts */
  IntrHndl intrHndl[248];		/* Interrupt Handlers */         
} InterruptTbl;
  /* Interrupt Handler Type.
   */
#define	NORMAL_IH	0
#define	IN_CACHE_IH	0x10   
  /* Interrupt Table Itself.
   */
extern InterruptTbl interruptTbl;

#endif   
/*-------------*/
/* End of file */
/*-------------*/

