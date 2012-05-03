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
 * RTEMS File Systems Directory Routines.
 *
 * These functions manage blocks in the directory format. A directory entry is
 * a variable length record in the block. The entry consists of a length, hash
 * and the string. The length allows the next entry to be located and the hash
 * allows a simple check to be performed wihtout a string compare. Directory
 * entries do not span a block and removal of an entry results in the space in
 * the block being compacted and the spare area being initialised to ones.
 *
 * The maximum length can be 1 or 2 bytes depending on the value in the
 * superblock.
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <inttypes.h>

#if SIZEOF_OFF_T == 8
#define PRIooff_t PRIo64
#elif SIZEOF_OFF_T == 4
#define PRIooff_t PRIo32
#else
#error "unsupported size of off_t"
#endif

#include <rtems/rfs/rtems-rfs-block.h>
#include <rtems/rfs/rtems-rfs-buffer.h>
#include <rtems/rfs/rtems-rfs-file-system.h>
#include <rtems/rfs/rtems-rfs-trace.h>
#include <rtems/rfs/rtems-rfs-dir.h>
#include <rtems/rfs/rtems-rfs-dir-hash.h>

/**
 * Validate the directory entry data.
 */
#define rtems_rfs_dir_entry_valid(_f, _l, _i) \
  (((_l) <= RTEMS_RFS_DIR_ENTRY_SIZE) || ((_l) >= rtems_rfs_fs_max_name (_f)) \
   || (_i < RTEMS_RFS_ROOT_INO) || (_i > rtems_rfs_fs_inodes (_f)))

