/* 
 *  Simple interface to the PowerPC 405 MMU
 *
 *   Michael Hamel ADInstruments 2008
 *
  */


#include <bsp.h>
#include <libcpu/powerpc-utility.h>
#include "mmu_405.h"
#include <inttypes.h>

/* #define qLogTLB */
/* #define qLogTLBDetails */


/*----------------------------- TLB handling -------------------------------- */
/* The following are in assembler in mmu_405asm.S  */
extern  void MMU_GetTLBEntry(uint8_t index, uint32_t* tagword, uint32_t* dataword, uint8_t* pid);
extern  void MMU_SetTLBEntry(uint8_t index, uint32_t hiword, uint32_t loword, uint8_t pid);
extern  void MMU_ClearTLBs(void);
extern  int16_t MMU_FindTLBEntry(uint32_t address);


enum { kNTLBs = 64 };    /* for 403GCX and 405 */

static bool    sFreeTLBs[kNTLBs];
static uint8_t  sLastIndex = 0;
static int    sNInUse = 0;

static void MMUFault(const char* what)
/* Used for all setup faults; these can't really be ignored */
{
  printk("\n>>>MMU fatal error %s\n",what);
  rtems_fatal_error_occurred(RTEMS_INTERNAL_ERROR);
}

static uint8_t AllocTLB(void)
{
  uint8_t index;
  
  index = sLastIndex;
  do {
    index++;
    if (index == kNTLBs)
      index = 0;
    if (index == sLastIndex)
      MMUFault("TLB table full");
  } while (! sFreeTLBs[index]);
  sFreeTLBs[index] = false;
  sLastIndex = index;
  sNInUse++;
  return index;
}

static void FreeTLB(uint8_t index)
{
  MMU_SetTLBEntry(index,0,0,0);
  sFreeTLBs[index] = true;
  sLastIndex = index-1;
  sNInUse--;
}


/*---------------------------- MMU operations ---------------------------------- */

int DataMissException(BSP_Exception_frame *f, unsigned int vector);
int InstructionMissException(BSP_Exception_frame *f, unsigned int vector);
int InstructionFetchException(BSP_Exception_frame *f, unsigned int vector);
void mmu_initialise(void);
int mmu_get_tlb_count(void);
uint8_t  mmu_new_processID(void);
uint8_t mmu_current_processID(void);

void
mmu_initialise(void)
/* Clear the TLBs and set up exception handlers for the MMU miss handlers */
{
  int i;
  
  MMU_ClearTLBs();
  for (i=0; i<kNTLBs; i++) {
    sFreeTLBs[i] = true;
    MMU_SetTLBEntry(i,0,0,0xFF);
  }
  ppc_exc_set_handler(ASM_ISI_VECTOR ,InstructionFetchException);
  ppc_exc_set_handler(ASM_BOOKE_ITLBMISS_VECTOR ,DataMissException);
  ppc_exc_set_handler(ASM_BOOKE_DTLBMISS_VECTOR ,InstructionMissException);
}

static void
MakeTLBEntries(uint32_t startAt, uint32_t nBytes, bool EX, bool WR, bool I, uint8_t PID)
{
  uint32_t mask, options, tagWord, dataWord;
  uint8_t   index, sizeCode, pid;
  
  if ((startAt & 0x3FF) != 0)
    MMUFault("TLB entry not on 1K boundary");
  if ((nBytes & 0x3FF) != 0)
    MMUFault("TLB size not on 1K boundary");
    
  options = 0;
  if (EX) options += 0x200;
  if (WR) options += 0x100;
  if (I) options += 5;
  
  #ifdef qLogTLB
    printk("TLB: make entries for $%X bytes from $%X..$%X PID %d",nBytes, startAt, startAt+nBytes-1, PID);
    if (EX) printk(" EX");
    if (WR) printk(" WR");
    if (I) printk(" I");
    printk("\n");
  #endif
  
  while (nBytes > 0) {
    /* Find the largest block we can base on this address */
    mask = 0x3FF;
    sizeCode = 0;
    while (mask < nBytes && ((startAt & mask)==0) && sizeCode < 8) {
      mask = (mask<<2) + 3;
      sizeCode++;
    }
    mask >>= 2;
    sizeCode--;
    
    /* Make a TLB entry describing this, ZSEL=0 */
    tagWord = startAt | (sizeCode<<7) | 0x40;
    dataWord = startAt | options;
    index = AllocTLB();
    MMU_SetTLBEntry( index , tagWord, dataWord, PID);
    
    {
      /* Paranoia: check that we can read that back... */
      uint8_t tdex, oldpid;
      
      oldpid = mmu_current_processID();
      mmu_set_processID(PID);
      tdex = MMU_FindTLBEntry(startAt);
      mmu_set_processID(oldpid);
      
      if (tdex != index) {
        printk(" Add TLB %d: At %" PRIx32 " for $%" PRIx32
               " sizecode %d tagWord $%" PRIx32 "  ",
               index, startAt, mask+1,sizeCode,tagWord);
        printk(" -- find failed, %d/%d!\n",tdex,index);
        MMU_GetTLBEntry(index, &tagWord, &dataWord, &pid);
        printk(" -- reads back $%" PRIx32 " : $%" PRIx32
               ", PID %d\n",tagWord,dataWord,pid);
      } else {
        #ifdef qLogTLBDetails
        printk(" Add TLB %d: At %X for $%X sizecode %d tagWord $%X\n",index, startAt, mask+1,sizeCode,tagWord);
        #endif
      }
    }
  
    /* Subtract block from startAddr and nBytes */
    mask++;    /* Convert to a byte count */
    startAt += mask;
    nBytes -= mask;
  }
  #ifdef qLogTLB
    printk(" %d in use\n",sNInUse);
  #endif
}

