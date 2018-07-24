#ifndef _mmu_405_h
#define _mmu_405_h

/* 
   Simple interface to the PowerPC 405 MMU
   
   The intention here is just to allow the MMU to be used to define cacheability and
   read/write/execute permissions in a simple enough way to fit entirely into the
   64-entry TLB cache.
   
   This code does not do address relocation and does not generate any MMU-related interrupts.
   
   The process ID support is there for a possible future extension where RTEMS supports
   setting the process ID on task switches, which allows per-process stack protection
    
   This code will call fatal_error() if your add_space() calls overrun the 64 entries

   Michael Hamel ADInstruments 2008

*/


#ifdef __cplusplus
extern "C" {
#endif

#include "stdint.h"

enum {
	kAllProcessIDs = 0
};

typedef enum MMUAccessType {
	executable,
	readOnlyData,
	readOnlyNoCache,
	readWriteData,
	readWriteNoCache,
	readWriteExecutable
} MMUAccessType;

/* Initialise and clear the MMU */
void		mmu_initialise(void);

/* Turn on/off data access translation */
bool		mmu_enable_data(bool enable);

/* Turn on instruction translation */
bool		mmu_enable_code(bool enable);

/* Define properties for an area of memory (must be 1K-aligned) */
void		mmu_add_space(uint32_t startAddr, uint32_t endAddr, MMUAccessType permissions, uint8_t processID);

/* Delete a memory property definition */
void		mmu_remove_space(uint32_t startAddr, uint32_t endAddr);

/* Return number of TLB entries out of total in use */
int			mmu_get_tlb_count(void);

/* Allocate a new process ID and return it */
uint8_t		mmu_new_processID(void);

/* Free a process ID that has been in use */
void		mmu_free_processID(uint8_t freeThis);

/* Return the current process ID */
uint8_t		mmu_current_processID(void);

/* Change the process ID to ID and return the old value */
uint8_t		mmu_set_processID(uint8_t toID);


#ifdef __cplusplus
}
#endif

#endif //_mmu_405.h
