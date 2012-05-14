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

#if HAVE_CONFIG_H
#include "config.h"
#endif

/**
 * Set to 1 to enable warnings when developing.
 */
#define RTEMS_RFS_BITMAP_WARNINGS 0

#if RTEMS_RFS_BITMAP_WARNINGS
#include <stdio.h>
#endif
#include <stdlib.h>
#include <rtems/rfs/rtems-rfs-bitmaps.h>

/**
 * Test a bit in an element. If set return true else return false.
 *
 * @param target The target to test the bit in.
 * @param bit The bit to test.
 * @retval true The bit is set.
 * @retval false The bit is clear.
 */
static bool
rtems_rfs_bitmap_test (rtems_rfs_bitmap_element target,
                       rtems_rfs_bitmap_bit     bit)
{
  return RTEMS_RFS_BITMAP_TEST_BIT (target, bit);
}

/**
 * Set the bits in the element. Bits not set in the bit argument are left
 * unchanged.
 *
 * @param target The target element bits are set.
 * @param bits The bits in the target to set. A 1 in the bits will set the
 *             same bit in the target.
 */
static rtems_rfs_bitmap_element
rtems_rfs_bitmap_set (rtems_rfs_bitmap_element target,
                      rtems_rfs_bitmap_element bits)
{
  return RTEMS_RFS_BITMAP_SET_BITS (target, bits);
}

/**
 * Clear the bits in the element. Bits not set in the bit argument are left
 * unchanged.
 *
 * @param target The target element to clear the bits in.
 * @param bits The bits in the target to clear. A 1 in the bits will clear the
 *             bit in the target.
 */
static rtems_rfs_bitmap_element
rtems_rfs_bitmap_clear (rtems_rfs_bitmap_element target,
                        rtems_rfs_bitmap_element bits)
{
  return RTEMS_RFS_BITMAP_CLEAR_BITS (target, bits);
}

/**
 * Merge the bits in 2 variables based on the mask. A set bit in the mask will
 * merge the bits from bits1 and a clear bit will merge the bits from bits2.
 * The mask is always defined as 1 being set and 0 being clear.
 */
static rtems_rfs_bitmap_element
rtems_rfs_bitmap_merge (rtems_rfs_bitmap_element bits1,
                        rtems_rfs_bitmap_element bits2,
                        rtems_rfs_bitmap_element mask)
{
  /*
   * Use the normal bit operators because we do not change the bits just merge
   * the 2 separate parts.
   */
  bits1 &= mask;
  bits2 &= RTEMS_RFS_BITMAP_INVERT_MASK (mask);
  return bits1 | bits2;
}

/**
 * Match the bits of 2 elements and return true if they match else return
 * false.
 *
 * @param bits1 One set of bits to match.
 * @param bits2 The second set of bits to match.
 * @retval true The bits match.
 * @retval false The bits do not match.
 */
static bool
rtems_rfs_bitmap_match (rtems_rfs_bitmap_element bits1,
                        rtems_rfs_bitmap_element bits2)
{
  return bits1 ^ bits2 ? false : true;
}

#if RTEMS_NOT_USED_BUT_KEPT
/**
 * Match the bits of 2 elements within the mask and return true if they match
 * else return false.
 *
 * @param mask The mask over which the match occurs. A 1 is a valid mask bit.
 * @param bits1 One set of bits to match.
 * @param bits2 The second set of bits to match.
 * @retval true The bits match.
 * @retval false The bits do not match.
 */
static bool
rtems_rfs_bitmap_match_masked (rtems_rfs_bitmap_element mask,
                               rtems_rfs_bitmap_element bits1,
                               rtems_rfs_bitmap_element bits2)
{
  return (bits1 ^ bits2) & mask ? false : true;
}
#endif

/**
 * Return the map after loading from disk if not already loaded.
 *
 * @param control The bitmap control.
 * @param rtems_rfs_bitmap_map* Pointer to the bitmap map data if no error.
 * @return int The error number (errno). No error if 0.
 */
