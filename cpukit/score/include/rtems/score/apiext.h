/*  apiext.h
 *
 *  This is the API Extensions Handler.
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


#ifndef __API_EXTENSIONS_h
#define __API_EXTENSIONS_h

#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

/*
 *  The control structure which defines the points at which an API
 *  can add an extension to the system initialization thread.
 */
 
typedef void (*API_extensions_Predriver_hook)(void);
typedef void (*API_extensions_Postdriver_hook)(void);
typedef void (*API_extensions_Postswitch_hook)(
                 Thread_Control *
             );
 
 
typedef struct {
  Chain_Node                      Node;
  API_extensions_Predriver_hook   predriver_hook;
  API_extensions_Postdriver_hook  postdriver_hook;
  API_extensions_Postswitch_hook  postswitch_hook;
}  API_extensions_Control;

/*
 *  This is the list of API extensions to the system initialization.
 */
 
SCORE_EXTERN Chain_Control _API_extensions_List;

/*
 *  _API_extensions_Initialization
 *
 *  DESCRIPTION:
 *
 *  This routine initializes the API extension handler.
 *
 */
 
void _API_extensions_Initialization( void );
 
/*
 *  _API_extensions_Add
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _API_extensions_Add(
  API_extensions_Control *the_extension
);

/*
 *  _API_extensions_Run_predriver
 *
 *  DESCRIPTION:
 *
 *  XXX
 */
 
void _API_extensions_Run_predriver( void );

/*
 *  _API_extensions_Run_postdriver
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

void _API_extensions_Run_postdriver( void );

/*
 *  _API_extensions_Run_postswitch
 *
 *  DESCRIPTION:
 *
 *  XXX
 */

void _API_extensions_Run_postswitch( void );

#endif
/* end of include file */
