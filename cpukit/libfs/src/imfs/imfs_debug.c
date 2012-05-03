/*
 *  IMFS debug support routines
 *
 *  COPYRIGHT (c) 1989-1999.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#if HAVE_CONFIG_H
  #include "config.h"
#endif

#include "imfs.h"

#include <inttypes.h>
#include <unistd.h>
#include <stdio.h>

/*
 *  IMFS_print_jnode
 *
 *  This routine prints the contents of the specified jnode.
 */
static void IMFS_print_jnode(
  IMFS_jnode_t *the_jnode
)
{
  IMFS_assert( the_jnode );

  fprintf(stdout, "%s", the_jnode->name );
  switch( IMFS_type( the_jnode ) ) {
    case IMFS_DIRECTORY:
      fprintf(stdout, "/" );
      break;

    case IMFS_DEVICE:
      fprintf(stdout, " (device %" PRId32 ", %" PRId32 ")",
        the_jnode->info.device.major, the_jnode->info.device.minor );
      break;

    case IMFS_LINEAR_FILE:
      fprintf(stdout, " (file %" PRId32 " %p)",
        (uint32_t)the_jnode->info.linearfile.size,
        the_jnode->info.linearfile.direct
      );
      break;

    case IMFS_MEMORY_FILE:
      /* Useful when debugging .. varies between targets  */
#if 0
      fprintf(stdout, " (file %" PRId32 " %p %p %p)",
        (uint32_t)the_jnode->info.file.size,
        the_jnode->info.file.indirect,
        the_jnode->info.file.doubly_indirect,
        the_jnode->info.file.triply_indirect
      );
#else
      fprintf(stdout, " (file %" PRId32 ")",
        (uint32_t)the_jnode->info.file.size );
#endif
      break;

    case IMFS_HARD_LINK:
      fprintf(stdout, " links not printed\n" );
      return;

    case IMFS_SYM_LINK:
      fprintf(stdout, " links not printed\n" );
      return;

    case IMFS_FIFO:
      fprintf(stdout, " FIFO not printed\n" );
      return;

    default:
      fprintf(stdout, " bad type %d\n", IMFS_type( the_jnode ) );
      return;
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
static void IMFS_dump_directory(
  IMFS_jnode_t  *the_directory,
  int            level
)
{
  rtems_chain_node     *the_node;
  rtems_chain_control  *the_chain;
  IMFS_jnode_t         *the_jnode;
  int                   i;

  IMFS_assert( the_directory );
  IMFS_assert( level >= 0 );
  IMFS_assert( IMFS_is_directory( the_directory ) );

  the_chain = &the_directory->info.directory.Entries;

  for ( the_node = rtems_chain_first( the_chain );
        !rtems_chain_is_tail( the_chain, the_node );
        the_node = the_node->next ) {

    the_jnode = (IMFS_jnode_t *) the_node;

    for ( i=0 ; i<=level ; i++ )
      fprintf(stdout, "...." );
    IMFS_print_jnode( the_jnode );
    if ( IMFS_is_directory( the_jnode ) )
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
  fprintf(stdout, "*************** Dump of Entire IMFS ***************\n" );
  fprintf(stdout, "/\n" );
  IMFS_dump_directory( rtems_filesystem_root->location.node_access, 0 );
  fprintf(stdout, "***************      End of Dump       ***************\n" );
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
