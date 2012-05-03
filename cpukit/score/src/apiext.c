/*  apiext.c
 *
 *  XXX
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

#include <rtems/system.h>
#include <rtems/score/apiext.h>

/*
 *  _API_extensions_Initialization
 */

void _API_extensions_Initialization( void )
{
 _Chain_Initialize_empty( &_API_extensions_List );
}

/*
 *  _API_extensions_Add
 */

void _API_extensions_Add(
  API_extensions_Control *the_extension
)
{
  _Chain_Append( &_API_extensions_List, &the_extension->Node );
}

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
  /*
   *  _API_extensions_Run_predriver
   */

  void _API_extensions_Run_predriver( void )
  {
    Chain_Node             *the_node;
    API_extensions_Control *the_extension;

    for ( the_node = _Chain_First( &_API_extensions_List );
	  !_Chain_Is_tail( &_API_extensions_List, the_node ) ;
	  the_node = the_node->next ) {

      the_extension = (API_extensions_Control *) the_node;

      if ( the_extension->predriver_hook )
	(*the_extension->predriver_hook)();
    }
  }
#endif

/*
 *  _API_extensions_Run_postdriver
 */

void _API_extensions_Run_postdriver( void )
{
  Chain_Node             *the_node;
  API_extensions_Control *the_extension;

  for ( the_node = _Chain_First( &_API_extensions_List );
        !_Chain_Is_tail( &_API_extensions_List, the_node ) ;
        the_node = the_node->next ) {

    the_extension = (API_extensions_Control *) the_node;

    /*
     *  Currently all APIs configure this hook so it is always non-NULL.
     */
#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    if ( the_extension->postdriver_hook )
#endif
      (*the_extension->postdriver_hook)();
  }
}

/*
 *  _API_extensions_Run_postswitch
 */

void _API_extensions_Run_postswitch( void )
{
  Chain_Node             *the_node;
  API_extensions_Control *the_extension;

  for ( the_node = _Chain_First( &_API_extensions_List );
        !_Chain_Is_tail( &_API_extensions_List, the_node ) ;
        the_node = the_node->next ) {

    the_extension = (API_extensions_Control *) the_node;

    (*the_extension->postswitch_hook)( _Thread_Executing );
  }
}

/* end of file */
