/*
 *  COPYRIGHT (c) 2010 Chris Johns <chrisj@rtems.org>
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */
/**
 * @file
 *
 * @ingroup rtems-rfs
 *
 * RTEMS File Systems Bitmap Routines.
 *
 * These functions manage bit maps. A bit map consists of the map of bit
 * allocated in a block and a search map where a bit represents 32 actual
 * bits. The search map allows for a faster search for an available bit as 32
 * search bits can checked in a test.
 */

#if !defined (_RTEMS_RFS_BITMAPS_H_)
#define _RTEMS_RFS_BITMAPS_H_

#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system-fwd.h>
#include <rtems/rfs/rtems-rfs-trace.h>

/**
 * Define the way the bits are configured. We can have them configured as clear
 * being 0 or clear being 1. This does not effect how masks are defined. A mask
 * always has a 1 for set and 0 for clear.
 */
#define RTEMS_RFS_BITMAP_CLEAR_ZERO 0

#if RTEMS_RFS_BITMAP_CLEAR_ZERO
/*
 * Bit set is a 1 and clear is 0.
 */
#define RTEMS_RFS_BITMAP_BIT_CLEAR          0
#define RTEMS_RFS_BITMAP_BIT_SET            1
#define RTEMS_RFS_BITMAP_ELEMENT_SET        (RTEMS_RFS_BITMAP_ELEMENT_FULL_MASK)
#define RTEMS_RFS_BITMAP_ELEMENT_CLEAR      (0)
#define RTEMS_RFS_BITMAP_SET_BITS(_t, _b)   ((_t) | (_b))
#define RTEMS_RFS_BITMAP_CLEAR_BITS(_t, _b) ((_t) & ~(_b))
#define RTEMS_RFS_BITMAP_TEST_BIT(_t, _b)   (((_t) & (1 << (_b))) != 0 ? true : false)
#else
/*
 * Bit set is a 0 and clear is 1.
 */
#define RTEMS_RFS_BITMAP_BIT_CLEAR          1
#define RTEMS_RFS_BITMAP_BIT_SET            0
#define RTEMS_RFS_BITMAP_ELEMENT_SET        (0)
#define RTEMS_RFS_BITMAP_ELEMENT_CLEAR      (RTEMS_RFS_BITMAP_ELEMENT_FULL_MASK)
#define RTEMS_RFS_BITMAP_SET_BITS(_t, _b)   ((_t) & ~(_b))
#define RTEMS_RFS_BITMAP_CLEAR_BITS(_t, _b) ((_t) | (_b))
#define RTEMS_RFS_BITMAP_TEST_BIT(_t, _b)   (((_t) & (1 << (_b))) == 0 ? true : false)
#endif

/**
 * Invert a mask. Masks are always 1 for set and 0 for clear.
 */
#define RTEMS_RFS_BITMAP_INVERT_MASK(_mask) (~(_mask))

/**
 * This is the full mask of the length of the element. A mask is always a 1 for
 * set and 0 for clear. It is not effected by the state of
 * RTEMS_RFS_BITMAP_CLEAR_ZERO.
 */
#define RTEMS_RFS_BITMAP_ELEMENT_FULL_MASK (0xffffffffUL)

/**
 * The bitmap search window. Searches occur around a seed in either direction
 * for half the window.
 */
#define RTEMS_RFS_BITMAP_SEARCH_WINDOW (rtems_rfs_bitmap_element_bits () * 64)

/**
 * A bit in a map.
 */
typedef int32_t rtems_rfs_bitmap_bit;

/**
 * The basic element of a bitmap. A bitmap is manipulated by elements.
 */
typedef uint32_t rtems_rfs_bitmap_element;

/**
 * The power of 2 number of bits in the element.
 */
#define RTEMS_RFS_ELEMENT_BITS_POWER_2 (5)

/**
 * A bitmap or map is an array of bitmap elements.
 */
typedef rtems_rfs_bitmap_element* rtems_rfs_bitmap_map;

/**
 * The bitmap control is a simple way to manage the various parts of a bitmap.
 */
typedef struct rtems_rfs_bitmap_control_s
{
  rtems_rfs_buffer_handle* buffer;      //< Handle the to buffer with the bit
                                        //map.
  rtems_rfs_file_system*   fs;          //< The map's file system.
  rtems_rfs_buffer_block   block;       //< The map's block number on disk.
  size_t                   size;        //< Number of bits in the map. Passed
                                        //to create.
  size_t                   free;        //< Number of bits in the map that are
                                        //free (clear).
  rtems_rfs_bitmap_map     search_bits; //< The search bit map memory.
} rtems_rfs_bitmap_control;

/**
 * Return the number of bits for the number of bytes provided.
 */
#define rtems_rfs_bitmap_numof_bits(_bytes) (8 * (_bytes))

/**
 * Return the number of bits for the number of bytes provided.  The search
 * element and the element must have the same number of bits.
 */
#define rtems_rfs_bitmap_element_bits() \
  rtems_rfs_bitmap_numof_bits (sizeof (rtems_rfs_bitmap_element))

/**
 * Return the number of bits a search element covers.
 */
#define rtems_rfs_bitmap_search_element_bits() \
  (rtems_rfs_bitmap_element_bits() * rtems_rfs_bitmap_element_bits())