int
rtems_rfs_dir_lookup_ino (rtems_rfs_file_system*  fs,
                          rtems_rfs_inode_handle* inode,
                          const char*             name,
                          int                     length,
                          rtems_rfs_ino*          ino,
                          uint32_t*               offset)
{
  rtems_rfs_block_map     map;
  rtems_rfs_buffer_handle entries;
  int                     rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
  {
    int c;
    printf ("rtems-rfs: dir-lookup-ino: lookup ino: root=%" PRId32 ", path=",
            inode->ino);
    for (c = 0; c < length; c++)
      printf ("%c", name[c]);
    printf (", len=%d\n", length);
  }

  *ino = RTEMS_RFS_EMPTY_INO;
  *offset = 0;

  rc = rtems_rfs_block_map_open (fs, inode, &map);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
      printf ("rtems-rfs: dir-lookup-ino: map open failed for ino %" PRIu32 ": %d: %s",
              rtems_rfs_inode_ino (inode), rc, strerror (rc));
    return rc;
  }

  rc = rtems_rfs_buffer_handle_open (fs, &entries);
  if (rc > 0)
  {
    if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
      printf ("rtems-rfs: dir-lookup-ino: handle open failed for ino %" PRIu32 ": %d: %s",
              rtems_rfs_inode_ino (inode), rc, strerror (rc));
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }
  else
  {
    rtems_rfs_block_no block;
    uint32_t           hash;

    /*
     * Calculate the hash of the look up string.
     */
    hash = rtems_rfs_dir_hash (name, length);

    /*
     * Locate the first block. The map points to the start after open so just
     * seek 0. If an error the block will be 0.
     */
    rc = rtems_rfs_block_map_seek (fs, &map, 0, &block);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
        printf ("rtems-rfs: dir-lookup-ino: block map find failed: %d: %s\n",
                rc, strerror (rc));
      if (rc == ENXIO)
        rc = ENOENT;
      rtems_rfs_buffer_handle_close (fs, &entries);
      rtems_rfs_block_map_close (fs, &map);
      return rc;
    }

    while ((rc == 0) && block)
    {
      uint8_t* entry;

      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
        printf ("rtems-rfs: dir-lookup-ino: block read, ino=%" PRIu32 " bno=%" PRId32 "\n",
                rtems_rfs_inode_ino (inode), map.bpos.bno);

      rc = rtems_rfs_buffer_handle_request (fs, &entries, block, true);
      if (rc > 0)
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
          printf ("rtems-rfs: dir-lookup-ino: block read, ino=%" PRIu32 " block=%" PRId32 ": %d: %s\n",
                  rtems_rfs_inode_ino (inode), block, rc, strerror (rc));
        break;
      }

      /*
       * Search the block to see if the name matches. A hash of 0xffff or 0x0
       * means the entry is empty.
       */

      entry = rtems_rfs_buffer_data (&entries);

      map.bpos.boff = 0;

      while (map.bpos.boff < (rtems_rfs_fs_block_size (fs) - RTEMS_RFS_DIR_ENTRY_SIZE))
      {
        uint32_t ehash;
        int      elength;

        ehash  = rtems_rfs_dir_entry_hash (entry);
        elength = rtems_rfs_dir_entry_length (entry);
        *ino = rtems_rfs_dir_entry_ino (entry);

        if (elength == RTEMS_RFS_DIR_ENTRY_EMPTY)
          break;

        if (rtems_rfs_dir_entry_valid (fs, elength, *ino))
        {
          if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
            printf ("rtems-rfs: dir-lookup-ino: "
                    "bad length or ino for ino %" PRIu32 ": %u/%" PRId32 " @ %04" PRIx32 "\n",
                    rtems_rfs_inode_ino (inode), elength, *ino, map.bpos.boff);
          rc = EIO;
          break;
        }

        if (ehash == hash)
        {
          if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO_CHECK))
            printf ("rtems-rfs: dir-lookup-ino: "
                    "checking entry for ino %" PRId32 ": bno=%04" PRIx32 "/off=%04" PRIx32
                    " length:%d ino:%" PRId32 "\n",
                    rtems_rfs_inode_ino (inode), map.bpos.bno, map.bpos.boff,
                    elength, rtems_rfs_dir_entry_ino (entry));

          if (memcmp (entry + RTEMS_RFS_DIR_ENTRY_SIZE, name, length) == 0)
          {
            *offset = rtems_rfs_block_map_pos (fs, &map);

            if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO_FOUND))
              printf ("rtems-rfs: dir-lookup-ino: "
                      "entry found in ino %" PRIu32 ", ino=%" PRIu32 " offset=%" PRIu32 "\n",
                      rtems_rfs_inode_ino (inode), *ino, *offset);

            rtems_rfs_buffer_handle_close (fs, &entries);
            rtems_rfs_block_map_close (fs, &map);
            return 0;
          }
        }

        map.bpos.boff += elength;
        entry += elength;
      }

      if (rc == 0)
      {
        rc = rtems_rfs_block_map_next_block (fs, &map, &block);
        if ((rc > 0) && (rc != ENXIO))
        {
          if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
            printf ("rtems-rfs: dir-lookup-ino: "
                    "block map next block failed in ino %" PRIu32 ": %d: %s\n",
                    rtems_rfs_inode_ino (inode), rc, strerror (rc));
        }
        if (rc == ENXIO)
          rc = ENOENT;
      }
    }

    if ((rc == 0) && (block == 0))
    {
      rc = EIO;
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_LOOKUP_INO))
        printf ("rtems-rfs: dir-lookup-ino: block is 0 in ino %" PRIu32 ": %d: %s\n",
                rtems_rfs_inode_ino (inode), rc, strerror (rc));
    }
  }

  rtems_rfs_buffer_handle_close (fs, &entries);
  rtems_rfs_block_map_close (fs, &map);
  return rc;
}

