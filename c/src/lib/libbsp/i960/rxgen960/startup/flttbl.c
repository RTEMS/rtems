/*-------------------------------------*/
/* flttbl.c                            */
/* Last change :  3.11.94              */
/*-------------------------------------*/  
/*
 *  $Id$
 */

#include "i960.h"
#include "string.h"
#include "sctns.h"
#include "fault.h"
#include "asmfault.h"
#include "flttbl.h"
/*-------------------------------------*/
  /* Fault Table. It (as well as all the rest of the
   * code of this file will always stay in ROM, so
   * that it wouldn't be destroyed by silly user code.
   * Thus, at least faults will be always caugth,
   */
FaultTblEntry faultTbl[] = {
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Parallel */
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Trace */
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Operation */ 
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Arithmetic */ 
  {0, 0},                       		          /* Reserved */ 
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Constraint */ 
  {0, 0},                                           /* Reserved */
  {faultHndlEntry + LOCAL_FH, LOCAL_FW},            /* Protection */
  {0, 0},                                           /* Reserved */
  {faultHndlEntry + LOCAL_FH, LOCAL_FW}            /* Type */
};

void fltTblInit(void)
{
  static unsigned int fltTblCheckSum(void);

  faultCheckSum = fltTblCheckSum();
}
static unsigned int fltTblCheckSum(void)
{
  unsigned int * f = faultStart;
  unsigned int * l = faultEnd; 
  unsigned int sum;
  
  for (sum = 0; f < l; f ++)  {
    sum += * f;
  }
  return sum;
}
void faultTblHandler(unsigned int * fp, unsigned int * faultBuffer)
{
  unsigned int * ip;
  struct typeWord {
    unsigned int sbtp : 8;
    unsigned int : 8;
    unsigned int type : 8;
    unsigned int : 8;
  } tw;
  unsigned int type;
  unsigned int sbtp;
  unsigned int ac;
  unsigned int pc;
  unsigned int inst;

  char nib;
  unsigned int i;

    /* Address of faulting instruction.
     */
  ip = (unsigned int *) fp[-1]; 
    /* Type/Subtype word.
     */

  /* put address of faulting instruction to console */
  kkprintf("Fault: %x\n", ip);
 
  tw = * (struct typeWord *) & fp[-2];  
    /* Type and subtype.
     */
  type = tw.type;
  sbtp = tw.sbtp; 
    /* Arithmetic controls.
     */
  ac = fp[-3]; 
    /* Process controls.
     */
  pc = fp[-4];    
    /* Global and local registers are in faultBuffer
     * already. Save the rest. Change RIP to IP.
     */
  faultBuffer[IP_REGNUM] = (unsigned int) ip;
  faultBuffer[ACW_REGNUM] = ac;
  faultBuffer[PCW_REGNUM] = pc;  
    /* Bad instruction itself. We do
     * this here since it may be repaired (by copying from PROM).
     */
  inst = * ip; 
    /* Now, to handling.
     */
  if (faultCheckSum != fltTblCheckSum())  {
      /* RAM-based fault repair stuff
       * is broken. No chance to recover.
       * Repair RAM memory which is
       * destroyed by silly user.
       */
    copyCodeToRom(); 
      /* And call RAM-based fault handler.
       */
    faultBad(1, inst, faultBuffer, type, sbtp);
  } 
  else  {
      /* There exist a chance to recover.
       */
    faultGood(inst, faultBuffer, type, sbtp);  
  }
}
/*-------------*/
/* End of file */
/*-------------*/

