/*-------------------------------------*/
/* fault.h                             */
/* Last change :  2.11.94              */
/*-------------------------------------*/
#ifndef _FAULT_H_
#define _FAULT_H_

  /* (RAM-based) Fault Handler.
   * Is invoked when there is no chance to repair current state. 
   */
extern void faultBad(int invokedFromRom,
                     unsigned int inst, unsigned int * faultBuffer, 
                     unsigned int type, unsigned int sbtp);
  /* (RAM-based) Fault Handler.
   * Is invoked when there is a chance to repair current state. 
   */
extern void faultGood(unsigned int instr, unsigned int * faultBuffer, 
                      unsigned int type, unsigned int sbtp);
  /* Some addresses that are defined in rom.ld.
   */
extern unsigned int faultCheckSum;

extern unsigned int faultBuffer[];

extern unsigned int faultStart[];
extern unsigned int faultEnd[];   
  /* Interface for user to register fault handlers of his own.
   * Fault names.
   */
#define ParallelFLT	0
#define TraceFLT	1
#define OperationFLT	2
#define ArithmeticFLT	3
#define ConstraintFLT	5
#define ProtectionFLT	7
#define TypeFLT		9  
  /* User-registered fault handler.
   */
typedef void (* UserFaultHandler)(unsigned int inst, unsigned int * faultBuf, 
                                  unsigned int type, unsigned int sbtp);
  /* Register user-defined fault handler. The third argument is
   * how many times this fault handler will be valid. This to avoid
   * the situation when handler is bad and it causes a fault itself.
   */
extern int faultRegister(int fault, UserFaultHandler, int cnt);
  /* Validate handler for one more time.
   */
extern int faultOk(int fault);
                       
#endif
/*-------------*/
/* End of file */
/*-------------*/

