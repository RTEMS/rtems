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
 * RTEMS File Systems Bitmap Unit Test..
 *
 * This is a unit test module for the bit map functions.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdio.h>
#include <stdlib.h>
#include <inttypes.h>

#include <rtems/rfs/rtems-rfs-bitmaps.h>
#include <rtems/rfs/rtems-rfs-file-system.h>

#define rtems_rfs_exit_on_error(_rc, _r, _c, _b)                         \
  if ((_rc > 0) || _r) { free (_b); rtems_rfs_bitmap_close (_c); return; }

static bool
rtems_rfs_bitmap_ut_test_range (rtems_rfs_bitmap_control* control,
                                int                       test,
                                bool                      set,
                                rtems_rfs_bitmap_bit      bit,
                                size_t                    size)
{
  unsigned int count;
  bool         result;
  for (count = 0; count < size; count++)
  {
    int rc = rtems_rfs_bitmap_map_test (control, bit + count, &result);
    if (rc > 0)
    {
      printf (" %2d. Test bit %" PRId32 " in range (%" PRId32 ",%ld] is %s: ",
            test, bit + count, bit, bit + size - 1, !set ? "set" : "clear");
      printf ("FAIL (%s)\n", strerror (rc));
      return false;
    }
    if (!set)
      result = !result;
    if (!result)
    {
      printf (" %2d. Test bit %" PRId32 " in range (%" PRId32 ",%ld] is %s: ",
              test, bit + count, bit, bit + size - 1, !set ? "set" : "clear");
      printf (" %s\n", !result ? "pass" : "FAIL");
      return false;
    }
  }

  printf (" %2d. Test bit range (%" PRId32 ",%ld] all %s: pass\n",
          test, bit, bit + size - 1, set ? "set" : "clear");

  return true;
}

static bool
rtems_rfs_bitmap_ut_alloc_seq_test (rtems_rfs_bitmap_control* control,
                                    int                       test,
                                    rtems_rfs_bitmap_bit      bit,
                                    size_t                    size)
{
  bool state;
  int  i;
  int  rc;

  printf (" %2d. Set all bits\n", test);
  rc = rtems_rfs_bitmap_map_set_all (control);
  if (rc > 0)
  {
    printf (" %2d. set all bits: FAIL (%s)\n", test, strerror (rc));
    return false;
  }

  for (i = 0; i < size; i++)
    rtems_rfs_bitmap_map_clear (control, bit + i);

  printf (" %2d. Cleared bits (%" PRId32 ", %ld] (%zd)\n",
          test, bit, bit + size - 1, size);

  for (i = 0; i < rtems_rfs_bitmap_element_bits (); i++)
  {
    rc = rtems_rfs_bitmap_map_test (control, bit + i, &state);
    if (rc > 0)
    {
      printf (" %2d. test bit: FAIL (%s)\n", test, strerror (rc));
      return false;
    }
    if (state)
    {
      printf (" %2d. Cleared bit still set: bit = %" PRId32 "\n", test, bit + i);
      return false;
    }
  }

  for (i = 0, bit = 0; i < size; i++)
  {
    rtems_rfs_bitmap_bit seed = bit;
    bool                 result;
    int                  rc;
    rc = rtems_rfs_bitmap_map_alloc (control, seed, &result, &bit);
    if (rc > 0)
    {
      printf (" %2d. map all: FAIL (%s)\n", test, strerror (rc));
      return false;
    }
    if (!result)
    {
      printf (" %2d. Find bit with seed = %" PRId32 ": %s: bit = %" PRId32 "\n",
              test, seed, result ? "pass" : "FAIL", bit);
      return false;
    }
  }

  printf (" %2d. Alloc'ed all bits (%" PRId32 ", %ld] (%zd)\n",
          test, bit, bit + size - 1, size);

  return true;
}

