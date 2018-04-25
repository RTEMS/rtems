/*

Low-level interface to the PPC405 MMU

M.Hamel ADInstruments 2008

*/

#include <rtems/asm.h>

/* Useful MMU SPR values */

#define SPR_ZPR		0x3B0
#define SPR_PID		0x3B1

			.text

/* void MMU_ClearTLBs(); */
			PUBLIC_VAR(MMU_ClearTLBs)
SYM (MMU_ClearTLBs):
			tlbia
			isync
			lis		r3,0x5555		// *** Gratuitous fiddle of ZPR to 0101010101 to take it out of 
			mtspr	SPR_ZPR,r3		// the picture
			blr
			
/* void MMU_SetTLBEntry(UInt8 index, UInt32 tagword, UInt32 dataword, UInt8 SPR_PID) */
			PUBLIC_VAR(MMU_SetTLBEntry)
SYM (MMU_SetTLBEntry):
			mfspr	r7,SPR_PID		// Save the current SPR_PID
			mtspr	SPR_PID,r6		// Write to SPR_PID
			tlbwehi	r4,r3			// Write hiword
			mtspr	SPR_PID,r7		// Restore the SPR_PID
			tlbwelo	r5,r3			// Write loword
			isync
			blr

/* void MMU_GetTLBEntry(UInt8 index, UInt32& tagword, UInt32& dataword, UInt8& SPR_PID) */
			PUBLIC_VAR(MMU_GetTLBEntry)
SYM (MMU_GetTLBEntry):		
			mfspr	r7,SPR_PID		// Save the current SPR_PID
			tlbrehi	r8,r3			// Read hiword & SPR_PID
			mfspr	r9,SPR_PID		// Copy the SPR_PID
			mtspr	SPR_PID,r7		// Restore original SPR_PID so we can proceed
			stw		r8,0(r4)		// Write to r4 pointer
			stb		r9,0(r6)		// Write to r6 pointer
			tlbrelo	r8,r3			// Read loword
			stw		r8,0(r5)		// Write to r5 pointer
			blr
			
/* SInt16 MMU_FindTLBEntry(UInt32 address) */
/* Returns index of covering TLB entry (0..63), or -1 if there isn't one */
			PUBLIC_VAR(MMU_FindTLBEntry)
SYM (MMU_FindTLBEntry):		
			tlbsx.	r3,0,r3
			beqlr
			li		r3,0xFFFFFFFF
			blr

/* bool		mmu_enable_code(bool enable); */
			PUBLIC_VAR(mmu_enable_code)
SYM (mmu_enable_code):	
			li		r5,0x20	// IR bit
			b		msrbits
			
/* bool		mmu_enable_data(bool enable); */
			PUBLIC_VAR(mmu_enable_data)
SYM (mmu_enable_data):	
			li		r5,0x10		// DR bit
msrbits:	cmpwi	r3,0			// Common code: parameter 0?
			mfmsr	r4				// r4 = MSR state
			beq		clrBit
			or		r6,r4,r5		// If 1, r6 = MSR with bit set
			b		setmsr
clrBit:		andc	r6,r4,r5		// If 0 r6 = MSR with bit clear
setmsr:		mtmsr	r6				// Write new MSR
			and.	r3,r4,r5		// Result = old MSR bit
			beqlr						// If zero return zero
			li		r3,0xFF		// If nonzero return byte -1
			blr
	
			
			
