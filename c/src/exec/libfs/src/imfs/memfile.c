/*
 *  IMFS Device Node Handlers
 *
 *  This file contains the set of handlers used to process operations on
 *  IMFS memory file nodes.  The memory files are created in memory using
 *  malloc'ed memory.  Thus any data stored in one of these files is lost
 *  at system shutdown unless special arrangements to copy the data to 
 *  some type of non-volailte storage are made by the application.
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.OARcorp.com/rtems/license.html.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <stdlib.h>
#include <assert.h>
#include <errno.h>

#include <rtems.h>
#include <rtems/libio.h>
#include "imfs.h"
#include <rtems/libio_.h>
#include <rtems/seterr.h>

#define MEMFILE_STATIC 

/*
 *  Prototypes of private routines 
 */

MEMFILE_STATIC int IMFS_memfile_extend(
   IMFS_jnode_t  *the_jnode,
   off_t          new_length
);

MEMFILE_STATIC int IMFS_memfile_addblock(
   IMFS_jnode_t  *the_jnode,
   unsigned int   block
);

MEMFILE_STATIC int IMFS_memfile_remove_block(
   IMFS_jnode_t  *the_jnode,
   unsigned int   block
);

MEMFILE_STATIC block_p *IMFS_memfile_get_block_pointer(
   IMFS_jnode_t   *the_jnode,
   unsigned int    block,
   int             malloc_it
);

MEMFILE_STATIC int IMFS_memfile_read(
   IMFS_jnode_t    *the_jnode,
   off_t            start,
   unsigned char   *destination,
   unsigned int     length
);

MEMFILE_STATIC int IMFS_memfile_write(
   IMFS_jnode_t          *the_jnode,
   off_t                  start,
   const unsigned char   *source,
   unsigned int           length
);

void *memfile_alloc_block(void);

void memfile_free_block(
  void *memory
);

/*
 *  memfile_open
 *
 *  This routine processes the open() system call.  Note that there is
 *  nothing special to be done at open() time.
 */

int memfile_open(
  rtems_libio_t *iop,
  const char    *pathname,
  unsigned32     flag,
  unsigned32     mode
)
{
  IMFS_jnode_t  *the_jnode;

  the_jnode = iop->file_info;

  if (iop->flags & LIBIO_FLAGS_APPEND)
    iop->offset = the_jnode->info.file.size;

  iop->size = the_jnode->info.file.size;
  return 0;
}

/*
 *  memfile_close
 *
 *  This routine processes the close() system call.  Note that there is
 *  nothing to flush or memory to free at this point.
 */

int memfile_close(
  rtems_libio_t *iop
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  if (iop->flags & LIBIO_FLAGS_APPEND)
    iop->offset = the_jnode->info.file.size;

  return 0;
}

/*
 *  memfile_read
 *
 *  This routine processes the read() system call.
 */

int memfile_read(
  rtems_libio_t *iop,
  void          *buffer,
  unsigned32     count
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  return IMFS_memfile_read( the_jnode, iop->offset, buffer, count );
}

/*
 *  memfile_write
 *
 *  This routine processes the write() system call.
 */

int memfile_write(
  rtems_libio_t *iop,
  const void    *buffer,
  unsigned32     count
)
{
  IMFS_jnode_t   *the_jnode;
  int             status;

  the_jnode = iop->file_info;

  status = IMFS_memfile_write( the_jnode, iop->offset, buffer, count );
  iop->size = the_jnode->info.file.size;

  return status;
}

/*
 *  memfile_ioctl
 *
 *  This routine processes the ioctl() system call.
 *
 *  NOTE:  No ioctl()'s are supported for in-memory files.
 */

int memfile_ioctl(
  rtems_libio_t *iop,
  unsigned32     command,
  void          *buffer
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  return 0;
}

/*
 *  memfile_lseek
 *
 *  This routine processes the lseek() system call.
 */

int memfile_lseek(
  rtems_libio_t   *iop,
  off_t            offset,
  int              whence
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  if (the_jnode->type == IMFS_LINEAR_FILE) {
    if (iop->offset > the_jnode->info.linearfile.size)
      iop->offset = the_jnode->info.linearfile.size;
  }
  else {  /* Must be a block file (IMFS_MEMORY_FILE). */
    if (IMFS_memfile_extend( the_jnode, iop->offset ))
      rtems_set_errno_and_return_minus_one( ENOSPC );

    iop->size = the_jnode->info.file.size;
  }
  return iop->offset;
}

