/*******************************************************************************
*                                                                              *
* Copyright 2013 Altera Corporation. All Rights Reserved.                      *
*                                                                              *
* Redistribution and use in source and binary forms, with or without           *
* modification, are permitted provided that the following conditions are met:  *
*                                                                              *
* 1. Redistributions of source code must retain the above copyright notice,    *
*    this list of conditions and the following disclaimer.                     *
*                                                                              *
* 2. Redistributions in binary form must reproduce the above copyright notice, *
*    this list of conditions and the following disclaimer in the documentation *
*    and/or other materials provided with the distribution.                    *
*                                                                              *
* 3. The name of the author may not be used to endorse or promote products     *
*    derived from this software without specific prior written permission.     *
*                                                                              *
* THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDER "AS IS" AND ANY EXPRESS OR *
* IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF *
* MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE, ARE DISCLAIMED. IN NO  *
* EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,       *
* SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, *
* PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;  *
* OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,     *
* WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR      *
* OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF       *
* ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.                                   *
*                                                                              *
*******************************************************************************/

/*! \file Altera - ALT_SOCAL */

#ifndef __ALTERA_SOCAL_H__
#define __ALTERA_SOCAL_H__

#ifdef __cplusplus
extern "C"
{
#endif  /* __cplusplus */

/*!
 * \addtogroup ALT_SOCAL_UTIL SoCAL Utilities
 *
 * This file contains utility and support functions for the Altera SoCAL.
 * @{
 */

#ifdef __ASSEMBLY__
#define ALT_CAST(type, ptr)  ptr
#else   /* __ASSEMBLY__ */
/*! Cast the pointer to specified pointer type.
 *
 *  Note: This macro expands to \e ptr value only for assembler language 
 *        targets.
 *
 *  \param type     The pointer type to cast to
 *  \param ptr      The pointer to apply the type cast to
 */
#define ALT_CAST(type, ptr)  ((type) (ptr))
#endif  /* __ASSEMBLY__ */


/*!
 * \addtogroup ALT_SOCAL_UTIL_RW_FUNC SoCAL Memory Read/Write Utilities
 *
 * This section implements read and write functionality for various
 * memory untis. The memory unit terms used for these functions are
 * consistent with those used in the ARM Architecture Reference Manual
 * ARMv7-A and ARMv7-R edition manual. The terms used for units of memory are:
 *
 *  Unit of Memory | Abbreviation | Size in Bits
 * :---------------|:-------------|:------------:
 *  Byte           | byte         |       8
 *  Half Word      | hword        |      16
 *  Word           | word         |      32
 *  Double Word    | dword        |      64
 *
 * @{
 */

/*! Write the 8 bit byte to the destination address in device memory.
 *  \param dest - Write destination pointer address
 *  \param src  - 8 bit data byte to write to memory
 */
#define alt_write_byte(dest, src)       (*ALT_CAST(volatile uint8_t *, (dest)) = (src))

/*! Read and return the 8 bit byte from the source address in device memory.
 *  \param src    Read source pointer address
 *  \returns      8 bit data byte value
 */
#define alt_read_byte(src)              (*ALT_CAST(volatile uint8_t *, (src)))

/*! Write the 16 bit half word to the destination address in device memory.
 *  \param dest - Write destination pointer address
 *  \param src  - 16 bit data half word to write to memory
 */
#define alt_write_hword(dest, src)      (*ALT_CAST(volatile uint16_t *, (dest)) = (src))

/*! Read and return the 16 bit half word from the source address in device memory.
 *  \param src    Read source pointer address
 *  \returns      16 bit data half word value
 */
#define alt_read_hword(src)             (*ALT_CAST(volatile uint16_t *, (src)))

/*! Write the 32 bit word to the destination address in device memory.
 *  \param dest - Write destination pointer address
 *  \param src  - 32 bit data word to write to memory
 */
#define alt_write_word(dest, src)       (*ALT_CAST(volatile uint32_t *, (dest)) = (src))

/*! Read and return the 32 bit word from the source address in device memory.
 *  \param src    Read source pointer address
 *  \returns      32 bit data word value
 */
#define alt_read_word(src)              (*ALT_CAST(volatile uint32_t *, (src)))

/*! Write the 64 bit double word to the destination address in device memory.
 *  \param dest - Write destination pointer address
 *  \param src  - 64 bit data double word to write to memory
 */
#define alt_write_dword(dest, src)      (*ALT_CAST(volatile uint64_t *, (dest)) = (src))

/*! Read and return the 64 bit double word from the source address in device memory.
 *  \param src    Read source pointer address
 *  \returns      64 bit data double word value
 */
#define alt_read_dword(src)             (*ALT_CAST(volatile uint64_t *, (src)))

/*! @} */

/*!
 * \addtogroup ALT_SOCAL_UTIL_SC_FUNC SoCAL Memory Bit Set/Clr/XOR/Replace Utilities
 *
 * This section implements useful macros to set, clear, change, and replace
 * selected bits within a word in memory or a memory-mapped register.
 * @{
 *
 */

/*! Set selected bits in the 8 bit byte at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to set in destination byte
 */
#define     alt_setbits_byte(dest, bits)        (alt_write_byte(dest, alt_read_byte(dest) | (bits)))

/*! Clear selected bits in the 8 bit byte at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to clear in destination byte
 */
#define     alt_clrbits_byte(dest, bits)        (alt_write_byte(dest, alt_read_byte(dest) & ~(bits)))

/*! Change or toggle selected bits in the 8 bit byte at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to change in destination byte
 */
#define     alt_xorbits_byte(dest, bits)        (alt_write_byte(dest, alt_read_byte(dest) ^ (bits)))

/*! Replace selected bits in the 8 bit byte at the destination address in device memory.
 *  \param  dest - Destination pointer address
 *  \param  msk  - Bits to replace in destination byte
 *  \param  src  - Source bits to write to cleared bits in destination byte
 */
#define     alt_replbits_byte(dest, msk, src)   (alt_write_byte(dest,(alt_read_byte(dest) & ~(msk)) | ((src) & (msk))))

/*! Set selected bits in the 16 bit halfword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to set in destination halfword
 */
#define     alt_setbits_hword(dest, bits)       (alt_write_hword(dest, alt_read_hword(dest) | (bits)))

/*! Clear selected bits in the 16 bit halfword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to clear in destination halfword
 */
#define     alt_clrbits_hword(dest, bits)       (alt_write_hword(dest, alt_read_hword(dest) & ~(bits)))

/*! Change or toggle selected bits in the 16 bit halfword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to change in destination halfword
 */
#define     alt_xorbits_hword(dest, bits)       (alt_write_hword(dest, alt_read_hword(dest) ^ (bits)))

/*! Replace selected bits in the 16 bit halfword at the destination address in device memory.
 *  \param  dest - Destination pointer address
 *  \param  msk  - Bits to replace in destination halfword
 *  \param  src  - Source bits to write to cleared bits in destination halfword
 */
#define     alt_replbits_hword(dest, msk, src)   (alt_write_hword(dest,(alt_read_hword(dest) & ~(msk)) | ((src) & (msk))))

/*! Set selected bits in the 32 bit word at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to set in destination word
 */
#define     alt_setbits_word(dest, bits)        (alt_write_word(dest, alt_read_word(dest) | (bits)))

/*! Clear selected bits in the 32 bit word at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to clear in destination word
 */
#define     alt_clrbits_word(dest, bits)        (alt_write_word(dest, alt_read_word(dest) & ~(bits)))

/*! Change or toggle selected bits in the 32 bit word at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to change in destination word
 */
#define     alt_xorbits_word(dest, bits)        (alt_write_word(dest, alt_read_word(dest) ^ (bits)))

/*! Replace selected bits in the 32 bit word at the destination address in device memory.
 *  \param  dest - Destination pointer address
 *  \param  msk  - Bits to replace in destination word
 *  \param  src  - Source bits to write to cleared bits in destination word
 */
#define     alt_replbits_word(dest, msk, src)   (alt_write_word(dest,(alt_read_word(dest) & ~(msk)) | ((src) & (msk))))

/*! Set selected bits in the 64 bit doubleword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to set in destination doubleword
 */
#define     alt_setbits_dword(dest, bits)       (alt_write_dword(dest, alt_read_dword(dest) | (bits)))

/*! Clear selected bits in the 64 bit doubleword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to clear in destination doubleword
 */
#define     alt_clrbits_dword(dest, bits)       (alt_write_dword(dest, alt_read_dword(dest) & ~(bits)))

/*! Change or toggle selected bits in the 64 bit doubleword at the destination address in device memory.
 *  \param dest - Destination pointer address
 *  \param bits - Bits to change in destination doubleword
 */
#define     alt_xorbits_dword(dest, bits)       (alt_write_dword(dest, alt_read_dword(dest) ^ (bits)))

/*! Replace selected bits in the 64 bit doubleword at the destination address in device memory.
 *  \param  dest - Destination pointer address
 *  \param  msk  - Bits to replace in destination doubleword
 *  \param  src  - Source bits to write to cleared bits in destination word
 */
#define     alt_replbits_dword(dest, msk, src)   (alt_write_dword(dest,(alt_read_dword(dest) & ~(msk)) | ((src) & (msk))))



/*! @} */

/*!
 * \addtogroup ALT_SOCAL_TYPE_IND_FUNC SoCAL Indirect (pointer-based) Utilities
 *
 * This section implements two other useful forms of the alt_write_*() macros above that
 * are preferable to use in some situations. These use an intermediate pointer (defined
 * in the containing compile unit) to move data in an indirect manner. These compile to very
 * tight ARM code, equivalent to the above versions.
 *
 * @{
 */

/*! Write the 8 bit byte to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to byte data
 *  \param src   - 8 bit data value to write to memory
 */
#define alt_indwrite_byte(dest, tmptr, src)  {(tmptr)=ALT_CAST(uint8_t*,(dest));(*ALT_CAST(volatile uint8_t*,(tmptr))=(src));}

/*! Write the 8 bit byte to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to byte data
 *  \param src   - Read destination pointer address
 */
#define alt_indread_byte(dest, tmptr, src)   {(tmptr)=ALT_CAST(uint8_t*,(src));(*ALT_CAST(volatile uint8_t*,(dest))=*(tmptr));}

/*! Write the 16 bit halfword to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to halfword data
 *  \param src   - 16 bit data value to write to memory
 */
#define alt_indwrite_hword(dest, tmptr, src) {(tmptr)=ALT_CAST(uint16_t*,(dest));(*ALT_CAST(volatile uint16_t*,(tmptr))=(src));}

/*! Write the 16 bit halfword to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to halfword data
 *  \param src   - Read destination pointer address
 */
#define alt_indread_hword(dest, tmptr, src)  {(tmptr)=ALT_CAST(uint16_t*,(src));(*ALT_CAST(volatile uint16_t*,(dest))=*(tmptr));}

/*! Write the 32 bit word to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to word data
 *  \param src   - 32 bit data value to write to memory
 */
#define alt_indwrite_word(dest, tmptr, src)  {(tmptr)=ALT_CAST(uint32_t*,(dest));(*ALT_CAST(volatile uint32_t*,(tmptr))=(src));}

/*! Write the 32 bit word to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to word data
 *  \param src   - Read destination pointer address
 */
#define alt_indread_word(dest, tmptr, src)   {(tmptr)=ALT_CAST(uint32_t*,(src));(*ALT_CAST(volatile uint32_t*,(dest))=*(tmptr));}

/*! Write the 64 bit dword to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to double-word data
 *  \param src   - 64 bit data value to write to memory
 */
#define alt_indwrite_dword(dest, tmptr, src) {(tmptr)=ALT_CAST(uint64_t*,(dest));(*ALT_CAST(volatile uint64_t*,(tmptr))=(src));}

/*! Write the 64 bit dword to the destination address in device memory.
 *  \param dest  - Write destination pointer address
 *  \param tmptr - Temporary pointer to double-word data
 *  \param src   - Read destination pointer address
 */
#define alt_indread_dword(dest, tmptr, src)  {(tmptr)=ALT_CAST(uint64_t*,(src));(*ALT_CAST(volatile uint64_t*,(dest))=*(tmptr));}


/*! @} */

/*!
 * \addtogroup ALT_SOCAL_CMPL_ASRT_FUNC SoCAL Compile Assert Utilities
 *
 * This section implements an assert-type functionality in the compiler rather than in the
 * debug run-time code. Additional macros can be built on the basic structure and defined
 * to test various conditions and throw a compile-time error if necessary.
 *
 * @{
 */

/*! alt_cat_compile_assert_text() concatenates text.
 *  \param txta - The first text fragment to be joined
 *  \param txtb - The second text fragment to be joined
 */
#define alt_cat_compile_assert_text(txta, txtb) txta##txtb

/*! alt_form_compile_assert_line() is the basis of other functions that check various
 *  conditions and possibly throw a compile-time error in response, giving an
 *  assert equivalent that operates at compile time rather than at run-time.
 *  \param test - Any valid boolean expression
 *  \param file - The filename where this expression is located (ASCII string)
 *  \param line - The line number where this expression is located
 */
#define alt_form_compile_assert_line(test, file, line) \
typedef char alt_cat_compile_assert_text(assertion_at_##file##_line_, line)[2*!!(test)-1]

/*! alt_check_struct_size() throws a compile-time error if the structure size (a) is
 *  larger than the size of the reference (b). \n
 *  alt_check_struct_size() works with groups of bitfields up to much larger
 *  structure sizes.
 *  \param a - Structure to be evaluated
 *  \param b - Reference size
 */
#define alt_check_struct_size(a, b) alt_form_compile_assert_line((sizeof(a) <= sizeof(b)),__FILE__,__LINE__)


/*! @} */
/*! @} */

#ifdef __cplusplus
}
#endif  /* __cplusplus */
#endif  /* __ALTERA_SOCAL_H__ */