static void
rtems_rfs_bitmap_ut_test_bitmap (size_t size)
{
  rtems_rfs_file_system    fs;
  rtems_rfs_bitmap_control control;
  rtems_rfs_buffer_handle  handle;
  rtems_rfs_buffer         buffer;
  rtems_rfs_bitmap_bit     bit = 0;
  rtems_rfs_bitmap_bit     first_bit;
  rtems_rfs_bitmap_bit     last_bit;
  bool                     result;
  size_t                   bytes;
  size_t                   clear;
  int                      rc;

  bytes = (rtems_rfs_bitmap_elements (size) *
           sizeof (rtems_rfs_bitmap_element));

  memset (&fs, 0, sizeof (fs));
  memset (&buffer, 0, sizeof (buffer));

  buffer.buffer = malloc (bytes);
  buffer.block = 1;

  if (!buffer.buffer)
  {
    printf (" Cannot allocate bitmap memory\n");
    return;
  }

#if RTEMS_RFS_BITMAP_CLEAR_ZERO
  memset (buffer.buffer, 0, bytes);
#else
  memset (buffer.buffer, 0xff, bytes);
#endif

  /*
   * Do not close the handle so no writes need occur.
   */
  rc = rtems_rfs_buffer_handle_open (&fs, &handle);
  if (rc > 0)
  {
    printf (" Cannot open the handle: %d: %s\n", rc, strerror (rc));
    free (buffer.buffer);
    return;
  }

  handle.buffer = &buffer;
  handle.bnum = 1;

  printf ("\nRFS Bitmap Test : size = %zd (%zd)\n",
          size, rtems_rfs_bitmap_elements (size));
  rc = rtems_rfs_bitmap_open (&control, &fs, &handle, size, 1);
  if (rc > 0)
  {
    printf (" Cannot open the bitmap: %s\n", strerror (rc));
    free (buffer.buffer);
    return;
  }

  /*
   * This is a new bitmap with no bits set. Try and find a bit with a few
   * seeds.
   */
  rc = rtems_rfs_bitmap_map_alloc (&control, size * 2, &result, &bit);
  printf ("  1. Find bit with seed > size: %s (%s)\n",
          result ? "FAIL" : "pass", strerror (rc));
  rtems_rfs_exit_on_error (rc, result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, size, &result, &bit);
  printf ("  2. Find bit with seed = size: %s (%s)\n",
          result ? "FAIL" : "pass", strerror (rc));
  rtems_rfs_exit_on_error (rc, result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, 0, &result, &bit);
  result = result && (bit == 0);
  printf ("  3. Find bit 0 with seed = 0: %s (%s): bit = %" PRId32 "\n",
          result ? "pass" : "FAIL", strerror (rc),  bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, size - 1, &result, &bit);
  result = result && (bit == (size - 1));
  printf ("  4. Find bit (size - 1) with seed = (size - 1) (%zd): %s (%s): bit = %" PRId32 "\n",
          size - 1, result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);


  /*
   * Test the bits allocated to make sure they are set.
   */

  rc = rtems_rfs_bitmap_map_test (&control, 0, &result);
  printf ("  5. Test bit 0: %s (%s)\n",
          result ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_test (&control, size - 1, &result);
  printf ("  6. Test bit (size - 1) (%zd): %s (%s)\n",
          size - 1, result ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  if (!rtems_rfs_bitmap_ut_test_range (&control, 7, false, 1, size - 2))
    rtems_rfs_exit_on_error (0, !result, &control, buffer.buffer);

  /*
   * Set all bits then clear one and find it.
   */
  rc = rtems_rfs_bitmap_map_set_all (&control);
  printf ("  8. Set all bits: %s (%s)\n",
          rc == 0 ? "PASS" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  bit = rand () % size;

  rc = rtems_rfs_bitmap_map_clear (&control, bit);
  printf ("  9. Clear bit %" PRId32 ": %s (%s)\n",
          bit, rc == 0 ? "PASS" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  last_bit = bit;
  rc = rtems_rfs_bitmap_map_alloc (&control, 0, &result, &bit);
  result = result && (bit == last_bit);
  printf (" 10. Find bit with seed = 0: %s (%s): bit = %" PRId32 "\n",
          result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, 0, &result, &bit);
  result = !result || (bit != last_bit);
  printf (" 11. Fail to find bit with seed = 0: %s (%s): bit = %" PRId32 "\n",
          result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, 0);
  printf (" 12. Clear bit 0: %s (%s)\n",
          rc == 0 ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, size - 1, &result, &bit);
  result = result && (bit == 0);
  printf (" 13. Find bit with seed = (size - 1): %s (%s): bit = %" PRId32 "\n",
          result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, size - 1);
  printf (" 14. Clear bit (size - 1) (%zd): %s (%s)\n",
          size - 1, rc == 0 ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, 0, &result, &bit);
  result = result && (bit == (size - 1));
  printf (" 15. Find bit with seed = 0: %s (%s): bit = %" PRId32 "\n",
          result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, 0);
  printf (" 16. Clear bit 0: %s (%s)\n",
          rc == 0 ? "pass" : "FAIL", strerror (rc));

  rc = rtems_rfs_bitmap_map_alloc (&control, size / 2, &result, &bit);
  result = result && (bit == 0);
  printf (" 17. Find bit with seed = (size / 2) (%zd): %s (%s): bit = %" PRId32 "\n",
          size / 2, result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, size - 1);
  printf (" 18. Clear bit (size - 1) (%zd): %s, (%s)\n",
          size - 1, rc == 0 ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_alloc (&control, size / 2, &result, &bit);
  result = result && (bit == (size - 1));
  printf (" 19. Find bit with seed = (size / 2) (%zd): %s (%s): bit = %" PRId32 "\n",
          size / 2, result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, 0);
  printf (" 20. Clear bit 0: %s (%s)\n",
          rc == 0 ? "pass" : "FAIL", strerror (rc));

  rc = rtems_rfs_bitmap_map_alloc (&control, (size / 2) - 1, &result, &bit);
  result = result && (bit == 0);
  printf (" 21. Find bit with seed = ((size / 2) - 1) (%zd): %s (%s): bit = %" PRId32 "\n",
          (size / 2) - 1, result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rc = rtems_rfs_bitmap_map_clear (&control, size - 1);
  printf (" 22. Clear bit (size - 1) (%zd): %s (%s)\n",
          size - 1, rc == 0 ? "pass" : "FAIL", strerror (rc));

  rc = rtems_rfs_bitmap_map_alloc (&control, (size / 2) - 1, &result, &bit);
  result = result && (bit == (size - 1));
  printf (" 23. Find bit with seed = ((size / 2) - 1) (%zd): %s (%s): bit = %" PRId32 "\n",
          (size / 2) - 1, result ? "pass" : "FAIL", strerror (rc), bit);
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  bit = rand () % (size / 2) + rtems_rfs_bitmap_element_bits ();
  result = rtems_rfs_bitmap_ut_alloc_seq_test (&control, 23, bit,
                                               rtems_rfs_bitmap_element_bits ());
  rtems_rfs_exit_on_error (0, !result, &control, buffer.buffer);

  bit = rand () % (size / 2) + rtems_rfs_bitmap_element_bits ();
  result = rtems_rfs_bitmap_ut_alloc_seq_test (&control, 24, bit, 57);
  rtems_rfs_exit_on_error (0, !result, &control, buffer.buffer);

  /*
   * Set all bits, clear a random numberone then create a search map and make
   * sure the clear count is correct.
   */
  rc = rtems_rfs_bitmap_map_set_all (&control);
  printf (" 25. Set all bits: %s (%s)\n",
          rc == 0 ? "PASS" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);

  first_bit = rand () % (size / 2) + rtems_rfs_bitmap_element_bits ();
  last_bit = first_bit + rand () % (size / 2) + rtems_rfs_bitmap_element_bits ();

  for (bit = first_bit; bit < last_bit; bit++)
  {
    rc = rtems_rfs_bitmap_map_clear (&control, bit);
    if (rc > 0)
    {
      printf (" 26. Clear bit %" PRId32 ": %s (%s)\n",
              bit, rc == 0 ? "PASS" : "FAIL", strerror (rc));
      rtems_rfs_exit_on_error (rc, false, &control, buffer.buffer);
    }
  }

  printf (" 26. Clear bit (%" PRId32 ", %" PRId32 "]: %s (%s)\n",
          first_bit, last_bit, rc == 0 ? "PASS" : "FAIL", strerror (rc));

  clear = rtems_rfs_bitmap_map_free (&control);
  result = clear == (last_bit - first_bit);
  printf (" 27. Check free count is %zd: %" PRId32 ": %s (%s)\n",
          clear, last_bit - first_bit,
          result ? "pass" : "FAIL", strerror (rc));

  rc = rtems_rfs_bitmap_create_search (&control);
  result = clear == rtems_rfs_bitmap_map_free (&control);
  printf (" 28. Create search check free count is %zd: %zd: %s (%s)\n",
          clear, rtems_rfs_bitmap_map_free (&control),
          result ? "pass" : "FAIL", strerror (rc));
  rtems_rfs_exit_on_error (rc, !result, &control, buffer.buffer);

  rtems_rfs_bitmap_close (&control);
  free (buffer.buffer);
}

void
rtems_rfs_bitmap_unit_test (void)
{
  printf ("RTEMS File System Bitmap Unit Test\n");
  printf (" Bit set value       : %d\n", RTEMS_RFS_BITMAP_BIT_SET);
  printf (" Bit clear value     : %d\n", RTEMS_RFS_BITMAP_BIT_CLEAR);
  printf (" Num bit per element : %zd\n", rtems_rfs_bitmap_element_bits ());

#if INT_MAX >= 0x23984237
  srand (0x23984237);
#else
  srand (0x2398);
#endif

  rtems_rfs_bitmap_ut_test_bitmap (2048);
  rtems_rfs_bitmap_ut_test_bitmap (420);
}