static int
rtems_rfs_bitmap_load_map (rtems_rfs_bitmap_control* control,
                           rtems_rfs_bitmap_map*     map)
{
  int rc;

  if (!control->buffer)
    return ENXIO;

  *map = NULL;

  rc = rtems_rfs_buffer_handle_request (control->fs,
                                        control->buffer,
                                        control->block,
                                        true);
  if (rc)
    return rc;

  *map = rtems_rfs_buffer_data (control->buffer);
  return 0;
}

rtems_rfs_bitmap_element
rtems_rfs_bitmap_mask (unsigned int size)
{
  rtems_rfs_bitmap_element mask = RTEMS_RFS_BITMAP_ELEMENT_FULL_MASK;
  mask >>= (rtems_rfs_bitmap_element_bits () - size);
  return mask;
}

rtems_rfs_bitmap_element
rtems_rfs_bitmap_mask_section (unsigned int start, unsigned int end)
{
  rtems_rfs_bitmap_element mask = 0;
  if (end > start)
    mask = rtems_rfs_bitmap_mask (end - start) << start;
  return mask;
}

int
rtems_rfs_bitmap_map_set (rtems_rfs_bitmap_control* control,
                          rtems_rfs_bitmap_bit      bit)
{
  rtems_rfs_bitmap_map map;
  rtems_rfs_bitmap_map search_map;
  int                  index;
  int                  offset;
  int                 rc;
  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;
  if (bit >= control->size)
    return EINVAL;
  search_map = control->search_bits;
  index      = rtems_rfs_bitmap_map_index (bit);
  offset     = rtems_rfs_bitmap_map_offset (bit);
  map[index] = rtems_rfs_bitmap_set (map[index], 1 << offset);
  if (rtems_rfs_bitmap_match(map[index], RTEMS_RFS_BITMAP_ELEMENT_SET))
  {
    bit = index;
    index  = rtems_rfs_bitmap_map_index (bit);
    offset = rtems_rfs_bitmap_map_offset (bit);
    search_map[index] = rtems_rfs_bitmap_set (search_map[index], 1 << offset);
    control->free--;
    rtems_rfs_buffer_mark_dirty (control->buffer);
  }
  return 0;
}

int
rtems_rfs_bitmap_map_clear (rtems_rfs_bitmap_control* control,
                            rtems_rfs_bitmap_bit      bit)
{
  rtems_rfs_bitmap_map map;
  rtems_rfs_bitmap_map search_map;
  int                  index;
  int                  offset;
  int                  rc;
  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;
  if (bit >= control->size)
    return EINVAL;
  search_map        = control->search_bits;
  index             = rtems_rfs_bitmap_map_index (bit);
  offset            = rtems_rfs_bitmap_map_offset (bit);
  map[index]        = rtems_rfs_bitmap_clear (map[index], 1 << offset);
  bit               = index;
  index             = rtems_rfs_bitmap_map_index (bit);
  offset            = rtems_rfs_bitmap_map_offset(bit);
  search_map[index] = rtems_rfs_bitmap_clear (search_map[index], 1 << offset);
  rtems_rfs_buffer_mark_dirty (control->buffer);
  control->free++;
  return 0;
}

int
rtems_rfs_bitmap_map_test (rtems_rfs_bitmap_control* control,
                           rtems_rfs_bitmap_bit      bit,
                           bool*                     state)
{
  rtems_rfs_bitmap_map map;
  int                  index;
  int                  rc;
  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;
  if (bit >= control->size)
    return EINVAL;
  index = rtems_rfs_bitmap_map_index (bit);
  *state = rtems_rfs_bitmap_test (map[index], bit);
  return 0;
}

