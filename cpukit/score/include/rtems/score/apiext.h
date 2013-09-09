/**
 *  @file  rtems/score/apiext.h
 *
 *  @brief API Extensions Handler
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

#include <rtems/score/chainimpl.h>
#include <rtems/score/thread.h>

#ifdef __cplusplus
extern "C" {
#endif

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
 *  This type defines the prototype of the Post Switch Hook.
 */
typedef void (*API_extensions_Post_switch_hook)(
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
}  API_extensions_Control;

/**
 * @brief Control structure for post switch hooks.
 */
typedef struct {
  Chain_Node Node;

  /**
   * @brief The hook invoked during each context switch in the context of the
   * heir thread.
   *
   * This hook must not be NULL.
   */
  API_extensions_Post_switch_hook hook;
} API_extensions_Post_switch_control;

/**
 *  This is the list of API extensions to the system initialization.
 */
SCORE_EXTERN Chain_Control _API_extensions_List;


/**
 * @brief The API extensions post switch list.
 */
SCORE_EXTERN Chain_Control _API_extensions_Post_switch_list;

/**
 *  @brief Initialize the API extensions handler.
 *
 *  This routine initializes the API extension handler.
 */
void _API_extensions_Initialization( void );

/**
 *  @brief Add extension set to the active set.
 *
 *  This routine adds @a the_extension to the active set of API extensions.
 *
 *  @param[in] the_extension is the extension set to add.
 */
void _API_extensions_Add(
  API_extensions_Control *the_extension
);

/**
 * @brief Adds the API extension post switch control to the post switch list.
 *
 * The post switch control is only added to the list if it is in the off chain
 * state.  Thus this function can be called multiple times with the same
 * post switch control and only the first invocation will actually add it to the
 * list.
 *
 * There is no protection against concurrent access.  This function must be
 * called within a _Thread_Disable_dispatch() critical section.
 *
 * @param [in, out] post_switch The post switch control.
 */
void _API_extensions_Add_post_switch(
  API_extensions_Post_switch_control *post_switch
);

#if defined(FUNCTIONALITY_NOT_CURRENTLY_USED_BY_ANY_API)
/**
 *  @brief Execute all pre-driver extensions.
 *
 *  This routine executes all of the predriver callouts.
 */
  void _API_extensions_Run_predriver( void );
#endif

/**
 *  @brief Execute all post-driver extensions.
 *
 *  This routine executes all of the postdriver callouts.
 */
void _API_extensions_Run_postdriver( void );

/**
 * @brief Runs all API extension post switch hooks.
 */
static inline void _API_extensions_Run_post_switch( Thread_Control *executing )
{
  const Chain_Control *chain = &_API_extensions_Post_switch_list;
  const Chain_Node    *tail = _Chain_Immutable_tail( chain );
  const Chain_Node    *node = _Chain_Immutable_first( chain );

  while ( node != tail ) {
    const API_extensions_Post_switch_control *post_switch =
      (const API_extensions_Post_switch_control *) node;

    (*post_switch->hook)( executing );

    node = _Chain_Immutable_next( node );
  }
}

/**@}*/

#ifdef __cplusplus
}
#endif

#endif
/* end of include file */
