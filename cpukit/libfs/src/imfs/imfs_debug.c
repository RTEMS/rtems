/*
 *  IMFS debug support routines
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 *
 *  $Id$
 */

#if HAVE_CONFIG_H
#include "config.h"
#endif

#include <assert.h>
#include <string.h>
#include <fcntl.h>
#include <errno.h>
#include <stdlib.h>
#include <unistd.h>   /* for close */

#include <stdio.h>
#include <sys/stat.h>

#include "imfs.h"
#include <rtems/libio_.h>

/*
 *  IMFS_types
 *
 *  Printable names for each of the IMFS file system types.
 */
 
char *IMFS_types[ IMFS_NUMBER_OF_TYPES ] = {
  "directory",
  "device",
  "link",
  "memory file",
  "linear file"
};

/*
 *  IMFS_print_jnode
 *
 *  This routine prints the contents of the specified jnode.
 */

void IMFS_print_jnode( 
  IMFS_jnode_t *the_jnode
)
{
  assert( the_jnode );

  printf( "%s", the_jnode->name );
  switch( the_jnode->type ) {
    case IMFS_DIRECTORY:
      printf( "/" );
      break;

    case IMFS_DEVICE:
      printf( " (device %d, %d)",
        the_jnode->info.device.major, the_jnode->info.device.minor );
      break;

    case IMFS_LINEAR_FILE:
      printf( " (file %d %p)",
        (int)the_jnode->info.linearfile.size,
        the_jnode->info.linearfile.direct
      );
      break;

    case IMFS_MEMORY_FILE:
      /* Useful when debugging .. varies between targets  */
#if 0
      printf( " (file %d %p %p %p)",
        (int)the_jnode->info.file.size,
        the_jnode->info.file.indirect,
        the_jnode->info.file.doubly_indirect,
        the_jnode->info.file.triply_indirect
      );
#else
      printf( " (file %d)", (int)the_jnode->info.file.size );
#endif
      break;

    case IMFS_HARD_LINK:
      printf( " links not printed\n" );
      assert(0);
      break;

    case IMFS_SYM_LINK:
      printf( " links not printed\n" );
      assert(0);
      break;

    default:
      printf( " bad type %d\n", the_jnode->type );
      assert(0);
      break;
  }
  puts("");
}

/*
 *  IMFS_dump_directory
 *
 *  This routine prints the contents of a directory in the IMFS.  If a
 *  directory is encountered, then this routine will recurse to process
 *  the subdirectory.
 */

void IMFS_dump_directory(
  IMFS_jnode_t  *the_directory,
  int            level
)
{
  Chain_Node           *the_node;
  Chain_Control        *the_chain;
  IMFS_jnode_t         *the_jnode;
  int                   i;

  assert( the_directory );

  assert( level >= 0 );

  assert( the_directory->type == IMFS_DIRECTORY );

  the_chain = &the_directory->info.directory.Entries;

  for ( the_node = the_chain->first;
        !_Chain_Is_tail( the_chain, the_node );
        the_node = the_node->next ) {

    the_jnode = (IMFS_jnode_t *) the_node;

    for ( i=0 ; i<=level ; i++ )
      printf( "...." );
    IMFS_print_jnode( the_jnode );
    if ( the_jnode->type == IMFS_DIRECTORY )
      IMFS_dump_directory( the_jnode, level + 1 );
  }
}

/*
 *  IMFS_dump
 *
 *  This routine dumps the entire IMFS that is mounted at the root
 *  directory.
 *
 *  NOTE: Assuming the "/" directory is bad.
 *        Not checking that the starting directory is in an IMFS is bad.
 */

void IMFS_dump( void )
{
  printf( "*************** Dump of Entire IMFS ***************\n" );
  printf( "/\n" );
  IMFS_dump_directory( rtems_filesystem_root.node_access, 0 );
  printf( "***************       End of Dump        ***************\n" );
}

/*
 *  IMFS_memfile_maximum_size()
 *
 *  This routine returns the size of the largest file which can be created
 *  using the IMFS memory file type.
 *
 */

int IMFS_memfile_maximum_size( void )
{
  return IMFS_MEMFILE_MAXIMUM_SIZE;
}
