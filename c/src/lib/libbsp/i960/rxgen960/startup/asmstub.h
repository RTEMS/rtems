/*-------------------------------------*/
/* asmstub.h                           */
/* Last change : 20. 1.95              */
/*-------------------------------------*/
#ifndef _ASMSTUB_H_
#define _ASMSTUB_H_

#include "prcb.h"

  /* Reset Processor taking Start Point and Prcb
   * as parameters.
   */
extern void asm_exit(void (* start)(void), struct PRCB * prcb);
  /* Call SYSCTL instruction.
   */
extern void asm_sysctl(unsigned int cmd, unsigned int a1, unsigned int a2);
  /* Alter a bit in the interrupt pending register.
   */
extern void asm_ipend(unsigned int iNmbr, unsigned int toSet);
  /* Alter a bit in the interrupt mask register.
   */
extern void asm_imask(unsigned int iNmbr, unsigned int toSet);
  /* Call MODPC instruction.
   */
extern int asm_modpc(unsigned int val, unsigned int mask);
  /* Change a cached interrupt vector.
   */
extern void asm_ivector(unsigned int vctr, void (* hndl)(void));

#endif
/*-------------*/
/* End of file */
/*-------------*/

