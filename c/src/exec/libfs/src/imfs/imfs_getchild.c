/*
 *  IMFS_find_match_in_dir()
 *
 *  This routine returns the child name in the given directory.
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

#include <errno.h>
#include <assert.h>
#include <string.h>
#include "imfs.h"

static char dotname[2] = ".";
static char dotdotname[3] = "..";

IMFS_jnode_t *IMFS_find_match_in_dir(
  IMFS_jnode_t *directory, 
  char         *name
)
{
  Chain_Node        *the_node;
  Chain_Control     *the_chain;
  IMFS_jnode_t      *the_jnode;

  /*
   *  Check for fatal errors.  A NULL directory show a problem in the
   *  the IMFS code.
   */

  assert( directory );
  if ( !name )
    return 0;

  assert( name );
  if ( !directory )
    return 0;

  /*
   *  Check for "." and ".."
   */

  if ( !strcmp( name, dotname ) )
    return directory;

  if ( !strcmp( name, dotdotname ) )
    return directory->Parent;

  the_chain = &directory->info.directory.Entries;

  for ( the_node = the_chain->first;
        !_Chain_Is_tail( the_chain, the_node );
        the_node = the_node->next ) {

    the_jnode = (IMFS_jnode_t *) the_node;

    if ( !strcmp( name, the_jnode->name ) )
      return the_jnode;
  }

  return 0;
}