int
rtems_rfs_bitmap_map_set_all (rtems_rfs_bitmap_control* control)
{
  rtems_rfs_bitmap_map map;
  size_t               elements;
  int                  e;
  int                  rc;

  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;

  elements = rtems_rfs_bitmap_elements (control->size);

  control->free = 0;

  for (e = 0; e < elements; e++)
    map[e] = RTEMS_RFS_BITMAP_ELEMENT_SET;

  elements = rtems_rfs_bitmap_elements (elements);

  for (e = 0; e < elements; e++)
    control->search_bits[e] = RTEMS_RFS_BITMAP_ELEMENT_SET;

  rtems_rfs_buffer_mark_dirty (control->buffer);

  return 0;
}

int
rtems_rfs_bitmap_map_clear_all (rtems_rfs_bitmap_control* control)
{
  rtems_rfs_bitmap_map map;
  rtems_rfs_bitmap_bit last_search_bit;
  size_t               elements;
  int                  e;
  int                  rc;

  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;

  elements = rtems_rfs_bitmap_elements (control->size);

  control->free = elements;

  for (e = 0; e < elements; e++)
    map[e] = RTEMS_RFS_BITMAP_ELEMENT_CLEAR;

  /*
   * Set the un-mapped bits in the last search element so the available logic
   * works.
   */
  last_search_bit = rtems_rfs_bitmap_map_offset (elements);

  if (last_search_bit == 0)
    last_search_bit = rtems_rfs_bitmap_element_bits ();

  elements = rtems_rfs_bitmap_elements (elements);

  for (e = 0; e < (elements - 1); e++)
    control->search_bits[e] = RTEMS_RFS_BITMAP_ELEMENT_CLEAR;

  control->search_bits[elements - 1] =
    rtems_rfs_bitmap_merge (RTEMS_RFS_BITMAP_ELEMENT_CLEAR,
                            RTEMS_RFS_BITMAP_ELEMENT_SET,
                            rtems_rfs_bitmap_mask (last_search_bit));

  rtems_rfs_buffer_mark_dirty (control->buffer);

  return 0;
}

