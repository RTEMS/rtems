/*-------------------------------------*/
/* systbl.h                            */
/* Last change : 14.10.94              */
/*-------------------------------------*/
#ifndef _SYSTBL_H_
#define _SYSTBL_H_

  /* System Procedure.
   */
typedef void (* SysProc)(void);   
  /* System Procedures Table.
   */
typedef struct {
  unsigned int reserved[3];		/* Reserved */
  unsigned int * svrStackPtr;           /* Supervisor Stack Pointer Base */
  unsigned int preserved[8];		/* Preserved */ 
  SysProc sysProc[259];      		/* System Procedures Entry Points */
} SystemTbl;
  /* Type of System Procedure.
   */
#define LOCAL_SP	0x0
#define SUPERVISOR_SP	0x2  
  /* Cinvert to System Procedure Type.
   */
#define SP(addr)	((SysProc) (addr))    
  /* System Procedures Table Itself.
   */
extern SystemTbl systemTbl;

#endif   
/*-------------*/
/* End of file */
/*-------------*/