int
rtems_rfs_dir_add_entry (rtems_rfs_file_system*  fs,
                         rtems_rfs_inode_handle* dir,
                         const char*             name,
                         size_t                  length,
                         rtems_rfs_ino           ino)
{
  rtems_rfs_block_map     map;
  rtems_rfs_block_pos     bpos;
  rtems_rfs_buffer_handle buffer;
  int                     rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_ADD_ENTRY))
  {
    int c;
    printf ("rtems-rfs: dir-add-entry: dir=%" PRId32 ", name=",
            rtems_rfs_inode_ino (dir));
    for (c = 0; c < length; c++)
      printf ("%c", name[c]);
    printf (", len=%zd\n", length);
  }

  rc = rtems_rfs_block_map_open (fs, dir, &map);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  /*
   * Search the map from the beginning to find any empty space.
   */
  rtems_rfs_block_set_bpos_zero (&bpos);

  while (true)
  {
    rtems_rfs_block_no block;
    uint8_t*           entry;
    int                offset;
    bool               read = true;

    /*
     * Locate the first block. If an error the block will be 0. If the map is
     * empty which happens when creating a directory and adding the first entry
     * the seek will return ENXIO. In this case we need to grow the directory.
     */
    rc = rtems_rfs_block_map_find (fs, &map, &bpos, &block);
    if (rc > 0)
    {
      if (rc != ENXIO)
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_ADD_ENTRY))
          printf ("rtems-rfs: dir-add-entry: "
                  "block map find failed for ino %" PRIu32 ": %d: %s\n",
                  rtems_rfs_inode_ino (dir), rc, strerror (rc));
        break;
      }

      /*
       * We have reached the end of the directory so add a block.
       */
      rc = rtems_rfs_block_map_grow (fs, &map, 1, &block);
      if (rc > 0)
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_ADD_ENTRY))
          printf ("rtems-rfs: dir-add-entry: "
                  "block map grow failed for ino %" PRIu32 ": %d: %s\n",
                  rtems_rfs_inode_ino (dir), rc, strerror (rc));
        break;
      }

      read = false;
    }

    bpos.bno++;

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, read);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_ADD_ENTRY))
        printf ("rtems-rfs: dir-add-entry: "
                "block buffer req failed for ino %" PRIu32 ": %d: %s\n",
                rtems_rfs_inode_ino (dir), rc, strerror (rc));
      break;
    }

    entry  = rtems_rfs_buffer_data (&buffer);

    if (!read)
      memset (entry, 0xff, rtems_rfs_fs_block_size (fs));

    offset = 0;

    while (offset < (rtems_rfs_fs_block_size (fs) - RTEMS_RFS_DIR_ENTRY_SIZE))
    {
      rtems_rfs_ino eino;
      int           elength;

      elength = rtems_rfs_dir_entry_length (entry);
      eino    = rtems_rfs_dir_entry_ino (entry);

      if (elength == RTEMS_RFS_DIR_ENTRY_EMPTY)
      {
        if ((length + RTEMS_RFS_DIR_ENTRY_SIZE) <
            (rtems_rfs_fs_block_size (fs) - offset))
        {
          uint32_t hash;
          hash = rtems_rfs_dir_hash (name, length);
          rtems_rfs_dir_set_entry_hash (entry, hash);
          rtems_rfs_dir_set_entry_ino (entry, ino);
          rtems_rfs_dir_set_entry_length (entry,
                                          RTEMS_RFS_DIR_ENTRY_SIZE + length);
          memcpy (entry + RTEMS_RFS_DIR_ENTRY_SIZE, name, length);
          rtems_rfs_buffer_mark_dirty (&buffer);
          rtems_rfs_buffer_handle_close (fs, &buffer);
          rtems_rfs_block_map_close (fs, &map);
          return 0;
        }

        break;
      }

      if (rtems_rfs_dir_entry_valid (fs, elength, eino))
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_ADD_ENTRY))
          printf ("rtems-rfs: dir-add-entry: "
                  "bad length or ino for ino %" PRIu32 ": %u/%" PRId32 " @ %04x\n",
                  rtems_rfs_inode_ino (dir), elength, eino, offset);
        rtems_rfs_buffer_handle_close (fs, &buffer);
        rtems_rfs_block_map_close (fs, &map);
        return EIO;
      }

      entry  += elength;
      offset += elength;
    }
  }

  rtems_rfs_buffer_handle_close (fs, &buffer);
  rtems_rfs_block_map_close (fs, &map);
  return rc;
}