static int
rtems_rfs_search_map_for_clear_bit (rtems_rfs_bitmap_control* control,
                                    rtems_rfs_bitmap_bit*     bit,
                                    bool*                     found,
                                    size_t                    window,
                                    int                       direction)
{
  rtems_rfs_bitmap_map      map;
  rtems_rfs_bitmap_bit      test_bit;
  rtems_rfs_bitmap_bit      end_bit;
  rtems_rfs_bitmap_element* search_bits;
  int                       search_index;
  int                       search_offset;
  rtems_rfs_bitmap_element* map_bits;
  int                       map_index;
  int                       map_offset;
  int                       rc;

  *found = false;

  /*
   * Load the bitmap.
   */
  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;

  /*
   * Calculate the bit we are testing plus the end point we search over.
   */
  test_bit = *bit;
  end_bit  = test_bit + (window * direction);

  if (end_bit < 0)
    end_bit = 0;
  else if (end_bit >= control->size)
    end_bit = control->size - 1;

  map_index     = rtems_rfs_bitmap_map_index (test_bit);
  map_offset    = rtems_rfs_bitmap_map_offset (test_bit);
  search_index  = rtems_rfs_bitmap_map_index (map_index);
  search_offset = rtems_rfs_bitmap_map_offset (map_index);

  search_bits = &control->search_bits[search_index];
  map_bits    = &map[map_index];

  /*
   * Check each bit from the search map offset for a clear bit.
   */
  do
  {
    /*
     * If any bit is clear find that bit and then search the map element. If
     * all bits are set there are no map bits so move to the next search
     * element.
     */
    if (!rtems_rfs_bitmap_match (*search_bits, RTEMS_RFS_BITMAP_ELEMENT_SET))
    {
      while ((search_offset >= 0)
             && (search_offset < rtems_rfs_bitmap_element_bits ()))
      {
        if (!rtems_rfs_bitmap_test (*search_bits, search_offset))
        {
          /*
           * Find the clear bit in the map. Update the search map and map if
           * found. We may find none are spare if searching up from the seed.
           */
          while ((map_offset >= 0)
                 && (map_offset < rtems_rfs_bitmap_element_bits ()))
          {
            if (!rtems_rfs_bitmap_test (*map_bits, map_offset))
            {
              *map_bits = rtems_rfs_bitmap_set (*map_bits, 1 << map_offset);
              if (rtems_rfs_bitmap_match(*map_bits,
                                         RTEMS_RFS_BITMAP_ELEMENT_SET))
                *search_bits = rtems_rfs_bitmap_set (*search_bits,
                                                     1 << search_offset);
              control->free--;
              *bit = test_bit;
              *found = true;
              rtems_rfs_buffer_mark_dirty (control->buffer);
              return 0;
            }

            if (test_bit == end_bit)
              break;

            map_offset += direction;
            test_bit   += direction;
          }
        }

        map_bits  += direction;
        map_index += direction;
        map_offset = direction > 0 ? 0 : rtems_rfs_bitmap_element_bits () - 1;

        test_bit = (map_index * rtems_rfs_bitmap_element_bits ()) + map_offset;

        search_offset += direction;

        if (((direction < 0) && (test_bit <= end_bit))
            || ((direction > 0) && (test_bit >= end_bit)))
          break;
      }
    }
    else
    {
      /*
       * Move to the next search element. We need to determine the number of
       * bits in the search offset that are being skipped so the map bits
       * pointer can be updated. If we are moving down and we have a search
       * offset of 0 then the search map adjustment is to the top bit of the
       * pervious search bit's value.
       *
       * Align test_bit either up or down depending on the direction to next 32
       * bit boundary.
       */
      rtems_rfs_bitmap_bit bits_skipped;
      test_bit &= ~((1 << RTEMS_RFS_ELEMENT_BITS_POWER_2) - 1);
      if (direction > 0)
      {
        bits_skipped = rtems_rfs_bitmap_element_bits () - search_offset;
        test_bit += bits_skipped * rtems_rfs_bitmap_element_bits ();
        map_offset = 0;
      }
      else
      {
        bits_skipped = search_offset + 1;
        /*
         * Need to remove 1 for the rounding up. The above rounds down and
         * adds 1. Remember the logic is for subtraction.
         */
        test_bit -= ((bits_skipped - 1) * rtems_rfs_bitmap_element_bits ()) + 1;
        map_offset = rtems_rfs_bitmap_element_bits () - 1;
      }
      map_bits += direction * bits_skipped;
      map_index += direction * bits_skipped;
    }

    search_bits  += direction;
    search_offset = direction > 0 ? 0 : rtems_rfs_bitmap_element_bits () - 1;
  }
  while (((direction < 0) && (test_bit >= end_bit))
         || ((direction > 0) && (test_bit <= end_bit)));

  return 0;
}