void
mmu_remove_space(uint32_t startAt, uint32_t endAt)
{
  int16_t index;
  int32_t size;
  uint32_t tagword, dataword, nBytes;
  uint8_t  pid, sCode;
  
  nBytes = endAt - startAt;
  
  #ifdef qLogTLB
  printk("TLB: delete entries for $%X bytes from $%X\n",nBytes,startAt);
  #endif
  
  while (nBytes > 0) {
    index = MMU_FindTLBEntry( (uint32_t)startAt );
    size = 1024;
    if (index >= 0) {
      MMU_GetTLBEntry(index, &tagword, &dataword, &pid);
      if ((tagword & 0x40) == 0)
        MMUFault("Undefine failed: redundant entries?");
      if ((tagword & 0xFFFFFC00) != (uint32_t)startAt)
        MMUFault("Undefine not on TLB boundary");
      FreeTLB(index);
      sCode = (tagword >> 7) & 7;
      while (sCode > 0) {
        size <<= 2;
        sCode--;
      }
      #ifdef qLogTLBDetails
      printk(" Free TLB %d: At %X for $%X\n",index, startAt, size);
      #endif
    }
    startAt += size;
    nBytes -= size;
  }
}

void
mmu_add_space(uint32_t startAddr, uint32_t endAddr, MMUAccessType permissions, uint8_t processID)
/* Convert accesstype to write-enable, executable, and cache-inhibit bits */
{
  bool EX, WR, I;
  
  EX = false;
  WR = false;
  I = false;
  switch (permissions) {
    case executable     : EX = true;  break;
    case readOnlyData     : break;
    case readOnlyNoCache   : I = true; break;
    case readWriteData     : WR = true; break;
    case readWriteNoCache   : WR = true; I= true; break;
    case readWriteExecutable: WR = true; EX = true; break;
  }
  MakeTLBEntries( (uint32_t)startAddr, (uint32_t)(endAddr-startAddr+1), EX, WR, I, processID);
}

int
mmu_get_tlb_count(void)
{
  return sNInUse;
}

/*---------------------------- CPU process ID handling ----------------------------------
 * Really dumb system where we just hand out sequential numbers and eventually fail
 * As long as we only use 8-9 processes this isn't a problem */

static uint8_t sNextPID = 1;

#define SPR_PID    0x3B1

uint8_t  mmu_new_processID(void)
{
  return sNextPID++;
}

void mmu_free_processID(uint8_t freeThis)
{
}

uint8_t mmu_current_processID(void)
{
  return PPC_SPECIAL_PURPOSE_REGISTER(SPR_PID);
}

uint8_t mmu_set_processID(uint8_t newID)
{
  uint8_t prev = mmu_current_processID();
  PPC_SET_SPECIAL_PURPOSE_REGISTER(SPR_PID,newID);
  return prev;
}


/* ------------------ Fault handlers ------------------ */

#define SPR_ESR    0x3D4
#define SPR_DEAR  0x3D5

enum { kESR_DST = 0x00800000 };

int DataMissException(BSP_Exception_frame *f, unsigned int vector)
{
  uint32_t addr, excSyn;
  
  addr = PPC_SPECIAL_PURPOSE_REGISTER(SPR_DEAR);
  excSyn  = PPC_SPECIAL_PURPOSE_REGISTER(SPR_ESR);
  if (excSyn & kESR_DST) printk("\n---Data write to $%" PRIx32
      " attempted at $%" PRIxPTR "\n",addr,f->EXC_SRR0);
            else printk("\n---Data read from $%" PRIx32 " attempted at $%"
                        PRIxPTR "\n",addr,f->EXC_SRR0);
  return -1;
}

int InstructionMissException(BSP_Exception_frame *f, unsigned int vector)
{
  printk("\n---Instruction fetch attempted from $%" PRIxPTR ", no TLB exists\n",
         f->EXC_SRR0);
  return -1;
}

int InstructionFetchException(BSP_Exception_frame *f, unsigned int vector)
{
  printk("\n---Instruction fetch attempted from $%" PRIxPTR
         ", TLB is no-execute\n",f->EXC_SRR0);
  return -1;
}