/*
 *  memfile_stat
 *
 *  This IMFS_stat() can be used.
 */

/*
 *  memfile_ftruncate
 *
 *  This routine processes the ftruncate() system call.
 */

int memfile_ftruncate(
  rtems_libio_t        *iop,
  off_t                 length
)
{
  IMFS_jnode_t   *the_jnode;

  the_jnode = iop->file_info;

  /*
   *  POSIX 1003.1b does not specify what happens if you truncate a file
   *  and the new length is greater than the current size.  We treat this
   *  as an extend operation.
   */

  if ( length > the_jnode->info.file.size )
    return IMFS_memfile_extend( the_jnode, length );

  /*
   *  The in-memory files do not currently reclaim memory until the file is
   *  deleted.  So we leave the previously allocated blocks in place for
   *  future use and just set the length.
   */

  the_jnode->info.file.size = length;
  iop->size = the_jnode->info.file.size;

  IMFS_update_atime( the_jnode );

  return 0;
}

/*
 *  IMFS_memfile_extend
 *
 *  This routine insures that the in-memory file is of the length
 *  specified.  If necessary, it will allocate memory blocks to 
 *  extend the file.
 */

MEMFILE_STATIC int IMFS_memfile_extend(
   IMFS_jnode_t  *the_jnode,
   off_t          new_length
)
{
  unsigned int   block;
  unsigned int   new_blocks;
  unsigned int   old_blocks;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_MEMORY_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  if ( new_length >= IMFS_MEMFILE_MAXIMUM_SIZE )
    rtems_set_errno_and_return_minus_one( EINVAL );

  if ( new_length <= the_jnode->info.file.size )
    return 0;

  /*
   *  Calculate the number of range of blocks to allocate
   */

  new_blocks = new_length / IMFS_MEMFILE_BYTES_PER_BLOCK;
  old_blocks = the_jnode->info.file.size / IMFS_MEMFILE_BYTES_PER_BLOCK;

  /*
   *  Now allocate each of those blocks.
   */

  for ( block=old_blocks ; block<=new_blocks ; block++ ) {
    if ( IMFS_memfile_addblock( the_jnode, block ) ) {
       for ( ; block>=old_blocks ; block-- ) {
          IMFS_memfile_remove_block( the_jnode, block );
       }
       rtems_set_errno_and_return_minus_one( ENOSPC );
    }
  }

  /*
   *  Set the new length of the file.
   */

  the_jnode->info.file.size = new_length;
  return 0;
}

/*
 *  IMFS_memfile_addblock
 *
 *  This routine adds a single block to the specified in-memory file.
 */

MEMFILE_STATIC int IMFS_memfile_addblock(
   IMFS_jnode_t  *the_jnode,
   unsigned int   block
)
{
  block_p  memory;
  block_p *block_entry_ptr;

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_MEMORY_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  block_entry_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 1 );
  if ( *block_entry_ptr )
    return 0;

#if 0
  printf( "%d %p", block, block_entry_ptr );
    fflush(stdout);
#endif

  memory = memfile_alloc_block();
  if ( !memory )
    return 1;
  *block_entry_ptr = memory;

  return 0;
}

/*
 *  IMFS_memfile_remove_block
 *
 *  This routine removes the specified block from the in-memory file.
 *
 *  NOTE:  This is a support routine and is called only to remove
 *         the last block or set of blocks in a file.  Removing a
 *         block from the middle of a file would be exceptionally
 *         dangerous and the results unpredictable.
 */

MEMFILE_STATIC int IMFS_memfile_remove_block(
   IMFS_jnode_t  *the_jnode,
   unsigned int   block
)
{
  block_p *block_entry_ptr;
  block_p  ptr;

  block_entry_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
  ptr = *block_entry_ptr;
  *block_entry_ptr = 0;

  memfile_free_block( ptr );

  return 1;
}

/*
 *  memfile_free_blocks_in_table
 *
 *  This is a support routine for IMFS_memfile_remove.  It frees all the
 *  blocks in one of the indirection tables.
 */

