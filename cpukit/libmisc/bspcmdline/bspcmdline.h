/**
 *  @file  rtems/bspcmdline.h
 *
 *  This include file contains all prototypes and specifications
 *  related to the BSP Command Line String and associated helper
 *  routines.  The helpers are useful for locating command line
 *  type arguments (e.g. --mode) and their associated right
 *  hand side (e.g. FAST in --mode=FAST).
 */

/*
 *  COPYRIGHT (c) 1989-2009.
 *  On-Line Applications Research Corporation (OAR).
 *
 *  The license and distribution terms for this file may be
 *  found in the file LICENSE in this distribution or at
 *  http://www.rtems.com/license/LICENSE.
 */

#ifndef __BSP_COMMAND_LINE_h
#define __BSP_COMMAND_LINE_h

/**
 *  @defgroup BSPCommandLine BSP Command Line Helpers
 *
 *  The BSP Command Line Handler provides a set of routines which assist
 *  in examining and decoding the Command Line String passed to the BSP
 *  at boot time.
 */
/**@{*/

#include <stddef.h> /* for size_t */

#ifdef __cplusplus
extern "C" {
#endif


/**
 *  @brief Obtain Pointer to BSP Boot Command String
 *
 *  This method returns a pointer to the BSP Boot Command String.  It
 *  is as likely to be NULL as point to a string as most BSPs do not
 *  have a start environment that provides a boot string.
 *
 *  @return This method returns the pointer to the BSP Boot Command String.
 */
const char *rtems_bsp_cmdline_get(void);

/**
 *  @brief Obtain COPY of the Entire Matching Argument
 *
 *  This method searches for the argument @a name in the BSP Boot Command
 *  String and returns a copy of the entire string associated with it in
 *  @a value up to a string of @a length.  This will include the argument
 *  and any right hand side portion of the string.  For example, one might
 *  be returned --mode=FAST if
 *  searching for --mode.
 *
 *  @param[in] name is the arugment to search for
 *  @param[in] value points to where the contents will
 *             be placed if located.
 *  @param[in] length is the maximum length to copy
 *
 *  @return This method returns NULL if not found and
 *          @a value if found.
 */
const char *rtems_bsp_cmdline_get_param(
  const char *name,
  char       *value,
  size_t      length
);


/**
 *  @brief Obtain COPY of the Right Hand Side of the Matching Argument
 *
 *  This method searches for the argument @a name in
 *  the BSP Boot Command String and returns the right hand side
 *  associated with it in @a value up to a maximum string @a length.
 *  This will NOT include the argument but only any right hand side
 *  portion of the string. *  For example, one might be returned FAST if
 *  searching for --mode.
 *
 *  @param[in] name is the arugment to search for
 *  @param[in] value points to where the contents will
 *             be placed if located.
 *  @param[in] length is the maximum length to copy
 *
 *  @return This method returns NULL if not found and
 *          @a value if found.
 */
const char *rtems_bsp_cmdline_get_param_rhs(
  const char *name,
  char       *value,
  size_t      length
);

/**
 *  @brief Obtain Pointer to the Entire Matching Argument
 *
 *  This method searches for the argument @a name in
 *  the BSP Boot Command String and returns a pointer to the
 *  entire string associated with it.  This will include the
 *  argument and any right hand side portion of the string.
 *  For example, one might be returned --mode=FAST if
 *  searching for --mode.
 *
 *  @param[in] name is the arugment to search for
 *
 *  @return This method returns NULL if not found and a pointer
 *          into the BSP Boot Command String if found.
 *
 *  @note The pointer will be to the original BSP Command
 *        Line string.  Exercise caution when using this.
 */
const char *rtems_bsp_cmdline_get_param_raw(
  const char *name
);

#ifdef __cplusplus
}
#endif

/**@}*/
#endif
