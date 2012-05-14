/*---------------------------------------------------------------------------------
  BIOS functions

  Copyright (C) 2005
    Michael Noland (joat)
    Jason Rogers (dovoto)
    Dave Murphy (WinterMute)

  This software is provided 'as-is', without any express or implied
  warranty.  In no event will the authors be held liable for any
  damages arising from the use of this software.

  Permission is granted to anyone to use this software for any
  purpose, including commercial applications, and to alter it and
  redistribute it freely, subject to the following restrictions:

  1.  The origin of this software must not be misrepresented; you
      must not claim that you wrote the original software. If you use
      this software in a product, an acknowledgment in the product
      documentation would be appreciated but is not required.
  2.  Altered source versions must be plainly marked as such, and
      must not be misrepresented as being the original software.
  3.  This notice may not be removed or altered from any source
      distribution.

---------------------------------------------------------------------------------*/

#ifndef BIOS_H_INCLUDE
#define BIOS_H_INCLUDE

/*!	\file bios.h

	\brief Nintendo DS Bios functions
*/


#ifdef __cplusplus
extern "C" {
#endif


#include <nds/jtypes.h>


typedef struct DecompressionStream {
  int (*getSize)(uint8 * source, uint16 * dest, uint32 r2);
  int (*getResult)(uint8 * source); // can be NULL
  uint8 (*readByte)(uint8 * source);
} PACKED TDecompressionStream;


typedef struct UnpackStruct {
  uint16 sourceSize; // in bytes
  uint8 sourceWidth;
  uint8 destWidth;
  uint32 dataOffset;
} PACKED TUnpackStruct, * PUnpackStruct;


/*! \fn swiSoftReset(void)
	\brief reset the DS.

*/
void swiSoftReset(void);

/*! \fn swiDelay( uint32 duration)
	\brief delay

   Delays for for a period X + Y*duration where X is the swi overhead and Y is a cycle of
<CODE><PRE>
     loop:
       sub r0, #1
       bgt loop
</PRE></CODE>
  of thumb fetches in BIOS memory
	\param duration
		length of delay
	\note
		Duration should be 1 or more, a duration of 0 is a huge delay

*/
void swiDelay(uint32 duration);

/*! \fn swiIntrWait(int waitForSet, uint32 flags)

	\brief wait for interrupt(s) to occur

	\param waitForSet
		0: Return if the interrupt has already occured
		1: Wait until the interrupt has been set since the call
	\param flags
		interrupt mask to wait for

*/

void swiIntrWait(int waitForSet, uint32 flags);

/*! \fn  swiWaitForVBlank(void)
	\brief Wait for vblank interrupt

	Waits for a vertical blank interrupt

	\note Identical to calling swiIntrWait(1, 1)
*/

void swiWaitForVBlank(void);

/*!	\fn int swiDivide(int numerator, int divisor)
	\param numerator
		signed integer to divide
	\param divisor
		signed integer to divide by
	\return numerator / divisor

	\fn int swiRemainder(int numerator, int divisor)
	\param numerator
		signed integer to divide
	\param divisor
		signed integer to divide by
	\return numerator % divisor

	\fn void swiDivMod(int numerator, int divisor, int * result, int * remainder)
	\param numerator
		signed integer to divide
	\param divisor
		signed integer to divide by
	\param result
		pointer to integer set to numerator / divisor
	\param remainder
		pointer to integer set to numerator % divisor
*/

int swiDivide(int numerator, int divisor);
int swiRemainder(int numerator, int divisor);
void swiDivMod(int numerator, int divisor, int * result, int * remainder);

/*!	\fn swiCopy(const void * source, void * dest, int flags)
	\param source
		pointer to transfer source
	\param dest
		dest = pointer to transfer destination
	\param flags
		copy mode and size
//  flags(26) = transfer width (0: halfwords, 1: words)
//  flags(24) = transfer mode (0: copy, 1: fill)
//  flags(20..0) = transfer count (always in words)

	\fn swiFastCopy (const void * source, void * dest, int flags)
	\param source
		pointer to transfer source
	\param dest
		dest = pointer to transfer destination
	\param flags
		copy mode and size
//  flags(24) = transfer mode (0: copy, 1: fill)
//  flags(20..0) = transfer count (in words)

	\note Transfers more quickly than swiCopy, but has higher interrupt latency
*/

#define COPY_MODE_HWORD  (0)
#define COPY_MODE_WORD  (1<<26)
#define COPY_MODE_COPY  (0)
#define COPY_MODE_FILL  (1<<24)

void swiCopy(const void * source, void * dest, int flags);
void swiFastCopy(const void * source, void * dest, int flags);

// swiSqrt (swi 0x0D)

int swiSqrt(int value);

// swiCRC16(uint16 crc, void * data, uint32 size) (swi 0x0E)
//   crc - starting CRC-16
//   data - pointer to data (processed nibble by nibble)
//   size - size in bytes
//
// Returns the CRC-16 after the data has been processed

uint16 swiCRC16(uint16 crc, void * data, uint32 size);

// int swiIsDebugger(void)
//   Returns 0 if running on a debugger (8 MB of ram instead of 4 MB)
//
// Note: swi 0x0F on both CPUs

int swiIsDebugger(void);

// swiUnpackBits
//  source      - Source address
//  destination - destination address (word aligned)
//  params      - Unpack structure
//       uint16 sourceSize (in bytes)
//       uint8 sourceWidth (1,2,4,8)
//       uint8 destWidth (1,2,4,8,16,32)
//       uint32 dataOffset (bits 0..30 are added to all non-zero
//                          destination writes, unless b31 is set,
//                          which does it for zeros too)
//
// Unpack data stored multiple elements to a byte into a larger space
// i.e. 8 elements per byte (i.e. b/w font), into 1 element per byte
//
// Note: swi 0x10 on both CPUs

void swiUnpackBits(uint8 * source, uint32 * destination, PUnpackStruct params);

// DecompressLZSSWram (swi 0x11)
//   source      - pointer to a header word, followed by compressed data
//                 word(31..8) = size of uncompressed data (in bytes)
//                 word(7..0) = ignored
//   destination - destination address
// Writes data a byte at a time
//
// DecompressLZSSVram (swi 0x12)
// Writes data a halfword at a time
// See DecompressRLEVram for parameter details

void swiDecompressLZSSWram(void * source, void * destination);
int swiDecompressLZSSVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

// swiDecompressHuffman (swi 0x13)
// See DecompressRLE for parameter details

int swiDecompressHuffman(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

// DecompressRLEWram (swi 0x14)
//   r0 - pointer to a header word, followed by compressed data
//        word(31..8) = size of uncompressed data (in bytes)
//        word(7..0) = ignored
//   r1 - destination address
// Writes data a byte at a time
//
// DecompressRLEVram (swi 0x15)
//   r0 - pointer to compressed source data, maybe! (always goes through
//        the function pointers, so could just be an offset)
//   r1 - pointer to destination (halfword)
//   r2 - no idea, passed to getSize function
//   r3 - pointer to a structure
// Writes data a halfword at a time
//
// Result of getSize is a word, with the size of decompressed data in
// bits 31..8, and bits 7..0 are ignored.  This value is also returned
// by the swi, unless getResult is non-NULL and returns a neg. value
//
// getResult is used to provide a result for the swi, given the source
// pointer after all data has been read (or if getSize < 0).  It can
// be NULL, unlike getSize and readByte.  Its value is only returned
// if negative, otherwise the typical result is used, so it is likely
// some sort of error-checking procedure
//
// readByte is used in lieu of directly accessing memory
//
// compressed data format:
//   bit(7): 0: uncompressed, 1: compressed
//   bit(6..0) when uncompressed: run length - 1, followed by run_length bytes of true data
//   bit(6..0) when compressed: run length - 3, followed by one byte of true data, to be repeated

void swiDecompressRLEWram(void * source, void * destination);
int swiDecompressRLEVram(void * source, void * destination, uint32 toGetSize, TDecompressionStream * stream);

// swiDecodeDelta8
//   source      - pointer to a header word, followed by encoded data
//                 word(31..8) = size of data (in bytes)
//                 word(7..0) = ignored
//   destination - destination address
// Writes data a byte at a time
//
// Note: ARM9 exclusive swi 0x16

#ifdef ARM9

/*! \fn swiWaitForIRQ
	\brief wait for any interrupt

	\note ARM9 exclusive
*/
void swiWaitForIRQ(void);

void swiSetHaltCR(uint32 data);


extern void swiDecodeDelta8(void * source, void * destination);

// swiDecodeDelta16
//   source      - pointer to a header word, followed by encoded data
//                 word(31..8) = size of data (in bytes)
//                 word(7..0) = ignored
//   destination - destination address
// Writes data a halfword at a time
//
// Note: ARM9 exclusive swi 0x18

void swiDecodeDelta16(void * source, void * destination);
#endif


#ifdef ARM7

//
// swiHalt (swi 0x06)
//   Same as swiSetHaltCR(0x80)
//
// swiSleep (swi 0x07)
//   Same as swiSetHaltCR(0xC0)
//
// swiSwitchToGBAMode (not a SWI)
//   Same as swiSetHaltCR(0x40)
//
// swiSetHaltCR (swi 0x1F)
//   Writes a byte of the data to 0x04000301:8
//
// Note: All of these are ARM7 exclusive
//
// swiSetHaltCR (swi 0x1F)
//   Writes a word of the data to 0x04000300:32
//
// Note: This is on the ARM9, but works differently to the ARM7 function!
//

void swiHalt(void);
void swiSleep(void);
void swiSwitchToGBAMode(void);
void swiSetHaltCR(uint8 data);

// swiGetSineTable(int index)
//  Returns an entry in the sine table (index = 0..63)
//
// Note: ARM7 exclusive swi 0x1A

uint16 swiGetSineTable(int index);
//
// swiGetPitchTable(int index)
//   Returns an entry in the pitch table (index = 0..767)
//
// Note: ARM7 exclusive swi 0x1B

uint16 swiGetPitchTable(int index);


// swiGetVolumeTable(int index)
//   Returns an entry in the volume table (index = 0..723)
//
// Note: ARM7 exclusive swi 0x1C
uint8 swiGetVolumeTable(int index);
// swiChangeSoundBias(int enabled, int delay)
//   enabled
//     0: decrement SOUND_BIAS once per delay until it reaches 0x000
//     1: increment SOUND_BIAS once per delay until it reaches 0x200
//   delay is in the same units of time as swiDelay
//
// Note: ARM7 exclusive SWI 0x08

void swiChangeSoundBias(int enabled, int delay);


#endif //ARM7


#ifdef __cplusplus
}
#endif


#endif