/**
 * Return the number of elements for a given number of bits.
 */
#define rtems_rfs_bitmap_elements(_bits) \
  ((((_bits) - 1) / rtems_rfs_bitmap_element_bits()) + 1)

/**
 * Release the bitmap buffer back to the buffer pool or cache.
 */
#define rtems_rfs_bitmap_release_buffer(_fs, _bm)    \
  rtems_rfs_buffer_handle_release (_fs, (_bm)->buffer)

/**
 * Return the element index for a given bit. We use a macro to hide any
 * implementation assuptions. Typically this would be calculated by dividing
 * the bit index by the number of bits in an element. Given we have a power of
 * 2 as the number of bits we can avoid the division by using a shift. A good
 * compiler should figure this out but I would rather enforce this than rely on
 * the specific backend of a compiler to do the right thing.
 */
#define rtems_rfs_bitmap_map_index(_b) \
  ((_b) >> RTEMS_RFS_ELEMENT_BITS_POWER_2)

/**
 * Return the bit offset for a given bit in an element in a map. See @ref
 * rtems_rfs_bitmap_map_index for a detailed reason why.
 */
#define rtems_rfs_bitmap_map_offset(_b) \
  ((_b) & ((1 << RTEMS_RFS_ELEMENT_BITS_POWER_2) - 1))

/**
 * Return the size of the bitmap.
 */
#define rtems_rfs_bitmap_map_size(_c) ((_c)->size)

/**
 * Return the number of free bits in the bitmap.
 */
#define rtems_rfs_bitmap_map_free(_c) ((_c)->free)

/**
 * Return the buffer handle.
 */
#define rtems_rfs_bitmap_map_handle(_c) ((_c)->buffer)

/**
 * Return the bitmap map block.
 */
#define rtems_rfs_bitmap_map_block(_c) ((_c)->block)

/**
 * Create a bit mask with the specified number of bits up to an element's
 * size. The mask is aligned to bit 0 of the element.
 *
 * @param size The number of bits in the mask.
 * @return The mask of the argument size number of bits.
 */
rtems_rfs_bitmap_element rtems_rfs_bitmap_mask (unsigned int size);

/**
 * Create a bit mask section. A mask section is a mask that is not aligned to
 * an end of the element.
 *
 * @param start The first bit of the mask numbered from 0.
 * @param end The end bit of the mask numbered from 0.
 * @return Mask section as defined by the start and end arguments.
 */
rtems_rfs_bitmap_element rtems_rfs_bitmap_mask_section (unsigned int start,
                                                        unsigned int end);

/**
 * Set a bit in a map and if all the bits are set, set the search map bit as
 * well.
 *
 * @param control The control for the map.
 * @param bit The bit in the map to set.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_map_set (rtems_rfs_bitmap_control* control,
                              rtems_rfs_bitmap_bit      bit);

/**
 * Clear a bit in a map and make sure the search map bit is clear so a search
 * will find this bit available.
 *
 * @param control The control for the map.
 * @param bit The bit in the map to clear.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_map_clear (rtems_rfs_bitmap_control* control,
                                rtems_rfs_bitmap_bit      bit);

/**
 * Test a bit in the map.
 *
 * @param control The bitmap control.
 * @param bit The bit to test.
 * @param state The state of the bit if no error is returned.
 * @return int The error number (errno). No error if 0.
 */
int
rtems_rfs_bitmap_map_test (rtems_rfs_bitmap_control* control,
                           rtems_rfs_bitmap_bit      bit,
                           bool*                     state);

/**
 * Set all bits in the bitmap and set the dirty bit.
 *
 * @param control The bitmap control.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_map_set_all (rtems_rfs_bitmap_control* control);

/**
 * Clear all bits in the bitmap and set the dirty bit.
 *
 * @param control The bitmap control.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_map_clear_all (rtems_rfs_bitmap_control* control);

/**
 * Find a free bit searching from the seed up and down until found. The search
 * is performing by moving up from the seed for the window distance then to
 * search down from the seed for the window distance. This is repeated out from
 * the seed for each window until a free bit is found. The search is performed
 * by checking the search map to see if the map has a free bit.
 *
 * @param control The map control.
 * @param seed The bit to search out from.
 * @param allocate A bit was allocated.
 * @param bit Returns the bit found free if true is returned.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_map_alloc (rtems_rfs_bitmap_control* control,
                                rtems_rfs_bitmap_bit      seed,
                                bool*                     allocate,
                                rtems_rfs_bitmap_bit*     bit);

/**
 * Create a search bit map from the actual bit map.
 *
 * @param control The map control.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_create_search (rtems_rfs_bitmap_control* control);

/**
 * Open a bitmap control with a map and search map.
 *
 * @param control The map control.
 * @param fs The file system data.
 * @param  buffer The buffer handle the map is stored in.
 * @param size The number of bits in the map.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_open (rtems_rfs_bitmap_control* control,
                           rtems_rfs_file_system*    fs,
                           rtems_rfs_buffer_handle*  buffer,
                           size_t                    size,
                           rtems_rfs_buffer_block    block);

/**
 * Close a bitmap.
 *
 * @param control The bit map control.
 * @return int The error number (errno). No error if 0.
 */
int rtems_rfs_bitmap_close (rtems_rfs_bitmap_control* control);

#endif