int
rtems_rfs_dir_del_entry (rtems_rfs_file_system*  fs,
                         rtems_rfs_inode_handle* dir,
                         rtems_rfs_ino           ino,
                         uint32_t                offset)
{
  rtems_rfs_block_map     map;
  rtems_rfs_block_no      block;
  rtems_rfs_buffer_handle buffer;
  bool                    search;
  int                     rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_DEL_ENTRY))
    printf ("rtems-rfs: dir-del-entry: dir=%" PRId32 ", entry=%" PRId32 " offset=%" PRIu32 "\n",
            rtems_rfs_inode_ino (dir), ino, offset);

  rc = rtems_rfs_block_map_open (fs, dir, &map);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_block_map_seek (fs, &map, offset, &block);
  if (rc > 0)
  {
    if (rc == ENXIO)
      rc = ENOENT;
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  /*
   * Only search if the offset is 0 else we are at that position.
   */
  search = offset ? false : true;

  while (rc == 0)
  {
    uint8_t* entry;
    int      eoffset;

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, true);
    if (rc > 0)
    {
      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_DEL_ENTRY))
        printf ("rtems-rfs: dir-del-entry: "
                "block buffer req failed for ino %" PRIu32 ": %d: %s\n",
                rtems_rfs_inode_ino (dir), rc, strerror (rc));
      break;
    }

    /*
     * If we are searching start at the beginning of the block. If not searching
     * skip to the offset in the block.
     */
    if (search)
      eoffset = 0;
    else
      eoffset = offset % rtems_rfs_fs_block_size (fs);

    entry = rtems_rfs_buffer_data (&buffer) + eoffset;

    while (eoffset < (rtems_rfs_fs_block_size (fs) - RTEMS_RFS_DIR_ENTRY_SIZE))
    {
      rtems_rfs_ino eino;
      int           elength;

      elength = rtems_rfs_dir_entry_length (entry);
      eino    = rtems_rfs_dir_entry_ino (entry);

      if (elength == RTEMS_RFS_DIR_ENTRY_EMPTY)
        break;

      if (rtems_rfs_dir_entry_valid (fs, elength, eino))
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_DEL_ENTRY))
          printf ("rtems-rfs: dir-del-entry: "
                  "bad length or ino for ino %" PRIu32 ": %u/%" PRId32
                  " @ %" PRIu32 ".%04x\n",
                  rtems_rfs_inode_ino (dir), elength, eino, block, eoffset);
        rc = EIO;
        break;
      }

      if (ino == rtems_rfs_dir_entry_ino (entry))
      {
        uint32_t remaining;
        remaining = rtems_rfs_fs_block_size (fs) - (eoffset + elength);
        memmove (entry, entry + elength, remaining);
        memset (entry + remaining, 0xff, elength);

        /*
         * If the remainder of the block is empty and this is the start of the
         * block and it is the last block in the map shrink the map.
         *
         * @note We could check again to see if the new end block in the map is
         *       also empty. This way we could clean up an empty directory.
         */
        elength = rtems_rfs_dir_entry_length (entry);

        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_DEL_ENTRY))
          printf ("rtems-rfs: dir-del-entry: "
                  "last block free for ino %" PRIu32 ": elength=%i block=%" PRIu32
                  " offset=%d last=%s\n",
                  ino, elength, block, eoffset,
                  rtems_rfs_block_map_last (&map) ? "yes" : "no");

        if ((elength == RTEMS_RFS_DIR_ENTRY_EMPTY) &&
            (eoffset == 0) && rtems_rfs_block_map_last (&map))
        {
          rc = rtems_rfs_block_map_shrink (fs, &map, 1);
          if (rc > 0)
          {
            if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_DEL_ENTRY))
              printf ("rtems-rfs: dir-del-entry: "
                      "block map shrink failed for ino %" PRIu32 ": %d: %s\n",
                      rtems_rfs_inode_ino (dir), rc, strerror (rc));
          }
        }

        rtems_rfs_buffer_mark_dirty (&buffer);
        rtems_rfs_buffer_handle_close (fs, &buffer);
        rtems_rfs_block_map_close (fs, &map);
        return 0;
      }

      if (!search)
      {
        rc = EIO;
        break;
      }

      entry   += elength;
      eoffset += elength;
    }

    if (rc == 0)
    {
      rc = rtems_rfs_block_map_next_block (fs, &map, &block);
      if (rc == ENXIO)
        rc = ENOENT;
    }
  }

  rtems_rfs_buffer_handle_close (fs, &buffer);
  rtems_rfs_block_map_close (fs, &map);
  return rc;
}

