/**
 *  @file  rtems/score/apiext.h
 *
 *  This is the API Extensions Handler.
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef _RTEMS_SCORE_APIEXT_H
#define _RTEMS_SCORE_APIEXT_H

/**
 *  @defgroup ScoreAPIExtension API Extension Handler
 *
 *  @ingroup Score
 *
 *  This handler encapsulates functionality which provides mechanisms for the
 *  SuperCore to perform API specific actions without there beingg
 *  "up-references" from the SuperCore to APIs.  If these referencesg
 *  were allowed in the implementation, the cohesion would be too high
 *  and adding an API would be more difficult.  The SuperCore is supposed
 *  to be largely independent of any API.
 */
/**@{*/

#include <rtems/score/chain.h>
#include <rtems/score/thread.h>

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
  /**
   *  This type defines the prototype of the Predriver Hook.
   */
  typedef void (*API_extensions_Predriver_hook)(void);
#endif

/**
 *  This type defines the prototype of the Postdriver Hook.
 */
typedef void (*API_extensions_Postdriver_hook)(void);

/**
 *  This type defines the prototype of the Postswitch Hook.
 */
typedef void (*API_extensions_Postswitch_hook)(
                 Thread_Control *
             );

/**
 *  The control structure which defines the points at which an API
 *  can add an extension to the system initialization thread.
 */
typedef struct {
  /** This field allows this structure to be used with the Chain Handler. */
  Chain_Node                      Node;
  #if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
    /**
     * This field is the callout invoked during RTEMS initialization after
     * RTEMS data structures are initialized before device driver initialization
     * has occurred.
     *
     * @note If this field is NULL, no extension is invoked.
     */
    API_extensions_Predriver_hook   predriver_hook;
  #endif
  /**
   * This field is the callout invoked during RTEMS initialization after
   * RTEMS data structures and device driver initialization has occurred
   * but before multitasking is initiated.
   *
   * @note If this field is NULL, no extension is invoked.
   */
  API_extensions_Postdriver_hook  postdriver_hook;
  /**
   * This field is the callout invoked during each context switch
   * in the context of the heir thread.
   *
   * @note If this field is NULL, no extension is invoked.
   */
  API_extensions_Postswitch_hook  postswitch_hook;
}  API_extensions_Control;

/**
 *  This is the list of API extensions to the system initialization.
 */
SCORE_EXTERN Chain_Control _API_extensions_List;

/**
 * @brief Initialize the API Extensions Handler
 *
 *  This routine initializes the API extension handler.
 */
void _API_extensions_Initialization( void );

/**
 * @brief Add Extension Set to the Active Set
 *
 *  This routine adds an extension to the active set of API extensions.
 *
 *  @param[in] the_extension is the extension set to add.
 */
void _API_extensions_Add(
  API_extensions_Control *the_extension
);

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
  /**
   * @brief Execute all Pre-Driver Extensions
   *
   *  This routine executes all of the predriver callouts.
   */
  void _API_extensions_Run_predriver( void );
#endif

/**
 * @brief Execute all Post-Driver Extensions
 *
 *  This routine executes all of the postdriver callouts.
 */
void _API_extensions_Run_postdriver( void );

/**
 * @brief Execute all Post Context Switch Extensions
 *
 *  This routine executes all of the post context switch callouts.
 */
void _API_extensions_Run_postswitch( void );

/**@}*/

#endif
/* end of include file */