void memfile_free_blocks_in_table(
  block_p **block_table,
  int       entries
)
{
  int      i;
  block_p *b;

  /*
   *  Perform internal consistency checks
   */

  assert( block_table );
  if ( !block_table )
    return;

  /*
   *  Now go through all the slots in the table and free the memory.
   */

  b = *block_table;

  for ( i=0 ; i<entries ; i++ ) {
    if ( b[i] ) {
      memfile_free_block( b[i] );
      b[i] = 0;
    }
  }

  /*
   *  Now that all the blocks in the block table are free, we can 
   *  free the block table itself.
   */

  memfile_free_block( *block_table );
  *block_table = 0;
}

/*
 *  IMFS_memfile_remove
 *
 *  This routine frees all memory associated with an in memory file.
 *
 *  NOTE:  This is an exceptionally conservative implementation.  
 *         It will check EVERY pointer which is non-NULL and insure
 *         any child non-NULL pointers are freed.  Optimistically, all that
 *         is necessary is to scan until a NULL pointer is found.  There
 *         should be no allocated data past that point.
 *
 *         In experimentation on the powerpc simulator, it was noted
 *         that using blocks which held 128 slots versus 16 slots made
 *         a significant difference in the performance of this routine.
 *
 *         Regardless until the IMFS implementation is proven, it
 *         is better to stick to simple, easy to understand algorithms.
 */

int IMFS_memfile_remove(
 IMFS_jnode_t  *the_jnode
)
{
  IMFS_memfile_t  *info;
  int              i;
  int              j;
  unsigned int     to_free;
  block_p         *p;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_MEMORY_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  /*
   *  Eventually this could be set smarter at each call to
   *  memfile_free_blocks_in_table to greatly speed this up.
   */

  to_free = IMFS_MEMFILE_BLOCK_SLOTS;

  /*
   *  Now start freeing blocks in this order:
   *    + indirect
   *    + doubly indirect
   *    + triply indirect
   */

  info = &the_jnode->info.file;

  if ( info->indirect ) {
    memfile_free_blocks_in_table( &info->indirect, to_free );
  }

  if ( info->doubly_indirect ) {

    for ( i=0 ; i<IMFS_MEMFILE_BLOCK_SLOTS ; i++ ) {
      if ( info->doubly_indirect[i] ) {
        memfile_free_blocks_in_table(
         (block_p **)&info->doubly_indirect[i], to_free );
      }
    }
    memfile_free_blocks_in_table( &info->doubly_indirect, to_free );

  }
  
  if ( info->triply_indirect ) {
    for ( i=0 ; i<IMFS_MEMFILE_BLOCK_SLOTS ; i++ ) {
      p = (block_p *) info->triply_indirect[i];
      if ( !p )  /* ensure we have a valid pointer */
         break;
      for ( j=0 ; j<IMFS_MEMFILE_BLOCK_SLOTS ; j++ ) {
        if ( p[j] ) {
          memfile_free_blocks_in_table( (block_p **)&p[j], to_free);
        }
      }
      memfile_free_blocks_in_table(
        (block_p **)&info->triply_indirect[i], to_free );
    }
    memfile_free_blocks_in_table(
        (block_p **)&info->triply_indirect, to_free );
  }

  return 0;
}

/*
 *  IMFS_memfile_read
 *
 *  This routine read from memory file pointed to by the_jnode into
 *  the specified data buffer specified by destination.  The file
 *  is NOT extended.  An offset greater than the length of the file
 *  is considered an error.  Read from an offset for more bytes than
 *  are between the offset and the end of the file will result in 
 *  reading the data between offset and the end of the file (truncated
 *  read).
 */