int
rtems_rfs_dir_read (rtems_rfs_file_system*  fs,
                    rtems_rfs_inode_handle* dir,
                    rtems_rfs_pos_rel       offset,
                    struct dirent*          dirent,
                    size_t*                 length)
{
  rtems_rfs_block_map     map;
  rtems_rfs_buffer_handle buffer;
  rtems_rfs_block_no      block;
  int                     rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_READ))
    printf ("rtems-rfs: dir-read: dir=%" PRId32 " offset=%" PRId64 "\n",
            rtems_rfs_inode_ino (dir), offset);

  *length = 0;

  rc = rtems_rfs_block_map_open (fs, dir, &map);
  if (rc > 0)
    return rc;

  if (((rtems_rfs_fs_block_size (fs) -
        (offset % rtems_rfs_fs_block_size (fs))) <= RTEMS_RFS_DIR_ENTRY_SIZE))
    offset = (((offset / rtems_rfs_fs_block_size (fs)) + 1) *
              rtems_rfs_fs_block_size (fs));

  rc = rtems_rfs_block_map_seek (fs, &map, offset, &block);
  if (rc > 0)
  {
    if (rc == ENXIO)
      rc = ENOENT;
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  /*
   * Look for an empty entry and if this is the last block that is the end of
   * the directory.
   */
  while (rc == 0)
  {
    uint8_t*      entry;
    rtems_rfs_ino eino;
    int           elength;
    int           remaining;

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, true);
    if (rc > 0)
    {
      rtems_rfs_buffer_handle_close (fs, &buffer);
      rtems_rfs_block_map_close (fs, &map);
      return rc;
    }

    entry  = rtems_rfs_buffer_data (&buffer);
    entry += map.bpos.boff;

    elength = rtems_rfs_dir_entry_length (entry);
    eino    = rtems_rfs_dir_entry_ino (entry);

    if (elength != RTEMS_RFS_DIR_ENTRY_EMPTY)
    {
      if (rtems_rfs_dir_entry_valid (fs, elength, eino))
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_READ))
          printf ("rtems-rfs: dir-read: "
                  "bad length or ino for ino %" PRIu32 ": %u/%" PRId32 " @ %04" PRIx32 "\n",
                  rtems_rfs_inode_ino (dir), elength, eino, map.bpos.boff);
        rc = EIO;
        break;
      }

      memset (dirent, 0, sizeof (struct dirent));
      dirent->d_off = offset;
      dirent->d_reclen = sizeof (struct dirent);

      *length += elength;

      remaining = rtems_rfs_fs_block_size (fs) - (map.bpos.boff + elength);

      if (remaining <= RTEMS_RFS_DIR_ENTRY_SIZE)
        *length += remaining;

      elength -= RTEMS_RFS_DIR_ENTRY_SIZE;
      if (elength > NAME_MAX)
        elength = NAME_MAX;

      memcpy (dirent->d_name, entry + RTEMS_RFS_DIR_ENTRY_SIZE, elength);

      dirent->d_ino = rtems_rfs_dir_entry_ino (entry);
      dirent->d_namlen = elength;

      if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_READ))
        printf ("rtems-rfs: dir-read: found off:%" PRIooff_t " ino:%ld name=%s\n",
                dirent->d_off, dirent->d_ino, dirent->d_name);
      break;
    }

    *length += rtems_rfs_fs_block_size (fs) - map.bpos.boff;

    if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_READ))
      printf ("rtems-rfs: dir-read: next block: off:%" PRId64 " length:%zd\n",
              offset, *length);

    rc = rtems_rfs_block_map_next_block (fs, &map, &block);
    if (rc == ENXIO)
      rc = ENOENT;
  }

  rtems_rfs_buffer_handle_close (fs, &buffer);
  rtems_rfs_block_map_close (fs, &map);
  return rc;
}

