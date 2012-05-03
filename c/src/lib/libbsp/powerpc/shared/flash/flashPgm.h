#ifndef BSP_FLASH_PGM_API_H
#define BSP_FLASH_PGM_API_H

/* Trivial Flash Programmer                                  */

/* Author: Till Straumann <strauman@slac.stanford.edu>, 2006
 * NOTE: copyright info at the bottom of this file
 */

/* IMPORTANT NOTE
 *
 * The flash API is NOT THREAD SAFE. During the execution of any of the
 * BSP_flashXXX() routines, flash (residing in the same device)
 * MUST NOT be accessed by other threads in ANY way (NOT EVEN READ!).
 * Read operations may return internal device register contents
 * instead of memory array data when issued while a flash device
 * is erased, written or queried by the library.
 *
 * The routines are intended for occasional maintenance use only
 * (i.e., not for implementing a file system or similar).
 *
 * While polling for the completion of block erase operations the
 * CPU is yielded to other threads. Busy waiting (interrupts and
 * thread dispatching remain enabled) on write operations is employed.
 */

#include <stdio.h>

#ifdef __cplusplus
  extern "C" {
#endif

/* Disengage flash write protection. Write protection is implemented
 * at the board or chipset level by disabling all write operations/bus cycles
 * to the flash device(s).
 * With write protection enabled, nothing but 'ordinary' (array) read operations
 * are possible.
 * Write protection must be disabled not only to erase and write contents
 * but also in order to read ID, size, status etc.
 * None of the operations (except for BSP_flashWriteEnable()) are possible
 * on a write-protected device.
 *
 *  'bank': flash bank # (usually 0)
 * RETURNS: 0 on success, nonzero on error (printing message to stderr).
 *
 *   NOTES: - some boards (MVME5500) don't support 'bank' granularity but
 *            enable/disable write protection for all devices at once.
 *          - a jumper-based protection mechanism might be in place
 *            in addition to the software-based one. Consult the user's
 *            manual of your board for more information.
 */
int
BSP_flashWriteEnable(int bank);

/* Engage flash write protection (see above)
 */
int
BSP_flashWriteDisable(int bank);

/* Erase a region of flash memory.
 *   'bank': flash bank # (usually 0).
 * 'offset': destination address offset (from start of bank).
 *   'size': number of bytes to erase.
 *  'quiet': if non-zero, suppress confirmation message / prompt
 *           if > 1 also suppress the progress indicator.
 *
 *  RETURNS: 0 on success, nonzero on error (printing messages to stderr).
 *
 *    NOTES: - 'offset' and 'size' must be block-aligned. Common 16-bit devices
 *           have a block size of 0x20000 bytes. If two such devices are
 *           operated in parallel to form a 32-bit word then the 'effective'
 *           block size is 0x40000 bytes. The block size can be queried by
 *           BSP_flashBlockSize(int bank);
 *
 *           - erase operation is verified.
 */
int
BSP_flashErase(int bank, uint32_t offset, uint32_t size, int quiet);

/* Write data from a buffer to flash. The target area is erased if necessary.
 *
 *   'bank': flash bank # (usually 0).
 * 'offset': destination address offset (from start of bank).
 *    'src': data source block address (in memory).
 *'n_bytes': number of bytes to copy.
 *  'quiet': if non-zero, suppress confirmation message / prompt
 *           if > 1 also suppress the progress indicator.
 *
 *    NOTES: - Erase operations are only performed where necessary. I.e.,
 *           if one or both of the boundaries of the destination region is/are
 *           not block-aligned then adjacent data are preserved provided that
 *           the relevant chunks of the destination are blank (erased).
 *
 *            | <neighbour> fffffff |
 *                          ^--- destination ----- ^
 *            | : block boundary
 *            f : blank/erased pieces
 *
 *           (If the start of the destination region up to the next block boundary
 *           is blank then '<neighbour>'-data is preserved. The end of the
 *           destination is treated the same way.)
 *
 *           - user confirmation is requested before changes are made
 *
 *           - 'src' must not point into the destination bank (no copy
 *           within a flash bank).
 *
 *           - erase and write operations are verified.
 *
 *  RETURNS: 0 on success, nonzero on error (message printed to stderr).
 */
int
BSP_flashWrite(int bank, uint32_t offset, char *src, uint32_t n_bytes, int quiet);

/* Copy contents of a file to flash.
 *
 *  'fname': Path of a file.
 *  'quiet': if non-zero, suppress confirmation message / prompt
 *           if > 1 also suppress the progress indicator.
 *
 *   NOTES: Convenience wrapper around BSP_flashWrite(); see above for
 *          args and return value.
 */
int
BSP_flashWriteFile(int bank, uint32_t offset, char *path, int quiet);

/* Dump info about available flash to file
 * (stdout is used if f==NULL).
 *
 * RETURNS: 0
 *   NOTES: Write protection must be disengaged (see above);
 */
int
BSP_flashDumpInfo(FILE *f);

/*
 * Obtain starting-address of flash bank (as seen from CPU)
 * (returns ((uint32_t) -1) if the bank argument is invalid).
 */

uint32_t
BSP_flashStart(int bank);

/*
 * Obtain size of flash bank (returns ((uint32_t) -1) if the
 * bank argument is invalid).
 */
uint32_t
BSP_flashSize(int bank);

/*
 * Obtain block size of flash bank (sector size times
 * number of devices in parallel; the block size determines
 * alignment and granularity accepted by BSP_flashErase()
 * (returns ((uint32_t) -1) if the bank argument is invalid).
 */
uint32_t
BSP_flashBlockSize(int bank);

#ifdef __cplusplus
  }
#endif

/*
 * Authorship
 * ----------
 * This software was created by
 *     Till Straumann <strauman@slac.stanford.edu>, 2005-2007,
 * 	   Stanford Linear Accelerator Center, Stanford University.
 *
 * Acknowledgement of sponsorship
 * ------------------------------
 * The software was produced by
 *     the Stanford Linear Accelerator Center, Stanford University,
 * 	   under Contract DE-AC03-76SFO0515 with the Department of Energy.
 *
 * Government disclaimer of liability
 * ----------------------------------
 * Neither the United States nor the United States Department of Energy,
 * nor any of their employees, makes any warranty, express or implied, or
 * assumes any legal liability or responsibility for the accuracy,
 * completeness, or usefulness of any data, apparatus, product, or process
 * disclosed, or represents that its use would not infringe privately owned
 * rights.
 *
 * Stanford disclaimer of liability
 * --------------------------------
 * Stanford University makes no representations or warranties, express or
 * implied, nor assumes any liability for the use of this software.
 *
 * Stanford disclaimer of copyright
 * --------------------------------
 * Stanford University, owner of the copyright, hereby disclaims its
 * copyright and all other rights in this software.  Hence, anyone may
 * freely use it for any purpose without restriction.
 *
 * Maintenance of notices
 * ----------------------
 * In the interest of clarity regarding the origin and status of this
 * SLAC software, this and all the preceding Stanford University notices
 * are to remain affixed to any copy or derivative of this software made
 * or distributed by the recipient and are to be affixed to any copy of
 * software made or distributed by the recipient that contains a copy or
 * derivative of this software.
 *
 * ------------------ SLAC Software Notices, Set 4 OTT.002a, 2004 FEB 03
 */

#endif