MEMFILE_STATIC int IMFS_memfile_read(
   IMFS_jnode_t    *the_jnode,
   off_t            start,
   unsigned char   *destination,
   unsigned int     length
)
{
  block_p             *block_ptr;
  unsigned int         block;
  unsigned int         my_length;
  unsigned int         to_copy = 0;
  unsigned int         last_byte;
  unsigned int         copied;
  unsigned int         start_offset;
  unsigned char       *dest;

  dest = destination;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_MEMORY_FILE ||
          the_jnode->type == IMFS_LINEAR_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE &&
       the_jnode->type != IMFS_LINEAR_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  /*
   *  Error checks on arguments
   */

  assert( dest );
  if ( !dest )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  If there is nothing to read, then quick exit.
   */

  my_length = length;
  if ( !my_length )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  Linear files (as created from a tar file are easier to handle
   *  than block files).
   */
  if (the_jnode->type == IMFS_LINEAR_FILE) {
    unsigned char  *file_ptr;

    file_ptr = (unsigned char *)the_jnode->info.linearfile.direct;

    if (my_length > (the_jnode->info.linearfile.size - start))
      my_length = the_jnode->info.linearfile.size - start;

    memcpy(dest, &file_ptr[start], my_length);

    IMFS_update_atime( the_jnode );

    return my_length;
  }

  /*
   *  If the last byte we are supposed to read is past the end of this
   *  in memory file, then shorten the length to read.
   */

  last_byte = start + length;
  if ( last_byte > the_jnode->info.file.size )
    my_length = the_jnode->info.file.size - start;

  copied = 0;

  /*
   *  Three phases to the read:
   *    + possibly the last part of one block
   *    + all of zero of more blocks
   *    + possibly the first part of one block
   */

  /*
   *  Phase 1: possibly the last part of one block
   */

  start_offset = start % IMFS_MEMFILE_BYTES_PER_BLOCK;
  block = start / IMFS_MEMFILE_BYTES_PER_BLOCK;
  if ( start_offset )  {
    to_copy = IMFS_MEMFILE_BYTES_PER_BLOCK - start_offset;
    if ( to_copy > my_length )
      to_copy = my_length;
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
    memcpy( dest, &(*block_ptr)[ start_offset ], to_copy );
    dest += to_copy;
    block++;
    my_length -= to_copy;
    copied += to_copy;
  }

  /*
   *  Phase 2: all of zero of more blocks
   */

  to_copy = IMFS_MEMFILE_BYTES_PER_BLOCK;
  while ( my_length >= IMFS_MEMFILE_BYTES_PER_BLOCK ) {
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
    memcpy( dest, &(*block_ptr)[ 0 ], to_copy );
    dest += to_copy;
    block++;
    my_length -= to_copy;
    copied += to_copy;
  }

  /*
   *  Phase 3: possibly the first part of one block
   */

  assert( my_length < IMFS_MEMFILE_BYTES_PER_BLOCK );

  if ( my_length ) {
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
    memcpy( dest, &(*block_ptr)[ 0 ], my_length );
    copied += my_length;
  }

  IMFS_update_atime( the_jnode );

  return copied;
}

/*
 *  IMFS_memfile_write
 *
 *  This routine writes the specified data buffer into the in memory
 *  file pointed to by the_jnode.  The file is extended as needed.
 */

MEMFILE_STATIC int IMFS_memfile_write(
   IMFS_jnode_t          *the_jnode,
   off_t                  start,
   const unsigned char   *source,
   unsigned int           length
)
{
  block_p             *block_ptr;
  unsigned int         block;
  int                  status;
  unsigned int         my_length;
  unsigned int         to_copy = 0;
  unsigned int         last_byte;
  unsigned int         start_offset;
  int                  copied;
  const unsigned char *src;

  src = source;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    rtems_set_errno_and_return_minus_one( EIO );

  assert( the_jnode->type == IMFS_MEMORY_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE )
    rtems_set_errno_and_return_minus_one( EIO );

  /*
   *  Error check arguments
   */

  assert( source );
  if ( !source )
    rtems_set_errno_and_return_minus_one( EINVAL );


  /*
   *  If there is nothing to write, then quick exit.
   */

  my_length = length;
  if ( !my_length )
    rtems_set_errno_and_return_minus_one( EINVAL );

  /*
   *  If the last byte we are supposed to write is past the end of this
   *  in memory file, then extend the length.
   */

  last_byte = start + length;
  if ( last_byte > the_jnode->info.file.size ) {
    status = IMFS_memfile_extend( the_jnode, last_byte );
    if ( status )
      rtems_set_errno_and_return_minus_one( ENOSPC );
  }

  copied = 0;

  /*
   *  Three phases to the write:
   *    + possibly the last part of one block
   *    + all of zero of more blocks
   *    + possibly the first part of one block
   */

  /*
   *  Phase 1: possibly the last part of one block
   */

  start_offset = start % IMFS_MEMFILE_BYTES_PER_BLOCK;
  block = start / IMFS_MEMFILE_BYTES_PER_BLOCK;
  if ( start_offset )  {
    to_copy = IMFS_MEMFILE_BYTES_PER_BLOCK - start_offset;
    if ( to_copy > my_length )
      to_copy = my_length;
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
#if 0
printf( "write %d at %d in %d: %*s\n", to_copy, start_offset, block, to_copy, src );
#endif
    memcpy( &(*block_ptr)[ start_offset ], src, to_copy );
    src += to_copy;
    block++;
    my_length -= to_copy;
    copied += to_copy;
  }

  /*
   *  Phase 2: all of zero of more blocks
   */

  to_copy = IMFS_MEMFILE_BYTES_PER_BLOCK;
  while ( my_length >= IMFS_MEMFILE_BYTES_PER_BLOCK ) {
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
#if 0
printf( "write %d in %d: %*s\n", to_copy, block, to_copy, src );
#endif
    memcpy( &(*block_ptr)[ 0 ], src, to_copy );
    src += to_copy;
    block++;
    my_length -= to_copy;
    copied += to_copy;
  }

  /*
   *  Phase 3: possibly the first part of one block
   */

  assert( my_length < IMFS_MEMFILE_BYTES_PER_BLOCK );

  to_copy = my_length;
  if ( my_length ) {
    block_ptr = IMFS_memfile_get_block_pointer( the_jnode, block, 0 );
    assert( block_ptr );
    if ( !block_ptr )
      return copied;
#if 0
printf( "write %d in %d: %*s\n", to_copy, block, to_copy, src );
#endif
    memcpy( &(*block_ptr)[ 0 ], src, my_length );
    my_length = 0;
    copied += to_copy;
  }

  IMFS_atime_mtime_update( the_jnode );

  return copied;
}