int
rtems_rfs_dir_empty (rtems_rfs_file_system*  fs,
                     rtems_rfs_inode_handle* dir)
{
  rtems_rfs_block_map     map;
  rtems_rfs_buffer_handle buffer;
  rtems_rfs_block_no      block;
  bool                    empty;
  int                     rc;

  if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_READ))
    printf ("rtems-rfs: dir-empty: dir=%" PRId32 "\n", rtems_rfs_inode_ino (dir));

  empty = true;

  rc = rtems_rfs_block_map_open (fs, dir, &map);
  if (rc > 0)
    return rc;

  rc = rtems_rfs_block_map_seek (fs, &map, 0, &block);
  if (rc > 0)
  {
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  rc = rtems_rfs_buffer_handle_open (fs, &buffer);
  if (rc > 0)
  {
    rtems_rfs_block_map_close (fs, &map);
    return rc;
  }

  /*
   * Look for an empty entry and if this is the last block that is the end of
   * the directory.
   */
  while (empty)
  {
    uint8_t* entry;
    int      offset;

    rc = rtems_rfs_buffer_handle_request (fs, &buffer, block, true);
    if (rc > 0)
      break;

    entry  = rtems_rfs_buffer_data (&buffer);
    offset = 0;

    while (offset < (rtems_rfs_fs_block_size (fs) - RTEMS_RFS_DIR_ENTRY_SIZE))
    {
      rtems_rfs_ino eino;
      int           elength;

      elength = rtems_rfs_dir_entry_length (entry);
      eino    = rtems_rfs_dir_entry_ino (entry);

      if (elength == RTEMS_RFS_DIR_ENTRY_EMPTY)
        break;

      if (rtems_rfs_dir_entry_valid (fs, elength, eino))
      {
        if (rtems_rfs_trace (RTEMS_RFS_TRACE_DIR_EMPTY))
          printf ("rtems-rfs: dir-empty: "
                  "bad length or ino for ino %" PRIu32 ": %u/%" PRIu32 " @ %04x\n",
                  rtems_rfs_inode_ino (dir), elength, eino, offset);
        rc = EIO;
        break;
      }

      /*
       * Ignore the current (.) and parent (..) entries. Anything else means
       * the directory is not empty.
       */
      if (((elength != (RTEMS_RFS_DIR_ENTRY_SIZE + 1)) ||
           (entry[RTEMS_RFS_DIR_ENTRY_SIZE] != '.')) &&
          ((elength != (RTEMS_RFS_DIR_ENTRY_SIZE + 2)) ||
           (entry[RTEMS_RFS_DIR_ENTRY_SIZE] != '.') ||
           (entry[RTEMS_RFS_DIR_ENTRY_SIZE + 1] != '.')))
      {
        empty = false;
        break;
      }

      entry  += elength;
      offset += elength;
    }

    if (empty)
    {
      rc = rtems_rfs_block_map_next_block (fs, &map, &block);
      if (rc > 0)
      {
        if (rc == ENXIO)
          rc = 0;
        break;
      }
    }
  }

  if ((rc == 0) && !empty)
    rc = ENOTEMPTY;

  rtems_rfs_buffer_handle_close (fs, &buffer);
  rtems_rfs_block_map_close (fs, &map);
  return rc;
}
