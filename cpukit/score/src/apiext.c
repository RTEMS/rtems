/*  apiext.c
 *
 *  XXX
 *
 *  COPYRIGHT (c) 1989, 1990, 1991, 1992, 1993, 1994.
 *  On-Line Applications Research Corporation (OAR).
 *  All rights assigned to U.S. Government, 1994.
 *
 *  This material may be reproduced by or for the U.S. Government pursuant
 *  to the copyright license under the clause at DFARS 252.227-7013.  This
 *  notice must appear in all copies of this file and its derivatives.
 *
 *  $Id$
 */


#include <rtems/system.h>
#include <rtems/core/apiext.h>

/*PAGE
 *
 *  _API_extensions_Initialization
 */
 
void _API_extensions_Initialization( void )
{
 _Chain_Initialize_empty( &_API_extensions_List );
}
 
/*PAGE
 *
 *  _API_extensions_Add
 */
 
void _API_extensions_Add(
  API_extensions_Control *the_extension
)
{
  _Chain_Append( &_API_extensions_List, &the_extension->Node );
}

/*PAGE
 *
 *  _API_extensions_Run_predriver
 */

void _API_extensions_Run_predriver( void )
{
  Chain_Node             *the_node;
  API_extensions_Control *the_extension;
 
  for ( the_node = _API_extensions_List.first ;
        !_Chain_Is_tail( &_API_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (API_extensions_Control *) the_node;
 
    if ( the_extension->predriver_hook )
      (*the_extension->predriver_hook)();
  }
}

/*PAGE
 *
 *  _API_extensions_Run_postdriver
 */

void _API_extensions_Run_postdriver( void )
{
  Chain_Node             *the_node;
  API_extensions_Control *the_extension;
 
  for ( the_node = _API_extensions_List.first ;
        !_Chain_Is_tail( &_API_extensions_List, the_node ) ;
        the_node = the_node->next ) {
 
    the_extension = (API_extensions_Control *) the_node;
 
    if ( the_extension->postdriver_hook )
      (*the_extension->postdriver_hook)();
  }
}

/* end of file */