/*
 *  IMFS_memfile_get_block_pointer
 *
 *  This routine looks up the block pointer associated with the given block
 *  number.  If that block has not been allocated and "malloc_it" is
 *  TRUE, then the block is allocated.  Otherwise, it is an error.
 */

#if 0
block_p *IMFS_memfile_get_block_pointer_DEBUG(
   IMFS_jnode_t   *the_jnode,
   unsigned int    block,
   int             malloc_it
);

block_p *IMFS_memfile_get_block_pointer(
   IMFS_jnode_t   *the_jnode,
   unsigned int    block,
   int             malloc_it
)
{
  block_p *p;

  p = IMFS_memfile_get_block_pointer_DEBUG( the_jnode, block, malloc_it );
  printf( "(%d -> %p) ", block, p );
  return p;
}

block_p *IMFS_memfile_get_block_pointer_DEBUG(
#else
block_p *IMFS_memfile_get_block_pointer(
#endif
   IMFS_jnode_t   *the_jnode,
   unsigned int    block,
   int             malloc_it
)
{
  unsigned int    my_block;
  IMFS_memfile_t *info;
  unsigned int    singly;
  unsigned int    doubly;
  unsigned int    triply;
  block_p        *p;
  block_p        *p1;
  block_p        *p2;

  /*
   *  Perform internal consistency checks
   */

  assert( the_jnode );
  if ( !the_jnode )
    return NULL;

  assert( the_jnode->type == IMFS_MEMORY_FILE );
  if ( the_jnode->type != IMFS_MEMORY_FILE )
    return NULL;

  info = &the_jnode->info.file;

  my_block = block;

  /*
   *  Is the block number in the simple indirect portion?
   */

  if ( my_block <= LAST_INDIRECT ) {
#if 0
printf( "(s %d) ", block );
fflush(stdout);
#endif
    p = info->indirect;

    if ( malloc_it ) {

      if ( !p ) {
        p = memfile_alloc_block();
        if ( !p )
           return 0;
        info->indirect = p;
      }
      return &info->indirect[ my_block ];
    }

    if ( !p )
      return 0;

    return &info->indirect[ my_block ];
  }

  /*
   *  Is the block number in the doubly indirect portion?
   */

  if ( my_block <= LAST_DOUBLY_INDIRECT ) {
#if 0
printf( "(d %d) ", block );
fflush(stdout);
#endif

    my_block -= FIRST_DOUBLY_INDIRECT;

    singly = my_block % IMFS_MEMFILE_BLOCK_SLOTS;
    doubly = my_block / IMFS_MEMFILE_BLOCK_SLOTS;

    p = info->doubly_indirect;
    if ( malloc_it ) {

      if ( !p ) {
        p = memfile_alloc_block();
        if ( !p )
           return 0;
        info->doubly_indirect = p;
      }

      p1 = (block_p *)p[ doubly ];
      if ( !p1 ) {
        p1 = memfile_alloc_block();
        if ( !p1 )
           return 0;
        p[ doubly ] = (block_p) p1;
      }

      return (block_p *)&p1[ singly ];
    }

    if ( !p )
      return 0;

    p = (block_p *)p[ doubly ];
    if ( !p ) 
      return 0;

#if 0
printf( "(d %d %d %d %d %p %p) ", block, my_block, doubly,
                                       singly, p, &p[singly] );
fflush(stdout);
#endif
    return (block_p *)&p[ singly ];
  }

#if 0
printf( "(t %d) ", block );
fflush(stdout);
#endif
  /*
   *  Is the block number in the triply indirect portion?
   */

  if ( my_block <= LAST_TRIPLY_INDIRECT ) {
    my_block -= FIRST_TRIPLY_INDIRECT;

    singly = my_block % IMFS_MEMFILE_BLOCK_SLOTS;
    doubly = my_block / IMFS_MEMFILE_BLOCK_SLOTS;
    triply = doubly / IMFS_MEMFILE_BLOCK_SLOTS;
    doubly %= IMFS_MEMFILE_BLOCK_SLOTS;

    p = info->triply_indirect;

    if ( malloc_it ) {
      if ( !p ) {
        p = memfile_alloc_block();
        if ( !p )
           return 0;
        info->triply_indirect = p;
      }

      p1 = (block_p *) p[ triply ];
      if ( !p1 ) {
        p1 = memfile_alloc_block();
        if ( !p1 )
           return 0;
        p[ triply ] = (block_p) p1;
      }

      p2 = (block_p *)p1[ doubly ];
      if ( !p2 ) {
        p2 = memfile_alloc_block();
        if ( !p2 )
           return 0;
        p1[ doubly ] = (block_p) p2;
      }
      return (block_p *)&p2[ singly ];
    }

    if ( !p )
      return 0;

#if 0
printf( "(t %d %d %d %d %d) ", block, my_block, triply, doubly, singly );
fflush(stdout);
#endif
    p1 = (block_p *) p[ triply ];
    if ( !p1 )
      return 0;

    p2 = (block_p *)p1[ doubly ];
    if ( !p )
      return 0;

    return (block_p *)&p2[ singly ];
  }

  /*
   *  This means the requested block number is out of range.
   */

  return 0;
}

/*
 *  memfile_alloc_block
 *
 *  Allocate a block for an in-memory file.
 */

int memfile_blocks_allocated = 0;

void *memfile_alloc_block(void)
{
  void *memory;

  memory = (void *)calloc(1, IMFS_MEMFILE_BYTES_PER_BLOCK);
  if ( memory )
    memfile_blocks_allocated++;

  return memory;
}

/*
 *  memfile_free_block
 *
 *  Free a block from an in-memory file.
 */

void memfile_free_block(
  void *memory
)
{
#if 0
printf( "(d %p) ", memory );
fflush(stdout);
#endif
  free(memory);
  memfile_blocks_allocated--;
}


/*
 *  memfile_rmnod
 *
 *  This routine is available from the optable to remove a node 
 *  from the IMFS file system.
 */

int memfile_rmnod(
  rtems_filesystem_location_info_t      *pathloc       /* IN */
)
{
  IMFS_jnode_t *the_jnode;  

  the_jnode = (IMFS_jnode_t *) pathloc->node_access;

  /* 
   * Take the node out of the parent's chain that contains this node 
   */

  if ( the_jnode->Parent != NULL ) {
    Chain_Extract( (Chain_Node *) the_jnode );
    the_jnode->Parent = NULL;
  }

  /*
   * Decrement the link counter and see if we can free the space.
   */

  the_jnode->st_nlink--;
  IMFS_update_ctime( the_jnode );

  /*
   * The file cannot be open and the link must be less than 1 to free.
   */

  if ( !rtems_libio_is_file_open( the_jnode ) && (the_jnode->st_nlink < 1) ) {

    /* 
     * Is the rtems_filesystem_current is this node?
     */

    if ( rtems_filesystem_current.node_access == pathloc->node_access )
       rtems_filesystem_current.node_access = NULL;

    /*
     * Free memory associated with a memory file.
     */
    if (the_jnode->type != IMFS_LINEAR_FILE)
      IMFS_memfile_remove( the_jnode );

    free( the_jnode );
  }

  return 0;

}


