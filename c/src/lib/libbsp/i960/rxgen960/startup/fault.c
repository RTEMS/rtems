/*-------------------------------------*/
/* fault.c                             */
/* Last change : 13. 7.95              */
/*-------------------------------------*/  
/*
 *  $Id$
 */

#include "prcb.h"
#include "i960.h"
#include "flttbl.h"
#include "fault.h"
#include "asmstub.h"
#include <stdio.h>
#include <string.h>

extern void romFaultStart(void);

/*-------------------------------------*/
  /* Table of user-registered fault handler entry.
   */
typedef struct {
  UserFaultHandler hndl;         /* Handler itself. */
  int cnt;                       /* Handler is valid for cnt times. */ 
} UserFaultEntry;  
  /* Table itself.
   */
static UserFaultEntry userFaultTable[] = {  
  {0, 0},                       /* Parallel */
  {0, 0},                       /* Trace */
  {0, 0},                       /* Operation */
  {0, 0},                       /* Arithmetic */
  {0, 0},                       /* Reserved */
  {0, 0},                       /* Constraint */
  {0, 0},                       /* Reserved */
  {0, 0},                       /* Protection */
  {0, 0},                       /* Reserved */
  {0, 0}                        /* Type */
};
  /* Number of Faults.
   */
#define FaultNmbr (sizeof(userFaultTable)/sizeof(UserFaultEntry))