int
rtems_rfs_bitmap_map_alloc (rtems_rfs_bitmap_control* control,
                            rtems_rfs_bitmap_bit      seed,
                            bool*                     allocated,
                            rtems_rfs_bitmap_bit*     bit)
{
  rtems_rfs_bitmap_bit upper_seed;
  rtems_rfs_bitmap_bit lower_seed;
  rtems_rfs_bitmap_bit window;     /* may become a parameter */
  int                  rc = 0;

  /*
   * By default we assume the allocation failed.
   */
  *allocated = false;

  /*
   * The window is the number of bits we search over in either direction each
   * time.
   */
  window = RTEMS_RFS_BITMAP_SEARCH_WINDOW;

  /*
   * Start from the seed and move in either direction. Search in window amounts
   * of bits from the original seed above then below. That is search from the
   * seed up then from the seed down a window number of bits, then repeat the
   * process from the window distance from the seed, again above then
   * below. Keep moving out until all bits have been searched.
   */
  upper_seed = seed;
  lower_seed = seed;

  /*
   * If the upper and lower seed values have reached the limits of the bitmap
   * we have searched all of the map. The seed may not be aligned to a window
   * boundary so we may need to search a partial window and this may also not
   * be balanced for the upper or lower seeds. We move to the limits, search
   * then return false if no clear bits are found.
   */
  while (((upper_seed >= 0) && (upper_seed < control->size))
         || ((lower_seed >= 0) && (lower_seed < control->size)))
  {
    /*
     * Search up first so bits allocated in succession are grouped together.
     */
    if (upper_seed < control->size)
    {
      *bit = upper_seed;
      rc = rtems_rfs_search_map_for_clear_bit (control, bit, allocated,
                                               window, 1);
      if ((rc > 0) || *allocated)
        break;
    }

    if (lower_seed >= 0)
    {
      *bit = lower_seed;
      rc = rtems_rfs_search_map_for_clear_bit (control, bit, allocated,
                                               window, -1);
      if ((rc > 0) || *allocated)
        break;
    }

    /*
     * Do not bound the limits at the edges of the map. Do not update if an
     * edge has been passed.
     */
    if (upper_seed < control->size)
      upper_seed += window;
    if (lower_seed >= 0)
      lower_seed -= window;
  }

  return 0;
}

int
rtems_rfs_bitmap_create_search (rtems_rfs_bitmap_control* control)
{
  rtems_rfs_bitmap_map search_map;
  rtems_rfs_bitmap_map map;
  size_t               size;
  rtems_rfs_bitmap_bit bit;
  int                  rc;

  rc = rtems_rfs_bitmap_load_map (control, &map);
  if (rc > 0)
    return rc;

  control->free = 0;
  search_map = control->search_bits;
  size = control->size;
  bit = 0;

  *search_map = RTEMS_RFS_BITMAP_ELEMENT_CLEAR;
  while (size)
  {
    rtems_rfs_bitmap_element bits;
    int                      available;
    if (size < rtems_rfs_bitmap_element_bits ())
    {
      bits = rtems_rfs_bitmap_merge (*map,
                                     RTEMS_RFS_BITMAP_ELEMENT_SET,
                                     rtems_rfs_bitmap_mask_section (0, size));
      available = size;
    }
    else
    {
      bits      = *map;
      available = rtems_rfs_bitmap_element_bits ();
    }

    if (rtems_rfs_bitmap_match (bits, RTEMS_RFS_BITMAP_ELEMENT_SET))
      rtems_rfs_bitmap_set (*search_map, bit);
    else
    {
      int b;
      for (b = 0; b < available; b++)
        if (!rtems_rfs_bitmap_test (bits, b))
          control->free++;
    }

    size -= available;

    if (bit == rtems_rfs_bitmap_element_bits ())
    {
      bit = 0;
      search_map++;
      *search_map = RTEMS_RFS_BITMAP_ELEMENT_CLEAR;
    }
    else
      bit++;
    map++;
  }

  return 0;
}

int
rtems_rfs_bitmap_open (rtems_rfs_bitmap_control* control,
                       rtems_rfs_file_system*    fs,
                       rtems_rfs_buffer_handle*  buffer,
                       size_t                    size,
                       rtems_rfs_buffer_block    block)
{
  size_t elements = rtems_rfs_bitmap_elements (size);

  control->buffer = buffer;
  control->fs = fs;
  control->block = block;
  control->size = size;

  elements = rtems_rfs_bitmap_elements (elements);
  control->search_bits = malloc (elements * sizeof (rtems_rfs_bitmap_element));

  if (!control->search_bits)
    return ENOMEM;

  return rtems_rfs_bitmap_create_search (control);
}

int
rtems_rfs_bitmap_close (rtems_rfs_bitmap_control* control)
{
  free (control->search_bits);
  return 0;
}