int faultRegister(int fault, UserFaultHandler hndl, int cnt)
{
  static unsigned int faultNewCheckSum(void);
  int rsl = 0;

  if (0 <= fault &&  fault <= FaultNmbr)  {
      /* Register handler.
       */
    userFaultTable[fault].hndl = hndl;
    userFaultTable[fault].cnt = cnt;
      /* Checksum has changed.
       */
    faultCheckSum = faultNewCheckSum();
    rsl = 1;
  }
  return rsl;
}
int faultOk(int fault)
{
  static unsigned int faultNewCheckSum(void);
  int rsl = 0;

  if (0 <= fault &&  fault <= FaultNmbr)  {
      /* Fault handler recovered successfully.
       * Can use it at least once more.
       */
    userFaultTable[fault].cnt ++;
      /* Check sum has changed.
       */
#if 0
    faultCheckSum = faultNewCheckSum();
#endif
    faultCheckSum ++; 
    rsl = 1;
  }
  return rsl;
}
void faultBad(int invokedFromRom,
              unsigned int inst, unsigned int * faultBuffer,
              unsigned int type, unsigned int sbtp)
{
  static void faultInfo(int invokedFromRom,
                        unsigned int inst, unsigned int * faultBuffer,
                        unsigned int type, unsigned int sbtp);

    /* Close the mem channel nicely.
     */
/*  memChnlI960Fault();*/
    /* Give some panic message.
     */ 
  faultInfo(invokedFromRom, inst, faultBuffer, type, sbtp);
     /* At this point RAM is repaired. Do
      * whatever you want.
      */
#if 0
  if (OsfIsUp)  {    
    asm_exit(romFaultStart, & ram_prcb);
  }
  else  {
    asm_exit(romStart, & ram_prcb);
  }  
# endif
  asm_exit(romFaultStart, & ram_prcb);
}
void faultGood(unsigned int inst, unsigned int * faultBuffer,
               unsigned int type, unsigned int sbtp)
{
  static unsigned int faultNewCheckSum(void);
 
  if (userFaultTable[type].hndl != 0 && userFaultTable[type].cnt > 0)  {
      /* This is done to avoid the situation when
       * handler causes a fault and, thus, infinite recursion.
       */
    userFaultTable[type].cnt --;
      /* Check sum has changed.
       */
#if 0
    faultCheckSum = faultNewCheckSum();
#endif
    faultCheckSum --;
      /* Invoke handler.
       */ 
    (* userFaultTable[type].hndl)(inst, faultBuffer, type, sbtp);
      /* If this returns => fault is bad.
       */ 
  }
  faultBad(0, inst, faultBuffer, type, sbtp);
}
static unsigned int faultNewCheckSum(void)
{
  unsigned int * f = faultStart;
  unsigned int * l = faultEnd; 
  unsigned int sum;
  
  for (sum = 0; f < l; f ++)  {
    sum += * f;
  }
  return sum;
}
static void faultInfo(int invokedFromRom,
                      unsigned int inst, unsigned int * faultBuffer,
                      unsigned int type, unsigned int sbtp)
{
  char * typeStr;
  char * sbtpStr;
  static char * faultParallelSbtpStr(unsigned int);
  static char * faultTraceSbtpStr(unsigned int);
  static char * faultOperationSbtpStr(unsigned int);
  static char * faultArithmeticSbtpStr(unsigned int);
  static char * faultReservedSbtpStr(unsigned int);
  static char * faultConstraintSbtpStr(unsigned int);
  static char * faultProtectionSbtpStr(unsigned int);
  static char * faultTypeSbtpStr(unsigned int);
  static char * faultUnknownSbtpStr(unsigned int);
  static struct {
    char * name;
    char * (* sbtpStr)(unsigned int);
  } faultInfo[] = {
    {"Parallel", faultParallelSbtpStr},
    {"Trace", faultTraceSbtpStr},
    {"Operation", faultOperationSbtpStr},
    {"Arithmetic", faultArithmeticSbtpStr},
    {"Reserved", faultReservedSbtpStr},
    {"Constraint", faultConstraintSbtpStr},
    {"Reserved", faultReservedSbtpStr},
    {"Protection", faultProtectionSbtpStr},
    {"Reserved", faultReservedSbtpStr},
    {"Type", faultTypeSbtpStr},
    {"Unknown", faultUnknownSbtpStr}
  };
  unsigned int ix;

/*  console_set_channel(CHANNEL_B);*/
  ix = type >= FaultNmbr ? FaultNmbr : type;
  typeStr = faultInfo[ix].name;
  sbtpStr = (* faultInfo[ix].sbtpStr)(sbtp);
  printf("\nFault at 0x%08x: %s[%s]\n",
           faultBuffer[IP_REGNUM], typeStr, sbtpStr);
  printf("Bad instruction: 0x%08x\n", inst);
  printf("AC=0x%08x PC=0x%08x\n",
           faultBuffer[ACW_REGNUM],
           faultBuffer[PCW_REGNUM]);
  printf("g0=0x%08x g1=0x%08x g2=0x%08x g3=0x%08x\n",
         faultBuffer[G0_REGNUM+0], faultBuffer[G0_REGNUM+1],
         faultBuffer[G0_REGNUM+2], faultBuffer[G0_REGNUM+3]);
  printf("g4=0x%08x g5=0x%08x g6=0x%08x g7=0x%08x\n",
         faultBuffer[G0_REGNUM+4], faultBuffer[G0_REGNUM+5],
         faultBuffer[G0_REGNUM+6], faultBuffer[G0_REGNUM+7]);
  printf("g8=0x%08x g9=0x%08x gA=0x%08x gB=0x%08x\n",
         faultBuffer[G0_REGNUM+8], faultBuffer[G0_REGNUM+9],
         faultBuffer[G0_REGNUM+10], faultBuffer[G0_REGNUM+11]); 
  printf("gC=0x%08x gD=0x%08x gE=0x%08x gF=0x%08x\n",
         faultBuffer[G0_REGNUM+12], faultBuffer[G0_REGNUM+13],
         faultBuffer[G0_REGNUM+14], faultBuffer[G0_REGNUM+15]);
  printf("r0=0x%08x r1=0x%08x r2=0x%08x r3=0x%08x\n",
         faultBuffer[R0_REGNUM+0], faultBuffer[R0_REGNUM+1],
         faultBuffer[R0_REGNUM+2], faultBuffer[R0_REGNUM+3]);
  printf("r4=0x%08x r5=0x%08x r6=0x%08x r7=0x%08x\n",
         faultBuffer[R0_REGNUM+4], faultBuffer[R0_REGNUM+5],
         faultBuffer[R0_REGNUM+6], faultBuffer[R0_REGNUM+7]);
  printf("r8=0x%08x r9=0x%08x rA=0x%08x rB=0x%08x\n",
         faultBuffer[R0_REGNUM+8], faultBuffer[R0_REGNUM+9],
         faultBuffer[R0_REGNUM+10], faultBuffer[R0_REGNUM+11]); 
  printf("rC=0x%08x rD=0x%08x rE=0x%08x rF=0x%08x\n",
         faultBuffer[R0_REGNUM+12], faultBuffer[R0_REGNUM+13],
         faultBuffer[R0_REGNUM+14], faultBuffer[R0_REGNUM+15]);                
  if (invokedFromRom)  {
    printf("RAM image damaged. No chance to recover\n");
  } 
  else  {
    printf("RAM image not damaged. Still no recovery\n"); 
  }
} 
static char * faultParallelSbtpStr(unsigned int sbtp)
{
  static char buf[10];
  
  sprintf(buf, "%d", sbtp);
  return buf;
}
static char * faultTraceSbtpStr(unsigned int sbtp)
{
  static char buf[256];
  int notEmpty;

  buf[0] = '\0';
  notEmpty = 0;
  if (sbtp & 0x2)  {
     strcat(buf, "Instruction");
     notEmpty = 1;
  }   
  if (sbtp & 0x4)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Branch");
    notEmpty = 1;
  } 
  if (sbtp & 0x8)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Call");
    notEmpty = 1;
  } 
  if (sbtp & 0x10)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Return");
    notEmpty = 1;
  } 
  if (sbtp & 0x20)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Prereturn");
    notEmpty = 1;
  }  
  if (sbtp & 0x40)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Supervisor");
    notEmpty = 1;
  }   
  if (sbtp & 0x80)  {
    if (notEmpty) strcat(buf, ":");
    strcat(buf, "Breakpoint");
    notEmpty = 1;
  } 
  if (! notEmpty)  {
    strcat(buf, "Unknown");
  }        
  return buf;
}
static char * faultOperationSbtpStr(unsigned int sbtp)
{
  char * rsl;
  
  if (sbtp == 0x1)      rsl = "Invalid Opcode";
  else if (sbtp == 0x2) rsl = "Unimplemented";
  else if (sbtp == 0x3) rsl = "Unaligned";
  else if (sbtp == 0x4) rsl = "Invalid Operand";
  else                  rsl = "Unknown";
  return rsl;
}
static char * faultArithmeticSbtpStr(unsigned int sbtp)
{
  char * rsl;
  
  if (sbtp == 0x1)      rsl = "Integer Overflow";
  else if (sbtp == 0x2) rsl = "Arithmetic Zero-Divide";
  else                  rsl = "Unknown";  
  return rsl;
}
static char * faultReservedSbtpStr(unsigned int sbtp)
{
  return "Unknown";  
}
static char * faultConstraintSbtpStr(unsigned int sbtp)
{
  char * rsl;
  
  if (sbtp == 0x1)      rsl = "Constraint Range";
  else if (sbtp == 0x2) rsl = "Priveleged";
  else                  rsl = "Unknown";    
  return rsl;
}
static char * faultProtectionSbtpStr(unsigned int sbtp)
{
  char * rsl;
  
  if (sbtp == 0x1)    rsl = "Length";
  else                rsl = "Unknown";     
  return rsl;
}
static char * faultTypeSbtpStr(unsigned int sbtp)
{
  char * rsl;
  
  if (sbtp == 0x1)    rsl = "Type Mismatch";
  else                rsl = "Unknown";     
  return rsl;
}
static char * faultUnknownSbtpStr(unsigned int sbtp)
{
  return "Unknown";
}
/*-------------*/
/* End of file */
/*-------------*/

